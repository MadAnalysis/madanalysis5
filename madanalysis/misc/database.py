################################################################################
#
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#
#  This file is part of MadAnalysis 5.
#  Official website: <https://launchpad.net/madanalysis5>
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

from typing import Text
from math import sqrt

"""
TODO: Instead of using regiondata: dict we can change our datastructure to be more object oriented
      which will give us more/easy control over the data. These functions are written to replicate `regiondata`
      in run_recast.py file. In order not to change too much functionality `dict` properties have been implemented
      this will allow the complete transition to be spread over time.
"""

class Region:
    """
    Data structure for an analysis region

    :param analysis: name of the analysis
    :param regionID: name of the region
    :param nobs: number of observed events
    :param nb: number of expected events
    :param deltanb: total uncertainty on the expected events
    :param lumi: luminosity of the analysis
    """

    def __init__(
        self,
        regionID: Text,
        nobs: int,
        nb: float,
        deltanb: float,
        lumi: float = 0.0,
        analysis: Text = "__unknown_analysis__",
    ) -> None:
        self.analysis = analysis
        self.region_id = regionID

        self.nobs = nobs
        self.nb = nb
        self.deltanb = deltanb

        self.final_nevt = -1
        self.initial_nevt = -1
        self.xsec = -1
        self.lumi = lumi

        self.s95exp = -1
        self.s95obs = -1
        self.cls = -1
        self.best = 0

    # define getitem and setitem attributes for backwards compatibility.
    # This will allow us not to change current development too much.

    def __getitem__(self, item):
        input_key = item
        if item == "Nf":
            input_key == "final_nevt"
        elif item == "N0":
            input_key == "initial_nevt"
        return vars(self)[input_key]

    def __setitem__(self, key, value):
        input_key = key
        if key == "Nf":
            input_key == "final_nevt"
        elif key == "N0":
            input_key == "initial_nevt"
        elif key not in vars(self).keys():
            return

        setattr(input_key, value)

    @property
    def eff(self) -> float:
        """
        Get efficiency of the region
        :return: float
        """
        return self.final_nevt / self.initial_nevt

    @property
    def stat(self) -> float:
        """
        Statistical uncertainty of the final number of events
        """
        return sqrt(self.eff * (1 - self.eff) / abs(self.initial_nevt * self.lumi))

    @property
    def luminosity(self) -> float:
        return self.lumi

    @luminosity.setter
    def luminosity(self, value: float):
        self.lumi = value

    @property
    def nsignal(self):
        return self.xsec * self.lumi * 1000.0 * self.eff

    @property
    def n95(self) -> float:
        """
        number of expected events for excluded SR at 95% CL
        """
        return self.s95exp * self.lumi * 1000.0 * self.eff

    @property
    def rSR(self) -> float:
        """
        Ratio between number of signal events and number of expected events for excluded SR at 95% CL
        """
        return self.nsignal / self.n95


class RegionData:
    def __init__(self):
        self.regiondata = {}

    def keys(self):
        return self.regiondata.keys()

    def items(self):
        return self.regiondata.items()

    def __getitem__(self, item):
        return self.regiondata.get(item, {})

    def __setitem__(self, key, value):
        if isinstance(value, dict):
            self.regiondata[key] = Region(
                regionID=key, nobs=value["nobs"], nb=value["nb"], deltanb=value["deltanb"]
            )
