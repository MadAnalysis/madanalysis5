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


"""List of particle combinations used as observable arguments (``and``-separated)."""

from __future__ import absolute_import
from __future__ import annotations
from typing import IO
from madanalysis.multiparticle.particle_combination import ParticleCombination
from operator import itemgetter, attrgetter
import logging
from six.moves import range

class ParticleObject():
    """Set of :class:`~madanalysis.multiparticle.particle_combination.ParticleCombination`
    objects, kept sorted by name and without duplicates.

    .. warning::
        The ``Write*`` methods are legacy code generators: several of them reference
        undefined names or methods that do not exist in
        :class:`~madanalysis.multiparticle.particle_combination.ParticleCombination`, and
        none of them is called anywhere (see the FIXME notes).
    """

    def __init__(self) -> None:
        """Create an empty list of combinations."""
        self.table=[]

    def __len__(self) -> int:
        """Get the number of combinations.

        Returns:
            ``int``:
            Number of combinations.
        """
        return len(self.table)

    def __getitem__(self,i: int) -> ParticleCombination:
        """Get a combination.

        Args:
            i (``int``): index of the combination.

        Returns:
            ``ParticleCombination``:
            The ``i``-th combination.
        """
        return self.table[i]

    def Display(self) -> None:
        """Log all combinations."""
        logging.getLogger('MA5').info(" **list of particles combination**" )
        for item in self.table:
            item.Display()
        logging.getLogger('MA5').info(" *********************************" )

    def DoYouUseMultiparticle(self,name: str) -> bool:
        """Check whether a (multi)particle is used in one of the combinations.

        Args:
            name (``str``): label of the (multi)particle.

        Returns:
            ``bool``:
            ``True`` if the (multi)particle is used.
        """
        for item in self.table:
            if item.DoYouUseMultiparticle(name):
                return True
        return False

    def GetStringDisplay(self) -> str:
        """Get the user-level representation (also available as the :attr:`name` property).

        Returns:
            ``str``:
            Combinations separated by ``" and "``.
        """
        text=""
        for ind in range(0,len(self.table)):
            if ind!=0:
                text+=" and "
            text+=self.table[ind].GetStringDisplay()
        return text    

    def Add(self,combination: list,ALL: bool = False) -> None:
        """Add a combination (a warning is logged for duplicates).

        Args:
            combination (``list[ExtraParticle]``): particles of the combination.
            ALL (``bool``, default ``False``): whether the combination is prefixed by ``all``.
        """
        part = ParticleCombination(combination)
        part.ALL = ALL
        if not self.Find(part):
            self.table.append(part)
            self.table=sorted(self.table,\
                              key=attrgetter("name"))
        else:
            logging.getLogger('MA5').warning(" Several copies of the combination '"\
                            + part.GetStringDisplay() + \
                            "' have been defined. Only one will be kept.")

    def SameCombinationNumber(self) -> bool:
        """Check that all combinations contain the same number of particles.

        Returns:
            ``bool``:
            ``True`` if all combinations have the same size (or if the list is empty).
        """
        if len(self.table)==0:
            return True
        nb = len(self.table[0])
        for item in self.table:
            if len(item)!=nb:
                return False
        return True    

    def Find(self,object: ParticleCombination) -> bool:
        """Check whether a combination is already stored (comparison of string representations).

        Args:
            object (``ParticleCombination``): combination to look for.

        Returns:
            ``bool``:
            ``True`` if an identical combination exists.
        """
        for item in self.table:
            if item.GetStringDisplay()==object.GetStringDisplay():
                return True
        return False

    name = property(GetStringDisplay)
    
    # FIXME: ParticleCombination defines none of WriteHeader/WriteJobHeader/WriteJobContainer/
    # WriteJobCleanContainer/WriteJobRank: the forwarding methods below raise AttributeError.
    # None of the Write* methods is called anywhere (dead code).
    def WriteHeader(self,file: IO[str]) -> None:
        """Legacy: write the C++ declarations of each combination.

        Args:
            file (``IO[str]``): output C++ file.
        """
        for item in self.table:
            item.WriteHeader(file)

    def WriteCppInitialize(self,file: IO[str]) -> None:
        """Legacy: write the C++ clearing of the container associated with the object.

        Args:
            file (``IO[str]``): output C++ file.
        """
        # FIXME: InstanceName is not imported in this module (NameError).
        newname=InstanceName.Get(self.name)
        file.write('Tab'+newname+'.clear();\n')
        
        pass
    
    def WriteOpeningExecute(self,file: IO[str]) -> None:
        """Legacy: write the opening of the C++ loops over particle indices.

        Args:
            file (``IO[str]``): output C++ file.
        """
        # Opening brace
        file.write('{')

        # Declaring indices 
        # FIXME: 'table' is undefined (self.table intended?) -> NameError.
        file.write('unsigned int ind['+str(len(table))+'];')

        # For loops
        for ind in range(0,len(table)):
            file.write('for (unsigned int ind['+str(ind)+']=0;ind['+str(ind)+']<data.parts.size();ind['+str(ind)+']++) {\n')

        # Check if combination contains at least twice copies of the same particles
        file.write('if (CheckSameIndex(ind,'+str(len(table))+')) continue;\n')

        # Check if combination is consistent with particle_object
        conds=[]               
        for item in table:
            # FIXME: 'Getcondition' is undefined and the string concatenation is malformed.
            conds.append('(Is'+Getcondition+'(+str(ind)+)')
        file.write('if('+'||'.join(conds)+') {\n')               
                       
    def WriteClosingExecute(self,file: IO[str]) -> None:
        """Legacy: write the closing of the C++ loops opened by :meth:`WriteOpeningExecute`.

        Args:
            file (``IO[str]``): output C++ file.
        """
        # Closing IF brace
        file.write('}\n')
        
        # Closing FOR brace    
        for ind in range(0,len(table)):
            file.write('}')
            
        # Closing block brace               
        file.write('\n}\n\n')
            
    def WriteCppFinalize(self,file: IO[str]) -> None:
        """Legacy: nothing is written.

        Args:
            file (``IO[str]``): output C++ file.
        """
        pass
    
    def WriteJobHeader(self,file: IO[str],rank: str,status: str,level: int) -> None:
        """Legacy: forward to ``WriteJobHeader`` of each combination.

        Args:
            file (``IO[str]``): output C++ file.
            rank (``str``): ranking criterion.
            status (``str``): status-code selection.
            level (``int``): running mode.
        """
        for item in self.table:
            item.WriteJobHeader(file,rank,status,level) 
        
    def WriteJobContainer(self,file: IO[str],rank: str,status: str) -> None:
        """Legacy: forward to ``WriteJobContainer`` of each combination.

        Args:
            file (``IO[str]``): output C++ file.
            rank (``str``): ranking criterion.
            status (``str``): status-code selection.
        """
        for item in self.table:
            item.WriteJobContainer(file,rank,status) 

    def WriteJobCleanContainer(self,file: IO[str],rank: str,status: str) -> None:
        """Legacy: forward to ``WriteJobCleanContainer`` of each combination.

        Args:
            file (``IO[str]``): output C++ file.
            rank (``str``): ranking criterion.
            status (``str``): status-code selection.
        """
        for item in self.table:
            item.WriteJobCleanContainer(file,rank,status) 

        
    def WriteJobRank(self,file: IO[str],rank: str,status: str,level: int) -> None:
        """Legacy: forward to ``WriteJobRank`` of each combination.

        Args:
            file (``IO[str]``): output C++ file.
            rank (``str``): ranking criterion.
            status (``str``): status-code selection.
            level (``int``): running mode.
        """
        for item in self.table:
            item.WriteJobRank(file,rank,status,level) 
