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

import json
from dataclasses import dataclass
from typing import Dict, List, Text, Union

import numpy as np

from madanalysis.configuration.weight_configuration import WeightCollection

from .histogram import Histogram


@dataclass
class HistogramProcessor:
    """
    Processor for multiweight histograms

    Args:
        positive_bin_weights (``Union[List[float], np.ndarray]``): positive sum of weights per bin
        negative_bin_weights (``Union[List[float], np.ndarray]``): negative sum of weights per bin
        weight_names (``Dict[int, Text]``): names of the weights
        xsection (``float``): cross section of the data
        name (``Text``): name of the histogram
    """

    histogram: Histogram
    weight_collection: WeightCollection
    total_nevents: int
    xsection: float
    name: Text = "__unknown_histogram__"

    def __post_init__(self):
        positive_bin_weights = np.array(self.histogram.positive.array)
        negative_bin_weights = np.abs(np.array(self.histogram.negative.array))

        positive_sumw = np.array(self.histogram.positive.sumw)
        negative_sumw = np.abs(np.array(self.histogram.negative.sumw))
        self.sumw = positive_sumw - negative_sumw

        positive_sumwentries = np.array(self.histogram.positive.sumwentries)
        negative_sumwentries = np.abs(np.array(self.histogram.negative.sumwentries))
        self.sumwentries = positive_sumwentries - negative_sumwentries

        positive_nevents = np.array(self.histogram.positive.nevents)
        negative_nevents = np.abs(np.array(self.histogram.negative.nevents))
        nevents = positive_nevents + negative_nevents

        self.eff = 0 if self.total_nevents == 0 else nevents[0] / self.total_nevents

        positive_overflow = np.array(self.histogram.positive.overflow)
        negative_overflow = np.abs(np.array(self.histogram.negative.overflow))
        self.overflow = positive_overflow - negative_overflow

        positive_underflow = np.array(self.histogram.positive.underflow)
        negative_underflow = np.abs(np.array(self.histogram.negative.underflow))
        self.underflow = positive_underflow - negative_underflow

        assert (
            positive_bin_weights.shape == negative_bin_weights.shape
        ), "Invalid bin shapes."

        self.bin_weights = positive_bin_weights - negative_bin_weights
        self.integral = np.sum(self.bin_weights, axis=0) + self.overflow + self.underflow

    def scale(self, lumi: float, scale_choice: int, central_pdfs: np.array) -> float:
        """
        Compute the scale of the nominal histogram

        Args:
            lumi (``float``): luminosity in fb-1
            scale_choice (``int``): tag indicating how the central scale choice has been made
            central_pdfs (``np.array``): list with all acceptable PDF choices for the central set

        Returns:
            ``float``:
            scale of the histogram
        """
        # find nominal weight location
        idx = self.weight_collection.nominal(
            scale_choice=scale_choice, central_pdfs=central_pdfs
        ).loc

        if self.integral[idx] == 0:
            return 0.0

        sumw = self.sumw[idx]
        sumwentries = self.sumwentries[idx]
        entries_per_events = 0 if sumwentries == 0 else sumw / sumwentries

        return float(
            self.xsection
            * lumi
            * 1000.0
            * self.eff
            * entries_per_events
            / self.integral[idx]
        )

    def uncertainty(self, lumi: float) -> np.ndarray:
        pass

    def to_json(self, file_path: "."):
        """Convert histogram to json file"""
        pass
