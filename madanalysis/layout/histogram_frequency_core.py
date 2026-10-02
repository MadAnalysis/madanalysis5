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


"""Content of a frequency histogram (e.g. ``NPID``) for one sign of the event weights."""

from __future__ import absolute_import
from __future__ import annotations
import logging


class HistogramFrequencyCore:
    """Statistics and content of a frequency histogram.

    Attributes:
        integral (``float``): sum of the bin contents.
        nevents (``int``): number of events.
        sumwentries (``float``): sum of the weights of the entries.
        sumw (``float``): sum of the weights.
        entries (``float``): number of entries.
        nentries (``int``): number of entries.
        overflow (``float``): overflow content.
        underflow (``float``): underflow content.
        array (``list[float]``): bin contents.
    """
    def __init__(self) -> None:
        """Initialise an empty histogram."""
        self.integral = 0.0
        self.nevents = 0
        self.sumwentries = 0.0
        self.sumw = 0.0
        self.entries = 0.0
        self.nentries = 0
        self.overflow = 0.0
        self.underflow = 0.0
        self.array = []

    def ComputeIntegral(self) -> None:
        """Compute :attr:`integral` from the bin contents."""
        self.integral = 0
        for value in self.array:
            self.integral += value

    def Print(self) -> None:
        """Log the statistics."""

        logging.getLogger("MA5").info(
            "nevents=" + str(self.nevents) + " entries=" + str(self.entries)
        )
