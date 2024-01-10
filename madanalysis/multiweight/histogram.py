"""This file includes classes for multiweight histograms"""

from dataclasses import dataclass
from typing import Callable, List, Text, Tuple, Union
from collections import namedtuple
import copy
import numpy as np
from madanalysis.configuration.weight_configuration import WeightCollection
from madanalysis.enumeration.stacking_method_type import StackingMethodType


_nevt = namedtuple("events", ["positive", "negative"])

# pylint: disable=C0103


@dataclass
class Description:
    """Histogram Description"""

    nbins: int
    xmin: float
    xmax: float

    def GetBinLowEdge(self, bn: int) -> float:
        """
        Retreive lower edge of the bin

        Args:
            bn (``int``): bin
        """

        # Special case
        if bn <= 0:
            return self.xmin

        if bn >= self.nbins:
            return self.xmax

        # Computing steps
        step = (self.xmax - self.xmin) / float(self.nbins)

        # value
        return self.xmin + bn * step

    def GetBinUpperEdge(self, bn: int) -> float:
        """
        retreive upper edge of the bin

        Args:
            bn (``int``): bin
        """

        # Special case
        if bn <= 0:
            return self.xmin
        if bn >= self.nbins:
            return self.xmax
        # Computing steps
        step = (self.xmax - self.xmin) / float(self.nbins)

        # value
        return self.xmin + (bn + 1) * step

    def GetBinMean(self, bn: int) -> float:
        """
        Get mean of the bin

        Args:
            bn (``int``): bin
        """

        # Special case
        if bn < 0:
            return self.xmin
        if bn >= self.nbins:
            return self.xmax
        # Computing steps
        step = (self.xmax - self.xmin) / float(self.nbins)
        # value
        return self.xmin + (bn + 0.5) * step


@dataclass
class WeightNames:
    """
    Representation of weight names

    Args:
        names (`List[Text]`): name of the weights
    """

    names: List[Text]

    def __getitem__(self, idx: int) -> Text:
        return self.names[idx]

    def __len__(self) -> int:
        return len(self.names)

    def __iter__(self) -> Text:
        yield from self.names

    def __eq__(self, other) -> bool:
        if not isinstance(other, WeightNames):
            return False

        return all(i == j for i, j in zip(self, other))


class MultiWeightBin:
    """
    Representation of a multiweight bin

    Args:
        weights (`List[float]`): sum of weights within the bin per weight
    """

    contract: Callable[[np.ndarray], float] = np.mean
    error: Callable[[np.ndarray], Union[float, Tuple[float, float]]] = np.std

    def __init__(self, weights: List[float]):
        self.weights = np.array(weights)

    def __repr__(self):
        return f"MultiWeightBin({len(self)} weights)"

    def __str__(self):
        return self.__repr__()

    @staticmethod
    def set_contractor(func: Callable[[np.ndarray], float]) -> None:
        """
        Set contractor function which computes central value for the bin

        Args:
            func (``Callable[[np.ndarray], float]``): contractor function
        """
        MultiWeightBin.contract = func

    @staticmethod
    def set_error(func: Union[float, Tuple[float, float]]) -> None:
        """
        Set error function

        Args:
            func (``Union[float, Tuple[float, float]]``): error function
        """
        MultiWeightBin.error = func

    @property
    def central_value(self) -> float:
        """Retreive the central value of the bin"""
        return float(MultiWeightBin.contract(self.weights))

    @property
    def error(self) -> Union[float, Tuple[float, float]]:
        """Retreive the error for the bin"""
        err = MultiWeightBin.error(self.weights)
        if isinstance(err, (tuple, list)):
            return float(err[0]), float(err[1])
        return float(err)

    def __getitem__(self, idx: int) -> float:
        return self.weights[idx]

    def __len__(self) -> int:
        return len(self.weights)

    def __iter__(self) -> float:
        yield from self.weights

    def __add__(self, other):
        if isinstance(other, MultiWeightBin):
            assert len(other) == len(self), "Dimensionality does not match"
            return MultiWeightBin(other.weights + copy.deepcopy(self.weights))
        elif isinstance(other, (int, float)):
            return MultiWeightBin(other + copy.deepcopy(self.weights))

        raise ValueError("Unknown operation")

    __radd__ = __add__

    def __iadd__(self, other):
        return self.__add__(other)

    def __sub__(self, other):
        other = -other
        return self.__add__(other)

    __rsub__ = __sub__

    def __pos__(self):
        return self

    def __neg__(self):
        return MultiWeightBin(-1.0 * copy.deepcopy(self.weights))

    def __mul__(self, other):
        if isinstance(other, MultiWeightBin):
            assert len(other) == len(self), "Dimensionality does not match"
            return MultiWeightBin(other.weights * copy.deepcopy(self.weights))
        elif isinstance(other, (int, float)):
            return MultiWeightBin(other * copy.deepcopy(self.weights))

        raise ValueError("Unknown operation")

    __rmul__ = __mul__

    def __abs__(self):
        return MultiWeightBin(np.abs(self.weights))


