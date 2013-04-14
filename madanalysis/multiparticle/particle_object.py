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


from madanalysis.multiparticle.particle_combination import ParticleCombination
from operator import itemgetter, attrgetter
import logging

class ParticleObject():

    def __init__(self):
        self.table=[]

    def __len__(self):
        return len(self.table)

    def __getitem__(self,i):
        return self.table[i]

    def Display(self):
        logging.info(" **list of particles combination**" )
        for item in self.table:
            item.Display()
        logging.info(" *********************************" )

    def DoYouUseMultiparticle(self,name):
        for item in self.table:
            if item.DoYouUseMultiparticle(name):
                return True
        return False

    def GetStringDisplay(self):
        text=""
        for ind in range(0,len(self.table)):
            if ind!=0:
                text+=" and "
            text+=self.table[ind].GetStringDisplay()
        return text    

    def Add(self,combination,ALL=False):
        part = ParticleCombination(combination)
        part.ALL = ALL
        if not self.Find(part):
            self.table.append(part)
            self.table=sorted(self.table,\
                              key=attrgetter("name"))
        else:
            logging.warning(" Several copies of the combination '"\
                            + part.GetStringDisplay() + \
                            "' have been defined. Only one will be kept.")

    def SameCombinationNumber(self):
        if len(self.table)==0:
            return True
        nb = len(self.table[0])
        for item in self.table:
            if len(item)!=nb:
                return False
        return True    

    def Find(self,object):
        for item in self.table:
            if item.GetStringDisplay()==object.GetStringDisplay():
                return True
        return False

    name = property(GetStringDisplay)
    
    def WriteHeader(self,file):
        for item in self.table:
            item.WriteHeader(file)

    def WriteCppInitialize(self,file):
        newname=InstanceName.Get(self.name)
        file.write('Tab'+newname+'.clear();\n')
        
        pass
    
    def WriteOpeningExecute(self,file):
        # Opening brace
        file.write('{')

        # Declaring indices 
        file.write('unsigned int ind['+str(len(table))+'];')

        # For loops
        for ind in range(0,len(table)):
            file.write('for (unsigned int ind['+str(ind)+']=0;ind['+str(ind)+']<data.parts.size();ind['+str(ind)+']++) {\n')

        # Check if combination contains at least twice copies of the same particles
        file.write('if (CheckSameIndex(ind,'+str(len(table))+')) continue;\n')

        # Check if combination is consistent with particle_object
        conds=[]               
        for item in table:
            conds.append('(Is'+Getcondition+'(+str(ind)+)')
        file.write('if('+'||'.join(conds)+') {\n')               
                       
    def WriteClosingExecute(self,file):
        # Closing IF brace
        file.write('}\n')
        
        # Closing FOR brace    
        for ind in range(0,len(table)):
            file.write('}')
            
        # Closing block brace               
        file.write('\n}\n\n')
            
    def WriteCppFinalize(self,file):
        pass
    
    def WriteJobHeader(self,file,rank,status,level):
        for item in self.table:
            item.WriteJobHeader(file,rank,status,level) 
        
    def WriteJobContainer(self,file,rank,status):
        for item in self.table:
            item.WriteJobContainer(file,rank,status) 

    def WriteJobCleanContainer(self,file,rank,status):
        for item in self.table:
            item.WriteJobCleanContainer(file,rank,status) 

        
    def WriteJobRank(self,file,rank,status,level):
        for item in self.table:
            item.WriteJobRank(file,rank,status,level) 
