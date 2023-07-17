################################################################################
#
#  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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


from __future__ import absolute_import

import copy
from six.moves import range
import numpy as np

from madanalysis.enumeration.normalize_type import NormalizeType
from madanalysis.enumeration.stacking_method_type import StackingMethodType
from madanalysis.dataset import dataset as Dataset
from .histogram_processor import HistogramProcessor


class PlotFlowForDataset:
    def __init__(self, main, dataset: Dataset):
        self.histos = []
        self.main = main
        self.dataset: Dataset = dataset

        # Getting xsection
        self.xsection = self.dataset.measured_global.xsection
        if self.dataset.xsection != 0.0:
            self.xsection = self.dataset.xsection

    def __len__(self):
        return len(self.histos)

    def __getitem__(self, i):
        return self.histos[i]

    # Computing integral
    def FinalizeReading(self):

        for histo in self.histos:
            histo.FinalizeReading(self.main, self.dataset)
        # Updating the value of the cross section (BENJ)
        self.xsection = self.dataset.measured_global.xsection
        if self.dataset.xsection != 0.0:
            self.xsection = self.dataset.xsection

    # Computing integral
    def CreateHistogram(self):

        iplot = 0

        # Loop over plot
        for select in self.main.selection:
            # Keep only histogram
            if select.__class__.__name__ != "Histogram":
                continue

            # Case of histogram frequency
            if self.histos[iplot].__class__.__name__ == "HistogramFrequency":
                NPID = True if select.observable.name == "NPID" else False
                self.histos[iplot].CreateHistogram(NPID, self.main)
            else:
                self.histos[iplot].CreateHistogram()
            iplot += 1

    # Computing scales
    def ComputeScale(self):

        iplot = 0
        # Loop over plot
        for iabshisto, select in enumerate(self.main.selection):

            # Keep only histogram
            if select.__class__.__name__ != "Histogram":
                continue

            processor = HistogramProcessor(
                self.histos[iplot],
                self.dataset.weight_collection,
                self.dataset.measured_global.nevents,
                self.xsection,
            )

            # Reset scale
            scale = 0.0

            # integral
            integral = (
                self.histos[iplot].positive.integral
                - self.histos[iplot].negative.integral
            )

            integral = np.mean(integral)

            # Case 1: Normalization to ONE
            if select.stack == StackingMethodType.NORMALIZE2ONE or (
                self.main.stack == StackingMethodType.NORMALIZE2ONE
                and self.main.selection[iabshisto].stack == StackingMethodType.AUTO
            ):
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

                # compute the good xsection value
                thexsection = self.xsection
                if self.main.normalize == NormalizeType.LUMI_WEIGHT:
                    thexsection = thexsection * self.dataset.weight

                scale = processor.scale(lumi=self.main.lumi)

            # Setting the computing scale
            self.histos[iplot].scale = copy.copy(scale)
            setattr(self.histos[iplot], "processor", processor)
            # Incrementing counter
            iplot += 1
