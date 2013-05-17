################################################################################
#  
#  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
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


from madanalysis.selection.histogram          import Histogram
from madanalysis.selection.instance_name      import InstanceName
from madanalysis.enumeration.observable_type  import ObservableType
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.interpreter.cmd_cut          import CmdCut
import logging

class Selection:

    def __init__(self):
        self.table = []

    def __len__(self):
        return len(self.table)

    def __getitem__(self,i):
        return self.table[i]

    def Display(self):
        logging.info("   *********** Selection ***********" )
        for ind in range(0,len(self.table)):
            logging.info("   "+str(ind+1)+". "+self.table[ind].GetStringDisplay())
        logging.info("   *********************************" )

    def Find(self,name):
        return False

    def Add(self,item):
        self.table.append(item)

    def Reset(self):
        self.table=[]
 
    def GetItemsUsingMultiparticle(self,name):
        theList=[]
        for ind in range(0,len(self.table)):
            if self.table[ind].DoYouUseMultiparticle(name):
                theList.append(ind)
        return theList        

    def CheckIndex(self,index):
        if index==0 or index>len(self.table):
            logging.error("selection["+str(index)+"] does not exist.")
            return False
        return True


    def Swap(self,i,j):
        if not self.CheckIndex(i):
            return
        if not self.CheckIndex(j):
            return
        tmp = self.table[i-1]
        self.table[i-1]=self.table[j-1]
        self.table[j-1]=tmp 

    def Get(self,name):
        return 

    def Remove(self,index):
        if not self.CheckIndex(index):
            return
        del self.table[index-1]
        return

    def GetNames(self):
        return []

    def RefreshStat(self):
        InstanceName.Clear()

        # Reinitializing counters
        self.Nevent_histos = 0
        self.Npart_histos  = 0
        self.Nhistos       = 0
        self.Nevent_cuts   = 0
        self.Npart_cuts    = 0
        self.Ncuts         = 0
        self.Npid          = 0

        # Loop over selection
        for item in self.table:

            # Histogram case
            if item.__class__.__name__=="Histogram":
                if item.observable.name in ["NPID","NAPID"]:
                    self.Npid+=1 
                    self.Nevent_histos+=1
                elif item.observable.name in ["TET", "MET", "THT", "MHT", "SQRTS"]:
                    self.Nevent_histos+=1
                else:
                    self.Npart_histos+=1

            # Cut case        
            elif item.__class__.__name__=="Cut":
                self.Ncuts+=1
                if len(item.part)==0:
                    self.Nevent_cuts+=1
                else:
                    self.Npart_cuts+=1

        # Updating global counters        
        self.Nhistos = self.Nevent_histos + self.Npart_histos         
        self.Ncuts   = self.Nevent_cuts   + self.Npart_cuts         


