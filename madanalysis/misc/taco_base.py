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


class TACOBase(ABC):
    def __init__(self, analysis: Text, regionID: Text):
        """
        Abstract Class for TACO interface

        Parameters
        ----------
        analysis: (Text) analysis name
        regionID: (Text) region name
        """
        self.analysis = analysis
        self.regionOD = regionID

    @abstractmethod
    def likelihood(self, mu: float = 1.0, nll: bool = False, expected: bool = False) -> float:
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
