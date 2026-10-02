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


"""Cut-flow counters of one cut (as read from the SampleAnalyzer SAF files)."""

from __future__ import absolute_import
from __future__ import annotations
import logging
class CutInfo:
    """Counters of one cut, split into events with positive and negative weights.

    Attributes:
        nentries_pos (``int``): number of selected events with a positive weight.
        nentries_neg (``int``): number of selected events with a negative weight.
        sumw_pos (``float``): sum of the positive weights.
        sumw_neg (``float``): sum of the absolute values of the negative weights.
        sumw2_pos (``float``): sum of the squared positive weights.
        sumw2_neg (``float``): sum of the squared negative weights.
        cutname (``str``): name of the cut.
        cutregion (``str``): name of the region.
    """

    def __init__(self) -> None:
        """Initialise the counters to zero."""
        self.Reset()

    def Reset(self) -> None:
        """Reset the counters to zero."""
        self.nentries_pos = 0
        self.nentries_neg = 0
        self.sumw_pos     = 0.
        self.sumw_neg     = 0.
        self.sumw2_pos    = 0.
        self.sumw2_neg    = 0.
        self.cutname      = ""
        self.cutregion    = ""

    def Print(self) -> None:
        """Log the counters."""
        logging.getLogger('MA5').info("nentries_pos = " + str(self.nentries_pos))
        logging.getLogger('MA5').info("nentries_neg = " + str(self.nentries_neg))
        logging.getLogger('MA5').info("sumw_pos     = " + str(self.sumw_pos))
        logging.getLogger('MA5').info("sumw_neg     = " + str(self.sumw_neg))
        logging.getLogger('MA5').info("sumw2_pos    = " + str(self.sumw2_pos))
        logging.getLogger('MA5').info("sumw2_neg    = " + str(self.sumw2_neg))
        
        
