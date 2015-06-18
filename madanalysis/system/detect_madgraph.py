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


class DetectMadgraph:

    def __init__(self, archi_info, user_info, session_info, debug):
        # mandatory options
        self.archi_info   = archi_info
        self.user_info    = user_info
        self.session_info = session_info
        self.debug        = debug
        self.name         = 'MadGraph'
        self.mandatory    = False
        self.log          = []
        self.logger       = logging.getLogger('madanalysis')

        # adding what you want here
        inputname = "/input/particles_name_default.txt"
        self.filenames = [ os.path.normpath(self.archi_info.ma5dir + inputname), \
                           os.path.normpath(os.getcwd()+inputname) ]
        self.goodfilename=""


    def PrintDisableMessage(self):
#        self.logger.info("Particle labels will be exported from MadAnalysis files.")
        pass
        

    def IsItVetoed(self):
        if self.user_info.madgraph_veto:
            self.logger.debug("user setting: veto on MadGraph")
            return True
        else:
            self.logger.debug("no user veto")
            return False

        
    def AutoDetection(self):
        for filename in self.filenames:
            if os.path.isfile(filename):
                self.logger.debug('file "'+filename+'" is found.')
                self.goodfilename=filename
                return DetectStatusType.FOUND
            else:
                self.logger.debug('file "'+filename+'" is NOT found.')
        return DetectStatusType.UNFOUND


    def ExtractInfo(self):
        return True


    def SaveInfo(self):
        self.session_info.has_madgraph=True
        self.session_info.madgraph_path=self.goodfilename
        return True


