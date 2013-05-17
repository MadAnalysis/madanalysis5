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


import madanalysis.core.main as Main
import logging
import os


class UFOParticle:
    def __init__(self,name):
        self.name   = name
        self.pdg    = 0
        self.charge = 0
        self.color  = 0
        self.mass   = ""
        self.width  = ""

class UFOParameter:
    def __init__(self,name,value):
        self.name  = name
        self.value = value
        
class UFOParameterCollection:
    def __init__(self):
        self.parameters = []

    def Add(self,name,value):
        self.parameters.append(UFOParameter(name,value))
        
    def Get(self,name):
        for i in range(0,len(self.parameters)):
            if name==self.parameters[i].name:
                return self.parameters[i]
        return None    

class UFOParticleCollection:
    def __init__(self):
        self.parts = []

    def Add(self,name):
        for i in range(0,len(self.parts)):
            if name==self.parts[i].name:
                return False
        self.parts.append(UFOParticle(name))
        return True

    def Get(self,name):
        for i in range(0,len(self.parts)):
            if name==self.parts[i].name:
                return self.parts[i]
        return None    

    def Remove(self,name):
        for i in range(0,len(self.parts)):
            if name==self.parts[i].name:
                del self.parts[i]
                return True
        return False
        
        
class UFOReader():

    def __init__(self,path,cmd_define):
        self.cmd_define = cmd_define
        self.path       = path
        self.isopen     = False
        self.parts      = UFOParticleCollection()
        self.parameters = UFOParameterCollection()

    @staticmethod
    def CheckStructure(path):
        if not os.path.isdir(path):
            return False
        if not os.path.isfile(path+'/__init__.py'):
            return False
        if not os.path.isfile(path+'/particles.py'):
            return False
        if not os.path.isfile(path+'/parameters.py'):
            return False
        return True
    
    def OpenParticle(self):

        # Checking if the file is opened
        if self.isopen:
            logging.error("cannot open the file called '" + 'particles.py' + "' : it is already opened")
            return False

        name = os.path.normpath(self.path+"/"+'particles.py')
        if os.path.isfile(name):
            self.file = open (name, "r")
        else:
            logging.info('UFO file called ' + name + ' is not found')
        return True    


    def OpenParameter(self):

        # Checking if the file is opened
        if self.isopen:
            logging.error("cannot open the file called '" + 'parameters.py' + "' : it is already opened")
            return False

        name = os.path.normpath(self.path+"/"+'parameters.py')
        if os.path.isfile(name):
            self.file = open (name, "r")
        else:
            logging.info('UFO file called ' + name + ' is not found')
        return True    


    def CreateParticle(self):
        invisibles = []
        for item in self.parts.parts:
            tmp=[]
            tmp.append(item.pdg)
            self.cmd_define.fill(item.name[1:-1],tmp)

            #is it invisible
            if item.charge!=0:
                continue
            if item.color!=1:
                continue

            #look for width
            param = self.parameters.Get(item.width[6:])
            try:
                value=float(param.value)
            except:
                try:
                    value=float(param.value[1:-1])
                except:    
                    continue
            if value!=0:
                continue

            #look for mass
            param = self.parameters.Get(item.mass[6:])
            try:
                value=float(param.value)
            except:
                try:
                    value=float(param.value[1:-1])
                except:    
                    continue
            #if value==0:
            #    continue

            invisibles.append(item)

        inv=""
        tmp=[]
        for item in invisibles:
            if item.pdg==22:
                continue
            inv+=item.name+" "
            tmp.append(item.pdg)

        #Check number of invisible of parts
        if len(tmp)==0:
            return True

        self.cmd_define.fill('invisible',tmp,forced=True)
        logging.info("Adding "+inv+"to 'invisible' multiparticle")

        

    def ReadParameter(self):
        ParameterBlock = False
        Nbrace    = 0
        isName    = False
        isValue   = False
        Name      = ""
        Value     = 0.
        NameTag   = 0
        ValueTag  = 0

        for line in self.file:

            #cleaning the line
            line = line.lstrip()

            #rejecting comment line
            if line.startswith('#'):
                continue

            #rejecting comment part of a line
            if '#' in line:
                line = line.split('#')[0]

            #insering space between connector
            line = line.replace("="," = ")
            line = line.replace("("," ( ")
            line = line.replace(")"," ) ")
            line = line.replace(","," , ")

            #splitting line
            words = line.split()

            #loop over words
            for word in words:

                #looking for ParticleBlock
                if word=="Parameter":
                    ParameterBlock=True
                elif word=="(":
                    Nbrace += 1
                elif word==")":
                    Nbrace -= 1
                    if Nbrace==0 and ParameterBlock:
                        ParameterBlock=False

                #skipping if no ParticleBlock  
                if not ParameterBlock:
                    continue

                #parameter label
                elif word=="name":
                    NameTag=1
                    continue
                elif word=="value":
                    ValueTag=1
                    continue

                #equal     
                elif word=="=":
                    if NameTag==1:
                        NameTag=2
                    if ValueTag==1:
                        ValueTag=2
                    continue
                    
                #value    
                if NameTag==2:
                    NameTag=0
                    isName=True
                    Name=word
                if ValueTag==2:
                    ValueTag=0
                    isValue=True
                    Value=word
                    

                if isName and isValue:
                    isName=False
                    isValue=False
                    
                    #debug message
                    logging.debug("Extracting a parameter labelled ["+Name+\
                                 "] with value=" + Value)
            
                    #feed particle
                    self.parameters.Add(Name[1:-1],Value)

                    #reset
                    Name=""
                    Value=""

        return True



    def ReadParticle(self):

        ParticleBlock = False
        Nbrace   = 0
        isPdg    = False
        isName   = False
        isMass   = False
        isWidth  = False
        isCharge = False
        isAntiname = False
        isColor = False 
        Name     = ""
        Pdg      = ""
        Mass     = ""
        Width    = ""
        Charge   = ""
        Antiname = ""
        Color    = ""
        PdgTag   = 0
        NameTag  = 0
        MassTag  = 0
        WidthTag = 0
        ChargeTag = 0
        AntinameTag = 0
        ColorTag = 0

        for line in self.file:

            #cleaning the line
            line = line.lstrip()

            #rejecting comment line
            if line.startswith('#'):
                continue

            #rejecting comment part of a line
            if '#' in line:
                line = line.split('#')[0]

            #insering space between connector
            line = line.replace("="," = ")
            line = line.replace("("," ( ")
            line = line.replace(")"," ) ")
            line = line.replace(","," , ")

            #splitting line
            words = line.split()

            #loop over words
            for word in words:

                #looking for ParticleBlock
                if word=="Particle":
                    ParticleBlock=True
                elif word=="(":
                    Nbrace += 1
                elif word==")":
                    Nbrace -= 1
                    if Nbrace==0 and ParticleBlock:
                        ParticleBlock=False

                #skipping if no ParticleBlock  
                if not ParticleBlock:
                    continue

                #parameter label
                if word=="pdg_code":
                    PdgTag=1
                    continue
                elif word=="name":
                    NameTag=1
                    continue
                elif word=="mass":
                    MassTag=1
                    continue
                elif word=="width":
                    WidthTag=1
                    continue
                elif word=="charge":
                    ChargeTag=1
                    continue
                elif word=="antiname":
                    AntinameTag=1
                    continue
                elif word=="color":
                    ColorTag=1
                    continue

                #equal     
                elif word=="=":
                    if PdgTag==1:
                        PdgTag=2
                    if NameTag==1:
                        NameTag=2
                    if MassTag==1:
                        MassTag=2
                    if WidthTag==1:
                        WidthTag=2
                    if ChargeTag==1:
                        ChargeTag=2
                    if AntinameTag==1:
                        AntinameTag=2
                    if ColorTag==1:
                        ColorTag=2
                    continue
                    
                #value    
                if PdgTag==2:
                    PdgTag=0
                    isPdg=True
                    Pdg=word
                if NameTag==2:
                    NameTag=0
                    isName=True
                    Name=word
                if WidthTag==2:
                    WidthTag=0
                    isWidth=True
                    Width=word
                if MassTag==2:
                    MassTag=0
                    isMass=True
                    Mass=word
                if ChargeTag==2:
                    ChargeTag=0
                    isCharge=True
                    Charge=word
                if AntinameTag==2:
                    AntinameTag=0
                    isAntiname=True
                    Antiname=word
                if ColorTag==2:
                    ColorTag=0
                    isColor=True
                    Color=word

                if isPdg and isName and isWidth and isMass and isCharge and isAntiname and isColor :
                    isPdg=False
                    isName=False
                    isWidth=False
                    isMass=False
                    isCharge=False
                    isAntiname=False
                    isColor=False
                    
                    #debug message
                    logging.debug("Extracting a particle labelled ["+Name+\
                                 "] with PDG-id=" + Pdg +\
                                 ", mass=" + Mass + ", width=" + \
                                 Width + ", charge=" + Charge + ", color=" + Color)
            
                    #feed particle
                    self.AddParticle(Name, Pdg, Mass, Width, Charge, Color)
                    if Antiname!=Name:
                        self.AddParticle(Antiname, Pdg, Mass, Width, Charge, Color, antiparticle=True)

                    #reset
                    Name=""
                    Pdg=""
                    Width=""
                    Mass=""
                    Charge=""
                    Color=""
        return True

    def AddParticle(self,Name,Pdg,Mass,Width,Charge,Color,antiparticle=False):

        #particle is already defined ?
        if self.parts.Get(Name) is not None:
            return

        #add the new particle in the list
        self.parts.Add(Name)
        thepart = self.parts.Get(Name)

        #add properties
        try:
            thepart.pdg = int(Pdg)
        except :
            logging.error("PDG-ID of the particle " + Name +\
                          " is not an integer value : " + Pdg)
            return
        if antiparticle:
            thepart.pdg*=-1

        try:
            thepart.color = int(Color)
        except :
            logging.error("Color structure of the particle " + Name +\
                          " is not an integer value : " + Color)
            return
        
        thepart.mass = Mass
        thepart.width = Width
        charges=Charge.split("/")
        for item in charges:
            tmp=0
            try:
                tmp=int(item)
            except:
                logging.error("Charge of the particle " + Name +\
                              " is not a float value : " + Charge)
                return

        if len(charges)==1:
            thepart.charge = float(Charge)
        elif len(charges)==2:
            thepart.charge = float(charges[0])/float(charges[1])
        else:
            logging.error("Charge of the particle " + Name +\
                          " is not a float value : " + Charge)
            return

        

    def CloseParticle(self):

        if self.isopen:
           self.file.close() 

        self.isopen = False
        return True


    def CloseParameter(self):

        if self.isopen:
           self.file.close() 

        self.isopen = False 

        logging.info(str(len(self.parts.parts)) + " particles have been successfully exported.")
        return True
