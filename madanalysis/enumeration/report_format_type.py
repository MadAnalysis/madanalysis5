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


"""Formats of the reports."""

from __future__ import annotations

import six

class metaclass(type):
    """Metaclass turning the class attribute access ``ReportFormatType.NAME`` into an integer code.

    Accessing ``ReportFormatType.NAME`` returns the index of ``NAME`` in ``ReportFormatType.values``; the
    conversion helpers below map such an index back to the associated properties.
    """

    def __getattr__(self, name: str) -> int:
        """Get the integer code of an enumeration entry.

        Args:
            name (``str``): name of the entry (e.g. ``ReportFormatType.HTML``).

        Raises:
            ``ValueError``: if ``name`` is not a key of ``values``.

        Returns:
            ``int``:
            Index of the entry in ``values``.
        """
        return list(self.values.keys()).index(name)

    def convert2cmd(self,format: int) -> str:
        """Get the name of the generation method of a report format.

        Args:
            format (``int``): integer code of the entry.

        Returns:
            ``str``:
            E.g. ``'generate_html'``.
        """
        name = list(self.values.keys())[format]
        return self.values[name][0]

    def convert2string(self,format: int) -> str:
        """Get the name of a report format.

        Args:
            format (``int``): integer code of the entry.

        Returns:
            ``str``:
            ``'LATEX'``, ``'PDFLATEX'`` or ``'HTML'``.
        """
        return list(self.values.keys())[format]

    def convert2filetype(self,format: int) -> str:
        """Get the figure file extension of a report format.

        Args:
            format (``int``): integer code of the entry.

        Returns:
            ``str``:
            ``'eps'`` or ``'png'``.
        """
        name = list(self.values.keys())[format]
        return self.values[name][1]

    
@six.add_metaclass(metaclass)
class ReportFormatType(object):
    """Report formats. Each entry of ``values`` is ``[generation_method_name, figure_extension]``.
    """
    values = { 'LATEX'    : ['generate_latex','eps'],\
               'PDFLATEX' : ['generate_pdflatex','png'],\
               'HTML'     : ['generate_html','png']  }



