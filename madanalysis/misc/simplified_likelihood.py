################################################################################
#
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#
#  This file is part of MadAnalysis 5.
#  Official website: <https://launchpad.net/madanalysis5>
#
#  MadAnalysis 5 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MadAnalysis 5 is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
#
################################################################################

#!/usr/bin/env python3

"""
.. module:: simplifiedLikelihoods
   :synopsis: Code that implements the simplified likelihoods as presented
              in CMS-NOTE-2017-001, see https://cds.cern.ch/record/2242860.
              In collaboration with Andy Buckley, Sylvain Fichet and Nickolas Wardle.
              Simplified likelihoods v2 (JHEP_021P_1018) are partly implemented.
              Adapted from SModelS

.. moduleauthor:: Wolfgang Waltenberger <wolfgang.waltenberger@gmail.com>

"""

from scipy import stats, optimize, integrate, special, linalg
from numpy import sqrt, exp, log, sign, array, ndarray
from functools import reduce
from typing import Text, Optional, Union, Tuple

# from smodels.tools.statistics import rootFromNLLs, determineBrentBracket

import numpy as np
import math
import copy
import logging

logger = logging.getLogger("MA5")

# rootFromNLLs, determineBrentBracket are retreived from smodels.tools.statistics
def rootFromNLLs(nllA, nll0A, nll, nll0, get_cls=False):
    """compute the CLs - alpha from the NLLs"""
    qmu = 0.0 if 2 * (nll - nll0) < 0.0 else 2 * (nll - nll0)
    sqmu = np.sqrt(qmu)
    qA = 2 * (nllA - nll0A)
    if qA < 0.0:
        qA = 0.0
    sqA = np.sqrt(qA)
    if qA >= qmu:
        CLsb = 1.0 - stats.multivariate_normal.cdf(sqmu)
        CLb = stats.multivariate_normal.cdf(sqA - sqmu)
    else:
        CLsb = 1.0 if qA == 0.0 else 1.0 - stats.multivariate_normal.cdf((qmu + qA) / (2 * sqA))
        CLb = 1.0 if qA == 0.0 else 1.0 - stats.multivariate_normal.cdf((qmu - qA) / (2 * sqA))

    CLs = CLsb / CLb if CLb > 0 else 0.0

    if get_cls:
        return 1.0 - CLs

    cl = 0.95
    root = CLs - 1.0 + cl
    return root


def determineBrentBracket(mu_hat, sigma_mu, rootfinder):
    """find a, b for brent bracketing
    :param mu_hat: mu that maximizes likelihood
    :param sigm_mu: error on mu_hat (not too reliable)
    :param rootfinder: function that finds the root (usually root_func)
    """
    sigma_mu = max(sigma_mu, 0.5)  # there is a minimum on sigma_mu
    # the root should be roughly at mu_hat + 2*sigma_mu
    a = mu_hat + 1.5 * sigma_mu
    ntrials = 20
    i = 0
    foundExtra = False
    while rootfinder(a) < 0.0:
        # if this is negative, we move it to the left
        i += 1
        a -= (i**2.0) * sigma_mu
        if i > ntrials or a < -10000.0:
            for a in [0.0, 1.0, -1.0, 3.0, -3.0, 10.0, -10.0, 0.1, -0.1, 0.01, -0.01]:
                if rootfinder(a) > 0.0:
                    foundExtra = True
                    break
            if not foundExtra:
                logger.error(
                    f"cannot find an a that is left of the root. last attempt, a={a:.2f}, root = {rootfinder(a):.2f}."
                )
                logger.error(f"mu_hat={mu_hat:.2f}, sigma_mu={sigma_mu:.2f}")
                return 0.0, 0.0
    i = 0
    foundExtra = False
    b = mu_hat + 2.5 * sigma_mu
    while rootfinder(b) > 0.0:
        # if this is positive, we move it to the right
        i += 1
        b += (i**2.0) * sigma_mu
        closestr, closest = float("inf"), None
        if i > ntrials:
            for b in [1.0, 0.0, 3.0, -1.0, 10.0, -3.0, 0.1, -0.1, -10.0, 100.0, -100.0, 1000.0]:
                root = rootfinder(b)
                if root < 0.0:
                    foundExtra = True
                    break
                if root < closestr:
                    closestr = root
                    closest = b
            if not foundExtra:
                logger.error(f"cannot find a b that is right of the root (i.e. rootfinder(b) < 0).")
                logger.error(f"closest to zero rootfinder({closest})={closestr}")
                logger.error(f"mu_hat was at {mu_hat:.2f} sigma_mu at {sigma_mu:.2f}")
                return 0.0, 0.0
    return a, b


