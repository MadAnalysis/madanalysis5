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


"""Combination of particle arguments (e.g. ``M(mu+ mu-)``)."""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any
from madanalysis.selection.instance_name import InstanceName
from operator import itemgetter, attrgetter
import logging
from six.moves import range

class ParticleCombination():
    """Ordered combination of :class:`~madanalysis.multiparticle.extraparticle.ExtraParticle`
    objects, sorted by particle name.

    Attributes:
        extraparticles (``list[ExtraParticle]``): the combined particles.
        ALL (``bool``): ``True`` when the combination is prefixed by ``all``.
    """

    def __init__(self,extraparticles: list) -> None:
        """Create the combination.

        Args:
            extraparticles (``list[ExtraParticle]``): particles to combine (sorted by name).
        """
        self.extraparticles=sorted(extraparticles,\
                                   key=attrgetter("particle.name"))
        self.ALL = False

    def __len__(self) -> int:
        """Get the number of combined particles.

        Returns:
            ``int``:
            Number of particles.
        """
        return len(self.extraparticles)

    def __getitem__(self,i: int) -> Any:
        """Get a combined particle.

        Args:
            i (``int``): index of the particle.

        Returns:
            ``ExtraParticle``:
            The ``i``-th particle.
        """
        return self.extraparticles[i]
        
    def Display(self) -> None:
        """Log the combination."""
        logging.getLogger('MA5').info(" combination = "+self.GetStringDisplay())

    def GetStringDisplay(self) -> str:
        """Get the user-level representation (also available as the :attr:`name` property).

        Returns:
            ``str``:
            E.g. ``"mu+ mu-"``, ``"all j"`` or ``"( b < t ) j"``.
        """
        text=""

        # Case of ALL
        if len(self.extraparticles)==1 and self.ALL:
            text+="all "+self.extraparticles[0].GetStringDisplay()
            return text

        # Other cases    
        for ind in range(0,len(self.extraparticles)):
            if ind!=0:
                text+=" "
            if len(self.extraparticles)>1 and \
               self.extraparticles[ind].mumType!="":
                text+="( "
                text+=self.extraparticles[ind].GetStringDisplay()
                text+=" )"
            else:
                text+=self.extraparticles[ind].GetStringDisplay()
        return text    

    def DoYouUseMultiparticle(self,name: str) -> bool:
        """Check whether a (multi)particle is used in the combination.

        Args:
            name (``str``): label of the (multi)particle.

        Returns:
            ``bool``:
            ``True`` if one of the particles has this name.
        """
        for item in self.extraparticles:
            # FIXME: ExtraParticle.name includes the rank and mother (e.g. 'mu[1]'), so this
            # comparison with a bare label fails for ranked or mother-constrained particles.
            if item.name==name.lower():
                return True
        return False
        
    # egality between 2 ParticleCombination
    def __eq__(self,other: ParticleCombination) -> bool:
        """Compare two combinations particle by particle.

        Args:
            other (``ParticleCombination``): combination to compare with.

        Returns:
            ``bool``:
            ``True`` if both combinations contain equal particles in the same order.
        """
        if len(self)!=len(other):
            return False
        for ind in range(0,len(self)):
            if self[ind]!=other[ind]:
                return False
        return True    
            
    name = property(GetStringDisplay)
        
