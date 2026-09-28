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


"""Base class of the MadAnalysis 5 interpreter commands."""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from madanalysis.core.main import Main
import logging
import glob
import os
from six.moves import range

class CmdBase():
    """Base class of the interpreter commands (``do``/``help``/``complete`` protocol).

    Every command of :class:`~madanalysis.interpreter.interpreter.Interpreter` is
    implemented as a subclass overloading :meth:`do`, :meth:`help` and :meth:`complete`.

    Attributes:
        reserved_words (``list[str]``): words that cannot be used as labels (class
            attribute, extended with the name of every created command).
        main (``Main``): session state.
        logger (``logging.Logger``): the ``MA5`` logger.
    """

    reserved_words=["exit","quit","eof","history","shell","from","as","all","or","and","main"]

    def __init__(self,main: Main,cmd_name: str) -> None:
        """Initialise the command and reserve its name.

        Args:
            main (``Main``): session state.
            cmd_name (``str``): name of the command (added to :attr:`reserved_words`).
        """
        # NOTE: reserved_words is a class attribute: the list is shared by all commands and
        # grows (with duplicates) each time a command object is created.
        self.reserved_words.append(cmd_name)
        self.main=main
        self.logger=logging.getLogger('MA5')

    def do(self,args: list[str]) -> None:
        """Execute the command (to be overloaded).

        Args:
            args (``list[str]``): arguments of the command.
        """
        self.logger.error("To developpers: CmdBase.do method must be overloaded!")

    def help(self) -> None:
        """Display the help of the command (to be overloaded)."""
        self.logger.error("To developpers: CmdBase.help method must be overloaded!")

    def complete(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the command (to be overloaded).

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions (``None`` for this default implementation).
        """
        self.logger.error("To developpers: CmdBase.complete method must be overloaded!")
        return
    
    @staticmethod 
    def directory_complete() -> list[str]:
        """List the folders of the current directory.

        Returns:
            ``list[str]``:
            Names of the folders.
        """
        output = []
        for file in glob.glob("*"):
            if os.path.isdir(file):
                output.append(file)
        return output        

    @staticmethod
    def finalize_complete(text: str,args: list[str]) -> list[str]:
        """Filter the candidate completions with the text being completed.

        Args:
            text (``str``): word being completed.
            args (``list[str]``): candidate completions.

        Returns:
            ``list[str]``:
            Candidates starting with ``text`` (all of them if ``text`` is empty).
        """
        if not text:
            return args
        else:
            return [ item for item in args if item.startswith(text) ]
    
    @staticmethod
    def IsAuthorizedLabel(label: str) -> bool:
        """Check whether a string can be used as a label (dataset, multiparticle, region, ...).

        A label starts with a letter or ``_`` and contains only letters, digits and the
        characters ``+``, ``-``, ``~``, ``_``.

        Args:
            label (``str``): candidate label.

        Returns:
            ``bool``:
            ``True`` if the label is valid, ``False`` otherwise.
        """

        # Rejecting empty label
        if len(label)==0:
            return False

        # Checking first character
        if not (label[0].isalpha() or label[0]=='_'):
            return False

        # Checking forbidden character
        allowed = ['+','-','~','_']
        for i in range(0,len(label)):
            if not (label[i].isalpha() or label[i].isdigit()):
                test = False
                for j in range(0,len(allowed)):
                    if label[i]==allowed[j]:
                        test=True
                        break
                if not test:
                    return False

        # Ok
        return True
