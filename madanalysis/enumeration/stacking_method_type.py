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


"""Stacking methods of the histograms (``set main.stacking_method``)."""

from __future__ import annotations

import six

class metaclass(type):
        """Metaclass turning the class attribute access ``StackingMethodType.NAME`` into an integer code.

        Accessing ``StackingMethodType.NAME`` returns the index of ``NAME`` in ``StackingMethodType.values``; the
        conversion helpers below map such an index back to the associated properties.
        """
        def __getattr__(self, name: str) -> int:
                """Get the integer code of an enumeration entry.

                Args:
                    name (``str``): name of the entry (e.g. ``StackingMethodType.STACK``).

                Raises:
                    ``ValueError``: if ``name`` is not a key of ``values``.

                Returns:
                    ``int``:
                    Index of the entry in ``values``.
                """
                return self.values.index(name)

@six.add_metaclass(metaclass)
class StackingMethodType(object):
        """Stacking methods: ``AUTO``, ``STACK``, ``SUPERIMPOSE`` and ``NORMALIZE2ONE``.
        """
        values = ['AUTO','STACK','SUPERIMPOSE','NORMALIZE2ONE']



