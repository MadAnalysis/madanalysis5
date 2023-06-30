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


from typing import Text, List
from dataclasses import dataclass, field
import numpy as np


@dataclass
class Weight:
    """Weight identifier class"""

    name: Text
    loc: int
    _aux: int = field(init=False, default=None)
    _dyn_scale: int = field(init=False, default=None)
    _muf: float = field(init=False, default=None)
    _mur: float = field(init=False, default=None)
    _pdf: int = field(init=False, default=None)
    _merging: float = field(init=False, default=None)

    def __post_init__(self) -> None:
        sectors = self.name.split("_")

        for sector in sectors:
            if "AUX" in sector:
                self._aux = int(sectors[1])
                break
            if "MERGING" in sector:
                self._merging = float(sector.split("=")[1])
            elif "DYN_SCALE" in sector:
                self._dyn_scale = int(sector.split("=")[1])
            elif "MUF" in sector:
                self._muf = float(sector.split("=")[1])
            elif "MUR" in sector:
                self._mur = float(sector.split("=")[1])
            elif "PDF" in sector:
                self._pdf = int(sector.split("=")[1])

    @property
    def aux(self) -> int:
        """retreive aux value"""
        return self._aux

    @property
    def dynamic_scale(self) -> int:
        """retreive dynamic scale"""
        return self._dyn_scale

    @property
    def muf(self) -> float:
        """retreive factorisation scale"""
        return self._muf

    @property
    def mur(self) -> float:
        """retreive **forget the name** scale"""
        return self._mur

    @property
    def pdfset(self) -> int:
        """retreive pdf set id"""
        return self._pdf

    @property
    def merging(self) -> float:
        """retreive merging scale"""
        return self._merging

    @property
    def is_nominal(self) -> bool:
        """Return true if the weight is nominal"""
        if all(
            x is None
            for x in [
                self.aux,
                self.pdfset,
                self.dynamic_scale,
                self.muf,
                self.mur,
                self.merging,
            ]
        ):
            return True

        return False


class WeightCollection:
    """Create a weight collection"""

    def __init__(self, collection: List[Weight] = None):
        self._collection = [] if collection is None else collection

    def append(self, name: Text, idx: int):
        """Add weight into the collection"""
        self._collection.append(Weight(name=name, loc=idx))

    def __iter__(self):
        yield from self._collection

    @property
    def nominal(self) -> Weight:
        """Get nominal weight"""
        for w in self:
            if w.is_nominal:
                return w
        raise ValueError("Can not find nominal weight")

    def group_for(self, group: Text):
        """Create a group"""
        assert group in ["muf", "mur", "pdfset", "dynamic_scale"]

        unique = np.unique(
            [getattr(w, group) for w in self if getattr(w, group) is not None]
        )

        if len(unique) == 0:
            return {}

        group_dict = {v: [] for v in unique}
        for w in self:
            if getattr(w, group) is not None:
                group_dict[getattr(w, group)].append(w)

        return group_dict

    def pdfset(self, pdfid: int) -> List[Weight]:
        """retreive weights corresponding to one pdf set"""
        return WeightCollection([w for w in self if w.pdfset == pdfid])

    @property
    def pdfsets(self) -> List[int]:
        """Retreive a list of pdfsets"""
        return np.unique([w.pdfset for w in self if w.pdfset is not None]).tolist()
