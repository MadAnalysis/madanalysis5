"""This file includes a generic multiweight bin description"""
import copy
from typing import List

import numpy as np


class MultiWeightBin:
    """
    Representation of a multiweight bin

    Args:
        weights (`List[float]`): sum of weights within the bin per weight
    """

    def __init__(self, weights: List[float]):
        self.weights = np.array(weights)

    def __repr__(self):
        return f"MultiWeightBin({len(self)} weights)"

    def __str__(self):
        return self.__repr__()

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

    def __truediv__(self, other):
        if isinstance(other, MultiWeightBin):
            return MultiWeightBin(self.weights / other.weights)