class Data:
    """A very simple observed container to collect all the data
    needed to fully define a specific statistical model"""

    def __init__(
        self,
        observed,
        backgrounds,
        covariance,
        third_moment=None,
        nsignal=None,
        name="model",
        deltas_rel=0.2,
        lumi=None,
    ):
        """
        :param observed: number of observed events per dataset
        :param backgrounds: expected bg per dataset
        :param covariance: uncertainty in background, as a covariance matrix
        :param nsignal: number of signal events in each dataset
        :param name: give the model a name, just for convenience
        :param deltas_rel: the assumed relative error on the signal hypotheses.
                           The default is 20%.
        :param lumi: luminosity of dataset in 1/fb, or None
        """
        self.observed = self.convert(observed)  # Make sure observed number of events are integers
        ## self.observed = np.around(self.convert(observed)) #Make sure observed number of events are integers
        self.backgrounds = self.convert(backgrounds)
        self.n = len(self.observed)
        self.covariance = self._convertCov(covariance)
        self.nsignal = self.convert(nsignal)
        self.lumi = lumi
        if self.nsignal is None:
            if len(self.backgrounds) == 1:
                # doesnt matter, does it?
                self.nsignal = np.array([1.0])
            self.signal_rel = self.convert(1.0)
        elif self.nsignal.sum():
            self.signal_rel = self.nsignal / self.nsignal.sum()
        else:
            self.signal_rel = array([0.0] * len(self.nsignal))

        self.third_moment = self.convert(third_moment)
        if (
            type(self.third_moment) != type(None)
            and np.sum([abs(x) for x in self.third_moment]) < 1e-10
        ):
            self.third_moment = None
        self.name = name
        self.deltas_rel = deltas_rel
        self._computeABC()

    def totalCovariance(self, nsig):
        """get the total covariance matrix, taking into account
        also signal uncertainty for the signal hypothesis <nsig>.
        If nsig is None, the predefined signal hypothesis is taken.
        """
        if self.isLinear():
            cov_tot = self.V + self.var_s(nsig)
        else:
            cov_tot = self.covariance + self.var_s(nsig)
        return cov_tot

    def zeroSignal(self):
        """
        Is the total number of signal events zero?
        """
        if self.nsignal is None:
            return True
        return len(self.nsignal[self.nsignal > 0.0]) == 0

    def var_s(self, nsig=None):
        """
        The signal variances. Convenience function.

        :param nsig: If None, it will use the model expected number of signal events,
                    otherwise will return the variances for the input value using the relative
                    signal uncertainty defined for the model.

        """

        if nsig is None:
            nsig = self.nsignal
        else:
            nsig = self.convert(nsig)
        return np.diag((nsig * self.deltas_rel) ** 2)

    def isScalar(self, obj):
        """
        Determine if obj is a scalar (float or int)
        """

        if isinstance(obj, ndarray):
            ## need to treat separately since casting array([0.]) to float works
            return False
        try:
            _ = float(obj)
            return True
        except (ValueError, TypeError):
            pass
        return False

    def convert(self, obj):
        """
        Convert object to numpy arrays.
        If object is a float or int, it is converted to a one element
        array.
        """

        if type(obj) == type(None):
            return obj
        if self.isScalar(obj):
            return array([obj])
        return array(obj)

    def __str__(self):
        return self.name + " (%d dims)" % self.n

    def _convertCov(self, obj):

        if self.isScalar(obj):
            return array([[obj]])
        if isinstance(obj[0], list):
            return array(obj)
        if isinstance(obj[0], float):
            ## if the matrix is flattened, unflatten it.
            return array([obj[self.n * i : self.n * (i + 1)] for i in range(self.n)])

        return obj

    def _computeABC(self):
        """
        Compute the terms A, B, C, rho, V. Corresponds with
        Eqs. 1.27-1.30 in the second paper.
        """
        self.V = self.covariance
        if self.third_moment is None:
            self.A = None
            self.B = None
            self.C = None
            return

        covD = self.diagCov()
        C = []
        for m2, m3 in zip(covD, self.third_moment):
            if m3 == 0.0:
                m3 = 1e-30
            k = -np.sign(m3) * sqrt(2.0 * m2)
            dm = sqrt(8.0 * m2**3 / m3**2 - 1.0)
            C.append(k * np.cos(4.0 * np.pi / 3.0 + np.arctan(dm) / 3.0))

        self.C = np.array(C)  ## C, as define in Eq. 1.27 (?) in the second paper
        self.B = sqrt(covD - 2 * self.C**2)  ## B, as defined in Eq. 1.28(?)
        self.A = self.backgrounds - self.C  ## A, Eq. 1.30(?)
        self.rho = np.array([[0.0] * self.n] * self.n)  ## Eq. 1.29 (?)
        for x in range(self.n):
            for y in range(x, self.n):
                bxby = self.B[x] * self.B[y]
                cxcy = self.C[x] * self.C[y]
                e = (4.0 * cxcy) ** (-1) * (
                    sqrt(bxby**2 + 8 * cxcy * self.covariance[x][y]) - bxby
                )
                self.rho[x][y] = e
                self.rho[y][x] = e

        self.sandwich()
        # self.V = sandwich ( self.B, self.rho )

    def sandwich(self):
        """
        Sandwich product
        """

        ret = np.array([[0.0] * len(self.B)] * len(self.B))
        for x in range(len(self.B)):
            for y in range(x, len(self.B)):
                T = self.B[x] * self.B[y] * self.rho[x][y]
                ret[x][y] = T
                ret[y][x] = T
        self.V = ret

    def isLinear(self):
        """
        Statistical model is linear, i.e. no quadratic term in poissonians
        """

        return type(self.C) == type(None)

    def diagCov(self):
        """
        Diagonal elements of covariance matrix. Convenience function.
        """

        return np.diag(self.covariance)

    def correlations(self):
        """
        Correlation matrix, computed from covariance matrix.
        Convenience function.
        """

        if hasattr(self, "corr"):
            return self.corr

        self.corr = copy.deepcopy(self.covariance)
        for x in range(self.n):
            self.corr[x][x] = 1.0
            for y in range(x + 1, self.n):
                rho = self.corr[x][y] / sqrt(self.covariance[x][x] * self.covariance[y][y])
                self.corr[x][y] = rho
                self.corr[y][x] = rho
        return self.corr

    def signals(self, mu):
        """
        Returns the number of expected signal events, for all datasets,
        given total signal strength mu.

        :param mu: Total number of signal events summed over all datasets.
        """

        return mu * self.signal_rel

    def nsignals(self, mu):
        """
        Returns the number of expected signal events, for all datasets,
        given total signal strength mu.

        :param mu: Total number of signal events summed over all datasets.
        """

        return mu * self.nsignal


