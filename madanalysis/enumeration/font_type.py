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


"""Font styles of the reports."""

from __future__ import annotations

import six

class metaclass(type):
                """Metaclass turning the class attribute access ``FontType.NAME`` into an integer code.

                Accessing ``FontType.NAME`` returns the index of ``NAME`` in ``FontType.values``; the
                conversion helpers below map such an index back to the associated properties.
                """
                def __getattr__(self, name: str) -> int:
                        """Get the integer code of an enumeration entry.

                        Args:
                            name (``str``): name of the entry (e.g. ``FontType.BF``).

                        Raises:
                            ``ValueError``: if ``name`` is not a key of ``values``.

                        Returns:
                            ``int``:
                            Index of the entry in ``values``.
                        """
                        return list(self.values.keys()).index(name)

                def convert2latex(self,font: int) -> str:
                        """Get the LaTeX opening tag of a font style.

                        Args:
                            font (``int``): integer code of the entry.

                        Returns:
                            ``str``:
                            Opening LaTeX command.
                        """
                        name = list(self.values.keys())[font]
                        return self.values[name][0]

                def convert2latexclose(self,font: int) -> str:
                        """Get the LaTeX closing tag of a font style.

                        Args:
                            font (``int``): integer code of the entry.

                        Returns:
                            ``str``:
                            Closing brace(s).
                        """
                        name = list(self.values.keys())[font]
                        return self.values[name][1]

                def convert2html(self,font: int) -> str:
                        """Get the HTML opening tag of a font style.

                        Args:
                            font (``int``): integer code of the entry.

                        Returns:
                            ``str``:
                            Opening HTML tag(s).
                        """
                        name = list(self.values.keys())[font]
                        return self.values[name][2]

                def convert2htmlclose(self,font: int) -> str:
                        """Get the HTML closing tag of a font style.

                        Args:
                            font (``int``): integer code of the entry.

                        Returns:
                            ``str``:
                            Closing HTML tag(s).
                        """
                        name = list(self.values.keys())[font]
                        return self.values[name][3]

@six.add_metaclass(metaclass)
class FontType(object):
        """Font styles of the report texts.

        Each entry of ``values`` is ``[latex_open, latex_close, html_open, html_close]``.
        """
        # FIXME: the HTML closing tags of ITBF (entry below) are not properly nested ('</b></i>' expected).
        values = {'none' : ['','','',''],\
                  'IT'   : ['\\textit{','}','<i>','</i>'],\
                  'BF'   : ['\\textbf{','}','<b>','</b>'],\
                  'TT'   : ['\\texttt{','}','  <tt>','</tt>'],\
                  'ITBF' : ['\\textit{\\textbf{','}}','<i><b>','</i></b>']}
        

