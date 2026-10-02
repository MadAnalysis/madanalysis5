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


"""Interpreter command ``reset``: reinitialise the session."""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from madanalysis.core.main import Main
    from madanalysis.interpreter.interpreter import Interpreter
from madanalysis.IOinterface.particle_reader      import ParticleReader
from madanalysis.IOinterface.multiparticle_reader import MultiparticleReader
from madanalysis.enumeration.ma5_running_type     import MA5RunningType
from madanalysis.interpreter.cmd_define           import CmdDefine
from madanalysis.interpreter.cmd_base             import CmdBase
import logging
from six.moves import input

class CmdReset(CmdBase):
    """Command ``reset``."""


    def __init__(self,main: Main) -> None:
        """Register the ``reset`` command.

        Args:
            main (``Main``): session state.
        """
        CmdBase.__init__(self,main,"reset")


    def do(self,args: list[str],myinterpreter: Interpreter) -> bool | None:
        """Reset the datasets, the selection, the global parameters, the (multi)particles and
        the history (after confirmation, unless in forced mode).

        Args:
            args (``list[str]``): arguments of the command (must be empty).
            myinterpreter (``Interpreter``): interpreter whose history is cleared.

        Returns:
            ``bool | None``:
            ``False`` if the user refuses the reset, ``None`` otherwise.
        """

        # Checking argument number
        if len(args) != 0:
            logging.getLogger('MA5').error("wrong number of arguments for the command 'reset'.")
            self.help()
            return

        # Ask question
        if not self.main.forced:
            logging.getLogger('MA5').warning("You are going to reinitialize MadAnalysis 5. The current configuration will be lost.")
            logging.getLogger('MA5').warning("Are you sure to do that ? (Y/N)")
            allowed_answers=['n','no','y','yes']
            answer=""
            while answer not in  allowed_answers:
               answer=input("Answer: ")
               answer=answer.lower()
            if answer=="no" or answer=="n":
                return False

        # NOTE: the regions and the jet collections are not reset.
        # Reset datasets
        self.main.datasets.Reset()

        # Reset selection
        self.main.selection.Reset()

        # Reset main
        self.main.ResetParameters()

        # Reset multiparticles
        self.ResetMultiparticles()

        # Reset history
        # FIXME: replaces the History object by a plain list: History.Add (called by precmd)
        # and the 'history' command then fail.
        myinterpreter.history=[] 

        return

    def ResetMultiparticles(self) -> None:
        """Reload the default particles and multiparticles of the running mode."""

        # Reset multiparticles
        self.main.multiparticles.Reset()

        # Opening a CmdDefine
        cmd_define = CmdDefine(self.main)

        # Loading particles
        input = ParticleReader(self.main.archi_info.ma5dir,cmd_define,self.main.mode)
        input.Load()
        input = MultiparticleReader(self.main.archi_info.ma5dir,cmd_define,self.main.mode,self.main.forced)
        input.Load()
        

    def help(self) -> None:
        """Display the help of the ``reset`` command."""
        logging.getLogger('MA5').info("   Syntax: reset")
        logging.getLogger('MA5').info("   Reinitializing all variables")


    def complete(self,text: str,line: str,begidx: int,endidx: int) -> list[str]:
        """Tab completion of the ``reset`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str]``:
            Empty list (no completion).
        """
        return []
