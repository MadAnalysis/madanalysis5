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
import logging
import numpy as np
from madanalysis.layout.histogram_core import HistogramCore


class Histogram:
    def __init__(self):
        self.Reset()

    def Print(self):

        # General info
        inform = self.name + " " + str(self.nbins) + str(self.xmin) + " " + str(self.xmax)
        if self.ymin != [] or self.ymax != []:
            inform = inform + " " + str(self.ymin) + " " + str(self.ymax)
        logging.getLogger("MA5").info(inform)

        # Data
        self.positive.Print()
        self.negative.Print()
        self.summary.Print()

    def FinalizeReading(self, main, dataset):

        # convert everything to numpy arrays
        for loc in ["positive", "negative", "summary"]:
            for tp in [
                "nevents",
                "nentries",
                "sumwentries",
                "sumw",
                "sumw2",
                "sumwx",
                "sumw2x",
                "underflow",
                "overflow",
            ]:
                setattr(getattr(self, loc), tp, np.array(getattr(getattr(self, loc), tp)))

        # Statistics
        self.summary.nevents = np.array(self.positive.nevents) + np.array(self.negative.nevents)
        self.summary.nentries = np.array(self.positive.nentries) + np.array(self.negative.nentries)

        # sumw
        self.summary.sumw = np.clip(
            np.array(self.positive.sumw) - np.array(self.negative.sumw), 0, None
        )

        # sumw2
        self.summary.sumw2 = np.clip(
            np.array(self.positive.sumw2) - np.array(self.negative.sumw2), 0, None
        )

        # sumwx
        self.summary.sumwx = np.array(self.positive.sumwx) - np.array(self.negative.sumwx)
        # no correction on it

        # sumw2x
        self.summary.sumw2x = np.array(self.positive.sumw2x) - np.array(self.negative.sumw2x)
        # no correction on it

        # underflow
        self.summary.underflow = np.clip(
            np.array(self.positive.underflow) - np.array(self.negative.underflow), 0, None
        )

        # overflow
        self.summary.overflow = np.clip(
            np.array(self.positive.overflow) - np.array(self.negative.overflow), 0, None
        )

        # Data
        data = []
        for i, array in enumerate(self.positive.array):
            data.append(np.array(self.positive.array[i]) - np.array(self.negative.array[i]))
            if np.any(data[-1] < 0):
                self.warnings.append(
                    "dataset="
                    + dataset.name
                    + " -> bin "
                    + str(i)
                    + " has a negative content : "
                    + str(data[-1])
                    + ". This value is set to zero"
                )
                data[-1] = np.clip(data[-1], 0, None)
        self.summary.array = np.array(data[:])  # [:] -> clone of data

        # Integral
        self.positive.ComputeIntegral()
        self.negative.ComputeIntegral()
        self.summary.ComputeIntegral()

    def CreateHistogram(self):
        pass

    def Reset(self):

        # General info
        self.name = ""
        self.nbins = 100
        self.xmin = 0.0
        self.xmax = 100.0
        self.ymin = []
        self.ymax = []
        self.scale = 0.0

        # Data
        self.positive = HistogramCore()
        self.negative = HistogramCore()
        self.summary = HistogramCore()

        # ROOT histo
        self.myhisto = 0

        # warnings
        self.warnings = []

        # regions
        self.regions = []

    def GetRegions(self):
        return self.regions

    def GetBinLowEdge(self, bin):

        # Special case
        if bin <= 0:
            return self.xmin

        if bin >= self.nbins:
            return self.xmax

        # Computing steps
        step = (self.xmax - self.xmin) / float(self.nbins)

        # value
        return self.xmin + bin * step

    def GetBinUpperEdge(self, bin):

        # Special case
        if bin <= 0:
            return self.xmin

        if bin >= self.nbins:
            return self.xmax

        # Computing steps
        step = (self.xmax - self.xmin) / float(self.nbins)

        # value
        return self.xmin + (bin + 1) * step

    def GetBinMean(self, bin):

        # Special case
        if bin < 0:
            return self.xmin

        if bin >= self.nbins:
            return self.xmax

        # Computing steps
        step = (self.xmax - self.xmin) / float(self.nbins)

        # value
        return self.xmin + (bin + 0.5) * step
