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
from madanalysis.system.config_checker          import ConfigChecker


class DetectPAD:

    def __init__(self,archi_info, user_info, session_info, debug, ma5):
        self.archi_info   = archi_info
        self.user_info    = user_info
        self.session_info = session_info
        self.debug        = debug
        self.ma5tune      = ma5
        if self.ma5tune:
            self.name  = 'PADForMA5tune'
        else:
            self.name  = 'PAD'
        self.mandatory   = False
        self.force       = False
        self.build_file  = ''
        self.build_path  = ''
        self.version     = ''
        self.logger      = logging.getLogger('MA5')


    def IsItVetoed(self):
        if self.ma5tune:
            if self.user_info.padma5_veto:
                self.logger.debug("user setting: veto on PADForMA5Tune")
                return True
            else:
                self.logger.debug("no user veto")
                return False
        else:
            if self.user_info.pad_veto:
                self.logger.debug("user setting: veto on PAD")
                return True
            else:
                self.logger.debug("no user veto")
                return False


    def AreDependenciesInstalled(self):
        checker = ConfigChecker(self.archi_info, self.user_info, self.session_info, False, False)
        if self.ma5tune:
            if not checker.checkDelphesMA5tune(True):
                self.logger.debug("dependency 'DelphesMA5tune' is not installed")
                return False
        else:
            if not checker.checkDelphes(True):
                self.logger.debug("dependency 'Delphes' is not installed")
                return False
        return True

    def ManualDetection(self):
        msg = ''

        if self.ma5tune:
            # User setting
            if self.user_info.padma5_build_path==None:
                return DetectStatusType.UNFOUND, msg

            self.logger.debug("User setting: PADForMA5Tune build path is specified.")

            # Folder name
            folder = os.path.normpath(self.user_info.padma5_build_path)

        else:
            # User setting
            if self.user_info.pad_build_path==None:
                return DetectStatusType.UNFOUND, msg

            self.logger.debug("User setting: PAD build path is specified.")

            # Folder name
            folder = os.path.normpath(self.user_info.pad_build_path)

        filename = folder+'/MadAnalysis5job'

        # Detection of fastjet-config
        self.logger.debug("Detecting MadAnalysis5job in the path specified by the user ...")
        if not os.path.isfile(filename):
            logging.getLogger('MA5').debug('-> not found')
            msg  = "MadAnalysis5job program is not found in folder: "+folder+"\n"
            msg += "Please check that "+self.name+" is properly installed."
            return DetectStatusType.UNFOUND, msg

        self.build_file=filename
        self.build_path=folder

        self.logger.debug("MadAnalysis5job program found in: "+self.build_path)

        # Ok
        return DetectStatusType.FOUND, msg


    def ToolsDetection(self):
        msg = ''

        if self.ma5tune:
            thefolder = 'PADForMA5tune'
        else:
            thefolder = 'PAD'

        filename = os.path.normpath(self.archi_info.ma5dir+'/tools/'+thefolder+'/Build/MadAnalysis5job')
        self.logger.debug("Look for "+self.name+" in the folder here:"+filename+" ...")
        if os.path.isfile(filename):
            self.logger.debug("-> found")
            self.build_file=filename
            self.build_path=os.path.dirname(self.build_file)
        else:
            self.logger.debug("-> not found")
            return DetectStatusType.UNFOUND, msg

        return DetectStatusType.FOUND, msg


    def ExtractInfo(self):
        theCommands = [self.build_file,'--info']
        ok, out, err = ShellCommand.ExecuteWithCapture(theCommands,'./')
        if not ok:
            msg = 'MadAnalyis5job program does not work properly.'
            self.logger.debug("->ERROR: MadAnalyis5job program does not work properly.")
            self.logger.debug(str(out))
            self.logger.debug(str(err))
            return False
        lines=out.split('\n')
        ok = False
        nbAnalysis = 0
        for line in lines:
            line=line.lstrip()
            line=line.rstrip()
            if line.startswith('BEGIN '):
                self.logger.debug("  MA5 stamp found!")
                ok = True
                continue
            if ok:
                nbAnalysis += 1
            if line.endswith('END '):
                break
        if self.debug:
            self.logger.debug("  number of recast analyses: " + str(nbAnalysis))

        # Ok
        return ok


    def SaveInfo(self):
        # archi_info
        if self.ma5tune:
             self.session_info.has_padma5           = True
             self.session_info.padma5_build_path    = self.build_path
             self.session_info.padma5_original_bins = [self.build_file]
        else:
             self.session_info.has_pad              = True
             self.session_info.pad_build_path       = self.build_path
             self.session_info.pad_original_bins    = [self.build_file]
        
        # Ok
        return True


