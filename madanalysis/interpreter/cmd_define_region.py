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


"""Interpreter command ``define_region``: declare signal regions."""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from madanalysis.core.main import Main
import madanalysis.interpreter.cmd_base as CmdBase
import logging

class CmdDefineRegion(CmdBase.CmdBase):
    """Command ``define_region <list of regions>``."""

    def __init__(self,main: Main) -> None:
        """Register the ``define_region`` command.

        Args:
            main (``Main``): session state.
        """
        self.logger       = logging.getLogger('MA5')
        CmdBase.CmdBase.__init__(self,main,"define_region")

    def do(self,args: list[str]) -> bool | None:
        """Declare one or more signal regions.

        Args:
            args (``list[str]``): arguments of the command (split by
                :meth:`~madanalysis.interpreter.interpreter_base.InterpreterBase.split_arg`).

        Returns:
            ``bool | None``:
            ``True`` if no region name is given, ``None`` otherwise.
        """
        #Checking argument number
        if len(args) == 0:
            logging.getLogger('MA5').error("wrong number of arguments for the command 'define_region'.")
            self.help()
            return True

        # Calling fill
        # NOTE: the result of fill is discarded.
        self.fill(args,self.main.forced)

    def fill(self,args: list[str],forced: bool = False) -> bool:
        """Check the region names and add them to :attr:`Main.regions`.

        A name must not be a reserved word, must be a valid label and must not be used by
        a dataset, a (multi)particle, an observable or another region.

        Args:
            args (``list[str]``): names of the regions.
            forced (``bool``, default ``False``): unused.

        Returns:
            ``bool``:
            ``True`` if all regions have been added, ``False`` otherwise.
        """
        # Checking if the name is authorized
        for x in args:
            if x in self.reserved_words:
                self.logger.error("the name '" + x + "' is a reserved keyword. Please choose a different name.")
                return False

        # Checking if the name is authorized
        for x in args:
            if not self.IsAuthorizedLabel(x):
                self.logger.error("syntax error with the name '" + x + "'.")
                self.logger.error("A correct name contains only characters being letters, digits or the '+', '-', '~' and '_' symbols.")
                self.logger.error("Moreover, a correct name starts with a letter or the '_' symbol.")
                return False

        # Checking if no dataset with the same name has been defined
        for x in args:
            if self.main.datasets.Find(x):
                logging.getLogger('MA5').error("A dataset '"+x+"' already exists. Please choose a different name.")
                return False

        # Checking if no (multi)particle with the same name has been defined
        for x in args:
            if self.main.multiparticles.Find(x):
                logging.getLogger('MA5').error("A (multi)particle '"+x+"' already exists. Please choose a different name.")
                return False

        # Checking if no observable with the same name has been defined
        for x in args:
            if x in self.main.observables.full_list:
                logging.getLogger('MA5').error("An observable '"+x+"' already exists. Please choose a different name.")
                return False

        # Checking if the region has already been defined
        for x in args:
            if self.main.regions.Find(x):
                logging.getLogger('MA5').error("A region '"+x+"' already exists. Please choose a different name.")
                return False
            # FIXME: regions are added one by one inside the check loop: if a later name is already
            # defined, the earlier regions are still added although False is returned.
            self.main.regions.Add(x)

        return True

    # NOTE: the instance argument is named 'help' instead of 'self'.
    def help(help) -> None:
        """Display the help of the ``define_region`` command."""
        logging.getLogger('MA5').info("   Syntax: define_region <list of regions>")
        logging.getLogger('MA5').info("   Creates one or more analysis regions.")

    def complete(self,text: str,line: str,begidx: int,endidx: int) -> bool:
        """Tab completion of the ``define_region`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``bool``:
            Always ``True`` (no completion).
        """
        # FIXME: complete should return a list of completions, not True.
        return True

