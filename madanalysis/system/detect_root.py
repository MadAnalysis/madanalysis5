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
from madanalysis.enumeration.detect_status_type import DetectStatusType


class DetectRoot:

    def __init__(self,archi_info, user_info, session_info, debug):
        self.archi_info   = archi_info
        self.user_info    = user_info
        self.session_info = session_info
        self.debug        = debug
        self.name      = 'Root'
        self.mandatory = False
        self.force     = False
        self.bin_path  = ''
        self.inc_path  = ''
        self.lib_path  = ''
        self.libraries = {}
        self.features  = []
        self.logger    = logging.getLogger('MA5')


    def IsItVetoed(self):
        if self.user_info.root_veto:
            self.logger.debug("user setting: veto on Root")
            return True
        else:
            self.logger.debug("no user veto")
            return False


    def ManualDetection(self):
        msg = ''
        
        # User setting
        if self.user_info.root_bin==None:
            return DetectStatusType.UNFOUND, msg
        self.logger.debug("User setting: root bin path is specified.")
        self.bin_path=os.path.normpath(self.user_info.root_bin)
        self.logger.debug("root-config program found in: "+self.bin_path)

        # Detection of root-config
        self.logger.debug("Detecting root-config in the path specified by the user ...")
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
            self.logger.debug("")
            self.logger.debug("  which:         " + str(self.bin_path))

        # Which all
        if self.debug:
            result = ShellCommand.Which('root-config',all=True,mute=True)
            if len(result)==0:
                msg = 'ROOT module called "root-config" is not detected.\n'\
		      +'Two explanations :n'\
		      +' - ROOT is not installed. You can download it '\
		      +'from http://root.cern.ch\n'\
		      +' - ROOT binary folder must be placed in the '\
                      +'global environment variable $PATH'
                return False, msg
            self.logger.debug("  which-all:     ")
            for file in result:
                self.logger.debug("    - "+str(file))
        return True, msg


    def ExtractInfo(self):
        # Using root-config for getting lib and header paths
        self.logger.debug("Trying to get library and header paths ...") 
        theCommands = ['root-config','--libdir','--incdir']
        ok, out, err = ShellCommand.ExecuteWithCapture(theCommands,'./')
        if not ok:
            msg = 'ROOT module called "root-config" is not detected.\n'\
                  +'Two explanations :n'\
		  +' - ROOT is not installed. You can download it '\
		  +'from http://root.cern.ch\n'\
		  +' - ROOT binary folder must be placed in the '\
                  +'global environment variable $PATH'
            return False,msg

        # Extracting ROOT library and header path
        out=out.lstrip()
        out=out.rstrip()
        root_tmp = out.split()
        self.inc_path = os.path.normpath(root_tmp[1])
        self.lib_path = os.path.normpath(root_tmp[0])
        self.logger.debug("-> root-config found") 
        self.logger.debug("-> root header  folder: "+self.inc_path) 
        self.logger.debug("-> root library folder: "+self.lib_path) 

        # Check: looking for files
        FilesToFind=[os.path.normpath(self.lib_path+'/libCore.so'), \
                     os.path.normpath(self.inc_path+'/TH1F.h')]
        for file in FilesToFind:
            self.logger.debug("Try to find "+file+" ...")
            if os.path.isfile(file):
                self.libraries[file.split('/')[-1]]=file+":"+str(os.stat(file).st_mtime)
            else:
                self.PrintFAIL(warning=False)
	        self.logger.error("ROOT file called '"+file+"' is not found")
                self.logger.error("Please check that ROOT is properly installed.")
                return False
           
        # Getting the features
        if self.bin_path!='':
            theCommands = [self.bin_path+'/root-config','--features']
        else:
            theCommands = ['root-config','--features']
        ok, out, err = ShellCommand.ExecuteWithCapture(theCommands,'./')
        if not ok:
            self.logger.error('problem with root-config')
            return False
        out=out.lstrip()
        out=out.rstrip()
        features = str(out).split()
        features.sort()
        for feature in features:
            self.features.append(feature)
        if self.debug:
            self.logger.debug("  features:      " + str(self.features))

        # Ok
        return True


    def SaveInfo(self):
        # archi_info
        self.archi_info.has_root = True
        self.archi_info.root_priority = self.force
        self.archi_info.root_bin_path = self.bin_path
        self.archi_info.root_inc_path = self.inc_path
        self.archi_info.root_lib_path = self.lib_path
        for k, v in self.libraries.iteritems():
            self.archi_info.libraries[k]=v
        for feature in self.features:
            if not feature in self.archi_info.root_features:
                self.archi_info.root_features.append(feature)

        # Adding ROOT library path to Python path
        sys.path.append(self.archi_info.root_lib_path)

        # Ok
        return True


