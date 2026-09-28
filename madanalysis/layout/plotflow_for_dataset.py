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


"""Histograms of one dataset and their normalisation."""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING, Any

if TYPE_CHECKING:
    from madanalysis.core.main import Main
    from madanalysis.dataset.dataset import Dataset
from madanalysis.enumeration.uncertainty_type import UncertaintyType
from madanalysis.enumeration.normalize_type import NormalizeType
from madanalysis.enumeration.report_format_type import ReportFormatType
from madanalysis.enumeration.observable_type import ObservableType
from madanalysis.enumeration.color_type import ColorType
from madanalysis.enumeration.linestyle_type import LineStyleType
from madanalysis.enumeration.backstyle_type import BackStyleType
from madanalysis.enumeration.stacking_method_type import StackingMethodType
import copy
from six.moves import range

# pylint: disable=consider-using-enumerate


class PlotFlowForDataset:
    """Histograms of one dataset, in the order of the selection.

    Attributes:
        histos (``list[Any]``): ``Histogram``, ``HistogramLogX`` or ``HistogramFrequency``
            objects.
        main (``Main``): session state.
        dataset (``Dataset``): dataset.
        xsection (``float``): cross section used for the normalisation (user value if
            set, measured value otherwise).
    """
    def __init__(self, main: Main, dataset: Dataset) -> None:
        """Initialise an empty collection.

        Args:
            main (``Main``): session state.
            dataset (``Dataset``): dataset.
        """
        self.histos = []
        self.main = main
        self.dataset = dataset

        # Getting xsection
        self.xsection = self.dataset.measured_global.xsection
        if self.dataset.xsection != 0.0:
            self.xsection = self.dataset.xsection

    def __len__(self) -> int:
        """Get the number of histograms.

        Returns:
            ``int``:
            Number of histograms.
        """
        return len(self.histos)

    def __getitem__(self, i: int) -> Any:
        """Get a histogram.

        Args:
            i (``int``): index among the histograms.

        Returns:
            ``Any``:
            The histogram.
        """
        return self.histos[i]

    # Computing integral
    def FinalizeReading(self) -> None:
        """Finalise the reading of the histograms and update the cross section."""

        for histo in self.histos:
            histo.FinalizeReading(self.main, self.dataset)
        # Updating the value of the cross section (BENJ)
        self.xsection = self.dataset.measured_global.xsection
        if self.dataset.xsection != 0.0:
            self.xsection = self.dataset.xsection

    # Computing integral
    def CreateHistogram(self) -> None:
        """Finalise the binning/labels of every histogram."""

        iplot = 0

        # Loop over plot
        for iabshisto in range(0, len(self.main.selection)):
            # Keep only histogram
            if self.main.selection[iabshisto].__class__.__name__ != "Histogram":
                continue

            # Case of histogram frequency
            if self.histos[iplot].__class__.__name__ == "HistogramFrequency":
                if self.main.selection[iabshisto].observable.name == "NPID":
                    NPID = True
                else:
                    NPID = False
                self.histos[iplot].CreateHistogram(NPID, self.main)
            else:
                self.histos[iplot].CreateHistogram()
            iplot += 1

    # Computing scales
    def ComputeScale(self) -> None:
        """Compute the normalisation factor of every histogram.

        * ``normalize2one`` stacking: ``1 / integral``;
        * ``main.normalize = none``: ``1``;
        * ``lumi``/``lumi_weight``: ``xsection * lumi * 1000 * eff * sumw/sumwentries /
          integral``, with ``eff`` the fraction of events reaching the histogram (and the
          dataset weight for ``lumi_weight``).
        """

        iplot = 0

        # Loop over plot
        for iabshisto in range(0, len(self.main.selection)):

            # Keep only histogram
            if self.main.selection[iabshisto].__class__.__name__ != "Histogram":
                continue

            # Reset scale
            scale = 0.0

            # Case 1: Normalization to ONE
            if self.main.selection[
                iabshisto
            ].stack == StackingMethodType.NORMALIZE2ONE or (
                self.main.stack == StackingMethodType.NORMALIZE2ONE
                and self.main.selection[iabshisto].stack == StackingMethodType.AUTO
            ):
                integral = (
                    self.histos[iplot].positive.integral
                    - self.histos[iplot].negative.integral
                )
                if integral > 0.0:
                    scale = 1.0 / integral
                else:
                    scale = 0.0

            # Case 2: No normalization
            elif self.main.normalize == NormalizeType.NONE:
                scale = 1.0

            # Case 3 and 4 : Normalization formula depends on LUMI
            #                or depends on WEIGHT+LUMI
            elif self.main.normalize in [NormalizeType.LUMI, NormalizeType.LUMI_WEIGHT]:

                # integral
                integral = (
                    self.histos[iplot].positive.integral
                    - self.histos[iplot].negative.integral
                )

                # compute efficiency : Nevent / Ntotal
                if self.dataset.measured_global.nevents == 0:
                    eff = 0
                else:
                    eff = (
                        self.histos[iplot].positive.nevents
                        + self.histos[iplot].negative.nevents
                    ) / float(self.dataset.measured_global.nevents)

                # compute the good xsection value
                thexsection = self.xsection
                if self.main.normalize == NormalizeType.LUMI_WEIGHT:
                    thexsection = thexsection * self.dataset.weight

                # compute final entries/event ratio
                entries_per_events = 0
                sumw = self.histos[iplot].positive.sumw - self.histos[iplot].negative.sumw
                Nentries = (
                    self.histos[iplot].positive.sumwentries
                    - self.histos[iplot].negative.sumwentries
                )
                if sumw != 0 and Nentries != 0:
                    entries_per_events = sumw / Nentries

                # compute the scale
                if integral != 0:
                    scale = (
                        thexsection
                        * self.main.lumi
                        * 1000
                        * eff
                        * entries_per_events
                        / integral
                    )
                else:
                    scale = 1  # no scale for empty plot

            # Setting the computing scale
            self.histos[iplot].scale = copy.copy(scale)

            # Incrementing counter
            iplot += 1
