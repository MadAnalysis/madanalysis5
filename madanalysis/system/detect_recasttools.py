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


class DetectRecastTools:

    def __init__(self, archi_info, user_info, session_info, debug):
        # mandatory options
        self.archi_info   = archi_info
        self.user_info    = user_info
        self.session_info = session_info
        self.debug        = debug
        self.name         = 'Recasting tools'
        self.mandatory    = False
        self.log          = []

        # adding what you want here
        self.recasttools_path = ""
        

    def PrintDisableMessage(self):
        logging.warning("Recasting tools are not found and will be disabled.")


    def PrintInstallMessage(self):
        logging.warning("To enable this functionnality, please type 'install RecastingTools'.")


    def IsItVetoed(self):
        if self.user_info.recasttools_veto:
            logging.debug("user setting: veto on the Recasting Tools package")
            return True
        else:
            logging.debug("no user veto")
            return False


    def CommonDetection(self,folder):
        filename = os.path.normpath(folder+'/exclusion_CLs.py')
        logging.debug("Looking for the file "+filename+" ...")
        if os.path.isfile(filename):
            logging.debug("-> found")
            return DetectStatusType.FOUND
        else:
            logging.debug("-> not found")
            return DetectStatusType.UNFOUND


    def ManualDetection(self):
        if self.user_info.recasttools_path==None:
            return DetectStatusType.UNFOUND

        # Folder
        logging.debug("User setting: the path for the recasting tools has been specified")
        self.recasttools_path = self.user_info.recasttools_path

        # Launch the search
        return self.CommonDetection(self.recasttools_path)
        
        
    def ToolsDetection(self):
        # Folder
        self.recasttools_path = os.path.normpath(self.archi_info.ma5dir+"/tools/RecastingTools")

        # Launch the search
        return self.CommonDetection(self.recasttools_path)


    def SaveInfo(self):
        self.session_info.has_recasttools=True
        self.session_info.recasttools_path = self.recasttools_path
        return True