class LikelihoodComputer:

    debug_mode = False

    def __init__(self, data, toys=30000):
        """
        :param data: a Data object.
        :param toys: number of toys when marginalizing
        """

        self.model = data
        self.toys = toys

    def dNLLdMu(self, mu, signal_rel, theta_hat):
        """
        d (- ln L)/d mu, if L is the likelihood. The function
        whose root gives us muhat, i.e. the mu that maximizes
        the likelihood.

        :param mu: total number of signal events
        :param signal_rel: array with the relative signal strengths for each dataset (signal region)
        :param theta_hat: array with nuisance parameters

        """
        if not self.model.isLinear():
            logger.debug("implemented only for linear model")
        # n_pred^i := mu s_i + b_i + theta_i
        # NLL = sum_i [ - n_obs^i * ln ( n_pred^i ) + n_pred^i ]
        # d NLL / d mu = sum_i [ - ( n_obs^i * s_ i ) / n_pred_i + s_i ]

        # Define relative signal strengths:
        n_pred = mu * signal_rel + self.model.backgrounds + theta_hat

        for ctr, d in enumerate(n_pred):
            if d == 0.0:
                if (self.model.observed[ctr] * signal_rel[ctr]) == 0.0:
                    #    logger.debug("zero denominator, but numerator also zero, so we set denom to 1.")
                    n_pred[ctr] = 1e-5
                else:
                    # n_pred[ctr]=1e-5
                    raise Exception(
                        "we have a zero value in the denominator at pos %d, with a non-zero numerator. dont know how to handle."
                        % ctr
                    )
        ret = -self.model.observed * signal_rel / n_pred + signal_rel

        if type(ret) in [array, ndarray, list]:
            ret = sum(ret)
        return ret

    def extendedOutput(self, extended_output, default=None):
        if extended_output:
            ret = {"muhat": default, "sigma_mu": default, "lmax": default}
            return ret
        return default

    def findAvgr(self, mu, theta_hat, signal_rel):
        """from the difference observed - background, find got inital
        values for lower and upper"""
        mu_c = self.model.observed - self.model.backgrounds - theta_hat
        mu_r, wmu_r = [], []
        n_pred = mu * signal_rel + self.model.backgrounds + theta_hat
        obs = self.model.observed
        for i, s in enumerate(n_pred):
            if s == 0.0:
                n_pred[i] = 1e-6
        hessian = self.model.observed * signal_rel**2 / n_pred**2
        wtot = 0.0
        for s in zip(mu_c, signal_rel, hessian):
            if s[1] > 1e-10:
                w = 1.0
                if s[2] > 0.0:
                    w = s[2]
                wtot += w
                mu_r.append(s[0] / s[1])
                wmu_r.append(w * s[0] / s[1])
        ret = min(mu_r), sum(wmu_r) / wtot, max(mu_r)
        return ret

    def findMuHat(
        self, nsig, allowNegativeSignals=False, extended_output=False, nll=False, marginalize=False
    ):
        """
        Find the most likely signal strength mu
        given the relative signal strengths in each dataset (signal region).

        :param nsig: array with relative signal strengths or signal yields
        :param allowNegativeSignals: if true, then also allow for negative values
        :param extended_output: if true, return also sigma_mu, the estimate of the error of mu_hat, and lmax, the likelihood at mu_hat
        :param nll: if true, return nll instead of lmax in the extended output

        :returns: mu_hat, i.e. the maximum likelihood estimate of mu, if extended output is requested, it returns mu_hat, sigma_mu -- the standard deviation around mu_hat, and llhd, the likelihood at mu_hat
        """
        if (self.model.backgrounds == self.model.observed).all():
            return self.extendedOutput(extended_output, 0.0)

        if type(nsig) in [list, ndarray]:
            nsig = array(nsig)

        nsig[nsig == 0.0] = 1e-20
        if sum(nsig < 0.0):
            raise Exception("Negative relative signal strengths!")

        ## we need a very rough initial guess for mu(hat), to come
        ## up with a first theta
        # self.nsig = array([0.]*len(self.model.observed))
        self.nsig = nsig
        ## we start with theta_hat being all zeroes
        # theta_hat = array([0.]*len(self.model.observed))
        mu_hat_old, mu_hat = 0.0, 1.0
        ctr = 0
        widener = 3.0
        while (
            abs(mu_hat - mu_hat_old) > 1e-10
            and abs(mu_hat - mu_hat_old) / (mu_hat + mu_hat_old) > 0.5e-2
            and ctr < 20
        ):
            theta_hat, _ = self.findThetaHat(mu_hat * nsig)
            ctr += 1
            mu_hat_old = mu_hat
            minr, avgr, maxr = self.findAvgr(mu_hat, theta_hat, nsig)
            # for i,s in enumerate ( signal_rel ):
            #    if abs(s) < 1e-19:
            #        mu_c[i]=0.
            ## find mu_hat by finding the root of 1/L dL/dmu. We know
            ## that the zero has to be between min(mu_c) and max(mu_c).
            lstarters = [
                avgr - 0.2 * abs(avgr),
                minr,
                0.0,
                -1.0,
                1.0,
                10.0,
                -0.1,
                0.1,
                -100.0,
                100.0,
                -1000.0,
            ]
            closestl, closestr = None, float("inf")
            for lower in lstarters:
                lower_v = self.dNLLdMu(lower, nsig, theta_hat)
                if lower_v < 0.0:
                    break
                if lower_v < closestr:
                    closestl, closestr = lower, lower_v
            if lower_v > 0.0:
                logger.debug(
                    f"did not find a lower value with rootfinder(lower) < 0. Closest: f({closestl})={closestr}"
                )
                return self.extendedOutput(extended_output, 0.0)
            ustarters = [
                avgr + 0.2 * abs(avgr),
                maxr,
                0.0,
                1.0,
                10.0,
                -1.0 - 0.1,
                0.1,
                100.0,
                -100.0,
                1000.0,
                -1000.0,
                0.01,
                -0.01,
            ]
            closestl, closestr = None, float("inf")
            for upper in ustarters:
                upper_v = self.dNLLdMu(upper, nsig, theta_hat)
                if upper_v > 0.0:
                    break
                if upper_v < closestr:
                    closestl, closestr = upper, upper_v
            if upper_v < 0.0:
                logger.debug("did not find an upper value with rootfinder(upper) > 0.")
                return self.extendedOutput(extended_output, 0.0)
            mu_hat = optimize.brentq(self.dNLLdMu, lower, upper, args=(nsig, theta_hat), rtol=1e-9)
            if not allowNegativeSignals and mu_hat < 0.0:
                mu_hat = 0.0
                theta_hat, _ = self.findThetaHat(mu_hat * nsig)
            if self.debug_mode:
                self.theta_hat = theta_hat

        # print ( f">> found mu_hat {mu_hat:.2g} for signal {sum(signal_rel):.2f} allowNeg {allowNegativeSignals} l,u={lower},{upper} lastavgr={avgr}" )
        # print ( f">>>> obs {self.model.observed[:2]} bg {self.model.backgrounds[:2]}" )
        # print ( f">>>> mu_c {np.mean(mu_c)}" )
        if extended_output:
            sigma_mu = self.getSigmaMu(mu_hat, nsig, theta_hat)
            llhd = self.likelihood(self.model.signals(mu_hat), marginalize=marginalize, nll=nll)
            ret = {"muhat": mu_hat, "sigma_mu": sigma_mu, "lmax": llhd}
            return ret
        return mu_hat

    def getSigmaMu(self, mu, nsig, theta_hat):
        """
        Get an estimate for the standard deviation of mu at <mu>, from
        the inverse hessian

        :param nsig: array with signal yields or relative signal strengths
                     in each dataset (signal region)
        """
        if not self.model.isLinear():
            logger.debug("implemented only for linear model")
        # d^2 mu NLL / d mu^2 = sum_i [ n_obs^i * s_i**2 / n_pred^i**2 ]

        # Define relative signal strengths:
        n_pred = mu * nsig + self.model.backgrounds + theta_hat

        for ctr, d in enumerate(n_pred):
            if d == 0.0:
                if (self.model.observed[ctr] * nsig[ctr]) == 0.0:
                    #    logger.debug("zero denominator, but numerator also zero, so we set denom to 1.")
                    n_pred[ctr] = 1.0
                else:
                    raise Exception(
                        "we have a zero value in the denominator at pos %d, with a non-zero numerator. dont know how to handle."
                        % ctr
                    )
        hessian = self.model.observed * nsig**2 / n_pred**2

        if type(hessian) in [array, ndarray, list]:
            hessian = sum(hessian)
        # the error is the square root of the inverse of the hessian
        if hessian == 0.0:
            # if all observations are zero, we replace them by the expectations
            if sum(self.model.observed) == 0:
                hessian = sum(nsig**2 / n_pred)
        stderr = float(np.sqrt(1.0 / hessian))
        return stderr
        """
        if type(nsig) in [ list, ndarray ]:
            s_effs = sum(nsig)

        sgm_mu = float(sqrt(sum(self.model.observed) + sum(np.diag(self.model.covariance)))/s_effs)

        return sgm_mu
        """

    # Define integrand (gaussian_(bg+signal)*poisson(nobs)):
    # def prob(x0, x1 )
    def probMV(self, nll, theta):
        """probability, for nuicance parameters theta
        :params nll: if True, compute negative log likelihood"""
        # theta = array ( thetaA )
        # ntot = self.model.backgrounds + self.nsig
        # lmbda = theta + self.ntot ## the lambda for the Poissonian
        if self.model.isLinear():
            lmbda = self.model.backgrounds + self.nsig + theta
        else:
            lmbda = self.nsig + self.model.A + theta + self.model.C * theta**2 / self.model.B**2
        lmbda[lmbda <= 0.0] = 1e-30  ## turn zeroes to small values
        obs = self.model.observed

        def is_integer(x):
            if type(x) in [int, np.int64]:
                return True
            if type(x) in [float]:
                return x.is_integer()
            return False

        ## not needed for now
        allintegers = np.all([is_integer(i) for i in obs])
        if nll:
            if allintegers:
                poisson = stats.poisson.logpmf(obs, lmbda)
            else:
                poisson = -lmbda + obs * np.log(lmbda) - special.loggamma(obs + 1)
        else:
            if allintegers:
                poisson = stats.poisson.pmf(obs, lmbda)
            else:
                # poisson = np.exp(-lmbda)*np.power(lmbda,obs)/special.gamma(obs+1)
                logpoiss = -lmbda + obs * np.log(lmbda) - special.loggamma(obs + 1)
                poisson = np.exp(logpoiss)
        try:
            M = [0.0] * len(theta)
            C = self.model.V
            # if self.model.n == 1: I think not a good idea
            #    C = self.model.totalCovariance(self.nsig)
            dTheta = theta - M
            expon = -0.5 * np.dot(np.dot(dTheta, self.weight), dTheta) + self.logcoeff
            # print ( "expon", expon, "coeff", self.coeff )
            if nll:
                gaussian = expon  #  np.log ( self.coeff )
                # gaussian2 = stats.multivariate_normal.logpdf(theta,mean=M,cov=C)
                ret = -gaussian - sum(poisson)
            else:
                gaussian = np.exp(expon)
                # gaussian = self.coeff * np.exp ( expon )
                # gaussian2 = stats.multivariate_normal.pdf(theta,mean=M,cov=C)
                ret = gaussian * (reduce(lambda x, y: x * y, poisson))
            return ret
        except ValueError as e:
            raise Exception("ValueError %s, %s" % (e, self.model.V))
            # raise Exception("ValueError %s, %s" % ( e, self.model.totalCovariance(self.nsig) ))
            # raise Exception("ValueError %s, %s" % ( e, self.model.V ))

    def nllOfNuisances(self, theta):
        """probability, for nuicance parameters theta,
        as a negative log likelihood."""
        return self.probMV(True, theta)

    def dNLLdTheta(self, theta):
        """the derivative of nll as a function of the thetas.
        Makes it easier to find the maximum likelihood."""
        if self.model.isLinear():
            xtot = theta + self.model.backgrounds + self.nsig
            xtot[xtot <= 0.0] = 1e-30  ## turn zeroes to small values
            nllp_ = self.ones - self.model.observed / xtot + np.dot(theta, self.weight)
            return nllp_
        lmbda = self.nsig + self.model.A + theta + self.model.C * theta**2 / self.model.B**2
        lmbda[lmbda <= 0.0] = 1e-30  ## turn zeroes to small values
        # nllp_ = ( self.ones - self.model.observed / lmbda + np.dot( theta , self.weight ) ) * ( self.ones + 2*self.model.C * theta / self.model.B**2 )
        T = self.ones + 2 * self.model.C / self.model.B**2 * theta
        nllp_ = T - self.model.observed / lmbda * (T) + np.dot(theta, self.weight)
        return nllp_

    def d2NLLdTheta2(self, theta):
        """the Hessian of nll as a function of the thetas.
        Makes it easier to find the maximum likelihood."""
        # xtot = theta + self.ntot
        if self.model.isLinear():
            xtot = theta + self.model.backgrounds + self.nsig
            xtot[xtot <= 0.0] = 1e-30  ## turn zeroes to small values
            nllh_ = self.weight + np.diag(self.model.observed / (xtot**2))
            return nllh_
        lmbda = self.nsig + self.model.A + theta + self.model.C * theta**2 / self.model.B**2
        lmbda[lmbda <= 0.0] = 1e-30  ## turn zeroes to small values
        T = self.ones + 2 * self.model.C / self.model.B**2 * theta
        # T_i = 1_i + 2*c_i/B_i**2 * theta_i
        nllh_ = (
            self.weight
            + np.diag(self.model.observed * T**2 / (lmbda**2))
            - np.diag(self.model.observed / lmbda * 2 * self.model.C / self.model.B**2)
            + np.diag(2 * self.model.C / self.model.B**2)
        )
        return nllh_

    def getThetaHat(self, nobs, nb, nsig, covb, max_iterations):
        """Compute nuisance parameter theta that
        maximizes our likelihood (poisson*gauss)."""
        self.nsig = nsig
        sigma2 = covb + self.model.var_s(nsig)  ## np.diag ( (self.model.deltas)**2 )
        ## for now deal with variances only
        ntot = nb + nsig
        cov = np.array(sigma2)
        # weight = cov**(-1) ## weight matrix
        weight = linalg.inv(cov)
        diag_cov = np.diag(cov)
        # first: no covariances:
        q = diag_cov * (ntot - nobs)
        p = ntot + diag_cov
        thetamaxes = []
        thetamax = -p / 2.0 * (1 - sign(p) * sqrt(1.0 - 4 * q / p**2))
        thetamaxes.append(thetamax)
        ndims = len(p)

        def distance(theta1, theta2):
            for ctr, i in enumerate(theta1):
                if i == 0.0:
                    theta1[ctr] = 1e-20
            for ctr, i in enumerate(theta2):
                if i == 0.0:
                    theta2[ctr] = 1e-20
            return sum(np.abs(theta1 - theta2) / np.abs(theta1 + theta2))

        ictr = 0
        while ictr < max_iterations:
            ictr += 1
            q = diag_cov * (ntot - nobs)
            p = ntot + diag_cov
            for i in range(ndims):
                # q[i] = diag_cov[i] * ( ntot[i] - nobs[i] )
                # p[i] = ntot[i] + diag_cov[i]
                for j in range(ndims):
                    if i == j:
                        continue
                    dq = thetamax[j] * ntot[i] * diag_cov[i] * weight[i, j]
                    dp = thetamax[j] * weight[i, j] * diag_cov[i]
                    if abs(dq / q[i]) > 0.3:
                        # logger.warning ( "too big a change in iteration." )
                        dq = np.abs(0.3 * q[i]) * np.sign(dq)
                    if abs(dp / p[i]) > 0.3:
                        # logger.warning ( "too big a change in iteration." )
                        dp = np.abs(0.3 * p[i]) * np.sign(dp)
                    q[i] += dq
                    p[i] += dp
                thetamax = -p / 2.0 * (1 - sign(p) * sqrt(1.0 - 4 * q / p**2))
            thetamaxes.append(thetamax)
            if len(thetamaxes) > 2:
                d1 = distance(thetamaxes[-2], thetamax)
                d2 = distance(thetamaxes[-3], thetamaxes[-2])
                if d1 > d2:
                    raise Exception("diverging when computing thetamax: %f > %f" % (d1, d2))
                if d1 < 1e-5:
                    return thetamax
        return thetamax

    def findThetaHat(self, nsig):
        """Compute nuisance parameter theta that maximizes our likelihood
        (poisson*gauss).
        """

        ## first step is to disregard the covariances and solve the
        ## quadratic equations
        ini = self.getThetaHat(
            self.model.observed, self.model.backgrounds, nsig, self.model.covariance, 0
        )
        self.cov_tot = self.model.V
        # if self.model.n == 1:
        #    self.cov_tot = self.model.totalCovariance ( nsig )
        # self.ntot = self.model.backgrounds + self.nsig
        # if not self.model.isLinear():
        # self.cov_tot = self.model.V + self.model.var_s(nsig)
        # self.cov_tot = self.model.totalCovariance (nsig)
        # self.ntot = None
        self.weight = np.linalg.inv(self.cov_tot)
        # self.coeff = 1.
        logdet = np.linalg.slogdet(self.cov_tot)
        self.logcoeff = -self.model.n / 2 * np.log(2 * np.pi) - 0.5 * logdet[1]
        # self.coeff = (2*np.pi)**(-self.model.n/2) * np.exp(-.5* logdet[1] )
        # print ( "coeff", self.coeff, "n", self.model.n, "det", np.linalg.slogdet ( self.cov_tot ) )
        # print ( "cov_tot", self.cov_tot[:10] )
        self.ones = 1.0
        if type(self.model.observed) in [list, ndarray]:
            self.ones = np.ones(len(self.model.observed))
        self.gammaln = special.gammaln(self.model.observed + 1)
        try:
            ret_c = optimize.fmin_ncg(
                self.nllOfNuisances,
                ini,
                fprime=self.dNLLdTheta,
                fhess=self.d2NLLdTheta2,
                full_output=True,
                disp=0,
            )
            # then always continue with TNC
            if type(self.model.observed) in [int, float]:
                bounds = [(-10 * self.model.observed, 10 * self.model.observed)]
            else:
                bounds = [(-10 * x, 10 * x) for x in self.model.observed]
            ini = ret_c
            ret_c = optimize.fmin_tnc(
                self.nllOfNuisances, ret_c[0], fprime=self.dNLLdTheta, disp=0, bounds=bounds
            )
            # print ( "[findThetaHat] mu=%s bg=%s observed=%s V=%s, nsig=%s theta=%s, nll=%s" % ( self.nsig[0]/self.model.efficiencies[0], self.model.backgrounds, self.model.observed,self.model.covariance, self.nsig, ret_c[0], self.nllOfNuisances(ret_c[0]) ) )
            if ret_c[-1] not in [0, 1, 2]:
                return ret_c[0], ret_c[-1]
            else:
                return ret_c[0], 0
                logger.debug("tnc worked.")

            ret = ret_c[0]
            return ret, -2
        except (IndexError, ValueError) as e:
            logger.error("exception: %s. ini[-3:]=%s" % (e, ini[-3:]))
            raise Exception("cov-1=%s" % (self.model.covariance + self.model.var_s(nsig)) ** (-1))
        return ini, -1

    def marginalizedLLHD1D(self, nsig, nll):
        """
        Return the likelihood (of 1 signal region) to observe nobs events given the
        predicted background nb, error on this background (deltab),
        expected number of signal events nsig and the relative error on the signal (deltas_rel).

        :param nsig: predicted signal (float)
        :param nobs: number of observed events (float)
        :param nb: predicted background (float)
        :param deltab: uncertainty on background (float)

        :return: likelihood to observe nobs events (float)

        """
        self.sigma2 = self.model.covariance + self.model.var_s(nsig)  ## (self.model.deltas)**2
        self.sigma_tot = sqrt(self.sigma2)
        self.lngamma = math.lgamma(self.model.observed[0] + 1)
        #     Why not a simple gamma function for the factorial:
        #     -----------------------------------------------------
        #     The scipy.stats.poisson.pmf probability mass function
        #     for the Poisson distribution only works for discrete
        #     numbers. The gamma distribution is used to create a
        #     continuous Poisson distribution.
        #
        #     Why not a simple gamma function for the factorial:
        #     -----------------------------------------------------
        #     The gamma function does not yield results for integers
        #     larger than 170. Since the expression for the Poisson
        #     probability mass function as a whole should not be huge,
        #     the exponent of the log of this expression is calculated
        #     instead to avoid using large numbers.

        # Define integrand (gaussian_(bg+signal)*poisson(nobs)):
        def prob(x, nsig):
            poisson = exp(self.model.observed * log(x) - x - self.lngamma)
            gaussian = stats.norm.pdf(x, loc=self.model.backgrounds + nsig, scale=self.sigma_tot)

            return poisson * gaussian

        # Compute maximum value for the integrand:
        xm = self.model.backgrounds + nsig - self.sigma2
        # If nb + nsig = sigma2, shift the values slightly:
        if xm == 0.0:
            xm = 0.001
        xmax = (
            xm
            * (1.0 + sign(xm) * sqrt(1.0 + 4.0 * self.model.observed * self.sigma2 / xm**2))
            / 2.0
        )

        # Define initial integration range:
        nrange = 5.0
        a = max(0.0, xmax - nrange * sqrt(self.sigma2))
        b = xmax + nrange * self.sigma_tot
        like = integrate.quad(prob, a, b, (nsig), epsabs=0.0, epsrel=1e-3)[0]
        if like == 0.0:
            return 0.0

        # Increase integration range until integral converges
        err = 1.0
        ctr = 0
        while err > 0.01:
            ctr += 1
            if ctr > 10.0:
                raise Exception("Could not compute likelihood within required precision")

            like_old = like
            nrange = nrange * 2
            a = max(0.0, (xmax - nrange * self.sigma_tot)[0][0])
            b = (xmax + nrange * self.sigma_tot)[0][0]
            like = integrate.quad(prob, a, b, (nsig), epsabs=0.0, epsrel=1e-3)[0]
            if like == 0.0:
                continue
            err = abs(like_old - like) / like

        # Renormalize the likelihood to account for the cut at x = 0.
        # The integral of the gaussian from 0 to infinity gives:
        # (1/2)*(1 + Erf(mu/sqrt(2*sigma2))), so we need to divide by it
        # (for mu - sigma >> 0, the normalization gives 1.)
        norm = (1.0 / 2.0) * (
            1.0 + special.erf((self.model.backgrounds + nsig) / sqrt(2.0 * self.sigma2))
        )
        like = like / norm

        if nll:
            like = -log(like)

        return like[0][0]

    def marginalizedLikelihood(self, nsig, nll):
        """compute the marginalized likelihood of observing nsig signal event"""
        if (
            self.model.isLinear() and self.model.n == 1
        ):  ## 1-dimensional non-skewed llhds we can integrate analytically
            return self.marginalizedLLHD1D(nsig, nll)

        vals = []
        self.gammaln = special.gammaln(self.model.observed + 1)
        thetas = stats.multivariate_normal.rvs(
            mean=[0.0] * self.model.n,
            # cov=(self.model.totalCovariance(nsig)),
            cov=self.model.V,
            size=self.toys,
        )  ## get ntoys values
        for theta in thetas:
            if self.model.isLinear():
                lmbda = nsig + self.model.backgrounds + theta
            else:
                lmbda = nsig + self.model.A + theta + self.model.C * theta**2 / self.model.B**2
            if self.model.isScalar(lmbda):
                lmbda = array([lmbda])
            for ctr, v in enumerate(lmbda):
                if v <= 0.0:
                    lmbda[ctr] = 1e-30
                # print ( "lmbda=",lmbda )
            poisson = self.model.observed * np.log(lmbda) - lmbda - self.gammaln
            # poisson = np.exp(self.model.observed*np.log(lmbda) - lmbda - self.model.backgrounds - self.gammaln)
            vals.append(np.exp(sum(poisson)))
            # vals.append ( reduce(lambda x, y: x*y, poisson) )
        mean = np.mean(vals)
        if nll:
            if mean == 0.0:
                mean = 1e-100
            mean = -log(mean)
        return mean

    def profileLikelihood(self, nsig, nll):
        """compute the profiled likelihood for nsig.
        Warning: not normalized.
        Returns profile likelihood and error code (0=no error)
        """
        # compute the profiled (not normalized) likelihood of observing
        # nsig signal events
        theta_hat, _ = self.findThetaHat(nsig)
        if self.debug_mode:
            self.theta_hat = theta_hat
        ret = self.probMV(nll, theta_hat)

        return ret

    def likelihood(self, nsig, marginalize=False, nll=False):
        """compute likelihood for nsig, profiling the nuisances
        :param marginalize: if true, marginalize, if false, profile
        :param nll: return nll instead of likelihood
        """
        nsig = self.model.convert(nsig)
        self.ntot = self.model.backgrounds + nsig
        if marginalize:
            # p,err = self.profileLikelihood ( nsig, deltas )
            return self.marginalizedLikelihood(nsig, nll)
            # print ( "p,l=",p,l,p/l )
        else:
            return self.profileLikelihood(nsig, nll)

    def lmax(self, nsig=None, marginalize=False, nll=False, allowNegativeSignals=False):
        """convenience function, computes likelihood for nsig = nobs-nbg,
        :param marginalize: if true, marginalize, if false, profile nuisances.
        :param nsig: number of signal events, needed only for combinations
                     if None, then it gets replaced with obsN - expBG
        :param nll: return nll instead of likelihood
        :param allowNegativeSignals: if False, then negative nsigs are replaced with 0.
        """
        if type(nsig) == type(None):
            nsig = self.model.observed - self.model.backgrounds
        if len(self.model.observed) == 1:
            if not allowNegativeSignals and nsig[0] < 0.0:
                nsig = [0.0]
            self.muhat = float(nsig[0])
            if abs(self.model.nsignal) > 1e-100:
                self.muhat = float(nsig[0] / self.model.nsignal[0])
            self.sigma_mu = np.sqrt(self.model.observed[0] + self.model.covariance[0][0])
            return self.likelihood(nsig, marginalize=marginalize, nll=nll)
        fmh = self.findMuHat(
            nsig, allowNegativeSignals=allowNegativeSignals, extended_output=True, nll=nll
        )
        muhat_, sigma_mu, lmax = fmh["muhat"], fmh["sigma_mu"], fmh["lmax"]
        self.muhat = muhat_
        self.sigma_mu = sigma_mu
        return self.likelihood(muhat * nsig, marginalize=marginalize, nll=nll)

    def findMuHatViaGradient(
        self,
        signal_rel,
        allowNegativeSignals=False,
        extended_output=False,
        nll=False,
        marginalize=False,
    ):
        """currently not used but finding muhat via gradient descent"""

        def myllhd(mu):
            return self.likelihood(mu * signal_rel, nll=True, marginalize=marginalize)

        import scipy.optimize

        bounds = None
        if not allowNegativeSignals:
            bounds = [(0, 1000.0)]
        o = scipy.optimize.minimize(myllhd, 0.0, bounds=bounds)
        llhd = o.fun
        if not nll:
            llhd = np.exp(-o.fun)
        hess = o.hess_inv
        try:
            hess = hess.todense()
        except Exception as e:
            pass
        mu_hat = float(o.x[0])
        if extended_output:
            sigma_mu = float(np.sqrt(hess[0][0]))
            return mu_hat, sigma_mu, llhd
        return mu_hat

    def chi2(self, nsig, marginalize=False):
        """
        Computes the chi2 for a given number of observed events nobs given
        the predicted background nb, error on this background deltab,
        expected number of signal events nsig and the relative error on
        signal (deltas_rel).
        :param marginalize: if true, marginalize, if false, profile
        :param nsig: number of signal events
        :return: chi2 (float)

        """
        nsig = self.model.convert(nsig)

        # Compute the likelhood for the null hypothesis (signal hypothesis) H0:
        llhd = self.likelihood(nsig, marginalize=marginalize, nll=True)

        # Compute the maximum likelihood H1, which sits at nsig = nobs - nb
        # (keeping the same % error on signal):
        if len(nsig) == 1:
            nsig = self.model.observed - self.model.backgrounds
            maxllhd = self.likelihood(nsig, marginalize=marginalize, nll=True)
        else:
            maxllhd = self.lmax(nsig, marginalize=marginalize, nll=True, allowNegativeSignals=False)
        chi2 = 2 * (llhd - maxllhd)

        if not np.isfinite(chi2):
            logger.error("chi2 is not a finite number! %s,%s,%s" % (chi2, llhd, maxllhd))
        # Return the test statistic -2log(H0/H1)
        return chi2


