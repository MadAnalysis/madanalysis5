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


"""Base class describing an observable of the normal mode."""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any
from madanalysis.enumeration.ma5_running_type import MA5RunningType


class ObservableBase:
    """Description of an observable usable in ``plot``, ``select`` and ``reject`` commands.

    Attributes:
        name (``str``): name of the observable (e.g. ``"PT"``).
        args (``list[int]``): types of the arguments
            (:class:`~madanalysis.enumeration.argument_type.ArgumentType` codes).
        combination (``int``): :class:`~madanalysis.enumeration.combination_type.CombinationType` code.
        plot_auto (``bool``): whether automatic binning is used.
        plot_nbins / plot_xmin / plot_xmax: default binning.
        plot_unitX_tlatex / plot_unitX_latex (``str``): unit of the x-axis (TLatex / LaTeX).
        code_parton / code_hadron / code_reco (``str``): C++ code computing the observable
            in each running mode (empty if not available).
        cut_event / cut_candidate (``bool``): whether the observable can be used in event
            or candidate cuts.
        tlatex / latex (``str``): symbol of the observable (TLatex / LaTeX).
    """
    def __init__(
        self,
        name: str,
        args: list[int],
        combination: int,
        plot_auto: bool,
        plot_nbins: int,
        plot_xmin: float,
        plot_xmax: float,
        plot_unitX_tlatex: str,
        plot_unitX_latex: str,
        code_parton: str,
        code_hadron: str,
        code_reco: str,
        cut_event: bool,
        cut_candidate: bool,
        tlatex: str,
        latex: str,
    ) -> None:
        """Create an observable (see the class attributes for the meaning of the arguments).

        Args:
            name (``str``): name of the observable.
            args (``list[int]``): argument types.
            combination (``int``): combination type.
            plot_auto (``bool``): automatic binning.
            plot_nbins (``int``): default number of bins.
            plot_xmin (``float``): default lower bound.
            plot_xmax (``float``): default upper bound.
            plot_unitX_tlatex (``str``): unit (TLatex).
            plot_unitX_latex (``str``): unit (LaTeX).
            code_parton (``str``): C++ code at parton level.
            code_hadron (``str``): C++ code at hadron level.
            code_reco (``str``): C++ code at reco level.
            cut_event (``bool``): usable in event cuts.
            cut_candidate (``bool``): usable in candidate cuts.
            tlatex (``str``): symbol (TLatex).
            latex (``str``): symbol (LaTeX).
        """
        self.name = name
        self.args = args
        self.plot_auto = plot_auto
        self.plot_nbins = plot_nbins
        self.plot_xmin = plot_xmin
        self.plot_xmax = plot_xmax
        self.plot_unitX_tlatex = plot_unitX_tlatex
        self.plot_unitX_latex = plot_unitX_latex
        self.code_parton = code_parton
        self.code_hadron = code_hadron
        self.code_reco = code_reco
        self.cut_event = cut_event
        self.cut_candidate = cut_candidate
        self.combination = combination
        self.tlatex = tlatex
        self.latex = latex

    def code(self, level: int) -> str | None:
        """Get the C++ code of the observable for a running mode.

        Args:
            level (``int``): :class:`~madanalysis.enumeration.ma5_running_type.MA5RunningType` code.

        Returns:
            ``str | None``:
            The C++ code (empty if unavailable), or ``None`` for an unknown mode.
        """
        if level == MA5RunningType.PARTON:
            return self.code_parton
        elif level == MA5RunningType.HADRON:
            return self.code_hadron
        elif level == MA5RunningType.RECO:
            return self.code_reco
        else:
            return None

    @staticmethod
    def Clone(
        obs: ObservableBase,
        name: str | None = None,
        args: list[int] | None = None,
        combination: int | None = None,
        plot_auto: bool | None = None,
        plot_nbins: int | None = None,
        plot_xmin: float | None = None,
        plot_xmax: float | None = None,
        plot_unitX_tlatex: str | None = None,
        plot_unitX_latex: str | None = None,
        code_parton: str | None = None,
        code_hadron: str | None = None,
        code_reco: str | None = None,
        cut_event: bool | None = None,
        cut_candidate: bool | None = None,
        tlatex: str | None = None,
        latex: str | None = None,
    ) -> ObservableBase:
        """Copy an observable, overriding some of its properties.

        Args:
            obs (``ObservableBase``): observable to copy.
            name (``str | None``, default ``None``): new name.
            args (``list[int] | None``, default ``None``): new argument types.
            combination (``int | None``, default ``None``): new combination type.
            plot_auto (``bool | None``, default ``None``): new automatic-binning flag.
            plot_nbins (``int | None``, default ``None``): new number of bins.
            plot_xmin (``float | None``, default ``None``): new lower bound.
            plot_xmax (``float | None``, default ``None``): new upper bound.
            plot_unitX_tlatex (``str | None``, default ``None``): new unit (TLatex).
            plot_unitX_latex (``str | None``, default ``None``): new unit (LaTeX).
            code_parton (``str | None``, default ``None``): new parton-level code.
            code_hadron (``str | None``, default ``None``): new hadron-level code.
            code_reco (``str | None``, default ``None``): new reco-level code.
            cut_event (``bool | None``, default ``None``): new event-cut flag.
            cut_candidate (``bool | None``, default ``None``): new candidate-cut flag.
            tlatex (``str | None``, default ``None``): new symbol (TLatex).
            latex (``str | None``, default ``None``): new symbol (LaTeX).

        Returns:
            ``ObservableBase``:
            The new observable (``None`` arguments keep the original values).
        """

        # create clone of obs
        newobs = ObservableBase(
            obs.name,
            obs.args,
            obs.combination,
            obs.plot_auto,
            obs.plot_nbins,
            obs.plot_xmin,
            obs.plot_xmax,
            obs.plot_unitX_tlatex,
            obs.plot_unitX_latex,
            obs.code_parton,
            obs.code_hadron,
            obs.code_reco,
            obs.cut_event,
            obs.cut_candidate,
            obs.tlatex,
            obs.latex,
        )

        # replace
        if name is not None:
            newobs.name = name
        if args is not None:
            newobs.args = args
        if combination is not None:
            newobs.combination = combination
        if plot_auto is not None:
            newobs.plot_auto = plot_auto
        if plot_nbins is not None:
            newobs.plot_nbins = plot_nbins
        if plot_xmin is not None:
            newobs.plot_xmin = plot_xmin
        if plot_xmax is not None:
            newobs.plot_xmax = plot_xmax
        if plot_unitX_tlatex is not None:
            newobs.plot_unitX_tlatex = plot_unitX_tlatex
        if plot_unitX_latex is not None:
            # FIXME: the LaTeX unit overwrites the TLatex unit (newobs.plot_unitX_latex intended).
            newobs.plot_unitX_tlatex = plot_unitX_latex
        if code_parton is not None:
            newobs.code_parton = code_parton
        if code_hadron is not None:
            newobs.code_hadron = code_hadron
        if code_reco is not None:
            newobs.code_reco = code_reco
        if cut_event is not None:
            newobs.cut_event = cut_event
        if cut_candidate is not None:
            newobs.cut_candidate = cut_candidate
        if tlatex is not None:
            newobs.tlatex = tlatex
        if latex is not None:
            newobs.latex = latex

        # return the clone
        return newobs
