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
import glob
import os
import commands
import sys
import re
import platform
from shell_command  import ShellCommand
from madanalysis.enumeration.detect_status_type import DetectSatusType


class DetectRoot:

    def __init__(self,main):
        self.main      = main
        self.name      = 'Root'
        self.force     = False
        self.bin_path  = ''
        self.inc_path  = ''
        self.lib_path  = ''
        self.libraries = {}
        self.features  = []


    def Initialize(self):
        return True


    def Finalize(self):
        return True


    def ManualDetection(self):
        msg = ''
        
        # User setting
        if self.user_info.root_bin=='0':
            return DetectStatusType.UNFOUND, msg
        logging.debug("User setting: root bin path is specified.")
        self.bin_path=os.path.normpath(self.user_info.root_bin)
        logging.debug("root-config program found in: "+self.bin_path)

        # Detection of root-config
        logging.debug("Detecting root-config in the path specified by the user ...")
        if not os.path.isfile(self.bin_path+'/root-config'):
            msg  = "root-config program is not found in folder: "+self.root_bin_path+"\n"
            msg += "Please check that ROOT is properly installed."
            return DetectStatusType.UNFOUND, msg

        # Ok
        return DetectStatusType.FOUND, msg

                
    def AutoDetection(self):
        msg = ''

        # Trying to call root-config with which
        result = ShellCommand.Which('root-config')
        if len(result)==0:
            msg = 'ROOT module called "root-config" is not detected.\n'\
                  +'Two explanations :n'\
		  +' - ROOT is not installed. You can download it '\
		  +'from http://root.cern.ch\n'\
		  +' - ROOT binary folder must be placed in the '\
                  +'global environment variable $PATH'
            return DetectStatusType.UNFOUND, msg
        self.bin_path=os.path.normpath(result[0][:-11])

        # Debug mode
        if self.debug:
            logging.debug("")
            logging.debug("  which:         " + str(self.bin_path))

        # Which all
        if self.debug:
            result = ShellCommand.Which('root-config',all=True,mute=True)
            if len(result)==0:
                msg = 'ROOT module called "root-config" is not detected.\n'\
		      +'Two explanations :n'\
		      +' - ROOT is not installed. You can download it '\
		      +'from http://root.cern.ch\n'\
		      +' - ROOT binary folder must be placed in the '\
                      +'global environment variable $PATH')
                return False, msg
            logging.debug("  which-all:     ")
            for file in result:
                logging.debug("    - "+str(file))
        return True, msg


    def ExtractInfo(self):
        # Using root-config for getting lib and header paths
        logging.debug("Trying to get library and header paths ...") 
        theCommands = ['root-config','--libdir','--incdir']
        ok, out, err = ShellCommand.ExecuteWithCapture(theCommands,'./')
        if not ok:
            self.PrintFAIL(warning=False)
            logging.error('ROOT module called "root-config" is not detected.\n'\
                          +'Two explanations :n'\
		          +' - ROOT is not installed. You can download it '\
		          +'from http://root.cern.ch\n'\
		          +' - ROOT binary folder must be placed in the '\
                          +'global environment variable $PATH')
            return False

        # Extracting ROOT library and header path
        out=out.lstrip()
        out=out.rstrip()
        root_tmp = out.split()
        self.inc_path = os.path.normpath(root_tmp[1])
        self.lib_path = os.path.normpath(root_tmp[0])
        logging.debug("-> root-config found") 
        logging.debug("-> root header  folder: "+self.inc_path) 
        logging.debug("-> root library folder: "+self.lib_path) 

        # Check: looking for files
        FilesToFind=[os.path.normpath(self.lib_path+'/libCore.so'), \
                     os.path.normpath(self.inc_path+'/TH1F.h')]
        for file in FilesToFind:
            logging.debug("Try to find "+file+" ...")
            if os.path.isfile(file):
                self.libraries[file.split('/')[-1]]=file+":"+str(os.stat(file).st_mtime)
            else:
                self.PrintFAIL(warning=False)
	        logging.error("ROOT file called '"+file+"' is not found")
                logging.error("Please check that ROOT is properly installed.")
                return False
           
        # Getting the features
        ok, out, err = ShellCommand.ExecuteWithCapture([self.bin_path+'/root-config','--features'],'./')
        if not ok:
            self.PrintFAIL(warning=False)
            logging.error('problem with root-config')
            return False
        out=out.lstrip()
        out=out.rstrip()
        features = str(out).split()
        features.sort()
        for feature in features:
            self.features.append(feature)
        if self.debug:
            logging.debug("  features:      " + str(self.features))

        # Ok
        return True


    def SaveInfo(self):
        # archi_info
        self.main.archi_info.root_priority = self.force
        self.main.archi_info.root_bin_path = self.bin_path
        self.main.archi_info.root_inc_path = self.inc_path
        self.main.archi_info.root_lib_path = self.lib_path
        for k, v in self.libraries:
            self.main.archi_info.libraries[k]=v
        for feature in self.features:
            self.main.archi_info.root_features.append(feature)

        # Adding ROOT library path to Python path
        sys.path.append(self.archi_info.root_lib_path)

        # Ok
        return True