class UpperLimitComputer:
    debug_mode = False

    def __init__(self, ntoys=30000, cl=0.95):

        """
        :param ntoys: number of toys when marginalizing
        :param cl: desired quantile for limits
        """
        self.toys = ntoys
        self.cl = cl

    def ulOnSigmaTimesEff(
        self, model, marginalize=False, toys=None, expected=False, trylasttime=False
    ):
        """upper limit on the fiducial cross section sigma times efficiency,
            obtained from the defined
            Data (using the signal prediction
            for each signal regio/dataset), by using
            the q_mu test statistic from the CCGV paper (arXiv:1007.1727).

        :params marginalize: if true, marginalize nuisances, else profile them
        :params toys: specify number of toys. Use default is none
        :params expected: if false, compute observed,
                          true: compute a priori expected, "posteriori":
                          compute a posteriori expected
        :params trylasttime: if True, then dont try extra
        :returns: upper limit on fiducial cross section
        """
        ul = self.ulOnYields(
            model, marginalize=marginalize, toys=toys, expected=expected, trylasttime=trylasttime
        )
        if ul == None:
            return ul
        if model.lumi is None:
            logger.error(f"asked for upper limit on fiducial xsec, but no lumi given with the data")
            return ul
        return ul / model.lumi

    def _ul_preprocess(
        self,
        model: Data,
        marginalize: Optional[bool] = False,
        toys: Optional[float] = None,
        expected: Optional[Union[bool, Text]] = False,
        trylasttime: Optional[bool] = False,
        signal_type: Optional[Text] = "signal_rel",
    ) -> Tuple:
        """
        Process the upper limit calculator
        :param model: statistical model
        :param marginalize: if true, marginalize nuisances, else profile them
        :param toys: specify number of toys. Use default is none
        :param expected: if false, compute observed,
                          true: compute a priori expected, "posteriori":
                          compute a posteriori expected
        :param trylasttime: if True, then dont try extra
        :param signal_type: signal_type will allow both SModelS and MadAnalysis interface
                            to use this function simultaneously. For signal_rel upper limit
                            is calculated for normalised signal events for nsignals upper limit
                            is calculated for number of signal events.
        :return: mu_hat, sigma_mu, root_func
        """
        assert signal_type in [
            "signal_rel",
            "nsignal",
        ], f"Signal type can only be `signal_rel` or `nsignal`. `{signal_type}` is given"
        # if expected:
        #    marginalize = True
        if model.zeroSignal():
            """only zeroes in efficiencies? cannot give a limit!"""
            return None, None, None
        if toys == None:
            toys = self.toys
        oldmodel = model
        if expected:
            model = copy.deepcopy(oldmodel)
            if expected == "posteriori":
                # print ( "here!" )
                tempc = LikelihoodComputer(oldmodel, toys)
                theta_hat_, _ = tempc.findThetaHat(0 * getattr(oldmodel, signal_type))
            # model.observed = model.backgrounds
            for i, d in enumerate(model.backgrounds):
                # model.observed[i]=int(np.ceil(d))
                # model.observed[i]=int(np.round(d))
                if expected == "posteriori":
                    d += theta_hat_[i]
                model.observed[i] = float(d)
        computer = LikelihoodComputer(model, toys)
        mu_hat = computer.findMuHat(
            getattr(model, signal_type), allowNegativeSignals=False, extended_output=False
        )
        theta_hat0, _ = computer.findThetaHat(0 * getattr(model, signal_type))
        sigma_mu = computer.getSigmaMu(mu_hat, getattr(model, signal_type), theta_hat0)

        nll0 = computer.likelihood(
            getattr(model, "signals" if signal_type == "signal_rel" else "nsignals")(mu_hat),
            marginalize=marginalize,
            nll=True,
        )
        # print ( f"SL nll0 {nll0:.3f} muhat {mu_hat:.3f} sigma_mu {sigma_mu:.3f}" )
        if np.isinf(nll0) and marginalize == False and not trylasttime:
            logger.warning(
                "nll is infinite in profiling! we switch to marginalization, but only for this one!"
            )
            marginalize = True
            nll0 = computer.likelihood(
                getattr(model, "signals" if signal_type == "signal_rel" else "nsignals")(mu_hat),
                marginalize=True,
                nll=True,
            )
            if np.isinf(nll0):
                logger.warning("marginalization didnt help either. switch back.")
                marginalize = False
            else:
                logger.warning("marginalization worked.")
        aModel = copy.deepcopy(model)
        aModel.observed = array([x + y for x, y in zip(model.backgrounds, theta_hat0)])
        aModel.name = aModel.name + "A"
        # print ( f"SL finding mu hat with {aModel.signal_rel}: mu_hatA, obs: {aModel.observed}" )
        compA = LikelihoodComputer(aModel, toys)
        ## compute
        mu_hatA = compA.findMuHat(getattr(aModel, signal_type))
        nll0A = compA.likelihood(
            getattr(aModel, "signals" if signal_type == "signal_rel" else "nsignals")(mu_hatA),
            marginalize=marginalize,
            nll=True,
        )
        # print ( f"SL nll0A {nll0A:.3f} mu_hatA {mu_hatA:.3f} bg {aModel.backgrounds[0]:.3f} obs {aModel.observed[0]:.3f}" )
        # return 1.

        def root_func(mu: float, get_cls: bool = False) -> float:
            """
            Calculate the root
            :param mu: float POI
            :param get_cls: if true returns 1-CLs value
            """
            ## the function to find the zero of (ie CLs - alpha)
            nsig = getattr(model, "signals" if signal_type == "signal_rel" else "nsignals")(mu)
            computer.ntot = model.backgrounds + nsig
            nll = computer.likelihood(nsig, marginalize=marginalize, nll=True)
            nllA = compA.likelihood(nsig, marginalize=marginalize, nll=True)
            return rootFromNLLs(nllA, nll0A, nll, nll0, get_cls=get_cls)

        return mu_hat, sigma_mu, root_func

    def ulOnYields(self, model, marginalize=False, toys=None, expected=False, trylasttime=False):
        """upper limit on signal yields obtained from the defined
            Data (using the signal prediction
            for each signal regio/dataset), by using
            the q_mu test statistic from the CCGV paper (arXiv:1007.1727).

        :params marginalize: if true, marginalize nuisances, else profile them
        :params toys: specify number of toys. Use default is none
        :params expected: if false, compute observed,
                          true: compute a priori expected, "posteriori":
                          compute a posteriori expected
        :params trylasttime: if True, then dont try extra
        :returns: upper limit on yields
        """
        mu_hat, sigma_mu, root_func = self._ul_preprocess(
            model, marginalize, toys, expected, trylasttime
        )
        if mu_hat == None:
            return None
        a, b = determineBrentBracket(mu_hat, sigma_mu, root_func)
        mu_lim = optimize.brentq(root_func, a, b, rtol=1e-03, xtol=1e-06)
        return mu_lim

    def computeCLs(self, model, marginalize=False, toys=None, expected=False, trylasttime=False):
        """
        Compute the confidence level of the model
        :param model: statistical model
        :param marginalize: if true, marginalize nuisances, else profile them
        :param toys: specify number of toys. Use default is none
        :param expected: if false, compute observed,
                          true: compute a priori expected, "posteriori":
                          compute a posteriori expected
        :param trylasttime: if True, then dont try extra
        :return: 1 - CLs value
        """
        _, _, root_func = self._ul_preprocess(
            model, marginalize, toys, expected, trylasttime, signal_type="nsignal"
        )

        # 1-(CLs+alpha) -> alpha = 0.05
        return root_func(1.0, get_cls=True)


