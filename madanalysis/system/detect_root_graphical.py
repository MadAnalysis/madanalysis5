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


class DetectRootGraphical:

    def __init__(self, archi_info, user_info, session_info, debug):
        # mandatory options
        self.archi_info   = archi_info
        self.user_info    = user_info
        self.session_info = session_info
        self.debug        = debug
        self.name         = 'Root'
        self.mandatory    = False
        self.log          = []
        self.logger       = logging.getLogger('MA5')

        # adding what you want here

    def IsItVetoed(self):
        if self.user_info.root_veto:
            self.logger.debug("user setting: veto on Root")
            return True
        else:
            self.logger.debug("no user veto")
            return False

    def AreDependenciesInstalled(self):
        if not self.archi_info.has_root:
            self.logger.debug("dependency 'ROOT' is not installed")
            return False
        return True


    def AutoDetection(self):
        if self.archi_info.has_root:
            return DetectStatusType.FOUND,''
        else:
            return DetectStatusType.UNFOUND,''


    def SaveInfo(self):
        self.session_info.has_root=True
        return True


