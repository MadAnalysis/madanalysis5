#!/usr/bin/env python3

"""
.. module:: simplified_likelihood
   :synopsis: Code that implements the simplified likelihoods as presented
              in CMS-NOTE-2017-001, see https://cds.cern.ch/record/2242860.
              In collaboration with Andy Buckley, Sylvain Fichet and Nickolas Wardle.
              Additional developments will be presented in a future publication.
             

.. moduleauthor:: Wolfgang Waltenberger <wolfgang.waltenberger@gmail.com>

"""

from __future__ import print_function
from scipy import stats, optimize, integrate, special
from numpy  import sqrt, exp, log, sign, array, ndarray
from functools import reduce
import numpy as NP
import math
import copy

def getLogger():
    """
    Configure the logging facility. Maybe adapted to fit into
    your framework.
    """
    
    import logging
    
    logger = logging.getLogger("SL")
    formatter = logging.Formatter('%(module)s - %(levelname)s: %(message)s')
    ch = logging.StreamHandler()
    ch.setFormatter(formatter)
    ch.setLevel(logging.DEBUG)
    logger.addHandler(ch)
    return logger

logger=getLogger()


class Data:
    """ A very simple observed container to collect all the data
        needed to fully define a specific statistical model """

    def __init__(self, observed, backgrounds, covariance, third_moment=None,
                         nsignal=None, name="model", deltas_rel = 0.2):
        """
        :param observed: number of observed events per dataset
        :param backgrounds: expected bg per dataset
        :param covariance: uncertainty in background, as a covariance matrix
        :param nsignal: number of signal events in each dataset
        :param name: give the model a name, just for convenience
        :param deltas_rel: the assumed relative error on the signal hypotheses. 
                           The default is 20%.
        """
        self.observed = NP.around(self.convert(observed)) #Make sure observed number of events are integers
        self.backgrounds = self.convert(backgrounds)
        self.n = len(self.observed)
        self.covariance = self._convertCov(covariance)
        self.nsignal = self.convert(nsignal)
        if self.nsignal is None:
            self.signal_rel = self.convert(1.)        
        elif self.nsignal.sum():
            self.signal_rel = self.nsignal/self.nsignal.sum()            
        else: 
            self.signal_rel = array([0.]*len(self.nsignal))
            
        self.third_moment = self.convert(third_moment)
        if type(self.third_moment) != type(None) and NP.sum([ abs(x) for x in self.third_moment ]) < 1e-10:
            self.third_moment = None
        self.name = name
        self.deltas_rel = deltas_rel
        self._computeABC()

    def totalCovariance ( self, nsig ):
        """ get the total covariance matrix, taking into account
        also signal uncertainty for the signal hypothesis <nsig>. 
        If nsig is None, the predefined signal hypothesis is taken.
        """
        if self.isLinear():
            cov_tot = self.V + self.var_s( nsig )
        else:
            cov_tot = self.covariance+ self.var_s(nsig)
        return cov_tot


    def zeroSignal(self):
        """
        Is the total number of signal events zero?
        """
        
        return len(self.nsignal[self.nsignal>0.]) == 0

    def var_s(self,nsig=None):
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
        return NP.diag((nsig*self.deltas_rel)**2)

    def isScalar(self, obj):
        """
        Determine if obj is a scalar (float or int)
        """
        
        if type(obj) == ndarray:
            ## need to treat separately since casting array([0.]) to float works
            return False
        try:
            _ = float(obj)
            return True
        except:
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
            return array ( [ [ obj ] ] )
        if type(obj[0]) == list:
            return array ( obj )
        if type(obj[0]) == float:
            ## if the matrix is flattened, unflatten it.
            return array([ obj[self.n*i:self.n*(i+1)] for i in range(self.n)])
        
        return obj

    def _computeABC( self ):
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
        C=[]
        for m2,m3 in zip(covD, self.third_moment):
            if m3 == 0.:
                m3 = 1e-30
            k = -NP.sign(m3)*sqrt(2.*m2 )
            dm = sqrt ( 8.*m2**3/m3**2 - 1. )
            C.append( k*NP.cos ( 4.*NP.pi/3. + NP.arctan(dm) / 3. ))
            
        self.C=NP.array(C) ## C, as define in Eq. 1.27 (?) in the second paper
        self.B = sqrt( covD - 2*self.C**2 ) ## B, as defined in Eq. 1.28(?)
        self.A = self.backgrounds - self.C ## A, Eq. 1.30(?)
        self.rho = NP.array( [ [0.]*self.n ]*self.n ) ## Eq. 1.29 (?)
        for x in range(self.n):
            for y in range(x,self.n):
                bxby=self.B[x]*self.B[y]
                cxcy=self.C[x]*self.C[y]
                e=(4.*cxcy)**(-1)*(sqrt( bxby**2+8*cxcy*self.covariance[x][y])-bxby)
                self.rho[x][y]=e
                self.rho[y][x]=e

        self.sandwich()
        # self.V = sandwich ( self.B, self.rho )

    def sandwich( self ):
        """
        Sandwich product
        """
        
        ret = NP.array ( [ [0.]*len(self.B) ]*len(self.B) )
        for x in range(len(self.B)):
            for y in range(x,len(self.B)):
                T=self.B[x]*self.B[y]*self.rho[x][y]
                ret[x][y]=T
                ret[y][x]=T
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
        
        return NP.diag( self.covariance )

    def correlations(self):
        """
        Correlation matrix, computed from covariance matrix.
        Convenience function. 
        """
        
        if hasattr(self, "corr"):
            return self.corr
        
        self.corr = copy.deepcopy(self.covariance)
        for x in range(self.n):
            self.corr[x][x]=1.
            for y in range(x+1,self.n):
                rho=self.corr[x][y]/sqrt(self.covariance[x][x]*self.covariance[y][y])
                self.corr[x][y]=rho
                self.corr[y][x]=rho
        return self.corr

    def signals(self, mu):
        """
        Returns the number of expected signal events, for all datasets,
        given total signal strength mu.
        
        :param mu: Total number of signal events summed over all datasets.
        """
        
        return (mu*self.signal_rel)

