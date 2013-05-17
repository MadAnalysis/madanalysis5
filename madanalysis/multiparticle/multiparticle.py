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


import logging
class MultiParticle:

    def __init__(self,name,ids):
        self.name = name.lower()
        self.ids=[]
        for item in ids:
            if not item in self.ids:
                self.ids.append(item)
        self.ids.sort()
        
    def __len__(self):
        return len(self.ids)

    def __getitem__(self,i):
        return self.ids[i]

    def __eq__(self,other):
        if len(self)!=len(other):
            return False
        for ind in range(0,len(self)):
            if self[ind]!=other[ind]:
                return False
        return True    

    def Display(self):
        text = ""
        if len(self.ids)==1:
            text = "   The particle '"+ self.name + "' is defined by the PDG-id "
        else:
            text = "   The multiparticle '"+ self.name + "' is defined by the PDG-ids "
        text = text+self.GetStringDisplay();
        text = text[:-1]
        logging.info(text+".")

    def GetStringDisplay(self):
        text = ""
        for item in self.ids:
            text += str(item) + " "
        return text    

    def DisplayParameter(self,data):
        logging.error("Particles and Multiparticles have no attributes.")

    def SetParameter(self,parameter,value):
        logging.error("Particles and Multiparticles have no attribute denoted "+parameter+".")

    def GetParameters(self):
        return []

    def Find(self,id):
        if id in self.ids:
            return True
        return False

    def Add(self,id):
        if not self.Find(id):
            self.ids.append(id)
        self.ids.sort()
        
    def Remove(self,id):
        if self.Find(id):
            self.ids.remove(id)

    def GetIds(self):
        return self.ids

    def IsThereCommonPart(self,multi):
        for id in multi.ids:
            if id in self.ids:
                return True
        return False    
