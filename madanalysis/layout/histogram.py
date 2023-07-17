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

        self.positive.convert_to_numpy()
        self.negative.convert_to_numpy()
        self.summary.convert_to_numpy()

        # Statistics
        self.summary.nevents = self.positive.nevents + self.negative.nevents
        self.summary.nentries = self.positive.nentries + self.negative.nentries

        # sumw
        self.summary.sumw = np.clip(self.positive.sumw - self.negative.sumw, 0, None)

        # sumw2
        self.summary.sumw2 = np.clip(self.positive.sumw2 - self.negative.sumw2, 0, None)

        # sumwx
        self.summary.sumwx = self.positive.sumwx - self.negative.sumwx
        # no correction on it

        # sumw2x
        self.summary.sumw2x = self.positive.sumw2x - self.negative.sumw2x
        # no correction on it

        # underflow
        self.summary.underflow = np.clip(
            self.positive.underflow - self.negative.underflow, 0, None
        )

        # overflow
        self.summary.overflow = np.clip(
            self.positive.overflow - self.negative.overflow, 0, None
        )

        # compute mean and uncertainties for the statistics
        # ! @jackaraz: this portion of the code should be changed to accomodate different types of
        # ! PDF + scale unc combination for now its just mean and std
        for tp in [
            "nevents",
            "nentries",
            "sumw",
            "sumw2",
            "sumwx",
            "sumw2x",
            "underflow",
            "overflow",
        ]:
            # compute unc shape: (lower, upper)
            std = float(np.std(getattr(self.summary, tp)))
            setattr(self.summary, tp + "_unc", (std, std))
            # compute mean
            setattr(self.summary, tp, float(np.mean(getattr(self.summary, tp))))

        # Data
        data = []
        for i, array in enumerate(self.positive.array):
            data.append(np.array(array) - np.array(self.negative.array[i]))
            if np.any(data[-1] < 0):
                self.warnings.append(
                    f"dataset={dataset.name} -> bin {i} has a negative content : "
                    f"{str(data[-1])}. This value is set to zero."
                )
                data[-1] = np.clip(data[-1], 0, None)
        self.summary.array_full = np.array(data[:])  # [:] -> clone of data

        # Compute the mean and the error on the data
        # mean shape should be (Nbins, ) and the histogram unc shape should be (Nbins, 2)
        # where first column is the lower envelop and second is upper envelop
        histogram_mean = np.mean(self.summary.array_full, axis=1)
        self.summary.array = histogram_mean.reshape(-1)

        # Integral
        self.positive.ComputeIntegral()
        self.negative.ComputeIntegral()
        self.summary.ComputeIntegral()

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
