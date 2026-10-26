################################################################################
#  
#  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://github.com/MadAnalysis/madanalysis5>
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


"""Value with an uncertainty, and binomial error helpers."""

from __future__ import absolute_import
from __future__ import annotations
from math import sqrt
class Measure:
    """Value with an uncertainty.

    Attributes:
        mean (``float``): central value.
        error (``float``): uncertainty.
    """

    def __init__(self) -> None:
        """Initialise the value and its uncertainty to zero."""
        self.mean  = 0.
        self.error = 0.

    @staticmethod
    def binomialNEventError(k: float,N: float) -> float:
        r"""Binomial uncertainty on a number of selected events.

        .. math::
            \sigma_k = \sqrt{k\,|N-k|/N}

        Args:
            k (``float``): number of selected events.
            N (``float``): number of events before selection.

        Returns:
            ``float``:
            The uncertainty (0 if ``N`` is 0).
        """
        if N==0:
            return 0.
        else:
            return sqrt( float(k*abs(N-k)) / float(N) )

    @staticmethod
    def binomialError(k: float,N: float) -> float:
        r"""Binomial uncertainty on an efficiency ``k/N``.

        .. math::
            \sigma_\varepsilon = \sqrt{k\,|N-k|/N^3}

        Args:
            k (``float``): number of selected events.
            N (``float``): number of events before selection.

        Returns:
            ``float``:
            The uncertainty (0 if ``N`` is 0).
        """
        if N==0:
            return 0.
        else:
            return sqrt( float(k*abs(N-k)) / float(N*N*N) )


