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


"""Measured properties of an event sample (filled from the SampleAnalyzer output)."""

from __future__ import annotations

class SampleInfo():
    """Global information about an event sample or a dataset, as measured by SampleAnalyzer.

    Attributes:
        xsection (``float``): cross section in pb.
        xerror (``float``): uncertainty on the cross section in pb.
        nevents (``int``): number of events.
        sumw_positive (``float``): sum of the positive event weights.
        sumw_negative (``float``): sum of the absolute values of the negative event weights.
    """
    def __init__(self) -> None:
        """Initialise all quantities to zero."""
        self.xsection = 0.
        self.xerror   = 0.
        self.nevents  = 0
        self.sumw_positive = 0.
        self.sumw_negative = 0.
