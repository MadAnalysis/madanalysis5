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


"""Conversion of user-level names into valid (and unique) C++ identifiers.

Used when generating the C++ code of the analysis and when naming output folders
(e.g. the dataset ``ttbar`` is written in ``Output/SAF/_ttbar``).
"""

from __future__ import annotations

class InstanceName():
    """Static registry mapping user-level names to C++ identifiers.

    Attributes:
        table (``dict[str, str]``): already converted names (class attribute, shared by
            the whole session; cleared by :meth:`Clear`).
    """
    table = {}

    @staticmethod
    def Find(name: str) -> bool:
        """Check whether a name has already been converted.

        Args:
            name (``str``): user-level name.

        Returns:
            ``bool``:
            ``True`` if the name is in the registry.
        """
        if name in list(InstanceName.table.keys()):
            return True
        return False

    @staticmethod
    def Clear() -> None:
        """Empty the registry."""
        InstanceName.table.clear()
        
    @staticmethod
    def Get(name: str) -> str:
        """Get (or create) the C++ identifier associated with a name.

        The identifier is ``"_" + name`` with ``+ - ~ space < [ ]`` replaced by
        ``_p _m _t _ _l _I I_``. If it collides with an existing identifier, a ``__<n>``
        suffix is added.

        Args:
            name (``str``): user-level name.

        Returns:
            ``str``:
            The C++ identifier.
        """
        if name in list(InstanceName.table.keys()):
            return InstanceName.table[name]
        else:
            newname="_"+name.lstrip()
            newname=newname.replace('+','_p')
            newname=newname.replace('-','_m')
            newname=newname.replace('~','_t')
            newname=newname.replace(' ','_')
            newname=newname.replace('<','_l')
            newname=newname.replace('[','_I')
            newname=newname.replace(']','I_')
            # FIXME: this loop is a no-op (assigning to the loop variable does not modify 'newname');
            # other invalid characters are kept in the identifier.
            for item in newname:
                if not (item.isalpha() or item.isdigit() or item=="_"):
                    item="X"

            # FIXME: the incremented name ('__1', '__2', ...) is not re-checked against the registry, and
            # a name ending with '__<digits>' is incremented instead of suffixed.
            if newname in list(InstanceName.table.values()):
                parts = newname.split('__')
                try:
                    index=int(parts[-1])
                    parts[-1]=str(index+1)
                    newname='__'.join(parts)
                except:
                    newname+='__1'
            InstanceName.table[name]=newname
            return newname
                
