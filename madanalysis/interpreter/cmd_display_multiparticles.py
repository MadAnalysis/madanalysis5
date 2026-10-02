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


"""Interpreter command ``display_multiparticles``: list the (pre)defined multiparticles.
"""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from madanalysis.core.main import Main
import madanalysis.interpreter.cmd_base as CmdBase
import logging

class CmdDisplayMultiparticles(CmdBase.CmdBase):
    """Command ``display_multiparticles``."""

    def __init__(self,main: Main) -> None:
        """Register the ``display_multiparticles`` command.

        Args:
            main (``Main``): session state.
        """
        CmdBase.CmdBase.__init__(self,main,"display_multiparticles")

    def do(self,args: list[str]) -> None:
        """Display the (pre)defined multiparticles (arguments are ignored).

        Args:
            args (``list[str]``): arguments of the command (split by
                :meth:`~madanalysis.interpreter.interpreter_base.InterpreterBase.split_arg`).
        """
        self.main.multiparticles.DisplayMultiparticles()

    def help(self) -> None:
        """Display the help of the ``display_multiparticles`` command."""
        logging.getLogger('MA5').info("   Syntax: display_multiparticles")
        logging.getLogger('MA5').info("   Displays the list of all (pre)defined multiparticles.")

    # FIXME: extra 'main' argument: the interpreter calls complete(text,line,begidx,endidx),
    # which raises a TypeError (silently swallowed by readline).
    def complete(self,text: str,line: str,begidx: int,endidx: int,main: Main) -> None:
        """Tab completion of the ``display_multiparticles`` command (no completion).

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.
            main (``Main``): unused.

        Returns:
            ``None``:
            No completion.
        """
        return

