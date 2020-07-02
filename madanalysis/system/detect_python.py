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


class DetectPython:

    def __init__(self, archi_info, user_info, session_info, debug):
        # mandatory options
        self.archi_info   = archi_info
        self.user_info    = user_info
        self.session_info = session_info
        self.debug        = debug
        self.name      = 'Python'
        self.mandatory = True
        self.log       = []
        self.logger    = logging.getLogger('MA5')
        self.moreInfo='For more details, type: config_info python'
        # adding what you want here


    def PrintDisableMessage(self):
        self.logger.warning("Python not found. Please install it before using MadAnalysis 5")
        

    def AutoDetection(self):
        return DetectStatusType.FOUND,''


    def ExtractInfo(self):
        
        # Debug general
        if self.debug:
            self.logger.debug("")
            self.logger.debug("  Python release:         " + str(platform.python_version()))
            self.logger.debug("  Python build:           " + str(platform.python_build()))
            self.logger.debug("  Python compiler:        " + str(platform.python_compiler()))
            self.logger.debug("  Python prefix:          " + str(sys.prefix))
            self.logger.debug("  Python executable used: " + str(sys.executable))

        # Which python
        if self.debug:
            self.logger.debug("  sys.executable:         " + str(sys.executable))
            result = ShellCommand.Which('python',all=False,mute=True)
            if len(result)==0:
                self.logger.error('python compiler not found. Please install it before ' + \
                                  'using MadAnalysis 5')
                return False
            self.logger.debug("  which:                  " + str(result[0]))

        # Which all
        if self.debug:
            result = ShellCommand.Which('python',all=True,mute=True)
            if len(result)==0:
                self.logger.error('g++ compiler not found. Please install it before ' + \
                                  'using MadAnalysis 5')
                return False
            self.logger.debug("  which-all:              ")
            for file in result:
                self.logger.debug("    - "+str(file))

        # Python paths
        if self.debug:
            self.logger.debug("  Python internal paths: ")
            tmp = sys.path
            for path in tmp:
                self.logger.debug("    - "+path)
            self.logger.debug("  $PYTHONPATH: ")
            try:
                tmp = os.environ['PYTHONPATH']
            except:
                tmp = []
            if len(tmp)==0:
                self.logger.debug("    EMPTY OR NOT FOUND")
            else:
                tmp = tmp.split(':')
                for path in tmp:
                    self.logger.debug("    - "+path)
            self.logger.debug("")

        # Ok
        return True


    def SaveInfo(self):
        return True


