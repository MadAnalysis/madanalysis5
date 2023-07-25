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


from typing import Text, List, Dict, Any, Tuple
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
        self.name = self.name.replace("DYN_SCALE", "DYNSCALE")
        sectors = self.name.split("_")

        for sector in sectors:
            if "AUX" in sector:
                self._aux = int(sectors[1])
                break
            if "MERGING" in sector:
                self._merging = float(sector.split("=")[1])
            elif "DYNSCALE" in sector:
                self._dyn_scale = int(sector.split("=")[1])
            elif "MUF" in sector:
                self._muf = float(sector.split("=")[1])
            elif "MUR" in sector:
                self._mur = float(sector.split("=")[1])
            elif "PDF" in sector:
                self._pdf = int(sector.split("=")[1])

    def __repr__(self) -> Text:
        return (
            f"Weight(loc={self.loc}, pdf={self.pdfset}, "
            + f"muf={self.muf}, mur={self.mur}, dynamic={self.dynamic_scale}, "
            + f"merging={self.merging}, aux={self.aux})"
        )

    def __str__(self) -> Text:
        return self.__repr__()

    def to_dict(self) -> Dict[Text, Any]:
        """Convert to dictionary"""
        return {"loc": self.loc, "name": self.name}

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
        self._names = []

    def append(self, name: Text, idx: int) -> None:
        """Add weight into the collection"""
        if name not in self.names:
            self._collection.append(Weight(name=name, loc=idx))

    def __repr__(self) -> Text:
        if len(self) < 5:
            return "WeightCollection(" + ",".join([str(x) for x in self]) + ")"

        return f"WeightCollection(contains {len(self)} weight definitions)"

    def __str__(self) -> Text:
        return self.__repr__()

    def __iter__(self) -> Weight:
        yield from self._collection

    def __len__(self) -> int:
        return len(self._collection)

    def __getitem__(self, index: int) -> Weight:
        return self._collection[index]

    @property
    def names(self) -> List[Text]:
        """retreive weight names"""
        if len(self) == len(self._names):
            return self._names

        self._names = [x.name for x in self]
        return self._names

    def to_dict(self) -> List[Dict[Text, Any]]:
        """Convert to dictionary"""
        return [x.to_dict() for x in self]

    def from_dict(self, data: List[Dict[Text, Any]]) -> None:
        """Construct collection from data"""
        for dat in data:
            self.append(dat["name"], dat["loc"])

    @property
    def nominal(self) -> Weight:
        """Get nominal weight"""
        for w in self:
            if w.is_nominal:
                return w
        raise ValueError("Cannot find nominal weight")

    def group_for(self, group: Text) -> Dict:
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

    def get(self, **kwargs) -> List[Weight]:
        if len(kwargs) == 0:
            return []
        collection = []
        keys = ["muf", "mur", "dynamic_scale", "pdfset", "merging"]

        for weight in self:
            add = True
            for key in (key for key in keys if key in kwargs):
                if getattr(weight, key) != kwargs.get(key):
                    add = False
                    break
            if add:
                collection.append(weight)

        return WeightCollection(collection)

    @property
    def pdfsets(self) -> List[int]:
        """Retreive a list of pdfsets"""
        return np.unique([w.pdfset for w in self if w.pdfset is not None]).tolist()

    @property
    def scales(self) -> Dict[Text, List[float]]:
        """return scale for muf and mur"""
        muf = np.unique([w.muf for w in self if w.muf is not None]).tolist()
        muf.sort()
        mur = np.unique([w.mur for w in self if w.mur is not None]).tolist()
        mur.sort()
        return {"muf": muf, "mur": mur}

    @property
    def has_scale(self) -> bool:
        """is there any scale variations"""
        return (len(self.scales["mur"]) > 0) or (len(self.scales["muf"]) > 0)

    @property
    def central_scale(self) -> float:
        """retreive central scale"""
        scales = self.scales["muf"]
        return scales[len(scales) // 2]

    def get_scale_vars(self, point: int = 3, dynamic: int = None) -> Tuple:
        if dynamic is not None:
            dynamic = dynamic if self.has_dyn_scale(dynamic) else None

        scale_choices = []
        all_scale_choices = []
        for w in self:
            if w.dynamic_scale == dynamic:
                all_scale_choices.append([w.muf, w.mur])
                if point == 3:
                    if w.muf == w.mur and w.muf in [0.5, 1.0, 2.0]:
                        scale_choices.append([w.muf, w.mur])
                elif point == 7:
                    if [w.muf, w.mur] in [
                        [0.5, 0.5],
                        [0.5, 1.0],
                        [1.0, 0.5],
                        [1.0, 1.0],
                        [1.0, 2.0],
                        [2.0, 1.0],
                        [2.0, 2.0],
                    ]:
                        scale_choices.append([w.muf, w.mur])
                elif point == 9:
                    if w.muf in [0.5, 1.0, 2.0] and w.mur in [0.5, 1.0, 2.0]:
                        scale_choices.append([w.muf, w.mur])
                else:
                    scale_choices.append([w.muf, w.mur])

        if len(scale_choices) != point:
            scale_choices = all_scale_choices

        output = WeightCollection()
        for x in scale_choices:
            output += self.get_scale(dynamic=dynamic, muf=x[0], mur=x[1])

        return output

    def get_scale(
        self, dynamic: int = None, muf: float = 1.0, mur: float = 1.0
    ) -> List[Weight]:
        if dynamic is not None:
            dynamic = dynamic if self.has_dyn_scale(dynamic) else None
        return WeightCollection(
            [
                w
                for w in self
                if w.dynamic_scale == dynamic and w.muf == muf and w.mur == mur
            ]
        )

    def has_dyn_scale(self, scale: int) -> bool:
        """If weight collection has a particular dynamic scale"""
        assert scale in [1, 2, 3, 4], "invalid dynamic scale"
        for w in self:
            if w.dynamic_scale == scale:
                return True
        return False

    @property
    def loc(self) -> List[int]:
        """retreive the locations of the weights"""
        return [w.loc for w in self]

    def __iadd__(self, other):
        assert isinstance(other, WeightCollection)
        for items in other:
            self._collection.append(items)
        return self

    def __add__(self, other):
        assert isinstance(other, WeightCollection)
        for item in other:
            if item not in self._collection:
                self._collection.append(item)
            else:
                raise ValueError(f"{item} already exists.")
        return self