class MultiWeightHisto:
    """
    Multiweight histogram definition

    Args:
        name (``Text``, default ``"__unknown_histo__"``): name of the histogram
    """

    def __init__(
        self, name: Text = "__unknown_histo__", weight_collection: WeightCollection = None
    ):
        self.name = name
        self.scale = 0.0
        # Scale of the histogram
        self.central_idx = 0
        # Central weight location
        self.nominal_weight = None
        self.dynamic_scale_choice = None
        self.n_point_scale_variation = None

        self._positive_weights: List[MultiWeightBin] = None
        # positive weights
        self._negative_weights: List[MultiWeightBin] = None
        # negative weights
        self.weight_collection: WeightCollection = weight_collection
        # weights names

        self.overflow: Tuple[MultiWeightBin, MultiWeightBin] = (None, None)
        self.underflow: Tuple[MultiWeightBin, MultiWeightBin] = (None, None)

        self._desc: Description = None
        self.regions: List[Text] = []

        self._nevents: _nevt = None
        self._nentries: _nevt = None
        # Number of events

        self.sumw_over_events: Tuple[MultiWeightBin, MultiWeightBin] = (None, None)
        # Sum of event weights over events
        self.sumw_over_entries: Tuple[MultiWeightBin, MultiWeightBin] = (None, None)
        # Sum of event weights over entries
        self.sumw2: Tuple[MultiWeightBin, MultiWeightBin] = (None, None)
        # Sum of weights squared
        self.sum_value_weights: Tuple[MultiWeightBin, MultiWeightBin] = (None, None)
        # Sum of value times weights
        self.sum_value2_weights: Tuple[MultiWeightBin, MultiWeightBin] = (None, None)
        # Sum of value squared times weights

    def __repr__(self):
        return (
            "MultiWeightHisto("
            + f"name={self.name}, "
            + str(self.nevents)
            + ", "
            + str(self._desc)
            + ", "
            + str(self._positive_weights)
            + ")"
        )

    def set_central_weight_loc(
        self, scale_choice: int, n_point_scale_variation: int, central_pdfs: List[int]
    ) -> None:
        self.nominal_weight = self.weight_collection.nominal(scale_choice, central_pdfs)
        self.central_idx = self.nominal_weight.loc
        self.dynamic_scale_choice = scale_choice
        self.n_point_scale_variation = n_point_scale_variation
        print("Central PDF loc:", self.central_idx)

    @property
    def is_consistent(self) -> bool:
        """Is histogram consistent"""
        if self._positive_weights is None or self._negative_weights is None:
            return False
        weight_col = len(self.weight_collection)
        check_pos_weights = all(weight_col == len(bn) for bn in self._positive_weights)
        check_neg_weights = all(weight_col == len(bn) for bn in self._negative_weights)
        check_overflow = all(
            [weight_col == len(self.overflow[0]), weight_col == len(self.overflow[1])]
        )
        check_underflow = all(
            [weight_col == len(self.underflow[0]), weight_col == len(self.underflow[1])]
        )

        return all(
            [check_pos_weights, check_neg_weights, check_overflow, check_underflow]
        )

    @property
    def description(self) -> Description:
        """Description"""
        return self._desc

    def set_description(self, nbins: int, xmin: float, xmax: float) -> None:
        """Set histogram description"""
        self._desc = Description(nbins=nbins, xmin=xmin, xmax=xmax)

    def set_nevents(self, positive: int, negative: int) -> None:
        """Set number of events"""
        self._nevents = _nevt(positive=positive, negative=negative)

    @property
    def nevents(self) -> _nevt:
        """retreive number of events"""
        return self._nevents.positive + self._nevents.negative

    def set_nentries(self, positive: int, negative: int) -> None:
        """set number of entries"""
        self._nentries = _nevt(positive=positive, negative=negative)

    @property
    def nentries(self) -> _nevt:
        """retreive number of entries"""
        return self._nentries.positive + self._nentries.negative

    @property
    def shape(self) -> Tuple[int, int]:
        """
        Returns dimensionality of the histogram

        Returns:
            ``Tuple[int,int]``:
            Number of weights, number of bins
        """
        numb_of_weights = len(self.weight_collection)
        numb_of_bins = 0

        if self._positive_weights is not None:
            numb_of_bins = len(self._positive_weights)
        elif self._negative_weights is not None:
            numb_of_bins = len(self._negative_weights)
        return (numb_of_weights, numb_of_bins)

    def append_positive_weights(self, weights: List[float]) -> None:
        """
        Add positive weights

        Args:
            weights (``List[float]``): weights
        """
        if self._positive_weights is not None:
            assert len(weights) == len(
                self._positive_weights[-1]
            ), "Dimensionality does not match"
            self._positive_weights.append(MultiWeightBin(weights))
        else:
            self._positive_weights = [MultiWeightBin(weights)]

    def append_negative_weights(self, weights: List[float]) -> None:
        """
        Add negative weights

        Args:
            weights (``List[float]``): weights
        """
        if self._negative_weights is not None:
            assert len(weights) == len(
                self._negative_weights[-1]
            ), "Dimensionality does not match"
            self._negative_weights.append(MultiWeightBin(weights))
        else:
            self._negative_weights = [MultiWeightBin(weights)]

    def line_to_bin(self, line: Text) -> None:
        """
        Convert line to histogram bin

        Args:
            line (``Text``): a dataline from SAF file
        """
        positive, negative = [], []
        for iw, word in enumerate(line.split()):
            if word == "#":
                break
            if iw % 2 == 0:
                positive.append(float(word))
            else:
                negative.append(float(word))
        self.append_positive_weights(positive)
        self.append_negative_weights(negative)

    def weights_to_bin(
        self, dest: Text, weights: Tuple[List[float], List[float]]
    ) -> None:
        if len(weights) == 2:
            weights = (
                MultiWeightBin(weights=weights[0]),
                MultiWeightBin(weights=weights[1]),
            )
        else:
            weights = MultiWeightBin(weights=weights)
        setattr(self, dest, weights)

    @property
    def integral(self):
        """Compute the integral of the histogram"""
        return (
            sum(self._positive_weights)
            + self.underflow[0]
            + self.overflow[0]
            - (
                abs(sum(self._negative_weights))
                + abs(self.underflow[1])
                + abs(self.overflow[1])
            )
        )[self.central_idx]

    @property
    def central_sumw_over_events(self) -> float:
        """Sum of weights over events"""
        return self.sumw_over_events[0][self.central_idx] - abs(
            self.sumw_over_events[1][self.central_idx]
        )

    @property
    def central_sumw_over_entries(self) -> float:
        """Sum of weights over entries"""
        return self.sumw_over_entries[0][self.central_idx] - abs(
            self.sumw_over_entries[1][self.central_idx]
        )

    @property
    def sumw(self) -> float:
        """sum of weights"""
        if self.central_sumw_over_entries < 0:
            return 0.0
        return self.central_sumw_over_entries

    @property
    def weights(self) -> np.ndarray:
        return np.array(
            [
                (pos - abs(neg))[self.central_idx]
                for pos, neg in zip(self._positive_weights, self._negative_weights)
            ]
        )

    @property
    def all_scaled_weights(self):
        return [
            (pos - abs(neg)) * self.scale
            for pos, neg in zip(self._positive_weights, self._negative_weights)
        ]

    @property
    def scale_uncertainties(self) -> Tuple[np.ndarray, np.ndarray]:
        """
        Retreive scale uncertainties

        Returns:
            ``Tuple[np.ndarray, np.ndarray]``:
            lower and upper uncertainties per bin
        """
        bins = self.all_scaled_weights

        if not self.weight_collection.has_scale:
            central = [b[self.central_idx] for b in bins]
            return central, central

        weight_loc = (
            self.weight_collection.get_scale_vars(
                self.n_point_scale_variation, self.dynamic_scale_choice
            )
            .pdfset(self.nominal_weight.pdfset)
            .loc
        )

        upper, lower = [], []
        for current_bin in bins:
            current_weights = [current_bin[wloc] for wloc in weight_loc]
            upper.append(max(current_weights))
            lower.append(min(current_weights))
        return np.array(lower), np.array(upper)
