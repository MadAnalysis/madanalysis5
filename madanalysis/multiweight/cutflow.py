"""This file includes classes for multiweight cutflows"""

# import warnings
from dataclasses import dataclass
from typing import List, Text, Tuple

import numpy as np

from madanalysis.configuration.weight_configuration import Weight, WeightCollection
from madanalysis.enumeration.normalize_type import NormalizeType

from .bin import MultiWeightBin

__all__ = ["MultiWeightCut", "MultiWeightCutFlow"]


class Entries:
    def __init__(self, positive: List[float], negative: List[float]):
        assert len(positive) == len(negative), "Dimensionalities does not match"
        self.pos = MultiWeightBin(positive)
        self.neg = MultiWeightBin(negative)

    def __len__(self):
        return len(self.pos)

    def __call__(self) -> MultiWeightBin:
        return self.pos - abs(self.neg)

    def __mul__(self, other) -> MultiWeightBin:
        if isinstance(other, Entries):
            return self() * other()

        return self() * other

    __rmul__ = __mul__

    def __truediv__(self, other):
        if isinstance(other, Entries):
            return MultiWeightBin(self().weights / other().weights)

        return MultiWeightBin(self().weights / other)


class MultiWeightCut:
    """Cut definition"""

    __slots__ = ["nentries", "sumw", "sumw2", "name", "region"]

    def __init__(self):
        self.nentries: Entries = Entries([], [])
        self.sumw: Entries = Entries([], [])
        self.sumw2: Entries = Entries([], [])
        self.name: Text = "__unknonw_cut__"
        self.region: Text = "__unknonw_region__"

    def __repr__(self) -> Text:
        return f"{self.name}[{len(self)} weights]"

    def __str__(self) -> Text:
        return self.__repr__()

    def __len__(self) -> int:
        return len(self.nentries)

    def yields(
        self, xsec: float, lumi: float, initial_sumw: MultiWeightBin
    ) -> MultiWeightBin:
        """Compute yields"""
        if self.nentries()[0] == 0:
            return MultiWeightBin([0.0] * len(self))

        return xsec * lumi * self.sumw / initial_sumw

    @property
    def is_valid(self) -> bool:
        """Is the given cut valid"""
        return len(self.nentries) == len(self.sumw) == len(self.sumw2)

    def add(self, attr: Text, positive: List[float], negative: List[float]) -> None:
        """add an attribute"""
        setattr(self, attr, Entries(positive, negative))


