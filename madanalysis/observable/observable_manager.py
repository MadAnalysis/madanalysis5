################################################################################
#
#  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


"""Registry of the observables available in a running mode."""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from madanalysis.observable.observable_base import ObservableBase
from madanalysis.enumeration.ma5_running_type import MA5RunningType
import madanalysis.observable.observable_list


class ObservableManager:
    """Collect the observables of :mod:`madanalysis.observable.observable_list`.

    Attributes:
        full_list (``list[str]``): all observable names.
        plot_list (``list[str]``): observables available in the running mode.
        cut_event_list (``list[str]``): observables usable in event cuts.
        cut_candidate_list (``list[str]``): observables usable in candidate cuts.

    Unknown attributes are resolved as observable names (``manager.PT``), see
    :meth:`__getattr__`.
    """
    def __init__(self, mode: int) -> None:
        """Scan the observable catalogue for a running mode.

        Args:
            mode (``int``): :class:`~madanalysis.enumeration.ma5_running_type.MA5RunningType` code.
        """

        mlist = list(madanalysis.observable.observable_list.__dict__.keys())

        # extract native list
        self.full_list = []
        self.plot_list = []
        self.cut_event_list = []
        self.cut_candidate_list = []

        for item in mlist:
            if item.startswith("__"):
                continue
            if item == "ObservableBase":
                continue

            ref = self.get(item)
            if ref.__class__.__name__ != "ObservableBase":
                continue

            self.full_list.append(item)

            if mode == MA5RunningType.PARTON and ref.code_parton == "":
                continue
            elif mode == MA5RunningType.HADRON and ref.code_hadron == "":
                continue
            elif mode == MA5RunningType.RECO and ref.code_reco == "":
                continue
            self.plot_list.append(item)

            if ref.cut_event:
                self.cut_event_list.append(item)
            if ref.cut_candidate:
                self.cut_candidate_list.append(item)

    def get(self, name: str) -> ObservableBase | None:
        """Get an observable by name.

        Args:
            name (``str``): name of the observable (module-level variable name).

        Returns:
            ``ObservableBase | None``:
            The observable, or ``None`` if not defined.
        """
        if name not in list(madanalysis.observable.observable_list.__dict__.keys()):
            return None
        return madanalysis.observable.observable_list.__dict__[name]

    def findPlotObservable(self, obs: str) -> bool:
        """Check whether an observable can be plotted in the running mode.

        Args:
            obs (``str``): name of the observable.

        Returns:
            ``bool``:
            ``True`` if the observable is in :attr:`plot_list`.
        """
        if obs in self.plot_list:
            return True
        else:
            return False

    def findCutObservable(self, obs: str) -> bool:
        """Check whether an observable can be used in cuts.

        Args:
            obs (``str``): name of the observable.

        Returns:
            ``bool``:
            ``True`` if the observable is in ``cut_list`` (see FIXME).
        """
        # FIXME: 'cut_list' does not exist; __getattr__ returns None and 'obs in None' raises
        # a TypeError (cut_event_list/cut_candidate_list intended).
        if obs in self.cut_list:
            return True
        else:
            return False

    def __getattr__(self, name: str) -> ObservableBase | None:
        """Resolve unknown attributes as observable names.

        Args:
            name (``str``): attribute name.

        Returns:
            ``ObservableBase | None``:
            The observable, or ``None`` (no ``AttributeError`` is ever raised, which hides
            typos in attribute names).
        """
        return self.get(name)
