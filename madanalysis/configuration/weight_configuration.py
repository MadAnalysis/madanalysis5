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


import copy
import logging
from dataclasses import dataclass, field
from typing import Any, Dict, List, Text, Tuple, Union

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
    _alphas: float = field(init=False, default=None)

    def __eq__(self, other):
        return all(
            getattr(other, attr) == getattr(self, attr) for attr in ["name", "loc"]
        )

    def __post_init__(self) -> None:
        self.name = self.name.replace("DYN_SCALE", "DYNSCALE")

        # Check the weight name and make sure its written according
        # to standard conventions, and update it if necessary. This is
        # needed due to terribly written HEPMC files by MadSTR.
        # These files include an `AUX` tag for every weight name.
        # Therefore, if the weight name is `AUX_XXX`with XXX being
        # an integer, then we have a true `AUX` weight. Otherwise, we
        # remove the `AUX` string from the weight name.
        portions = self.name.split("_")
        if portions[0].upper() == "AUX":
            if len(portions) > 2:
                # This is not a true AUX remove the AUX tag
                self.name = "_".join(portions[1:])
            elif len(portions) == 2:
                try:
                    tmp = int(portions[1])
                except ValueError as err:
                    logging.getLogger("MA5").error(
                        f"Unknown weight definition: {self.name}"
                    )
                    logging.getLogger("MA5").debug(err)
                    self.name = "_".join(portions[1:])
            else:
                logging.getLogger("MA5").error(f"Unknown weight definition: {self.name}")
                logging.getLogger("MA5").error(
                    "This weight definition will be considered as auxiliary."
                )

        sectors = self.name.split("_")

        # PYTHIA NOMINAL WEIGHT
        # -> note: to ignore, we need to use the MG5  nominal weight
        if self.name == "Weight":
            return

        for sector in sectors:
            if "AUX" in sector:
                self._aux = int(sectors[1])
                break
            elif any([x in sector for x in ["scomp", "smax", "smin"]]):
                self._aux = float(sector.split("=")[1])
                break
            if "MERGING" in sector:
                self._merging = float(sector.split("=")[1])
            elif "DYNSCALE" in sector:
                self._dyn_scale = int(sector.split("=")[1])
            elif "MUF" in sector:
                if not "=" in sector:
                    self._muf = float(sector.replace("MUF", "MUF=").split("=")[1])
                else:
                    self._muf = float(sector.split("=")[1])
            elif "MUR" in sector:
                if not "=" in sector:
                    self._mur = float(sector.replace("MUR", "MUR=").split("=")[1])
                else:
                    self._mur = float(sector.split("=")[1])
            elif "PDF" in sector:
                self._pdf = int(sector.split("=")[1])
            elif "ALPSFACT" in sector:
                self._alphas = float(sector.split("=")[1])

    def __repr__(self) -> Text:
        return (
            f"Weight(loc={self.loc}, pdf={self.pdfset}, "
            + f"muf={self.muf}, mur={self.mur}, dynamic={self.dynamic_scale}, "
            + f"merging={self.merging}, aux={self.aux}, "
            + f"alpsfac={self.alphas})"
        )

    def __str__(self) -> Text:
        return self.__repr__()

    def to_dict(self) -> Dict[Text, Any]:
        """Convert to dictionary"""
        return {"loc": self.loc, "name": self.name}

    @property
    def aux(self) -> int:
        """retrieve aux value"""
        return self._aux

    @property
    def dynamic_scale(self) -> int:
        """retrieve dynamic scale"""
        return self._dyn_scale

    @property
    def muf(self) -> float:
        """retrieve the factorisation scale variation"""
        return self._muf

    @property
    def mur(self) -> float:
        """retrieve the renormalisation scale variation"""
        return self._mur

    @property
    def pdfset(self) -> int:
        """retrieve pdf set id"""
        return self._pdf

    @property
    def merging(self) -> float:
        """retrieve merging scale"""
        return self._merging

    @property
    def alphas(self) -> float:
        """retrieve the alpha_s variation"""
        return self._alphas


class WeightCollection:
    """Create a weight collection"""

    def __init__(self, collection: List[Weight] = None):
        self._collection: List[Weight] = [] if collection is None else collection
        self._names = []
        self._nominal: Weight = None

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
        """retrieve weight names"""
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

    def nominal(self, scale_choice: int, central_pdfs) -> Weight:
        """Get nominal weight"""
        weight_set = []
        if self._nominal is None:
            for w in self:
                if any([not x is None for x in [w.aux, w.alphas]]):
                    continue
                if w.muf != 1.0 or w.mur != 1.0 or w.dynamic_scale != scale_choice:
                    continue
                if not w.pdfset in central_pdfs:
                    continue
                weight_set.append(w)
            if len(weight_set) > 1:
                logging.getLogger("MA5").warning(
                    "Variations on multi-pdf sets are not supported at the moment."
                    f"{central_pdfs[weight_set[0].pdfset].name} will be used instead"
                )

        return weight_set[0]

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

    def pdfset(self, pdfid: Union[int, List[int]]) -> List[Weight]:
        """retrieve weights corresponding to one pdf set"""
        if isinstance(pdfid, int):
            return WeightCollection([w for w in self if w.pdfset == pdfid])
        return WeightCollection([self.get(pdfset=pdf) for pdf in pdfid])

    def get(self, **kwargs) -> List[Weight]:
        if len(kwargs) == 0:
            return []
        collection = []
        keys = ["muf", "mur", "dynamic_scale", "pdfset", "merging", "alphas"]

        for weight in self:
            add = True
            for key in (key for key in keys if key in kwargs):
                if getattr(weight, key) != kwargs.get(key):
                    add = False
                    break
            if add and weight not in collection:
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
    def has_pdf(self) -> bool:
        """is there any pdf definition"""
        return len(self.pdfsets) > 0

    @property
    def central_scale(self) -> float:
        """retrieve central scale"""
        scales = self.scales["muf"]
        return scales[len(scales) // 2]

    def get_scale_vars(self, point: int = 3, dynamic: int = None) -> Tuple:
        if dynamic is not None:
            dynamic = dynamic if self.has_dyn_scale(dynamic) else None

        scale_choices = []
        all_scale_choices = []
        for w in self:
            if not w.alphas is None:
                continue
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
        output = WeightCollection()
        for w in self:
            if (
                w.dynamic_scale == dynamic
                and w.muf == muf
                and w.mur == mur
                and w.alphas is None
            ):
                output += w
        return output

    def has_dyn_scale(self, scale: int) -> bool:
        """If weight collection has a particular dynamic scale"""
        assert scale in [1, 2, 3, 4], "invalid dynamic scale"
        for w in self:
            if w.dynamic_scale == scale:
                return True
        return False

    @property
    def loc(self) -> List[int]:
        """retrieve the locations of the weights"""
        return np.unique([w.loc for w in self]).tolist()

    def __iadd__(self, other):
        if isinstance(other, WeightCollection):
            for items in other:
                if items not in self._collection:
                    self._collection.append(items)
        elif isinstance(other, Weight):
            if other not in self._collection:
                self._collection.append(other)
        return self

    def __add__(self, other):
        current_col = copy.deepcopy(self._collection)
        if isinstance(other, WeightCollection):
            for item in other:
                if item not in current_col:
                    current_col.append(item)
        elif isinstance(other, Weight):
            if other not in current_col:
                current_col.append(other)
        return WeightCollection(current_col)