class MultiWeightCutFlow:
    def __init__(self, main, dataset):
        self.cutflow: List[MultiWeightCut] = []
        self.main = main
        self.dataset = dataset
        self._nominal_weight: Weight = None

    def __repr__(self) -> Text:
        return (
            f"MultiWeightCutFlow(ncuts: {len(self)})("
            + ", ".join(str(c) for c in self)
            + ")"
        )

    def __str__(self) -> Text:
        return self.__repr__()

    @property
    def numb_regions(self) -> int:
        """number of regions"""
        return len(self.main.regions.GetNames())

    @property
    def nominal_weight(self) -> Weight:
        """retreive weight collection"""
        if self._nominal_weight is None:
            self._nominal_weight = self.dataset.weight_collection.nominal(
                self.dataset.dynamic_scale_choice, self.main.lhapdf_info
            )
        return self._nominal_weight

    @property
    def has_scale(self) -> bool:
        """Does the weight collection has scale definitions"""
        return self.dataset.weight_collection.has_scale

    @property
    def has_pdf(self) -> bool:
        """Does the weight collection has pdf definitions"""
        return self.dataset.weight_collection.has_pdf

    @property
    def is_signal(self) -> bool:
        """Is this dataset signal"""
        return not self.dataset.background

    @property
    def xsec(self) -> float:
        """Cross section of the dataset"""
        weight = 1.0 if self.main.normalize == NormalizeType.LUMI else self.dataset.weight
        xsec = self.dataset.xsection
        return (xsec if xsec != 0.0 else self.dataset.measured_global.xsection) * weight

    def __len__(self) -> int:
        return len(self.cutflow)

    def __getitem__(self, item: int) -> MultiWeightCut:
        return self.cutflow[item]

    def __iter__(self) -> MultiWeightCut:
        yield from self.cutflow

    @property
    def names(self) -> List[Text]:
        """List cut names"""
        return [cut.name for cut in self]

    def append(self, cut: MultiWeightCut) -> None:
        """Add a new cut"""
        assert isinstance(cut, MultiWeightCut)
        if len(cut) > 1:
            if len(self) > 0:
                assert len(cut) == len(self[0])
                self.cutflow.append(cut)
            else:
                self.cutflow.append(cut)

    def scale_loc(self) -> List[int]:
        """Location of scale variations"""
        return (
            self.dataset.weight_collection.get_scale_vars(
                self.dataset.n_point_scale_variation, self.dataset.dynamic_scale_choice
            )
            .pdfset(self.nominal_weight.pdfset)
            .loc
        )

    @staticmethod
    def scale_uncertainties(
        yields: MultiWeightBin, nominal_loc: int, scale_loc: List[int]
    ) -> Tuple[float, float, float]:
        """
        Compute scale uncertainties

        Args:
            yields (``MultiWeightBin``): yields for the cut
            nominal_loc (``int``): location of nominal weight
            scale_loc (``List[int]``): location of the scale vars

        Returns:
            ``Tuple[float, float, float]``:
            nominal yields, lower unc, upper unc
        """
        nominal_yield = yields[nominal_loc]
        scale_region = [yields[loc] for loc in scale_loc]
        return (
            nominal_yield,
            nominal_yield - min(scale_region),
            max(scale_region) - nominal_loc,
        )

    @staticmethod
    def pdf_uncertainties(
        yields: MultiWeightBin,
        nominal_loc: int,
        pdf_var_loc: List[int],
        method: Text = "replicas",
    ) -> Tuple[float, float, float]:

        nominal_yield = yields[nominal_loc]
        all_weights = [yields[loc] for loc in pdf_var_loc]

        # Replicas
        if method == "replicas":
            # 2202.13416 eq 3.2
            central = np.mean([nominal_yield] + all_weights)
            var = np.sqrt(np.mean(np.square(all_weights - central)))
            return central, var, var

        current_upper_bin, current_lower_bin = [], []
        for idx in range(0, len(pdf_var_loc), 2):
            current_upper_bin.append(
                np.square(
                    max(
                        [
                            all_weights[pdf_var_loc[idx]] - nominal_yield,
                            all_weights[pdf_var_loc[idx + 1]] - nominal_yield,
                            0.0,
                        ]
                    )
                )
            )
            current_lower_bin.append(
                np.square(
                    max(
                        [
                            -all_weights[pdf_var_loc[idx]] + nominal_yield,
                            -all_weights[pdf_var_loc[idx + 1]] + nominal_yield,
                            0.0,
                        ]
                    )
                )
            )
        upper = np.sqrt(sum(current_upper_bin))
        lower = np.sqrt(sum(current_lower_bin))

        return nominal_yield, lower, upper

    @property
    def get_cutflow(self) -> List[Tuple[Text, float, float, float]]:

        cutflow = []
        initial_sumw = self[0].sumw()

        def quadrature(nominal: float, pdf: float, scale: float) -> float:
            if nominal > 0.0:
                return np.sqrt((pdf / nominal) ** 2 + (scale / nominal) ** 2) * nominal
            return 0.0

        pdf_locs = []
        if self.has_pdf:
            for idx, pdfid in enumerate(
                self.main.lhapdf_info[self.nominal_weight.pdfset]
            ):
                if idx != 0:
                    pdf_locs.append(
                        *self.dataset.weight_collection.get(
                            pdfset=pdfid, muf=1.0, mur=1.0
                        ).loc
                    )

        for cut in self:
            nominal, lower_scale, upper_scale, lower_pdf, upper_pdf = [0.0] * 5
            yields = cut.yields(self.xsec, self.main.lumi * 1000, initial_sumw)
            if self.has_scale:
                nominal, lower_scale, upper_scale = self.scale_uncertainties(
                    yields, self.nominal_weight.loc, self.scale_loc()
                )
            if self.has_pdf:
                nominal, lower_pdf, upper_pdf = self.pdf_uncertainties(
                    yields, self.nominal_weight.loc, pdf_locs, method="replicas"
                )

            cutflow.append(
                (
                    cut.name,
                    nominal,
                    quadrature(nominal, lower_pdf, lower_scale),
                    quadrature(nominal, upper_pdf, upper_scale),
                )
            )

        return cutflow