class LikelihoodComputer:

    debug_mode = False

    def __init__(self, data, ntoys = 10000 ):
        """
        :param data: a Data object.
        :param ntoys: number of toys when marginalizing
        """
        
        self.model = data
        self.ntoys = ntoys

    def dLdMu(self, mu, signal_rel, theta_hat):
        """
        d (ln L)/d mu, if L is the likelihood. The function
        whose root gives us muhat, i.e. the mu that maximizes
        the likelihood.
        
        :param mu: total number of signal events
        :param signal_rel: array with the relative signal strengths for each dataset (signal region)
        :param theta_hat: array with nuisance parameters
        
        """
        
        #Define relative signal strengths:
        denominator = mu*signal_rel  + self.model.backgrounds + theta_hat

        for ctr,d in enumerate(denominator):
            if d == 0.:
                if (self.model.observed[ctr]*signal_rel[ctr]) == 0.:
                #    logger.debug("zero denominator, but numerator also zero, so we set denom to 1.")
                    denominator[ctr]=1.
                else:
                    raise Exception("we have a zero value in the denominator at pos %d, with a non-zero numerator. dont know how to handle." % ctr)
        ret = self.model.observed*signal_rel/denominator - signal_rel
        
        if type(ret) in [ array, ndarray, list ]:
            ret = sum(ret)
        return ret

    def findMuHat(self, signal_rel):
        """
        Find the most likely signal strength mu
        given the relative signal strengths in each dataset (signal region).
        
        :param signal_rel: array with relative signal strengths 
        
        :returns: mu_hat, the total signal yield.
        """
        
        if (self.model.backgrounds == self.model.observed).all():
            return 0.
        
        if type(signal_rel) in [list, ndarray]:
            signal_rel = array(signal_rel)
            
        signal_rel[signal_rel==0.] = 1e-20
        if sum(signal_rel<0.):
            raise Exception("Negative relative signal strengths!")

        ## we need a very rough initial guess for mu(hat), to come
        ## up with a first theta
        self.nsig = array([0.]*len(self.model.observed))
        ## we start with theta_hat being all zeroes
        theta_hat = array([0.]*len(self.model.observed))
        mu_hat_old, mu_hat = 0., 1.
        ctr=0
        widener=3.
        while abs(mu_hat - mu_hat_old)>1e-10 and abs(mu_hat - mu_hat_old )/(mu_hat+mu_hat_old) > .5e-2 and ctr < 20:
            ctr+=1
            mu_hat_old = mu_hat
            #logger.info ( "theta hat[%d]=%s" % (ctr,list( theta_hat[:11] ) ) )
            #logger.info ( "   mu hat[%d]=%s" % (ctr, mu_hat ) )
            mu_c = NP.abs(self.model.observed - self.model.backgrounds - theta_hat)/signal_rel
            ## find mu_hat by finding the root of 1/L dL/dmu. We know
            ## that the zero has to be between min(mu_c) and max(mu_c).
            lower,upper = 0.,widener*max(mu_c)
            lower_v = self.dLdMu(lower, signal_rel, theta_hat)
            upper_v = self.dLdMu(upper, signal_rel, theta_hat)
            total_sign = NP.sign(lower_v * upper_v)
            if total_sign > -.5:
                if upper_v < lower_v < 0.:
                    ## seems like we really want to go for mu_hat = 0.
                    return 0.
                logger.debug ( "weird. cant find a zero in the Brent bracket "\
                               "for finding mu(hat). Let me try with a very small"
                               " value." )
                lower = 1e-4*max(mu_c)
                lower_v = self.dLdMu( lower, signal_rel, theta_hat )
                total_sign = NP.sign( lower_v * upper_v )
                if total_sign > -.5:
                    logger.debug ( "cant find zero in Brentq bracket. l,u,ctr=%s,%s,%s" % \
                                  ( lower, upper, ctr ) )
                    widener=widener*1.5
                    continue
            mu_hat = optimize.brentq ( self.dLdMu, lower, upper, args=(signal_rel, theta_hat ) )
            theta_hat,_ = self.findThetaHat( mu_hat*signal_rel)
            ctr+=1

        return mu_hat

    def getSigmaMu(self, signal_rel):
        """
        Get a rough estimate for the variance of mu around mu_max.
        
        :param signal_rel: array with relative signal strengths in each dataset (signal region)        
        """
        if type(signal_rel) in [ list, ndarray ]:
            s_effs = sum(signal_rel)
            
        sgm_mu = sqrt(sum(self.model.observed) + sum(NP.diag(self.model.covariance)))/s_effs
        
        return sgm_mu

    #Define integrand (gaussian_(bg+signal)*poisson(nobs)):
    # def prob(x0, x1 )
    
    def probMV(self, nll, theta ):
        """ probability, for nuicance parameters theta
        :params nll: if True, compute negative log likelihood """
        # theta = array ( thetaA )
        # ntot = self.model.backgrounds + self.nsig
        # lmbda = theta + self.ntot ## the lambda for the Poissonian
        if self.model.isLinear():
            lmbda = self.model.backgrounds + self.nsig + theta
        else:
            lmbda = self.nsig + self.model.A + theta + self.model.C * theta**2 / self.model.B**2
        lmbda[lmbda<=0.] = 1e-30 ## turn zeroes to small values
        if nll:
            #poisson = self.model.observed * log ( lmbda ) - lmbda #- self.gammaln
            poisson = stats.poisson.logpmf( self.model.observed, lmbda )
            #print ( "p",poisson,poisson2 )
        else:
            poisson = stats.poisson.pmf( self.model.observed, lmbda )
            #print ( "nonll",poisson )
        try:
            M = [0.]*len(theta)
            C = self.model.V
            if self.model.n == 1:
                C = self.model.totalCovariance(self.nsig)
            if nll:
                gaussian = stats.multivariate_normal.logpdf(theta,mean=M,cov=C)
                ret = - gaussian - sum(poisson)
            else:
                gaussian = stats.multivariate_normal.pdf(theta,mean=M,cov=C)
                ret = gaussian * ( reduce(lambda x, y: x*y, poisson) )
            return ret
        except ValueError as e:
            raise Exception("ValueError %s, %s" % ( e, self.model.totalCovariance(self.nsig) ))
            # raise Exception("ValueError %s, %s" % ( e, self.model.V ))

    def nll( self, theta ):
        """ probability, for nuicance parameters theta,
        as a negative log likelihood. """
        return self.probMV(True,theta)

    def nllprime( self, theta ):
        """ the derivative of nll as a function of the thetas.
        Makes it easier to find the maximum likelihood. """
        if self.model.isLinear():
            xtot = theta + self.model.backgrounds + self.nsig
            xtot[xtot<=0.] = 1e-30 ## turn zeroes to small values
            nllp_ = self.ones - self.model.observed / xtot + NP.dot( theta , self.weight )
            return nllp_
        lmbda = self.nsig + self.model.A + theta + self.model.C * theta**2 / self.model.B**2
        lmbda[lmbda<=0.] = 1e-30 ## turn zeroes to small values
        # nllp_ = ( self.ones - self.model.observed / lmbda + NP.dot( theta , self.weight ) ) * ( self.ones + 2*self.model.C * theta / self.model.B**2 )
        T=self.ones + 2*self.model.C/self.model.B**2*theta
        nllp_ = T - self.model.observed / lmbda * ( T ) + NP.dot( theta , self.weight )
        return nllp_

    def nllHess( self, theta ):
        """ the Hessian of nll as a function of the thetas.
        Makes it easier to find the maximum likelihood. """
        # xtot = theta + self.ntot
        if self.model.isLinear():
            xtot = theta + self.model.backgrounds + self.nsig
            xtot[xtot<=0.] = 1e-30 ## turn zeroes to small values
            nllh_ = self.weight + NP.diag ( self.model.observed / (xtot**2) )
            return nllh_
        lmbda = self.nsig + self.model.A + theta + self.model.C * theta**2 / self.model.B**2
        lmbda[lmbda<=0.] = 1e-30 ## turn zeroes to small values
        T=self.ones + 2*self.model.C/self.model.B**2*theta
        # T_i = 1_i + 2*c_i/B_i**2 * theta_i
        nllh_ = self.weight + NP.diag ( self.model.observed * T**2 / (lmbda**2) ) - NP.diag ( self.model.observed / lmbda * 2 * self.model.C / self.model.B**2 ) + NP.diag ( 2*self.model.C/self.model.B**2 )
        return nllh_

    def getThetaHat(self, nobs, nb, nsig, covb, max_iterations ):
            """ Compute nuisance parameter theta that
            maximizes our likelihood (poisson*gauss).  """
            self.nsig = nsig
            sigma2 = covb + self.model.var_s(nsig) ## NP.diag ( (self.model.deltas)**2 )
            ## for now deal with variances only
            ntot = nb + nsig
            cov = NP.array(sigma2)
            weight = cov**(-1) ## weight matrix
            diag_cov = NP.diag(cov)
            # first: no covariances:
            q = diag_cov * ( ntot - nobs )
            p = ntot + diag_cov
            thetamaxes = []
            thetamax = -p/2. * ( 1 - sign(p) * sqrt ( 1. - 4*q / p**2 ) )
            thetamaxes.append ( thetamax )
            ndims = len(p)
            def distance ( theta1, theta2 ):
                for ctr,i in enumerate ( theta1 ):
                    if i == 0.:
                        theta1[ctr]=1e-20
                for ctr,i in enumerate ( theta2 ):
                    if i == 0.:
                        theta2[ctr]=1e-20
                return sum ( NP.abs(theta1 - theta2) / NP.abs ( theta1+theta2 ) )

            ictr = 0
            while ictr < max_iterations:
                ictr += 1
                q = diag_cov * ( ntot - nobs )
                p = ntot + diag_cov
                for i in range(ndims):
                    #q[i] = diag_cov[i] * ( ntot[i] - nobs[i] )
                    #p[i] = ntot[i] + diag_cov[i]
                    for j in range(ndims):
                        if i==j: continue
                        dq = thetamax[j]*ntot[i]*diag_cov[i]*weight[i,j]
                        dp = thetamax[j]*weight[i,j]*diag_cov[i]
                        if abs ( dq / q[i] ) > .3:
                            #logger.warning ( "too big a change in iteration." )
                            dq=NP.abs( .3 * q[i] ) * NP.sign ( dq )
                        if abs ( dp / p[i] ) > .3:
                            #logger.warning ( "too big a change in iteration." )
                            dp=NP.abs( .3 * p[i] ) * NP.sign ( dp )
                        q[i] += dq
                        p[i] += dp
                    thetamax = -p/2. * ( 1 - sign(p) * sqrt ( 1. - 4*q / p**2 ) )
                thetamaxes.append ( thetamax )
                if len(thetamaxes)>2:
                    d1 = distance ( thetamaxes[-2], thetamax )
                    d2 = distance ( thetamaxes[-3], thetamaxes[-2] )
                    if d1 > d2:
                        raise Exception("diverging when computing thetamax: %f > %f" % ( d1, d2 ))
                    if d1 < 1e-5:
                        return thetamax
            return thetamax

    def findThetaHat(self, nsig):
            """ Compute nuisance parameter theta that maximizes our likelihood
                (poisson*gauss).
            """
            
            ## first step is to disregard the covariances and solve the
            ## quadratic equations
            ini = self.getThetaHat ( self.model.observed, self.model.backgrounds, nsig, self.model.covariance, 0 )
            self.cov_tot = self.model.V
            if self.model.n == 1:
                self.cov_tot = self.model.totalCovariance ( nsig )
            # self.ntot = self.model.backgrounds + self.nsig
            # if not self.model.isLinear():
                # self.cov_tot = self.model.V + self.model.var_s(nsig)
                # self.cov_tot = self.model.totalCovariance (nsig)
                #self.ntot = None
            self.weight = NP.linalg.inv(self.cov_tot)
            self.ones = 1.
            if type ( self.model.observed) in [ list, ndarray ]:
                self.ones = NP.ones ( len (self.model.observed) )
            self.gammaln = special.gammaln(self.model.observed + 1)
            try:
                ret_c = optimize.fmin_ncg ( self.nll, ini, fprime=self.nllprime,
                                       fhess=self.nllHess, full_output=True, disp=0 )
                # then always continue with TNC
                if type ( self.model.observed ) in [ int, float ]:
                    bounds = [ ( -10*self.model.observed, 10*self.model.observed ) ]
                else:
                    bounds = [ ( -10*x, 10*x ) for x in self.model.observed ]
                ini = ret_c
                ret_c = optimize.fmin_tnc ( self.nll, ret_c[0], fprime=self.nllprime,
                                            disp=0, bounds=bounds )
                # print ( "[findThetaHat] mu=%s bg=%s observed=%s V=%s, nsig=%s theta=%s, nll=%s" % ( self.nsig[0]/self.model.efficiencies[0], self.model.backgrounds, self.model.observed,self.model.covariance, self.nsig, ret_c[0], self.nll(ret_c[0]) ) )
                if ret_c[-1] not in [ 0, 1, 2 ]:
                    return ret_c[0],ret_c[-1]
                else:
                    return ret_c[0],0
                    logger.debug ( "tnc worked." )

                ret = ret_c[0]
                return ret,-2
            except Exception as e:
                logger.error("exception: %s. ini[-3:]=%s" % (e,ini[-3:]) )
                raise Exception("cov-1=%s" % (self.model.covariance+self.model.var_s(nsig))**(-1))
            return ini,-1

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
            self.sigma2 = self.model.covariance + self.model.var_s(nsig)## (self.model.deltas)**2
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


            #Define integrand (gaussian_(bg+signal)*poisson(nobs)):
            def prob( x, nsig ):
                poisson = exp(self.model.observed*log(x) - x - self.lngamma )
                gaussian = stats.norm.pdf(x,loc=self.model.backgrounds+nsig,scale=self.sigma_tot)

                return poisson*gaussian

            #Compute maximum value for the integrand:
            xm = self.model.backgrounds + nsig - self.sigma2
            #If nb + nsig = sigma2, shift the values slightly:
            if xm == 0.:
                xm = 0.001
            xmax = xm*(1.+sign(xm)*sqrt(1. + 4.*self.model.observed*self.sigma2/xm**2))/2.

            #Define initial integration range:
            nrange = 5.
            a = max(0.,xmax-nrange*sqrt(self.sigma2))
            b = xmax+nrange*self.sigma_tot
            like = integrate.quad(prob,a,b,(nsig), epsabs=0.,epsrel=1e-3)[0]
            if like==0.:
                return 0.

            #Increase integration range until integral converges
            err = 1.
            ctr=0
            while err > 0.01:
                ctr+=1
                if ctr > 10.:
                    raise Exception("Could not compute likelihood within required precision")
                    
                like_old = like
                nrange = nrange*2
                a = max(0.,(xmax-nrange*self.sigma_tot)[0][0] )
                b = (xmax+nrange*self.sigma_tot)[0][0]
                like = integrate.quad(prob,a,b,(nsig),
                                          epsabs=0.,epsrel=1e-3)[0]
                if like == 0.:
                    continue
                err = abs(like_old-like)/like

            #Renormalize the likelihood to account for the cut at x = 0.
            #The integral of the gaussian from 0 to infinity gives:
            #(1/2)*(1 + Erf(mu/sqrt(2*sigma2))), so we need to divide by it
            #(for mu - sigma >> 0, the normalization gives 1.)
            norm = (1./2.)*(1. + special.erf((self.model.backgrounds+nsig)/sqrt(2.*self.sigma2)))
            like = like/norm

            if nll:
                like = - log ( like )

            return like[0][0]

    def marginalizedLikelihood(self, nsig, nll ):
            """ compute the marginalized likelihood of observing nsig signal event"""
            if self.model.isLinear() and self.model.n == 1: ## 1-dimensional non-skewed llhds we can integrate analytically
                return self.marginalizedLLHD1D ( nsig, nll )

            vals=[]
            self.gammaln = special.gammaln(self.model.observed + 1)
            thetas = stats.multivariate_normal.rvs(mean=[0.]*self.model.n,
                          # cov=(self.model.totalCovariance(nsig)),
                          cov=self.model.V,
                          size=self.ntoys ) ## get ntoys values
            for theta in thetas :
                if self.model.isLinear():
                    lmbda = nsig + self.model.backgrounds + theta
                else:
                    lmbda = nsig + self.model.A + theta + self.model.C*theta**2/self.model.B**2
                if self.model.isScalar( lmbda ):
                    lmbda = array([lmbda])
                for ctr,v in enumerate( lmbda ):
                    if v<=0.: lmbda[ctr]=1e-30
