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

from abc import ABC, abstractmethod
from typing import Text


class MA5DataSet:
    def __init__(self, srname):
        """param srname: the name of the signal region
        ("dataset" is SModelS' name for a signal region)"""
        self.srname = srname

    def getID(self):
        return self.srname


class MA5CrossSection:
    def __init__(self, value: float, sqrts: float = 13.0):
        """production cross section
        :param value: the value of the production cross section
        :param sqrts: center of mass energy
        """
        self.value = value
        self.sqrts = sqrts


class TACOBase(ABC):
    def __init__(
        self,
        analysis: Text = "__unknown_analysis__",
        regionID: Text = "__unknown_region__",
        xsection: float = -1,
    ):
        """
        Abstract Class for TACO interface

        Parameters
        ----------
        analysis: (Text) analysis name
        regionID: (Text) region name
        """
        self.analysis = analysis
        self.dataset = MA5DataSet(regionID)
        self.xsection = MA5CrossSection(xsection, 13.0)

    @abstractmethod
    def likelihood(
        self, mu: float = 1.0, nll: bool = False, expected: bool = False, useCached=False
    ) -> float:
        """
        Returns the value of the likelihood.
        Inspired by the `pyhf.infer.mle` module but for non-log likelihood

        Parameters
        ----------
        mu: float
            POI: signal strength
        nll: bool
            if true, return nll, not llhd
        expected: bool
            if true, compute expected likelihood, else observed.
        """
        raise NotImplementedError

    @abstractmethod
    def sigma_mu(self, expected: bool = False) -> float:
        """
        get an estimate for the standard deviation of mu around mu_hat this is only used for
        initialisation of the optimizer, so can be skipped in the first version,

        expected:
            get muhat for the expected likelihood, not observed.
        """
        raise NotImplementedError

    @abstractmethod
    def muhat(self, expected: bool = False, allowNegativeSignals: bool = True) -> float:
        """
        get the value of mu for which the likelihood is maximal. this is only used for
        initialisation of the optimizer, so can be skipped in the first version of the code.

        Parameters
        ----------
        expected: bool
            get muhat for the expected likelihood, not observed.
        allowNegativeSignals: bool
            if true, allow negative values for mu
        """
        raise NotImplementedError

    @abstractmethod
    def getUpperLimit(self, expected: bool = False):
        """
        code here that retrieves the upper limit, expected or observed
        """
        raise NotImplementedError

    def dataType(self):  # a technicality
        return "efficiencyMap"

    def analysisId(self):
        """
        return the analysis id, e.g. "atlas_susy_2018_02"
        """
        return self.analysis
