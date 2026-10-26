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


"""Line styles of the histograms (``set <dataset>.linestyle``)."""

from __future__ import annotations

import six


class metaclass(type):
                """Metaclass turning the class attribute access ``LineStyleType.NAME`` into an integer code.

                Accessing ``LineStyleType.NAME`` returns the index of ``NAME`` in ``LineStyleType.values``; the
                conversion helpers below map such an index back to the associated properties.
                """

                def __getattr__(self, name: str) -> int:
                        """Get the integer code of an enumeration entry.

                        Args:
                            name (``str``): name of the entry (e.g. ``LineStyleType.SOLID``).

                        Raises:
                            ``ValueError``: if ``name`` is not a key of ``values``.

                        Returns:
                            ``int``:
                            Index of the entry in ``values``.
                        """
                        return list(self.values.keys()).index(name)

                def convert2code(self,color: int) -> int:
                        """Get the ROOT line-style code.

                        Args:
                            color (``int``): integer code of the entry.

                        Returns:
                            ``int``:
                            ROOT ``TAttLine`` style code.
                        """
                        name = list(self.values.keys())[color]
                        return self.values[name][0]

                def convert2string(self,color: int) -> str:
                        """Get the user-level name of a line style.

                        Args:
                            color (``int``): integer code of the entry.

                        Returns:
                            ``str``:
                            Name as used in ``set <dataset>.linestyle``.
                        """
                        name = list(self.values.keys())[color]
                        return self.values[name][1]

                def convert2matplotlib(self,style: int) -> str:
                        """Get the Matplotlib line style.

                        Args:
                            style (``int``): integer code of the entry.

                        Returns:
                            ``str``:
                            Style as a Python literal string (e.g. ``'"dashed"'``).
                        """
                        name = list(self.values.keys())[style]
                        return self.values[name][2]


@six.add_metaclass(metaclass)
class LineStyleType(object):
        """Line styles. Each entry of ``values`` is ``[root_code, user_name, matplotlib_style]``.
        """
        values = { 'SOLID'      : [1,'solid','"solid"'],\
                   'DASHED'     : [2,'dashed','"dashed"'],\
                   'DOTTED'     : [3,'dotted','"dotted"'],\
                   'DASHDOTTED' : [4,'dash-dotted','"dashdot"'] }

