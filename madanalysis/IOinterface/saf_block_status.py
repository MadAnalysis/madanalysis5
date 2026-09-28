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


"""State of a ``<block>`` while parsing a SAF file line by line."""

from __future__ import annotations

class SafBlockStatus():
    """Status of a SAF block during parsing.

    Attributes:
        activated (``bool``): ``True`` between the opening and closing tags.
        Nactivated (``int``): number of times the block has been opened.
        Nlines (``int``): number of lines read inside the current block.
    """
    def __init__(self) -> None:
        """Initialise a closed block."""
        self.activated  = False
        self.Nactivated = 0
        self.Nlines     = 0

    def activate(self) -> None:
        """Mark the block as opened (opening tag found)."""
        self.activated  =  True
        self.Nactivated += 1

    def desactivate(self) -> None:
        """Mark the block as closed (closing tag found) and reset the line counter."""
        self.activated = False
        self.Nlines    = 0

    def newline(self) -> None:
        """Count a line read inside the block."""
        self.Nlines += 1

