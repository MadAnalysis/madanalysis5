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


"""Definition of a (multi)particle: a label associated with a set of PDG codes."""

from __future__ import absolute_import
from __future__ import annotations
import logging
from six.moves import range
class MultiParticle:
    """A particle (one PDG code) or multiparticle (several PDG codes) label.

    Attributes:
        name (``str``): label in lower case.
        ids (``list[int]``): sorted list of unique PDG codes.
    """

    def __init__(self,name: str,ids: list[int]) -> None:
        """Create a (multi)particle.

        Args:
            name (``str``): label (converted to lower case).
            ids (``list[int]``): PDG codes (duplicates are removed, the list is sorted).
        """
        self.name = name.lower()
        self.ids=[]
        for item in ids:
            if not item in self.ids:
                self.ids.append(item)
        self.ids.sort()
        
    def __len__(self) -> int:
        """Get the number of PDG codes.

        Returns:
            ``int``:
            Number of PDG codes (1 for a particle).
        """
        return len(self.ids)

    def __getitem__(self,i: int) -> int:
        """Get a PDG code.

        Args:
            i (``int``): index in the sorted list of codes.

        Returns:
            ``int``:
            The PDG code.
        """
        return self.ids[i]

    def __eq__(self,other: MultiParticle) -> bool:
        """Compare the PDG codes of two (multi)particles (labels are ignored).

        Args:
            other (``MultiParticle``): object to compare with.

        Returns:
            ``bool``:
            ``True`` if both objects have the same PDG codes.
        """
        if len(self)!=len(other):
            return False
        for ind in range(0,len(self)):
            if self[ind]!=other[ind]:
                return False
        return True    

    def Display(self) -> None:
        """Log the definition of the (multi)particle."""
        text = ""
        if len(self.ids)==1:
            text = "   The particle '"+ self.name + "' is defined by the PDG-id "
        else:
            text = "   The multiparticle '"+ self.name + "' is defined by the PDG-ids "
        text = text+self.GetStringDisplay();
        text = text[:-1]
        logging.getLogger('MA5').info(text+".")

    def GetStringDisplay(self) -> str:
        """Get the PDG codes as a string.

        Returns:
            ``str``:
            Space-separated PDG codes (with a trailing space).
        """
        text = ""
        for item in self.ids:
            text += str(item) + " "
        return text    

    def DisplayParameter(self,data: str) -> None:
        """Log an error: (multi)particles have no attributes.

        Args:
            data (``str``): requested attribute (ignored).
        """
        logging.getLogger('MA5').error("Particles and Multiparticles have no attributes.")

    def SetParameter(self,parameter: str,value: str) -> None:
        """Log an error: (multi)particles have no attributes.

        Args:
            parameter (``str``): requested attribute.
            value (``str``): requested value (ignored).
        """
        logging.getLogger('MA5').error("Particles and Multiparticles have no attribute denoted "+parameter+".")

    def GetParameters(self) -> list[str]:
        """Get the settable attributes.

        Returns:
            ``list[str]``:
            Always an empty list.
        """
        return []

    def Find(self,id: int) -> bool:
        """Check whether a PDG code belongs to the definition.

        Args:
            id (``int``): PDG code.

        Returns:
            ``bool``:
            ``True`` if ``id`` is in :attr:`ids`.
        """
        if id in self.ids:
            return True
        return False

    def Add(self,id: int) -> None:
        """Add a PDG code (ignored if already present) and sort the codes.

        Args:
            id (``int``): PDG code.
        """
        if not self.Find(id):
            self.ids.append(id)
        self.ids.sort()
        
    def Remove(self,id: int) -> None:
        """Remove a PDG code (ignored if absent).

        Args:
            id (``int``): PDG code.
        """
        if self.Find(id):
            self.ids.remove(id)

    def GetIds(self) -> list[int]:
        """Get the PDG codes.

        Returns:
            ``list[int]``:
            :attr:`ids` (not a copy).
        """
        return self.ids

    def IsThereCommonPart(self,multi: MultiParticle) -> bool:
        """Check whether two (multi)particles share at least one PDG code.

        Args:
            multi (``MultiParticle``): other (multi)particle.

        Returns:
            ``bool``:
            ``True`` if at least one PDG code is common.
        """
        for id in multi.ids:
            if id in self.ids:
                return True
        return False    
