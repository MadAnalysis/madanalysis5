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
        
    def CreateDirectory(self):
        logging.info("\nWelcome to the expert mode of MadAnalysis")
        logging.info("Please enter a folder name for creating an empty SampleAnalyzer job")
        answer=raw_input("Answer: ")
        answer=answer.replace(' ','_');
        answer=answer.replace('-','_');
        self.path = os.path.expanduser(answer)
        if not self.path.startswith('/'):
            self.path = self.main.currentdir+'/'+self.path
        self.path = os.path.normpath(self.path)

        # Checking folder
        if self.path in self.forbiddenpaths:
            logging.error("the folder '"+anwser+"' is a MadAnalysis folder. " + \
                         "You cannot overwrite it. Please choose another folder.")
            return False

        # Checking if the job folder exists
        if os.path.isdir(self.path):
            logging.warning("A directory called '"+self.path+"' is already "+ \
                            "defined.\nWould you like to remove it ? (Y/N)")
            allowed_answers=['n','no','y','yes']
            answer=""
            while answer not in  allowed_answers:
                answer=raw_input("Answer: ")
                answer=answer.lower()
            if answer=="no" or answer=="n":
                return False
            else:
                if not FolderWriter.RemoveDirectory(self.path,False):
                    return False
        return True


    def Copy(self):
        
        # Initializing the JobWriter
        jobber = JobWriter(self.main,self.path,False)
        
        # Writing process
        logging.info("   Creating folder '"+self.path+"'...")
        if not jobber.Open():
            logging.error("job submission aborted.")
            return False

        # Copying SampleAnalyzer
        logging.info("   Copying required 'SampleAnalyzer' source files...")
        if not jobber.CopyLHEAnalysis():
            logging.error("   job submission aborted.")
            return False

        # Recasting tools
        if self.main.session_info.has_recasttools:
          try:
            shutil.copyfile(self.main.archi_info.ma5dir+"/tools/RecastingTools/exclusion_CLs.py",self.path+'/exclusion_CLs.py')
          except:
            logging.error('Impossible to copy the recasting tools')
            return False
          try:
            os.chmod(self.path+"/exclusion_CLs.py",0755)
          except:
            logging.error('Impossible to render the recasting tools executable')
            return False


        # Writing an empty analysis
        logging.info("Please enter a name for your analysis")
        title=raw_input("Answer: ")
        if title=="":
            title="user"
        title=title.replace(' ', '_');
        title=title.replace('-', '_');
        logging.info("   Writing an empty analysis...")
        os.system("cd "+self.path+"/Build/SampleAnalyzer; python newAnalyzer.py " + title + " 1")

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
        logging.info("   Writing a 'Makefile'...")
        if not jobber.WriteMakefiles():
            logging.error("job submission aborted.")
            return False

        # Writing Main
        if not jobber.CreateBldDir(analysisName=title,outputName="user.saf"):
            logging.error("   job submission aborted.")
            return False
        if self.main.shower.enable:
             mode=self.main.shower.type
             if self.main.shower.type=='auto':
                 mode = commands.getstatusoutput('less ' + self.main.datasets[0].filenames[0] + ' | grep parton_shower ')
                 if mode[0]!=0:
                     logging.error('Cannot retrieve the showering information from the LHE files')
                     return False
                 mode = (mode[1].split())[0]
             if not jobber.CreateShowerDir(mode):
                 logging.error("   job submission aborted.")
                 return False

        # adding the CLs script if available
        

        return True    


            
    def GiveAdvice(self):
        logging.info("\nGuidelines for writing an analysis in expert mode\n")
        logging.info(" 1. Entering the directory '"+self.path+"/Build'\n")
        logging.info(" 2. Setting the environment variables by loading setup.sh or setup.csh according to your SHELL\n")
        logging.info(" 3. Entering the directory '"+self.path+"/Build/SampleAnalyzer/User/Analyzer'\n")
        logging.info(" 4. Editing Analysis 'user.h' and 'user.cpp' files\n")
        logging.info(" 5. Entering the directory '"+self.path+"/Build'\n")
        logging.info(" 6. Compiling with the command 'make'\n")
        logging.info(" 7. Writing a list of datasets\n")
        logging.info(" 8. Launching SampleAnalyzer with the list of datasets\n")
