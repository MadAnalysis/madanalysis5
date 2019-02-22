################################################################################
#  
#  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://launchpad.net/madanalysis5>
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


import copy
import logging
import madanalysis.region.region as Region

class RegionCollection:

    def __init__(self):
        self.logger = logging.getLogger('MA5')
        self.table = []

    def __len__(self):
        return len(self.table)

    def __getitem__(self,i):
        return self.table[i][1]

    def Display(self,selections):
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
                        self.logger.info("  ** Cut - "+str(icut)+': ' + cutstring[7:])
                        icut+=1
        self.logger.info(" **************************************************************" )

    def Find(self,name):
        name.lower()
        for item in self.table:
            if name == item[0]:
                return True
        return False

    def Add(self,name):
        name.lower()
        if not self.Find(name):
            self.table.append([name,Region.Region(name)])

    def Get(self,name):
        name.lower()
        for item in self.table:
            if name == item[0]:
                return item[1]
        return None

    def Remove(self,name):
        name.lower()
        if self.Find(name):
            newtable = []
            for item in self.table:
                if name != item[0]:
                    newtable.append(item)
            self.table = newtable        

    def Reset(self):
        self.table = []

    def GetNames(self):
        names=[]
        for item in self.table:
            names.append(item[0])
        return names

    def GetClusteredRegions(self, selections):
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

