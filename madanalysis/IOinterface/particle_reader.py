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


import madanalysis.core.main as Main
from madanalysis.enumeration.ma5_running_type import MA5RunningType
import logging
import os

class ParticleReader():

    def __init__(self,path,cmd_define,level=MA5RunningType.PARTON,forced=False):
        self.cmd_define = cmd_define
        self.npart      = 0
        self.path       = path
        self.level      = level
        self.isopen     = False
        self.forced     = forced
        self.logger = logging.getLogger('MA5')

    def Load(self):
        if self.level==MA5RunningType.PARTON:
            if not self.OpenPartonLevel():
                return False
            self.Read()
            self.Close()
        elif self.level==MA5RunningType.HADRON:
            if not self.OpenPartonLevel():
                return False
            self.Read()
            self.Close()
            self.OpenHadronLevel()
            self.Read()
            self.Close()
        elif self.level==MA5RunningType.RECO:
            if not self.OpenRecoLevel():
                return False
            self.Read()
            self.Close()
            
         
    def OpenPartonLevel(self):

        # Choose the good filename
        filename="input/particles_name_default.txt"

        # Checking if the file is opened
        if self.isopen:
            self.logger.error("cannot open the file called '" + filename + "' : it is already opened")
            return False

        # Name of the filename
        name = os.path.normpath(self.path+"/"+filename)
        relname = os.path.normpath(self.path+"/"+filename)

        if not os.path.isfile(name):
            name = os.path.normpath(self.path+"/madanalysis/"+filename)
            relname = os.path.normpath("madanalysis/"+filename)

        # Display labels
        if self.level!=MA5RunningType.HADRON:
            self.logger.info('Particle labels exported from ' + relname)
        else: 
            self.logger.info('Parton labels exported from ' + relname)

        # Open the file 
        if os.path.isfile(name):
            self.file = open (name, "r")
        else:
            self.logger.error("File not found")
            return False

        # Ok
        return True    

    def OpenHadronLevel(self):

        # Choose the good filename
        filename="input/hadron_default.txt"

        # Checking if the file is opened
        if self.isopen:
            self.logger.error("cannot open the file called '" + filename + "' : it is already opened")
            return False

        name = os.path.normpath(self.path+"/madanalysis/"+filename)
        relname = os.path.normpath("madanalysis/"+filename)
        self.logger.info('  => Hadron labels from ' + relname)
        if os.path.isfile(name):
            self.file = open (name, "r")
        else:
            self.logger.error("File not found")
            return False

        return True    

    def OpenRecoLevel(self):

        # Choose the good filename
        filename="input/reco_default.txt"

        # Checking if the file is opened
        if self.isopen:
            self.logger.error("cannot open the file called '" + filename + "' : it is already opened")
            return False

        name = os.path.normpath(self.path+"/madanalysis/"+filename)
        self.logger.info('Particle labels from ' + filename)
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
            if len(split)!=2:
                self.DisplayErrorMessage(split,counter)
                continue

            #debug message
            self.logger.debug("Extracting a particle labelled by ["+split[1]+"] with a PDG-id : "+split[0])
            
            #feed particle
            self.cmd_define.fill(split[1],[split[0]],self.forced)
            self.npart += 1

    @staticmethod        
    def DisplayErrorMessage(arg,counter):
            text = "Syntax error at line "\
                   + str(counter) +  " : "
            for item in arg:
                text += item + " "
            self.logger.error(text)    

    def Close(self):

        if self.isopen:
           self.file.close() 

        self.isopen = False 

        self.logger.info("  => " + str(self.npart) + " particles successfully exported.")
