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


import logging
import glob
import os
import commands
import sys
import re
import platform
from shell_command  import ShellCommand
from madanalysis.enumeration.detect_status_type import DetectStatusType


class DetectFastjet:

    def __init__(self,archi_info, user_info, session_info, debug):
        self.archi_info   = archi_info
        self.user_info    = user_info
        self.session_info = session_info
        self.debug        = debug
        self.name      = 'FastJet'
        self.mandatory = False
        self.force     = False
        self.lib_paths = []
        self.bin_file  = ''
        self.bin_path  = ''
        self.version   = ''
        self.logger    = logging.getLogger('MA5')


    def IsItVetoed(self):
        if self.user_info.fastjet_veto:
            self.logger.debug("user setting: veto on FastJet")
            return True
        else:
            self.logger.debug("no user veto")
            return False


    def ManualDetection(self):
        msg = ''

        # User setting
        if self.user_info.fastjet_bin_path==None:
            return DetectStatusType.UNFOUND, msg

        self.logger.debug("User setting: fastjet bin path is specified.")

        # File & folder name
        folder   = os.path.normpath(self.user_info.fastjet_bin_path)
        filename = folder+'/fastjet-config'

        # Detection of fastjet-config
        self.logger.debug("Detecting fastjet-config in the path specified by the user ...")
        if not os.path.isfile(filename):
            logging.getLogger('MA5').debug('-> not found')
            msg  = "fastjet-config program is not found in folder: "+folder+"\n"
            msg += "Please check that FastJet is properly installed."
            return DetectStatusType.UNFOUND, msg

        self.bin_file=filename
        self.bin_path=folder

        self.logger.debug("fastjet-config program found in: "+self.bin_path)

        # Ok
        return DetectStatusType.FOUND, msg

    def ToolsDetection(self):
        msg = ''

        filename = os.path.normpath(self.archi_info.ma5dir+'/tools/fastjet/bin/fastjet-config')
        self.logger.debug("Look for FastJet in the folder here:"+filename+" ...")
        if os.path.isfile(filename):
            self.logger.debug("-> found")
            self.bin_file=filename
            self.bin_path=os.path.dirname(self.bin_file)
        else:
            self.logger.debug("-> not found")
            return DetectStatusType.UNFOUND, msg

        return DetectStatusType.FOUND, msg


    def AutoDetection(self):
        msg = ''

        # Trying to call fastjet-config with which
        result = ShellCommand.Which('fastjet-config',mute=True)
        if len(result)==0:
            msg = 'The FastJet package is not found.'
            return DetectStatusType.UNFOUND, msg
        islink = os.path.islink(result[0])
        if not islink:
            self.bin_file=os.path.normpath(result[0])
        else:
            self.bin_file=os.path.normpath(os.path.realpath(result[0]))
        self.bin_path=os.path.dirname(self.bin_file)

        # Debug mode
        if self.debug:
            self.logger.debug("  which:         " + str(result[0]) + " [is it a link? "+str(islink)+"]")
            if islink:
                self.logger.debug("                 -> "+os.path.realpath(result[0]))

        # Which all
        if self.debug:
            result = ShellCommand.Which('fastjet-config',all=True,mute=True)
            if len(result)==0:
                msg = 'The FastJet package is not found.'
                return DetectStatusType.UNFOUND, msg
            self.logger.debug("  which-all:     ")
            for file in result:
                self.logger.debug("    - "+str(file))
        return DetectStatusType.FOUND, msg


    def ExtractInfo(self):
        theCommands = [self.bin_path+'/fastjet-config','--version']
        ok, out, err = ShellCommand.ExecuteWithCapture(theCommands,'./')
        if not ok:
            msg = 'fastjet-config program does not work properly.'
            return False#,msg
        out=out.lstrip()
        out=out.rstrip()
        self.version = str(out)
        if self.debug:
            self.logger.debug("  version:       " + self.version)

        # Using fastjet-config for getting lib and header paths
        self.logger.debug("Trying to get library and header paths ...") 
        theCommands = [self.bin_path+'/fastjet-config','--libs','--plugins']
        ok, out, err = ShellCommand.ExecuteWithCapture(theCommands,'./')
        if not ok:
            msg = 'fastjet-config program does not work properly.'
            return False#,msg

        # Extracting FastJet library and header path
        out=out.lstrip()
        out=out.rstrip()
        self.logger.debug("  Lib flags:     " + str(out))
        words = out.split()
        for word in words:
            if word.startswith('-L') and not word[2:] in self.lib_paths:
                self.lib_paths.append(word[2:])
        if self.debug:
            self.logger.debug("  Lib path:      " + str(self.lib_paths))

        # Ok
        return True


    def SaveInfo(self):
        # archi_info
        self.archi_info.has_fastjet           = True
        self.archi_info.fastjet_priority      = self.force
        self.archi_info.fastjet_bin_path      = self.bin_path
        self.archi_info.fastjet_original_bins = [self.bin_file]
        self.archi_info.fastjet_lib_paths     = self.lib_paths
        
        # Ok
        return True


