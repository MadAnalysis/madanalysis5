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


"""Fill styles of the histogram backgrounds (``set <dataset>.backstyle``)."""

from __future__ import annotations

import six

class metaclass(type):
        """Metaclass turning the class attribute access ``BackStyleType.NAME`` into an integer code.

        Accessing ``BackStyleType.NAME`` returns the index of ``NAME`` in ``BackStyleType.values``; the
        conversion helpers below map such an index back to the associated properties.
        """

        def __getattr__(self, name: str) -> int:
                """Get the integer code of an enumeration entry.

                Args:
                    name (``str``): name of the entry (e.g. ``BackStyleType.SOLID``).

                Raises:
                    ``ValueError``: if ``name`` is not a key of ``values``.

                Returns:
                    ``int``:
                    Index of the entry in ``values``.
                """
                return list(self.values.keys()).index(name)

        def convert2code(self,color: int) -> int:
                """Get the ROOT fill-style code of an entry.

                Args:
                    color (``int``): integer code of the entry.

                Returns:
                    ``int``:
                    ROOT ``TAttFill`` style code.
                """
                name = list(self.values.keys())[color]
                return self.values[name][0]

        def convert2string(self,color: int) -> str:
                """Get the user-level name of an entry.

                Args:
                    color (``int``): integer code of the entry.

                Returns:
                    ``str``:
                    Name as used in ``set <dataset>.backstyle = <name>``.
                """
                name = list(self.values.keys())[color]
                return self.values[name][1]

        def convert2matplotlib(self,color: int) -> str:
                """Get the Matplotlib hatch of an entry.

                Args:
                    color (``int``): integer code of the entry.

                Returns:
                    ``str``:
                    Hatch pattern as a Python literal string (e.g. ``'"/"'``) or ``'None'``.
                """
                name = list(self.values.keys())[color]
                return self.values[name][2]

@six.add_metaclass(metaclass)
class BackStyleType(object):
        """Histogram fill styles.

        Each entry of ``values`` is ``[root_fill_code, user_name, matplotlib_hatch]``.
        """
        values = { 'AUTO'   : [0,   'auto',  'None'],\
                   'SOLID'  : [1001,'solid', 'None'],\
                   'DOTTED' : [3002,'dotted','"."'],\
                   'HLINE'  : [3007,'hline', '"-"'],\
                   'DLINE'  : [3004,'dline', '"/"'],\
                   'VLINE'  : [3006,'vline', '"|"']  }

#            matplotlib -> hatch   = ['/', '\\', '|', '-', '+', 'x', 'o', 'O', '.', '*']

