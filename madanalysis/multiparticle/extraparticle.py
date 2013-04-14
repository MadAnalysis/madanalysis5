################################################################################
#  
#  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <http://madanalysis.irmp.ucl.ac.be>
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
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from multiparticle import MultiParticle
import logging

class ExtraParticle():

    def __init__(self,particle,PTrank=0):
        self.particle = particle
        self.PTrank   = PTrank
        self.mumType  = ""
        self.mumPart  = None
        
    def __len__(self):
        return len(self.particle.ids)

    def __getitem__(self,i):
        return self.particle.ids[i]

    def __eq__(self,other):
        if self.particle!=other.particle:
            return False
        if self.mumType!=other.mumType:
            return False
        if self.mumPart!=other.mumPart:
            return False
        return True

    def Display(self):
        logging.info("   ExtraParticle '" + self.particle.name +\
                     "' is defined by : " +\
                     self.GetStringDisplay()+".")

    def GetStringDisplay(self):
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

    def Find(self,id):
        return self.particle.Find(id)

    def GetIds(self):
        return self.particle.GetIds()

    name = property(GetStringDisplay)

        

