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


"""Collection of the signal regions defined in the session."""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any
import copy
import logging
import madanalysis.region.region as Region
import six
from six.moves import range

class RegionCollection:
    """Ordered collection of :class:`~madanalysis.region.region.Region` objects.

    Attributes:
        table (``list[list]``): list of ``[name, Region]`` pairs, in definition order.
    """

    def __init__(self) -> None:
        """Create an empty collection."""
        self.logger = logging.getLogger('MA5')
        self.table = []

    def __len__(self) -> int:
        """Get the number of regions.

        Returns:
            ``int``:
            Number of regions.
        """
        return len(self.table)

    def __getitem__(self,i: int) -> Any:
        """Get a region by position.

        Args:
            i (``int``): index of the region.

        Returns:
            ``Region``:
            The ``i``-th region.
        """
        return self.table[i][1]

    def Display(self,selections: Any) -> None:
        """Log all regions with the cuts attached to each of them.

        Args:
            selections (``Selection``): current selection (plots and cuts).
        """
        ireg=1
        self.logger.info(" ****************** List of defined regions ******************" )
        for value in self.table:
            myreg = value[0]
            self.logger.info(" > Region " + str(ireg) + ": " + myreg)
            ireg+=1
            icut=1
            for ind in range(0,len(selections)):
                if selections[ind].__class__.__name__=="Cut":
                    cutstring = selections[ind].GetStringDisplay().lstrip()
                    if ', regions' in cutstring:
                        cutstring=cutstring[:cutstring.find(', regions')]
                    if myreg in selections[ind].regions:
                        # NOTE: [7:] strips the 'select '/'reject ' prefix of the cut representation.
                        self.logger.info("  ** Cut - "+str(icut)+': ' + cutstring[7:])
                        icut+=1
        self.logger.info(" **************************************************************" )

    def Find(self,name: str) -> bool:
        """Check whether a region exists.

        Args:
            name (``str``): name of the region.

        Returns:
            ``bool``:
            ``True`` if the region exists.
        """
        # FIXME: str.lower() result discarded (same in Add/Get/Remove): lookups are case-sensitive.
        name.lower()
        for item in self.table:
            if name == item[0]:
                return True
        return False

    def Add(self,name: str) -> None:
        """Create a region (ignored if it already exists).

        Args:
            name (``str``): name of the region.
        """
        name.lower()
        if not self.Find(name):
            self.table.append([name,Region.Region(name)])

    def Get(self,name: str) -> Any:
        """Get a region by name.

        Args:
            name (``str``): name of the region.

        Returns:
            ``Region | None``:
            The region, or ``None`` if it does not exist.
        """
        name.lower()
        for item in self.table:
            if name == item[0]:
                return item[1]
        return None

    def Remove(self,name: str) -> None:
        """Remove a region.

        Args:
            name (``str``): name of the region.
        """
        name.lower()
        if self.Find(name):
            newtable = []
            for item in self.table:
                if name != item[0]:
                    newtable.append(item)
            self.table = newtable        

    def Reset(self) -> None:
        """Remove all regions."""
        self.table = []

    def GetNames(self) -> list[str]:
        """Get the names of all regions.

        Returns:
            ``list[str]``:
            Region names in definition order.
        """
        names=[]
        for item in self.table:
            names.append(item[0])
        return names

    def GetClusteredRegions(self, selections: Any) -> list[list[str]]:
        """Group the regions that are indistinguishable given the current cuts.

        Starting from a single cluster containing all regions, each cut splits every
        cluster into the regions to which the cut applies and those to which it does not.
        Two regions end up in the same cluster if and only if they share exactly the same
        cuts. Histograms can only be attached to regions of a same cluster.

        Args:
            selections (``Selection``): current selection (plots and cuts).

        Returns:
            ``list[list[str]]``:
            Clusters of region names (``[[]]`` if no region is defined).
        """
        clusteredregions = copy.copy([self.GetNames()])
        if clusteredregions == [[]]:
            return clusteredregions
        for myselection in selections:
            if myselection.__class__.__name__!="Cut":
                continue
            newclusteredregions = copy.copy(clusteredregions)
            for icluster in range(0,len(clusteredregions)):
                newcluster = []
                oldcluster = copy.copy(clusteredregions[icluster])
                for singleregion in clusteredregions[icluster]:
                    if singleregion in myselection.regions:
                        newcluster.append(singleregion)
                        oldcluster.remove(singleregion)
                newclusteredregions.append(newcluster)
                newclusteredregions[icluster] = oldcluster
            clusteredregions=newclusteredregions
            clusteredregions=[x for x in clusteredregions if x != [] ]
        clusteredregions=[list(set(x)) for x in clusteredregions ]
        return clusteredregions


    def LoadWithSAF(self,ast: Any) -> None:
        """Rebuild the collection from a parsed SAF tree.

        Args:
            ast (``Any``): parsed SAF tree.

        .. warning::
            No class of the code base implements the tree interface used here
            (``GetBranch``): this method is currently dead code.
        """
        # Reseting the region collection
        self.Reset()
        
        # Getting region branches
        regions = ast.GetBranch("regions",1)
        if regions==None:
            return

        # Looping over the branches of the tree
        for key, value in six.iteritems(regions.GetBranches()):

            # Keeping only 'region' branches
            if key[0]!='region':
                continue

            # Getting the name of the region (if it exists)
            name = value.GetParameterToStringWithoutQuotes('name')
            if name==None:
                # NOTE: the message mentions 'multiparticle' instead of 'region'.
                logging.getLogger('MA5').error('multiparticle name is not found in the tree')
                continue

            self.Add(name)
                
            
        
