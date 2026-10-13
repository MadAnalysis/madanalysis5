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


"""Base class for text files opened in writing mode."""

from __future__ import absolute_import
from __future__ import annotations
from typing import IO
import logging
class TextFileWriter():
    """Text file opened in writing mode.

    Attributes:
        filename (``str``): path of the file.
        isopen (``bool``): whether the file is open.
        file (``IO[str]``): file object (defined once opened).
    """

    def __init__(self,filename: str) -> None:
        """Store the file name (the file is not opened).

        Args:
            filename (``str``): path of the file.
        """
        self.filename = filename
        self.isopen = False

    def Open(self) -> bool:
        """Open the file.

        Returns:
            ``bool``:
            ``False`` if the file is already open or cannot be opened.
        """
        if self.isopen:
            logging.getLogger('MA5').error("the file called '"+self.filename+"' cannot be opened. It is already opened")
            return False
        try:
            self.file = open ( self.filename, "w" )
            self.isopen = True
            return True
        except:
            logging.getLogger('MA5').error("Impossible to create the file called '" + self.filename + "'")
            return False

    def Close(self) -> None:
        """Close the file if it is open."""
        if self.isopen:
            self.file.close()
            self.isopen = False

