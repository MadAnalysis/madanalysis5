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


"""Histogram with a logarithmic binning read from the SampleAnalyzer output."""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from madanalysis.core.main import Main
    from madanalysis.dataset.dataset import Dataset
from madanalysis.layout.histogram_core import HistogramCore
import logging
from math import sqrt, log10, pow
import array
from six.moves import range

class HistogramLogX:
    """Histogram with a logarithmic binning filled separately for positive and negative event weights.

    Attributes:
        name (``str``): name of the histogram.
        nbins (``int``): number of bins.
        xmin (``float``): lower bound of the x axis.
        xmax (``float``): upper bound of the x axis.
        ymin (``list[float]``): lower bound(s) of the y axis (empty if automatic).
        ymax (``list[float]``): upper bound(s) of the y axis (empty if automatic).
        scale (``float``): normalisation factor.
        positive (``HistogramCore``): content for positive weights.
        negative (``HistogramCore``): content for negative weights.
        summary (``HistogramCore``): net content.
        warnings (``list[str]``): warnings raised while reading.
        regions (``list[str]``): regions of the histogram.
    """

    def __init__(self) -> None:
        """Initialise an empty histogram (see :meth:`Reset`)."""
        self.Reset()


    def Print(self) -> None:
        """Log the definition and the statistics of the histogram."""

        # General info
        inform = self.name + ' ' + str(self.nbins) + str(self.xmin) + ' ' + str(self.xmax)
        if self.ymin!=[] or self.ymax!=[]:
           inform = inform + ' ' + str(self.ymin) + ' ' + str(self.ymax)
        logging.getLogger('MA5').info(inform)

        # Data
        self.positive.Print()
        self.negative.Print()
        self.summary.Print()


    def Reset(self) -> None:
        """Reset the definition and the content of the histogram."""

        # General info
        self.name  = ""
        self.nbins = 100
        self.xmin  = 0.
        self.xmax  = 100.
        self.ymin  = []
        self.ymax  = []
        self.scale = 0.

        # Data
        self.positive = HistogramCore()
        self.negative = HistogramCore()
        self.summary  = HistogramCore()

        # Warnings
        self.warnings = []

        # regions
        self.regions = []

    def GetRegions(self) -> list[str]:
        """Get the regions of the histogram.

        Returns:
            ``list[str]``:
            Region names.
        """
        return self.regions

    def FinalizeReading(self,main: Main,dataset: Dataset) -> None:
        """Build the summary (positive minus negative weights) after reading the SAF file.

        Negative bin contents (and statistics) are set to zero, with a warning stored in
        :attr:`warnings`.

        Args:
            main (``Main``): session state (unused).
            dataset (``Dataset``): dataset (used in the warnings).
        """

        # Statistics
        self.summary.nevents   = self.positive.nevents   + self.negative.nevents
        self.summary.nentries   = self.positive.nentries   + self.negative.nentries
        self.summary.sumw      = self.positive.sumw      - self.negative.sumw
        if self.summary.sumw<0:
            self.summary.sumw=0
        self.summary.sumw2     = self.positive.sumw2     - self.negative.sumw2
        if self.summary.sumw2<0:
            self.summary.sumw2=0
        self.summary.sumwx     = self.positive.sumwx     - self.negative.sumwx
        if self.summary.sumwx<0:
            self.summary.sumwx=0
        self.summary.sumw2x    = self.positive.sumw2x    - self.negative.sumw2x
        if self.summary.sumw2x<0:
            self.summary.sumw2x=0
        self.summary.underflow = self.positive.underflow - self.negative.underflow
        if self.summary.underflow<0:
            self.summary.underflow=0
        self.summary.overflow  = self.positive.overflow  - self.negative.overflow
        if self.summary.overflow<0:
            self.summary.overflow=0
            
        # Data
        data = []
        for i in range(0,len(self.positive.array)):
            data.append(self.positive.array[i]-self.negative.array[i])
            if data[-1]<0:
                self.warnings.append(\
                    'dataset='+dataset.name+\
                    ' -> bin '+str(i)+\
                    ' has a negative content : '+\
                    str(data[-1])+'. This value is set to zero')
                data[-1]=0
        self.summary.array = data[:] # [:] -> clone of data

        # Integral
        self.positive.ComputeIntegral()
        self.negative.ComputeIntegral()
        self.summary.ComputeIntegral()
            

    def CreateHistogram(self) -> None:
        """Compute the logarithmic bin edges (the result is not stored)."""

        # Logarithm binning
        step = (log10(self.xmax) - log10(self.xmin) ) / \
               float (self.nbins)
        binnings=[]
        for i in range(0,self.nbins):
            binnings.append( pow(10., log10(self.xmin)+i*step) )
        binnings.append(self.xmax)



    def GetBinLowEdge(self,bin: int) -> float:
        """Get the lower edge of a bin (logarithmic binning).

        Args:
            bin (``int``): 0-based bin index.

        Returns:
            ``float``:
            The lower edge (clamped to ``xmin``/``xmax`` outside the range).
        """

        # Special case
        if bin<=0:
            return self.xmin

        if bin>=self.nbins:
            return self.xmax
        
        # Computing steps
        step = (log10(self.xmax) - log10(self.xmin) ) / \
               float (self.nbins)
        
        # value
        return pow(10., log10(self.xmin)+bin*step)


    def GetBinUpperEdge(self,bin: int) -> float:
        """Get the upper edge of a bin (logarithmic binning).

        Args:
            bin (``int``): 0-based bin index.

        Returns:
            ``float``:
            The upper edge (clamped to ``xmin``/``xmax`` outside the range).
        """

        # Special case
        # FIXME: returns xmin for the first bin instead of its upper edge (xmin + step).
        if bin<=0:
            return self.xmin

        if bin>=self.nbins:
            return self.xmax
        
        # Computing steps
        step = (log10(self.xmax) - log10(self.xmin) ) / \
               float (self.nbins)
        
        # value
        return pow(10., log10(self.xmin)+(bin+1)*step)


    def GetBinMean(self,bin: int) -> float:
        """Get the centre of a bin (logarithmic binning).

        Args:
            bin (``int``): 0-based bin index.

        Returns:
            ``float``:
            The centre (clamped to ``xmin``/``xmax`` outside the range).
        """

        # Special case
        if bin<=0:
            return self.xmin

        if bin>=self.nbins:
            return self.xmax
        
        # Computing steps
        step = (log10(self.xmax) - log10(self.xmin) ) / \
               float (self.nbins)
        
        # value
        return pow(10., log10(self.xmin)+(bin+0.5)*step)