#                    print ( "lmbda=",lmbda )
                poisson = self.model.observed*NP.log(lmbda) - lmbda - self.gammaln
                # poisson = NP.exp(self.model.observed*NP.log(lmbda) - lmbda - self.model.backgrounds - self.gammaln)
                vals.append( NP.exp ( sum(poisson) ) )
                #vals.append ( reduce(lambda x, y: x*y, poisson) )
            mean = NP.mean( vals )
            if nll:
                if mean == 0.:
                    mean = 1e-100
                mean = - log ( mean )
            return mean


    def profileLikelihood( self, nsig, nll ):
        """ compute the profiled likelihood for nsig.
            Warning: not normalized.
            Returns profile likelihood and error code (0=no error)
        """
        # compute the profiled (not normalized) likelihood of observing
        # nsig signal events
        theta_hat,_ = self.findThetaHat ( nsig )
        ret = self.probMV ( nll, theta_hat )

        return ret

    def likelihood(self, nsig, marginalize=False, nll=False ):
        """ compute likelihood for nsig, profiling the nuisances
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
            dn = self.model.observed-self.model.backgrounds
            maxllhd = self.likelihood(dn, marginalize=marginalize, nll=True )
            
            chi2=2*(llhd-maxllhd)
            
            if not NP.isfinite ( chi2 ):
                logger.error("chi2 is not a finite number! %s,%s,%s" % \
                               (chi2, llhd,maxllhd))
            # Return the test statistic -2log(H0/H1)
            return chi2

class CLsComputer:
    debug_mode = False

    def __init__(self, ntoys=10000, cl=.95):

        """
        :param ntoys: number of toys when marginalizing
        :param cl: desired quantile for limits
        """
        self.ntoys = ntoys
        self.cl = cl

    def ulSigma(self, model, marginalize=False, toys=None, expected=False ):
        """ upper limit obtained from the defined Data (using the signal prediction
            for each signal regio/dataset), by using
            the q_mu test statistic from the CCGV paper (arXiv:1007.1727).

        :params marginalize: if true, marginalize nuisances, else profile them
        :params toys: specify number of toys. Use default is none
        :params expected: compute the expected value, not the observed.
        :returns: upper limit on *production* xsec (efficiencies unfolded)
        """
        if model.zeroSignal():
            """ only zeroes in efficiencies? cannot give a limit! """
            return None
        if toys==None:
            toys=self.ntoys
        oldmodel = model
        if expected:
            model = copy.deepcopy(oldmodel)
            #model.observed = model.backgrounds
            for i,d in enumerate(model.backgrounds):
                model.observed[i]=int(NP.round(d))
        computer = LikelihoodComputer(model, toys)
        mu_hat = computer.findMuHat(model.signal_rel)
        theta_hat0,_ = computer.findThetaHat(0*model.signal_rel)
        sigma_mu = computer.getSigmaMu(model.signal_rel)

        aModel = copy.deepcopy(model)
        aModel.observed = array([NP.round(x+y) for x,y in zip(model.backgrounds,theta_hat0)])
        #print ( "aModeldata=", aModel.observed )
        #aModel.observed = array ( [ round(x) for x in model.backgrounds ] )
        aModel.name = aModel.name + "A"
        compA = LikelihoodComputer(aModel, toys)
        ## compute
        mu_hatA = compA.findMuHat(aModel.signal_rel)
        if mu_hat < 0.:
            mu_hat = 0.
        nll0 = computer.likelihood(model.signals(mu_hat),
                                     marginalize=marginalize,
                                     nll=True)
        if NP.isinf(nll0) and marginalize==False:
            logger.warning("nll is infinite in profiling! we switch to marginalization, but only for this one!" )
            marginalize=True
            nll0 = computer.likelihood(model.signals(mu_hat),
                                         marginalize=True,
                                         nll=True)
            if NP.isinf(nll0):
                logger.warning("marginalization didnt help either. switch back.")
                marginalize=False
            else:
                logger.warning("marginalization worked.")
        nll0A = compA.likelihood(aModel.signals(mu_hatA),
                                   marginalize=marginalize,
                                   nll=True)
        
        def root_func(mu):
            ## the function to minimize.
            nsig = model.signals(mu)
            computer.ntot = model.backgrounds + nsig
            nll = computer.likelihood(nsig, marginalize=marginalize, nll=True )
            nllA = compA.likelihood(nsig, marginalize=marginalize, nll=True )
            qmu =  2*( nll - nll0 )
            if qmu<0.: qmu=0.
            sqmu = sqrt (qmu)
            qA =  2*( nllA - nll0A )
            # print ( "mu: %s, qMu: %s, qA: %s nll0A: %s nllA: %s" % ( mu, qmu, qA, nll0A, nllA ) )
            if qA<0.:
                qA=0.
            sqA = sqrt(qA)
            CLsb = 1. - stats.multivariate_normal.cdf(sqmu)
            CLb = 0.
            if qA >= qmu:
                CLb =  stats.multivariate_normal.cdf(sqA - sqmu)
            else:
                if qA == 0.:
                    CLsb = 1.
                    CLb  = 1.
                else:
                    CLsb = 1. - stats.multivariate_normal.cdf( (qmu + qA)/(2*sqA) )
                    CLb = 1. - stats.multivariate_normal.cdf( (qmu - qA)/(2*sqA) )
            CLs = 0.
            if CLb > 0.:
                CLs = CLsb/CLb
            root = CLs - 1. + self.cl
            return root


        a,b=1.5*mu_hat,2.5*mu_hat+2*sigma_mu
        ctr=0
        while True:
            while ( NP.sign ( root_func(a)* root_func(b) ) > -.5 ):
                b=1.4*b  ## widen bracket FIXME make a linear extrapolation!
                a=a-(b-a)*.3 ## widen bracket
                if a < 0.: a=0.
                ctr+=1
                if ctr>20: ## but stop after 20 trials
                    if toys > 2000:
                        logger.error("cannot find brent bracket after 20 trials. a,b=%s(%s),%s(%s)" % ( root_func(a),a,root_func(b),b ) )
                        return None
                    else:
                        logger.debug("cannot find brent bracket after 20 trials. but very low number of toys")
                        return self.ulSigma ( model, marginalize, 4*toys )
            try:
                mu_lim = optimize.brentq ( root_func, a, b, rtol=1e-03, xtol=1e-06 )
                return mu_lim
            except ValueError as e: ## it could still be that the signs arent opposite
                # in that case, try again
                pass
                
    def computeCLs(self, model, marginalize=False, toys=None, expected=False ):
        """ exclusion confidence level obtained from the defined Data (using the signal prediction
            for each signal regio/dataset), by using
            the q_mu test statistic from the CCGV paper (arXiv:1007.1727).

        :params marginalize: if true, marginalize nuisances, else profile them
        :params toys: specify number of toys. Use default is none
        :params expected: compute the expected value, not the observed.
        :returns: exclusion confidence level (1-CLs)
        """
        if model.zeroSignal():
            """ only zeroes in efficiencies? cannot give a limit! """
            return None
        if toys==None:
            toys=self.ntoys
        oldmodel = model
        if expected:
            model = copy.deepcopy(oldmodel)
            #model.observed = model.backgrounds
            for i,d in enumerate(model.backgrounds):
                model.observed[i]=int(NP.round(d))
        computer = LikelihoodComputer(model, toys)
        mu_hat = computer.findMuHat(model.nsignal)
        theta_hat0,_ = computer.findThetaHat(0*model.nsignal)
        sigma_mu = computer.getSigmaMu(model.nsignal)

        aModel = copy.deepcopy(model)
        aModel.observed = array([NP.round(x+y) for x,y in zip(model.backgrounds,theta_hat0)])
        #print ( "aModeldata=", aModel.observed )
        #aModel.observed = array ( [ round(x) for x in model.backgrounds ] )
        aModel.name = aModel.name + "A"
        compA = LikelihoodComputer(aModel, toys)
        ## compute
        mu_hatA = compA.findMuHat(aModel.nsignal)
        # if mu_hat < 0.:
            # mu_hat = 0.
        # L(mu = 0, theta(0))
        nll0 = computer.likelihood(0.,
                                     marginalize=marginalize,
                                     nll=True)
        if NP.isinf(nll0) and marginalize==False:
            logger.warning("nll is infinite in profiling! we switch to marginalization, but only for this one!" )
            marginalize=True
            nll0 = computer.likelihood(0.,
                                         marginalize=True,
                                         nll=True)
            if NP.isinf(nll0):
                logger.warning("marginalization didnt help either. switch back.")
                marginalize=False
            else:
                logger.warning("marginalization worked.")
        # L_A(mu = 0, theta(0))
        nll0A = compA.likelihood(0.,
                                   marginalize=marginalize,
                                   nll=True)
        
        # nsig = model.signals(1.)
        computer.ntot = model.backgrounds + model.nsignal
        # L(mu = 1, theta(1))
        nll = computer.likelihood(model.nsignal, marginalize=marginalize, nll=True )
        # L_A(mu = 1, theta(1))
        nllA = compA.likelihood(model.nsignal, marginalize=marginalize, nll=True )
        qmu =  2*( nll - nll0 )
        if qmu<0.: qmu=0.
        sqmu = sqrt (qmu)
        qA =  2*( nllA - nll0A )
        # print ( "mu: %s, qMu: %s, qA: %s nll0A: %s nllA: %s" % ( mu, qmu, qA, nll0A, nllA ) )
        if qA<0.:
            qA=0.
        sqA = sqrt(qA)
        if qA >= qmu:
            CLsb = 1. - stats.multivariate_normal.cdf(sqmu)
            CLb =  stats.multivariate_normal.cdf(sqA - sqmu)
        else:
            if qA == 0.:
                CLsb = 1.
                CLb  = 1.
            else:
                CLsb = 1. - stats.multivariate_normal.cdf( (qmu + qA)/(2*sqA) )
                CLb = 1. - stats.multivariate_normal.cdf( (qmu - qA)/(2*sqA) )
        # CLs = 0.
        # if CLb > 0.:
        CLs = CLsb/CLb
        return 1 - CLs


if __name__ == "__main__":
    C = [ 18774.2, -2866.97, -5807.3, -4460.52, -2777.25, -1572.97, -846.653, -442.531,
       -2866.97, 496.273, 900.195, 667.591, 403.92, 222.614, 116.779, 59.5958,
       -5807.3, 900.195, 1799.56, 1376.77, 854.448, 482.435, 258.92, 134.975,
       -4460.52, 667.591, 1376.77, 1063.03, 664.527, 377.714, 203.967, 106.926,
       -2777.25, 403.92, 854.448, 664.527, 417.837, 238.76, 129.55, 68.2075,
       -1572.97, 222.614, 482.435, 377.714, 238.76, 137.151, 74.7665, 39.5247,
       -846.653, 116.779, 258.92, 203.967, 129.55, 74.7665, 40.9423, 21.7285,
       -442.531, 59.5958, 134.975, 106.926, 68.2075, 39.5247, 21.7285, 11.5732]
    nsignal = [ x/100. for x in [47,29.4,21.1,14.3,9.4,7.1,4.7,4.3] ]
    m=Data( observed=[1964,877,354,182,82,36,15,11],
              backgrounds=[2006.4,836.4,350.,147.1,62.0,26.2,11.1,4.7],
              covariance= C,
#              third_moment = [ 0.1, 0.02, 0.1, 0.1, 0.003, 0.0001, 0.0002, 0.0005 ],
              third_moment = [ 0. ] * 8,
              nsignal = nsignal,
              name="CMS-NOTE-2017-001 model" )
    ulComp = UpperLimitComputer(ntoys=500, cl=.95)
    #uls = ulComp.ulSigma ( Data ( 15,17.5,3.2,0.00454755 ) )
    #print ( "uls=", uls )
    ul_old = 131.828*sum(nsignal) #With respect to the older refernece value one must normalize the xsec
    print ( "old ul=", ul_old )
    ul = ulComp.ulSigma ( m )
    print ( "ul (marginalized)", ul )
    ul = ulComp.ulSigma ( m, marginalize=False )
    print ( "ul (profiled)", ul )
