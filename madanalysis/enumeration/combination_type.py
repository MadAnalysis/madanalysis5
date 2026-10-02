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


"""Types of combinations of particles in observables (e.g. ``sPT``, ``dM``, ``rPT``)."""

from __future__ import annotations

import six

class metaclass(type):
        """Metaclass turning the class attribute access ``CombinationType.NAME`` into an integer code.

        Accessing ``CombinationType.NAME`` returns the index of ``NAME`` in ``CombinationType.values``; the
        conversion helpers below map such an index back to the associated properties.
        """

        def __getattr__(self, name: str) -> int:
            """Get the integer code of an enumeration entry.

            Args:
                name (``str``): name of the entry (e.g. ``CombinationType.SUMSCALAR``).

            Raises:
                ``ValueError``: if ``name`` is not a key of ``values``.

            Returns:
                ``int``:
                Index of the entry in ``values``.
            """
            return list(self.values.keys()).index(name)

        def convert_from_string(self,lowerletters: str) -> int:
            """Get the combination code associated with an observable prefix.

            Args:
                lowerletters (``str``): prefix (e.g. ``'s'``, ``'dv'``, ``''``).

            Returns:
                ``int``:
                Code of the combination, or the code of ``UNKNOWN`` if the prefix is not
                recognised.
            """
            for i,j in self.values.items():
                if lowerletters in j:
                    return self.__getattr__(i)
            return self.__getattr__('UNKNOWN')

        def convert2string(self,index: int) -> str:
            """Get the canonical prefix of a combination.

            Args:
                index (``int``): integer code of the entry.

            Returns:
                ``str``:
                First accepted prefix, or ``'ERROR'`` for ``UNKNOWN``.
            """
            if index==self.__getattr__('UNKNOWN'):
                return 'ERROR'
            else:
                name = list(self.values.keys())[index]
                return self.values[name][0]



@six.add_metaclass(metaclass)
class CombinationType(object):
    """Particle-combination prefixes of observables.

    Each entry of ``values`` lists the accepted prefixes: ``s`` (scalar sum),
    ``v`` (vector sum), ``ds``/``sd`` (scalar difference), ``d``/``dv``/``vd`` (vector
    difference) and ``r`` (ratio). ``DEFAULT`` corresponds to no prefix.
    """
    values = { 'UNKNOWN'    : [],\
               'DEFAULT'    : [''],\
               'SUMSCALAR'  : ['s'],\
               'SUMVECTOR'  : ['v'],\
               'DIFFSCALAR' : ['ds','sd'],\
               'DIFFVECTOR' : ['d','dv','vd'],\
               'RATIO'      : ['r'] }
               
