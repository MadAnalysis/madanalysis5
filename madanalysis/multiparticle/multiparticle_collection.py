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


"""Collection of the particle and multiparticle labels defined in the session."""

from __future__ import absolute_import
from __future__ import print_function
from __future__ import annotations
from typing import Any
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.multiparticle.multiparticle  import MultiParticle
import logging
import six
from six.moves import input

class MultiParticleCollection:
    """Dictionary of :class:`~madanalysis.multiparticle.multiparticle.MultiParticle` objects.

    Particles are the entries with a single PDG code, multiparticles those with several.

    Attributes:
        table (``dict[str, MultiParticle]``): definitions indexed by label.
    """

    def __init__(self) -> None:
        """Create an empty collection."""
        self.table = {}

    def __len__(self) -> int:
        """Get the number of labels.

        Returns:
            ``int``:
            Number of (multi)particles.
        """
        return len(self.table)

    def __getitem__(self,i: str) -> MultiParticle:
        """Get a definition by label.

        Args:
            i (``str``): label (dictionary key, despite the argument name).

        Raises:
            ``KeyError``: if the label is not defined.

        Returns:
            ``MultiParticle``:
            The definition.
        """
        return self.table[i]

    def DisplayMultiparticles(self) -> None:
        """Log the labels of all multiparticles (sorted)."""
        sorted_keys = sorted(self.table.keys())
        msg = ""
        for key in sorted_keys:
            if len(self.table[key])>1:
                msg += key + " " 
        logging.getLogger('MA5').info(msg)        

    def DisplayParticles(self) -> None:
        """Log the labels of all particles (sorted)."""
        sorted_keys = sorted(self.table.keys())
        msg = ""
        for key in sorted_keys:
            if len(self.table[key])==1:
                msg += key + " "
        logging.getLogger('MA5').info(msg)        

    def Find(self,name: str) -> bool:
        """Check whether a label is defined.

        Args:
            name (``str``): label.

        Returns:
            ``bool``:
            ``True`` if the label exists.
        """
        # FIXME: str.lower() result discarded (same in Add/Get/Remove): lookups are case-sensitive.
        name.lower()
        if name in list(self.table.keys()):
            return True
        return False

    def Add(self,name: str,ids: list[int],forced: bool = False) -> None:
        """Define (or redefine) a label.

        If the label already exists and ``forced`` is ``False``, the user is asked
        interactively whether the previous definition must be overwritten.

        Args:
            name (``str``): label.
            ids (``list[int]``): PDG codes.
            forced (``bool``, default ``False``): overwrite without asking.
        """
        name.lower()
        if self.Find(name) and not forced:
            logging.getLogger('MA5').warning("Particle/Multiparticle labelled '"+name+"' is" + \
                            " already defined.")
            logging.getLogger('MA5').warning("Would you like to overwrite the previous " + \
                            "definition ? (Y/N)")
            allowed_answers=['n','no','y','yes']
            answer=""
            while answer not in  allowed_answers:
                answer=input("Answer: ")
                answer=answer.lower()
            if answer=="no" or answer=="n":
                return
        self.table[name]=MultiParticle(name,ids)

    def Get(self,name: str) -> MultiParticle:
        """Get a definition by label.

        Args:
            name (``str``): label.

        Raises:
            ``KeyError``: if the label is not defined.

        Returns:
            ``MultiParticle``:
            The definition.
        """
        name.lower()
        return self.table[name]

    def Reset(self) -> None:
        """Remove all labels."""
        self.table = {}
            
    def ResetParticles(self) -> None:
        """Remove all particles (single PDG code)."""
        for key in list(self.table.keys()):
            if len(self.table[key])==1:
                del self.table[key]

    def ResetMultiparticles(self) -> None:
        """Remove all multiparticles (several PDG codes)."""
        for key in list(self.table.keys()):
            if len(self.table[key])!=1:
                del self.table[key]

    def Remove(self,name: str,level: int) -> None:
        """Remove a label.

        Args:
            name (``str``): label.
            level (``int``): running mode; ``hadronic`` and ``invisible`` are protected
                outside the reco mode.
        """
        name.lower()
        if self.Find(name):
            # NOTE: the reserved keywords are only protected outside the RECO mode.
            if level!=MA5RunningType.RECO and \
                   ( name=="hadronic" or name=="invisible" ) :
                logging.getLogger('MA5').error("this multiparticle cannot be removed (reserved keyword).")
            else:    
                del self.table[name]

    def GetNames(self) -> list[str]:
        """Get all labels.

        Returns:
            ``list[str]``:
            Sorted labels.
        """
        return sorted(self.table.keys())

    def GetName(self,id: int) -> str:
        """Get the label of the particle associated with a PDG code.

        Args:
            id (``int``): PDG code.

        Returns:
            ``str``:
            Label of a single-code entry containing ``id``, or ``""``.
        """
        for key,multi in self.table.items():
            if len(multi)==1 and multi.Find(id):
                return key
        return ""

    def GetAName(self,id1: int,id2: int) -> str:
        """Get a label for a pair of PDG codes (e.g. a particle and its antiparticle).

        Args:
            id1 (``int``): first PDG code.
            id2 (``int``): second PDG code.

        Returns:
            ``str``:
            ``"<label1>/<label2>"`` (smaller code first), a single label if only one is
            known, or ``""``.
        """
        if id1>id2:
            a=id2
            b=id1
        else:
            a=id1
            b=id2
        s1=""
        for key,multi in self.table.items():
            if len(multi)==1 and multi.Find(a):
                s1=key
        s2=""        
        for key,multi in self.table.items():
            if len(multi)==1 and multi.Find(b):
                s2=key
        if s1=="" and s2=="":
            return ""
        elif s1!="" and s2=="":
            return s1
        elif s1=="" and s2!="":
            return s2
        else:
            return s1 + "/" + s2


    def LoadWithSAF(self,ast: Any) -> None:
        """Rebuild the collection from a parsed SAF tree.

        Args:
            ast (``Any``): parsed SAF tree.

        .. warning::
            No class of the code base implements the tree interface used here
            (``GetBranch``): this method is currently dead code.
        """
        # Reseting the multiparticle collection
        self.Reset()
        
        # Getting multiparticles branches
        multiparticles = ast.GetBranch("multiparticles",1)
        if multiparticles==None:
            return

            # Looping over the branches of the tree
        for key, value in six.iteritems(multiparticles.GetBranches()):

            # Keeping only 'multiparticle' branches
            if key[0]!='multiparticle':
                continue

            # Getting the name of the multiparticle (if it exists)
            name = value.GetParameterToStringWithoutQuotes('name')
            if name==None:
                logging.getLogger('MA5').error('multiparticle name is not found in the tree')
                continue

            # Getting all PIDs
            tmp=[]
            for item in value.GetStack():
                try:
                    a = int(item)
                except:
                    print("ERROR: impossible to convert '"+str(item)+"' to integer value")
                # FIXME: if int() fails, the previous value of 'a' (or an undefined 'a') is appended.
                tmp.append(a)
            self.Add(name,tmp,forced=False)
                
            
        
