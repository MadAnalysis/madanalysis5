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


"""Content of a histogram for one sign of the event weights."""

from __future__ import absolute_import
from __future__ import annotations
import logging
from math import sqrt
from six.moves import range


class HistogramCore:
    """Statistics and content of a histogram.

    Attributes:
        nevents (``int``): number of events.
        nentries (``int``): number of entries.
        integral (``float``): sum of the bin contents (including under/overflow).
        sumwentries (``float``): sum of the weights of the entries.
        sumw (``float``): sum of the weights.
        sumw2 (``float``): sum of the squared weights.
        sumwx (``float``): sum of ``weight * x``.
        sumw2x (``float``): sum of ``weight * x**2``.
        underflow (``float``): underflow content.
        overflow (``float``): overflow content.
        nan (``float``): content of NaN entries.
        inf (``float``): content of infinite entries.
        array (``list[float]``): bin contents.
    """

    def __init__(self) -> None:
        """Initialise an empty histogram."""

        # statistics
        # - int
        self.nevents     = 0
        self.nentries    = 0
        # - float
        self.integral    = 0.
        self.sumwentries = 0.
        self.sumw        = 0.
        self.sumw2       = 0.
        self.sumwx       = 0.
        self.sumw2x      = 0.

        # content
        self.underflow   = 0.
        self.overflow    = 0.
        self.nan         = 0.
        self.inf         = 0.
        self.array       = []


    def ComputeIntegral(self) -> None:
        """Compute :attr:`integral` from the bins, the underflow and the overflow."""
        self.integral = 0
        for i in range(0,len(self.array)):
            self.integral+=self.array[i]
        self.integral += self.overflow
        self.integral += self.underflow
        

    def Print(self) -> None:
        """Log the statistics."""

        # FIXME: 'self.entries' does not exist (AttributeError); probably 'self.nentries'.
        logging.getLogger('MA5').info('nevents='+str(self.nevents)+\
                     ' entries='+str(self.entries))

        logging.getLogger('MA5').info('sumw='+str(self.sumw)+\
                     ' sumw2='+str(self.sumw2)+\
                     ' sumwx='+str(self.sumwx)+\
                     ' sumw2x='+str(self.sumw2x))

        logging.getLogger('MA5').info('underflow='+str(self.underflow)+\
                     ' overflow='+str(self.overflow))
        

    def GetMean(self) -> float:
        """Get the weighted mean of the distribution.

        Returns:
            ``float``:
            ``sumwx / sumw`` (0 if ``sumw`` is 0).
        """

        if self.sumw==0:
            return 0.
        else:
            return self.sumwx / self.sumw


    def GetRMS(self) -> float:
        """Get the weighted RMS of the distribution.

        Returns:
            ``float``:
            ``sqrt(|sumw2x/sumw - mean**2|)`` (0 if ``sumw`` is 0).
        """

        if self.sumw==0:
            return 0.
        else:
            mean = self.GetMean()
            return sqrt(abs(self.sumw2x/self.sumw - mean*mean))
        
 
        

        
