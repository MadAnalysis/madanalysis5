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


from madanalysis.IOinterface.folder_writer import FolderWriter
from madanalysis.IOinterface.job_writer import JobWriter
import logging
import glob
import os
import commands
import sys
import shutil

class ExpertMode:

    def __init__(self,main):
        self.main=main
        self.path=""
        self.forbiddenpaths=[]
        self.forbiddenpaths.append(os.path.normpath(self.main.archi_info.ma5dir+'/lib'))
        self.forbiddenpaths.append(os.path.normpath(self.main.archi_info.ma5dir+'/bin'))
        self.forbiddenpaths.append(os.path.normpath(self.main.archi_info.ma5dir+'/madanalysis'))
        
    def CreateDirectory(self,name):
        if name=="":
          logging.getLogger('MA5').debug("Debug mode: no directory name is specified.")
          logging.getLogger('MA5').info("\nWelcome to the expert mode of MadAnalysis")
          logging.getLogger('MA5').info("Please enter a folder name for creating an empty SampleAnalyzer job")
          answer=raw_input("Answer: ")
          answer=answer.replace(' ','_')
          name  =answer.replace('-','_')
        else:
          logging.getLogger('MA5').debug("Debug mode: a directory name is specified.")

        # Getting the full path
        self.path = os.path.expanduser(name)
        if not self.path.startswith('/'):
            if name in ['tools/PAD', 'tools/PADForMA5tune']:
                self.path = self.main.archi_info.ma5dir+'/'+self.path
            else:
                self.path = self.main.currentdir+'/'+self.path
        self.path = os.path.normpath(self.path)
        logging.getLogger('MA5').debug("Full path of the working directory: "+self.path)

        # Checking folder
        logging.getLogger('MA5').debug("Check that the path is not forbidden...")
        if self.path in self.forbiddenpaths:
            logging.getLogger('MA5').error("the folder '"+anwser+"' is a MadAnalysis folder. " + \
                         "You cannot overwrite it. Please choose another folder.")
            return False

        # Checking if the job folder exists, if not remove it
        if os.path.isdir(self.path):
            logging.getLogger('MA5').debug("Remove the previous folder?")
            question="A directory called '"+self.path+"' is already defined.\n"+\
                 "Would you like to remove it ? (Y/N)"
            from madanalysis.IOinterface.folder_writer import FolderWriter
            test = FolderWriter.RemoveDirectory(self.path,question)
            if not test[0] or not test[1]:
                return False

        return True


    def Copy(self,name):
        
        # Initializing the JobWriter
        jobber = JobWriter(self.main,self.path,False)
        
        # Writing process
        logging.getLogger('MA5').info("   Creating folder '"+self.path+"'...")
        if not jobber.Open():
            logging.getLogger('MA5').error("job submission aborted.")
            return False

        # Copying SampleAnalyzer
        logging.getLogger('MA5').info("   Copying required 'SampleAnalyzer' source files...")
        if not jobber.CopyLHEAnalysis():
            logging.getLogger('MA5').error("   job submission aborted.")
            return False

        # Writing an empty analysis
        if name=="":
          logging.getLogger('MA5').info("Please enter a name for your analysis")
          title=raw_input("Answer: ")
          if title=="":
              title="user"
          title=title.replace(' ', '_');
          name=title.replace('-', '_');
        logging.getLogger('MA5').info("   Writing an empty analysis...")
        os.system("cd "+self.path+"/Build/SampleAnalyzer; python newAnalyzer.py " + name + " 1")

        # Extracting analysis name
        file = open(self.path+"/Build/SampleAnalyzer/User/Analyzer/analysisList.h")
        title=""
        for line in file:
            if "Add" not in line:
                continue
            words = line.split('"')
            if len(words)>=3:
                title=words[1]
                break
        file.close()

        # Writing a Makefile
        logging.getLogger('MA5').info("   Writing a 'Makefile'...")
        if not jobber.WriteMakefiles():
            logging.getLogger('MA5').error("job submission aborted.")
            return False

        # Writing Main
        if not jobber.CreateBldDir(analysisName=title,outputName="user.saf"):
            logging.getLogger('MA5').error("   job submission aborted.")
            return False

        # adding the CLs script if available

        return True    


    def GiveAdvice(self):
        logging.getLogger('MA5').info("\nGuidelines for writing an analysis in expert mode\n")
        logging.getLogger('MA5').info(" 1. Entering the directory '"+self.path+"/Build'\n")
        logging.getLogger('MA5').info(" 2. Setting the environment variables by loading setup.sh or setup.csh according to your SHELL\n")
        logging.getLogger('MA5').info(" 3. Entering the directory '"+self.path+"/Build/SampleAnalyzer/User/Analyzer'\n")
        logging.getLogger('MA5').info(" 4. Editing Analysis 'user.h' and 'user.cpp' files\n")
        logging.getLogger('MA5').info(" 5. Entering the directory '"+self.path+"/Build'\n")
        logging.getLogger('MA5').info(" 6. Compiling with the command 'make'\n")
        logging.getLogger('MA5').info(" 7. Writing a list of datasets\n")
        logging.getLogger('MA5').info(" 8. Launching SampleAnalyzer with the list of datasets\n")
