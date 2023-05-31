################################################################################
#  
#  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://github.com/MadAnalysis/madanalysis5>
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


from __future__ import absolute_import
import logging
import glob
import os
import sys
import re
import platform
from shell_command  import ShellCommand
from madanalysis.enumeration.detect_status_type import DetectStatusType


class DetectMatplotlib:

    def __init__(self, archi_info, user_info, session_info, debug):
        # mandatory options
        self.archi_info   = archi_info
        self.user_info    = user_info
        self.session_info = session_info
        self.debug        = debug
        self.name         = 'Matplotlib'
        self.mandatory    = False
        self.log          = []
        self.logger       = logging.getLogger('MA5')
        self.moreInfo     = 'For more details, type: config_info matplotlib'
        # adding what you want here


    def IsItVetoed(self):
        if self.user_info.matplotlib_veto:
            self.logger.debug("user setting: veto on matplotlib")
            return True
        else:
            self.logger.debug("no user veto")
            return False

        
    def AutoDetection(self):

        # Checking if matplotlib is installed on the system
        try:
            import matplotlib
        except:
            return DetectStatusType.UNFOUND,''

        # Checking release
        if self.debug:
            # Matplotlib version
            self.logger.debug("  release = "+matplotlib.__version__)
            # Matplotlib location
            self.logger.debug("  where? = "+matplotlib.__file__)

        word=matplotlib.__version__
        word=word.lstrip()
        word=word.rstrip()
        numbers = word.split('.')
        if len(numbers)>=1:
            if numbers[0]=='0':
                self.logger.error("Release must be greater to 1.0.0. Please upgrade the Matplotlib package.")
                return DetectStatusType.UNFOUND,''
        else:
            self.logger.warning("Impossible to decode the Matplotlib release")
            
        # Ok
        return DetectStatusType.FOUND,''


    def SaveInfo(self):
        self.session_info.has_matplotlib = True
        return True