if __name__ == "__main__":
    C = [
        18774.2,
        -2866.97,
        -5807.3,
        -4460.52,
        -2777.25,
        -1572.97,
        -846.653,
        -442.531,
        -2866.97,
        496.273,
        900.195,
        667.591,
        403.92,
        222.614,
        116.779,
        59.5958,
        -5807.3,
        900.195,
        1799.56,
        1376.77,
        854.448,
        482.435,
        258.92,
        134.975,
        -4460.52,
        667.591,
        1376.77,
        1063.03,
        664.527,
        377.714,
        203.967,
        106.926,
        -2777.25,
        403.92,
        854.448,
        664.527,
        417.837,
        238.76,
        129.55,
        68.2075,
        -1572.97,
        222.614,
        482.435,
        377.714,
        238.76,
        137.151,
        74.7665,
        39.5247,
        -846.653,
        116.779,
        258.92,
        203.967,
        129.55,
        74.7665,
        40.9423,
        21.7285,
        -442.531,
        59.5958,
        134.975,
        106.926,
        68.2075,
        39.5247,
        21.7285,
        11.5732,
    ]
    nsignal = [x / 100.0 for x in [47, 29.4, 21.1, 14.3, 9.4, 7.1, 4.7, 4.3]]
    m = Data(
        observed=[1964, 877, 354, 182, 82, 36, 15, 11],
        backgrounds=[2006.4, 836.4, 350.0, 147.1, 62.0, 26.2, 11.1, 4.7],
        covariance=C,
        # third_moment = [ 0.1, 0.02, 0.1, 0.1, 0.003, 0.0001, 0.0002, 0.0005 ],
        third_moment=[0.0] * 8,
        nsignal=nsignal,
        name="CMS-NOTE-2017-001 model",
    )
    ulComp = UpperLimitComputer(ntoys=500, cl=0.95)
    # uls = ulComp.ulSigma ( Data ( 15,17.5,3.2,0.00454755 ) )
    # print ( "uls=", uls )
    ul_old = 131.828 * sum(
        nsignal
    )  # With respect to the older refernece value one must normalize the xsec
    print("old ul=", ul_old)
    ul = ulComp.ulOnYields(m, marginalize=True)
    cls = ulComp.computeCLs(m, marginalize=True)
    print("ul (marginalized)", ul)
    print("CLs (marginalized)", cls)
    ul = ulComp.ulOnYields(m, marginalize=False)
    cls = ulComp.computeCLs(m, marginalize=False)
    print("ul (profiled)", ul)
    print("CLs (profiled)", cls)

    """
    results:
    old ul= 180.999844
    ul (marginalized) 184.8081186162269
    CLs (marginalized) 1.0
    ul (profiled) 180.68039063387553
    CLs (profiled) 0.75
    """
