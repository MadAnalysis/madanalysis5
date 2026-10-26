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


# Python import
"""Command history of the MadAnalysis 5 interpreter."""

from __future__ import absolute_import
from __future__ import annotations
import os


#===============================================================================
#  History
#===============================================================================
class History():
    """List of the commands typed during the session.

    Attributes:
        history (``list[str]``): stored commands.
        autosave (``str``): name of the auto-save file (not used by this class).
    """

    def __init__(self,autosave: str = ".history") -> None:
        """Initialise an empty history.

        Args:
            autosave (``str``, default ``".history"``): name of the auto-save file.
        """
        self.history = []
        self.autosave = autosave


    def Add(self,line: str) -> bool | None:
        """Add a command to the history.

        The commands ``history``, ``exit``, ``quit`` and those starting with ``help`` or
        ``#*`` are not stored.

        Args:
            line (``str``): command line.

        Returns:
            ``bool | None``:
            ``False`` if the command is not stored, ``None`` otherwise.
        """
        # Cleaning the line (safety)
        # Not done in interpreter_base because space is required by tab completion
        line=line.rstrip()

        # Remove simple commands
        toBypass = ["history","exit","quit"]
        if line in toBypass:
            return False

        # Remove commands starting with
        # NOTE: '#*' is compared literally with startswith (not as a pattern): lines starting
        # with '#' only are stored.
        toBypass = ['help','#*']
        for item in toBypass:
            if line.startswith(item):
                return False

        # Add
        self.history.append(line)


    def Reset(self) -> None:
        """Remove all commands from the history."""
        self.history = []


    def Print(self) -> str:
        """Get the history as text.

        Returns:
            ``str``:
            The commands separated by newlines.
        """
        return '\n'.join(self.history)


    def Save(self,filename: str,forced: bool = False) -> bool:
        """Save the history into a file.

        Args:
            filename (``str``): name of the output file.
            forced (``bool``, default ``False``): overwrite an existing file.

        Returns:
            ``bool``:
            ``False`` if the file exists and ``forced`` is ``False``, ``True`` otherwise.
        """

        # Failure if the file does not exist
        if (not forced) and os.path.exists(filename):
            return False

        # Save the file
        file = open(filename, 'w')
        file.write('\n'.join(self.history))
        file.close()

        return True


    def __len__(self) -> int:
        """Get the number of stored commands.

        Returns:
            ``int``:
            Length of the history.
        """
        return len(self.history)


    def __getitem__(self, key: int) -> str:
        """Get a stored command.

        Args:
            key (``int``): index of the command.

        Returns:
            ``str``:
            The command.
        """
        return self.history[key]

 
