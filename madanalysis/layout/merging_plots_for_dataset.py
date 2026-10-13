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


"""Collection of the DJR merging-check histograms of one dataset."""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING, Any

if TYPE_CHECKING:
    from madanalysis.core.main import Main
    from madanalysis.dataset.dataset import Dataset
from madanalysis.enumeration.uncertainty_type     import UncertaintyType
from madanalysis.enumeration.normalize_type       import NormalizeType
from madanalysis.enumeration.report_format_type   import ReportFormatType
from madanalysis.enumeration.observable_type      import ObservableType
from madanalysis.enumeration.color_type           import ColorType
from madanalysis.enumeration.linestyle_type       import LineStyleType
from madanalysis.enumeration.backstyle_type       import BackStyleType
from madanalysis.enumeration.stacking_method_type import StackingMethodType
from math import sqrt


class MergingPlotsForDataset:
    """Merging-check histograms of one dataset.

    Attributes:
        dataset (``Dataset``): dataset.
        main (``Main``): session state.
        histos (``list[Any]``): histograms.
    """

    def __init__(self,main: Main,dataset: Dataset) -> None:
        """Initialise an empty collection.

        Args:
            main (``Main``): session state.
            dataset (``Dataset``): dataset.
        """
        self.dataset = dataset
        self.main    = main
        self.histos  = []

    def __len__(self) -> int:
        """Get the number of histograms.

        Returns:
            ``int``:
            Number of histograms.
        """
        return len(self.histos)


    def __getitem__(self,i: int) -> Any:
        """Get a histogram.

        Args:
            i (``int``): index.

        Returns:
            ``Any``:
            The histogram.
        """
        return self.histos[i]


    # Computing integral
    def FinalizeReading(self) -> None:
        """Finalise the reading (normalisation) of all histograms."""

        for histo in self.histos:
            histo.FinalizeReading(self.main,self.dataset)

            
    # Computing integral
    def CreateHistogram(self) -> None:
        """Build the summed histograms of all histograms."""

        for histo in self.histos:
            histo.CreateHistogram()



