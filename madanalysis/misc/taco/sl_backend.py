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

from madanalysis.misc.taco.base import TACOBase
from madanalysis.misc.simplified_likelihood import Data, LikelihoodComputer, UpperLimitComputer
from typing import Text


class slSingleRegion(TACOBase):
    def __init__(
        self,
        analysis: Text,
        regionID: Text,
        nobs: int,
        nb: float,
        deltanb: float,
        N0: float,
        Nf: float,
        xsection: float,
        lumi: float,
        sqrts: float = 13.0,
    ):
        super(slSingleRegion, self).__init__(analysis, regionID, xsection)
        self.xsection.sqrts = sqrts
        self.Nf = Nf
        self.N0 = N0
        self.lumi = lumi

        self.experimental_data = [nobs, nb, deltanb]
        self.data = Data(*self.experimental_data, nsignal=self.nsignal())

        self.marginalize = False
        self.cachedLlhds = {"exp": {}, "obs": {}}

    def nsignal(self, mu: float = 1.0):
        """
        Number of signal events

        Parameters
        ----------
        mu: (float) POI signal strength
        """
        return mu * self.xsection.value * 1000.0 * self.lumi * self.Nf / self.N0

    def computeCLs(self, expected: bool = False) -> float:
        """
        for testing purposes
        """
        interface = UpperLimitComputer()
        return interface.computeCLs(self.data, expected=expected)

    def likelihood(
        self,
        mu: float = 1.0,
        expected: bool = False,
        nll: bool = False,
        useCached=False,
    ) -> float:
        """

        Parameters
        ----------
        mu: (float) POI signal strength
        expected: if true, compute expected likelihood, else observed.
        nll: if True, return the negative log likelihood instead of the likelihood
        useCached: if true reuse cached results

        Returns
        -------
        likelihood with respect to given POI
        """

        if useCached:
            cached = None
            if expected:
                cached = self.cachedLlhds["exp"].get(mu, None)
            else:
                cached = self.cachedLlhds["obs"].get(mu, None)
            if cached is not None:
                return cached

        if expected:
            data = Data(
                self.experimental_data[1], *self.experimental_data[1:], nsignal=self.nsignal()
            )
            interface = LikelihoodComputer(self.data)
            llhd = interface.likelihood(self.nsignal(mu), marginalize=self.marginalize, nll=nll)
            cached = self.cachedLlhds["exp"][mu] = llhd
        else:
            interface = LikelihoodComputer(self.data)
            llhd = interface.likelihood(self.nsignal(mu), marginalize=self.marginalize, nll=nll)
            cached = self.cachedLlhds["obs"][mu] = llhd
        return llhd

    def sigma_mu(self, expected: bool = False, **kwargs) -> float:
        """
        get an estimate for the standard deviation of mu around mu_hat this is only used for
        initialisation of the optimizer, so can be skipped in the first version, it's even
        less important than muhat

        Parameters
        ----------
        expected: (float) if true, compute expected likelihood, else observed.
                 if "posteriori", compute a posteriori expected likelihood
                 (FIXME do we need this?)
        """

        if expected:
            data = Data(
                self.experimental_data[1], *self.experimental_data[1:], nsignal=self.nsignal()
            )
        else:
            data = self.data
        interface = LikelihoodComputer(data)
        theta_hat = interface.findThetaHat(self.nsignal())
        return interface.getSigmaMu(1.0, self.nsignal(), theta_hat)

    def muhat(self, expected: bool = False, allowNegativeSignals: bool = True) -> float:
        """
        Find the most likely signal strength mu.

        Parameters
        ----------
        expected
        allowNegativeSignals: if true, then also allow for negative values
        """
        if expected:
            data = Data(
                self.experimental_data[1], *self.experimental_data[1:], nsignal=self.nsignal()
            )
        else:
            data = self.data
        interface = LikelihoodComputer(data)
        return interface.findMuHat([self.nsignal()], allowNegativeSignals=allowNegativeSignals)

    def getUpperLimit(self, expected: bool = False):
        if expected:
            data = Data(
                self.experimental_data[1], *self.experimental_data[1:], nsignal=self.nsignal()
            )
        else:
            data = self.data
        interface = UpperLimitComputer()
        return interface.getUpperLimitOnMu(data, marginalize=self.marginalize, expected=expected)
