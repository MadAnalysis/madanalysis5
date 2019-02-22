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


from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.core.main                    import Main
import logging
import os

class MultiparticleReader():

    def __init__(self,path,cmd_define,level=MA5RunningType.PARTON,forced=False):
        self.cmd_define = cmd_define
        self.npart      = 0
        self.path       = path
        self.isopen     = False
        self.level      = level
        self.forced     = forced
        self.logger = logging.getLogger('MA5')

    def Load(self):
        if self.level==MA5RunningType.PARTON:
            if not self.OpenPartonLevel():
                return False
            self.Read()
            self.AddSpecialMultiparticles()
            self.Close()
        elif self.level==MA5RunningType.HADRON:
            if not self.OpenHadronLevel():
                return False
            self.Read()
            self.AddSpecialMultiparticles()
            self.Close()
        elif self.level==MA5RunningType.RECO:
            if not self.OpenRecoLevel():
                return False
            self.Read()
            self.Close()
            self.AddSpecialMultiparticles()
            self.Close()
         
    def OpenPartonLevel(self):        

        filename   = "input/multiparticles_default.txt"

        # Checking if the file is opened
        if self.isopen:
            self.logger.error("cannot open the file called '" + filename + "' : it is already opened")
            return False

        # Files
        name = os.path.normpath(self.path+"/"+filename)
        relname = os.path.normpath(self.path+"/"+filename)
        if not os.path.isfile(name):
            name = os.path.normpath(self.path+"/madanalysis/"+filename)
            relname = os.path.normpath("madanalysis/"+filename)

        # Display file location
        self.logger.info("Multiparticle labels exported from " + relname)

        # Open
        if os.path.isfile(name):
            self.file = open (name, "r")
        else:
            self.logger.error("File not found")
            return False

        # Ok
        return True    


    def OpenHadronLevel(self):        

        filename   = "input/multiparticles_hadron_default.txt"

        # Checking if the file is opened
        if self.isopen:
            self.logger.error("cannot open the file called '" + filename + "' : it is already opened")
            return False

        name = os.path.normpath(self.path+"/madanalysis/"+filename)
        relname = os.path.normpath("madanalysis/"+filename)
        self.logger.info('Multiparticle labels from '+relname)
        if os.path.isfile(name):
            self.file = open (name, "r")
        else:
            self.logger.error("File '"+name+"' not found")
            return False

        return True    


    def OpenRecoLevel(self):        

        filename   = "input/multiparticles_reco_default.txt"

        # Checking if the file is opened
        if self.isopen:
            self.logger.error("cannot open the file called '" + filename + "' : it is already opened")
            return False

        name = os.path.normpath(self.path+"/madanalysis/"+filename)
        relname = os.path.normpath("madanalysis/"+filename)
        self.logger.info('Multiparticle labels from '+relname)
        if os.path.isfile(name):
            self.file = open (name, "r")
        else:
            self.logger.error("File not found")
            return False

        return True    


    def Read(self):

        counter = 0
        for line in self.file:

            #increasing line number
            counter += 1

            #isolating '=' character 
            line = line.replace('=',' = ')
            
            #cleaning the line
            line = line.lstrip()

            #rejecting comment line
            if line.startswith('#'):
                continue

            #rejecting comment part of a line
            if '#' in line:
                line = line.split('#')[0]

            #splitting line
            split = line.split()

            #checking number of argument
            if len(split)==0:
                continue
            if len(split)<3:
                self.DisplayErrorMessage(split,counter)
                continue

            #checking '='
            if split[1]!="=":
                self.DisplayErrorMessage(split,counter)
                continue

            #debug message
            self.logger.debug("Extracting a multiparticle labelled by ["+split[0]+"] with a PDG-id : "+str(split[2:]))
            
            #feed multiparticle
            self.cmd_define.fill(split[0],split[2:],self.forced)
            self.npart += 1


    @staticmethod        
    def DisplayErrorMessage(arg,counter):
            text = "Syntax error at line "\
                   + str(counter) +  " : "
            for item in arg:
                text += item + " "
            self.logger.error(text)    


    def AddSpecialMultiparticles(self):
        # Creating invisible and hadronic multiparticles (mandatory in parton and hadron level)
        if self.level==MA5RunningType.RECO:
            return
            
        if not self.cmd_define.main.multiparticles.Find("invisible"):
            self.cmd_define.main.multiparticles.Add("invisible",[12,-12,14,-14,16,-16,1000022])
            self.npart += 1
            self.logger.info("  => Creation of the label 'invisible' (-> missing energy).")
        if not self.cmd_define.main.multiparticles.Find("hadronic"):
            self.cmd_define.main.multiparticles.Add("hadronic",[1,2,3,4,5,-1,-2,-3,-4,-5,21])
            self.npart += 1
            self.logger.info("  => Creation of the label 'hadronic' (-> jet energy).")
        

    def Close(self):

        if self.isopen:
           self.file.close() 

        self.isopen = False 

        self.logger.info("  => " + str(self.npart) + " multiparticles successfully exported.")
