################################################################################
#  
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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


class DetectSQLite:

    def __init__(self, archi_info, user_info, session_info, debug):
        # mandatory options
        self.archi_info   = archi_info
        self.user_info    = user_info
        self.session_info = session_info
        self.debug        = debug
        self.name         = 'sqlite3'
        self.mandatory    = False
        self.log          = []
        self.logger       = logging.getLogger('MA5')

        # adding what you want here

    def PrintDisableMessage(self):
        self.logger.warning("sqlite3 not detected, multiweight output format will not be supported!")
        

    def IsItVetoed(self):
        if self.user_info.sqlite_veto:
            self.logger.debug("user setting: veto on SQLite3")
            return True
        else:
            self.logger.debug("no user veto")
            return False

        
    def AutoDetection(self):
        # Which
        result = ShellCommand.Which('sqlite3',all=False,mute=True)
        if len(result)==0:
            return DetectStatusType.UNFOUND,''
        if self.debug:
            self.logger.debug("  which:         " + str(result[0]))

        # Ok
        return DetectStatusType.FOUND,''


    def ExtractInfo(self):
        # Which all
        if self.debug:
            result = ShellCommand.Which('sqlite3',all=True,mute=True)
            if len(result)==0:
                return False
            self.logger.debug("  which-all:     ")
            for file in result:
                self.logger.debug("    - "+str(file))

        # Ok
        return True


    def SaveInfo(self):
        self.session_info.has_sqlite3=True
        return True



