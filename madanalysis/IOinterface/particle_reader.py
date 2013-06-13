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
            logging.error("cannot open the file called '" + filename + "' : it is already opened")
            return False

        name = os.path.normpath(self.path+"/"+filename)
        if os.path.isfile(name):
            logging.info("MadGraph 5 found:")
            if self.level!=MA5RunningType.HADRON:
                logging.info('  => Particle labels exported from ' + name)
            else: 
                logging.info('  => Parton labels exported from ' + name)
            self.file = open (name, "r")
        else:
            name = os.path.normpath(self.path+"/madanalysis/"+filename)
            logging.info("MadGraph 5 NOT found:")
            if self.level!=MA5RunningType.HADRON:
                logging.info('  => Particle labels from ' + filename)
            else: 
                logging.info('  => Parton labels from ' + filename)
            if os.path.isfile(name):
                self.file = open (name, "r")
            else:
                logging.error("File not found")
                return False
        return True    

    def OpenHadronLevel(self):

        # Choose the good filename
        filename="input/hadron_default.txt"

        # Checking if the file is opened
        if self.isopen:
            logging.error("cannot open the file called '" + filename + "' : it is already opened")
            return False

        name = os.path.normpath(self.path+"/madanalysis/"+filename)
        logging.info('  => Hadron labels from ' + filename)
        if os.path.isfile(name):
            self.file = open (name, "r")
        else:
            logging.error("File not found")
            return False

        return True    

    def OpenRecoLevel(self):

        # Choose the good filename
        filename="input/reco_default.txt"

        # Checking if the file is opened
        if self.isopen:
            logging.error("cannot open the file called '" + filename + "' : it is already opened")
            return False

        name = os.path.normpath(self.path+"/madanalysis/"+filename)
        logging.info('Particle labels from ' + filename)
        if os.path.isfile(name):
            self.file = open (name, "r")
        else:
            logging.error("File not found")
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
            logging.debug("Extracting a particle labelled by ["+split[1]+"] with a PDG-id : "+split[0])
            
            #feed particle
            self.cmd_define.fill(split[1],[split[0]],self.forced)
            self.npart += 1

    @staticmethod        
    def DisplayErrorMessage(arg,counter):
            text = "Syntax error at line "\
                   + str(counter) +  " : "
            for item in arg:
                text += item + " "
            logging.error(text)    

    def Close(self):

        if self.isopen:
           self.file.close() 

        self.isopen = False 

        logging.info("  => " + str(self.npart) + " particles successfully exported.")
