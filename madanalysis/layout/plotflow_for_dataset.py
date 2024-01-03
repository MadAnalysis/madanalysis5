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

import copy, os

from six.moves import range

from madanalysis.enumeration.normalize_type import NormalizeType
from madanalysis.enumeration.stacking_method_type import StackingMethodType

# pylint: disable=C0200,C0103


class PlotFlowForDataset:
    def __init__(self, main, dataset):
        self.histos = []
        self.multiweight_histos = []
        self.main = main
        self.dataset = dataset

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
    def ComputeScale(self):

        iplot = 0

        with open(
            os.path.join(self.main.archi_info.ma5dir, "madanalysis/input/LHAPDF.txt"), "r"
        ) as f:
            pdf_list = [int(line.split(",")[0]) for line in f.readlines()[1:]]

        # Loop over plot
        for iabshisto in range(0, len(self.main.selection)):

            # Keep only histogram
            if self.main.selection[iabshisto].__class__.__name__ != "Histogram":
                continue

            # Reset scale
            scale = 0.0
            multiweight_scale = 0.0
            if self.multiweight_histos[iplot]:
                self.multiweight_histos[iplot].set_central_weight_loc(
                    scale_choice=self.dataset.dynamic_scale_choice,
                    n_point_scale_variation=self.dataset.n_point_scale_variation,
                    central_pdfs=pdf_list,
                )

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
                multiweight_integral = 0
                if self.multiweight_histos[iplot]:
                    multiweight_integral = self.multiweight_histos[iplot].integral
                if integral > 0.0:
                    scale = 1.0 / integral
                else:
                    scale = 0.0
                if multiweight_integral > 0.0:
                    multiweight_scale = 1.0 / multiweight_integral

            # Case 2: No normalization
            elif self.main.normalize == NormalizeType.NONE:
                scale = 1.0
                multiweight_scale = 1.0

            # Case 3 and 4 : Normalization formula depends on LUMI
            #                or depends on WEIGHT+LUMI
            elif self.main.normalize in [NormalizeType.LUMI, NormalizeType.LUMI_WEIGHT]:

                # integral
                integral = (
                    self.histos[iplot].positive.integral
                    - self.histos[iplot].negative.integral
                )
                multiweight_integral, multiweight_eff = 0, 0
                if len(self.multiweight_histos) != 0:
                    multiweight_integral = self.multiweight_histos[iplot].integral

                # compute efficiency : Nevent / Ntotal
                if self.dataset.measured_global.nevents == 0:
                    eff = 0
                else:
                    nevt = float(self.dataset.measured_global.nevents)
                    eff = (
                        self.histos[iplot].positive.nevents
                        + self.histos[iplot].negative.nevents
                    ) / nevt
                    if self.multiweight_histos[iplot]:
                        multiweight_eff = self.multiweight_histos[iplot].nevents / nevt

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

                if self.multiweight_histos[iplot]:
                    entries_per_events = 0
                    sumw = self.multiweight_histos[iplot].central_sumw_over_events
                    Nentries = self.multiweight_histos[iplot].central_sumw_over_entries
                    if Nentries != 0:
                        entries_per_events = sumw / Nentries
                    # compute the scale
                    if multiweight_integral != 0:
                        multiweight_scale = (
                            thexsection
                            * self.main.lumi
                            * 1000
                            * multiweight_eff
                            * entries_per_events
                            / multiweight_integral
                        )
                    else:
                        multiweight_scale = 1  # no scale for empty plot
                        print("here no scale", multiweight_integral)

            # Setting the computing scale
            self.histos[iplot].scale = copy.copy(scale)
            if len(self.multiweight_histos) != 0:
                self.multiweight_histos[iplot].scale = copy.copy(multiweight_scale)
                print("scale", self.multiweight_histos[iplot].scale)

            # Incrementing counter
            iplot += 1
