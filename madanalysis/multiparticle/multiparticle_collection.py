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


from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.multiparticle.multiparticle  import MultiParticle
import logging

class MultiParticleCollection:

    def __init__(self):
        self.table = {}

    def __len__(self):
        return len(self.table)

    def __getitem__(self,i):
        return self.table[i]

    def DisplayMultiparticles(self):
        sorted_keys = sorted(self.table.keys())
        msg = ""
        for key in sorted_keys:
            if len(self.table[key])>1:
                msg += key + " " 
        logging.info(msg)        

    def DisplayParticles(self):
        sorted_keys = sorted(self.table.keys())
        msg = ""
        for key in sorted_keys:
            if len(self.table[key])==1:
                msg += key + " "
        logging.info(msg)        

    def Find(self,name):
        name.lower()
        if name in self.table.keys():
            return True
        return False

    def Add(self,name,ids,forced=False):
        name.lower()
        if self.Find(name) and not forced:
            logging.warning("Particle/Multiparticle labelled '"+name+"' is" + \
                            " already defined.")
            logging.warning("Would you like to overwrite the previous " + \
                            "definition ? (Y/N)")
            allowed_answers=['n','no','y','yes']
            answer=""
            while answer not in  allowed_answers:
                answer=raw_input("Answer: ")
                answer=answer.lower()
            if answer=="no" or answer=="n":
                return
        self.table[name]=MultiParticle(name,ids)

    def Get(self,name):
        name.lower()
        return self.table[name]

    def Reset(self):
        self.table = {}
            
    def ResetParticles(self):
        for key in self.table.keys():
            if len(self.table[key])==1:
                del self.table[key]

    def ResetMultiparticles(self):
        for key in self.table.keys():
            if len(self.table[key])!=1:
                del self.table[key]

    def Remove(self,name,level):
        name.lower()
        if self.Find(name):
            if level!=MA5RunningType.RECO and \
                   ( name=="hadronic" or name=="invisible" ) :
                logging.error("this multiparticle cannot be removed (reserved keyword).")
            else:    
                del self.table[name]

    def GetNames(self):
        return sorted(self.table.keys())

    def GetName(self,id):
        for key,multi in self.table.items():
            if len(multi)==1 and multi.Find(id):
                return key
        return ""

    def GetAName(self,id1,id2):
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

