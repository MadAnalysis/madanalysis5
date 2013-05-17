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


from madanalysis.selection.instance_name import InstanceName
from operator import itemgetter, attrgetter
import logging

class ParticleCombination():

    def __init__(self,extraparticles):
        self.extraparticles=sorted(extraparticles,\
                                   key=attrgetter("particle.name"))
        self.ALL = False

    def __len__(self):
        return len(self.extraparticles)

    def __getitem__(self,i):
        return self.extraparticles[i]
        
    def Display(self):
        logging.info(" combination = "+self.GetStringDisplay())

    def GetStringDisplay(self):
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

    def DoYouUseMultiparticle(self,name):
        for item in self.extraparticles:
            if item.name==name.lower():
                return True
        return False
        
    # egality between 2 ParticleCombination
    def __eq__(self,other):
        if len(self)!=len(other):
            return False
        for ind in range(0,len(self)):
            if self[ind]!=other[ind]:
                return False
        return True    
            
    name = property(GetStringDisplay)
        
