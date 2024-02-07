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
from math import sqrt
import numpy as np


class HistogramCore:
    def __init__(self):

        # statistics
        # - int
        self.nevents = 0
        self.nentries = 0
        # - float
        self.integral = 0.0
        self.sumwentries = 0.0
        self.sumw = 0.0
        self.sumw2 = 0.0
        self.sumwx = 0.0
        self.sumw2x = 0.0

        # content
        self.underflow = 0.0
        self.overflow = 0.0
        self.nan = 0.0
        self.inf = 0.0
        self.array = []
        self.array_full = []

    def convert_to_numpy(self) -> None:
        """Convert data containers into numpy arrays for convenience"""
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
            setattr(self, tp, np.array(getattr(self, tp)))

    def ComputeIntegral(self):
        self.integral = np.sum(self.array, axis=0)
        self.integral += self.overflow
        self.integral += self.underflow

    def Print(self):

        logging.getLogger("MA5").info(
            "nevents=" + str(self.nevents) + " entries=" + str(self.nentries)
        )

        logging.getLogger("MA5").info(
            "sumw="
            + str(self.sumw)
            + " sumw2="
            + str(self.sumw2)
            + " sumwx="
            + str(self.sumwx)
            + " sumw2x="
            + str(self.sumw2x)
        )

        logging.getLogger("MA5").info(
            "underflow=" + str(self.underflow) + " overflow=" + str(self.overflow)
        )

    def GetMean(self):
        if self.sumw == 0:
            return 0.0

        return self.sumwx / self.sumw

    def GetRMS(self):
        if self.sumw == 0:
            return 0.0

        mean = self.GetMean()
        return sqrt(abs(self.sumw2x / self.sumw - mean * mean))
