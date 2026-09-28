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


"""Particle argument of an observable, with rank and mother requirements (e.g. ``mu[1]`` or ``mu < z``).
"""

from __future__ import absolute_import
from __future__ import annotations
from madanalysis.selection.instance_name import InstanceName
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from .multiparticle import MultiParticle
import logging

class ExtraParticle():
    """A (multi)particle as used in the argument of an observable.

    Attributes:
        particle (``MultiParticle``): the underlying (multi)particle.
        PTrank (``int``): rank of the particle (``mu[2]`` -> 2); ``0`` means all particles.
        mumType (``str``): mother relation: ``""`` (none), ``"<"`` (direct mother) or
            ``"<<"`` (any ancestor).
        mumPart (``ExtraParticle | None``): the required mother.
    """

    def __init__(self,particle: MultiParticle,PTrank: int = 0) -> None:
        """Create the particle argument without mother requirement.

        Args:
            particle (``MultiParticle``): the (multi)particle.
            PTrank (``int``, default ``0``): rank of the particle (``0`` = no rank).
        """
        self.particle = particle
        self.PTrank   = PTrank
        self.mumType  = ""
        self.mumPart  = None
        
    def __len__(self) -> int:
        """Get the number of PDG codes of the underlying (multi)particle.

        Returns:
            ``int``:
            Number of PDG codes.
        """
        return len(self.particle.ids)

    def __getitem__(self,i: int) -> int:
        """Get a PDG code of the underlying (multi)particle.

        Args:
            i (``int``): index of the code.

        Returns:
            ``int``:
            The PDG code.
        """
        return self.particle.ids[i]

    def __eq__(self,other: ExtraParticle) -> bool:
        """Compare the (multi)particle and the mother requirements.

        Args:
            other (``ExtraParticle``): object to compare with.

        Returns:
            ``bool``:
            ``True`` if the particles and mother requirements are equal.
        """
        # NOTE: PTrank is not compared.
        if self.particle!=other.particle:
            return False
        if self.mumType!=other.mumType:
            return False
        if self.mumPart!=other.mumPart:
            return False
        return True

    def Display(self) -> None:
        """Log the definition of the particle argument."""
        logging.getLogger('MA5').info("   ExtraParticle '" + self.particle.name +\
                     "' is defined by : " +\
                     self.GetStringDisplay()+".")

    def GetStringDisplay(self) -> str:
        """Get the user-level representation (also available as the :attr:`name` property).

        Returns:
            ``str``:
            E.g. ``"mu[1] < z"``.
        """
        text = self.particle.name
        
        # PT rank display
        if self.PTrank!=0:
            text += "[" + str(self.PTrank) + "]"
            
        # Mothers
        if self.mumType!='':
            text += " " + self.mumType + " "
            text += self.mumPart.GetStringDisplay()

        # Return text
        return text    

    def Find(self,id: int) -> bool:
        """Check whether a PDG code belongs to the underlying (multi)particle.

        Args:
            id (``int``): PDG code.

        Returns:
            ``bool``:
            ``True`` if the code belongs to the (multi)particle.
        """
        return self.particle.Find(id)

    def GetIds(self) -> list[int]:
        """Get the PDG codes of the underlying (multi)particle.

        Returns:
            ``list[int]``:
            PDG codes.
        """
        return self.particle.GetIds()

    name = property(GetStringDisplay)

        

