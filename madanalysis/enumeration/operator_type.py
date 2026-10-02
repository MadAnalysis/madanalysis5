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


"""Comparison operators of the cuts."""

from __future__ import annotations

import six

class metaclass(type):
        """Metaclass turning the class attribute access ``OperatorType.NAME`` into an integer code.

        Accessing ``OperatorType.NAME`` returns the index of ``NAME`` in ``OperatorType.values``; the
        conversion helpers below map such an index back to the associated properties.
        """
    
        def __getattr__(self, name: str) -> int:
            """Get the integer code of an enumeration entry.

            Unknown names are mapped to the index of ``UNKNOWN`` instead of raising.

            Args:
                name (``str``): name of the entry (e.g. ``OperatorType.GREATER``).

            Returns:
                ``int``:
                Index of the entry in ``values``.
            """
            if name in list(self.values.keys()):
                return list(self.values.keys()).index(name)
            else:
                return list(self.values.keys()).index('UNKNOWN')

        def convert2string(self,op: int) -> str:
            """Get the user symbol of an operator.

            Args:
                op (``int``): integer code of the entry.

            Returns:
                ``str``:
                Symbol such as ``'>='`` (``'='`` for ``EQUAL``).
            """
            name = list(self.values.keys())[op]
            return self.values[name][0]

        def convert2cpp(self,op: int) -> str:
            """Get the C++ operator.

            Args:
                op (``int``): integer code of the entry.

            Returns:
                ``str``:
                C++ operator (``'=='`` for ``EQUAL``).
            """
            name = list(self.values.keys())[op]
            return self.values[name][1]

@six.add_metaclass(metaclass)
class OperatorType(object):
    """Comparison operators. Each entry of ``values`` is ``[user_symbol, cpp_operator]``.
    """
    values = { 'GREATER'       : [">",">"],\
               'GREATER_EQUAL' : [">=",">="],\
               'LESS'          : ["<","<"],\
               'LESS_EQUAL'    : ["<=","<="],\
               'EQUAL'         : ["=","=="],\
               'NOT_EQUAL'     : ["!=","!="],\
               'UNKNOWN'       : ["",""]
               }


