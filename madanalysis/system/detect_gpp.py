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


class DetectGpp:

    def __init__(self, archi_info, user_info, session_info, debug):
        # mandatory options
        self.archi_info   = archi_info
        self.user_info    = user_info
        self.session_info = session_info
        self.debug        = debug
        self.name         = 'GNU GCC g++'
        self.mandatory    = True
        self.log          = []
        self.logger       = logging.getLogger('MA5')
        self.moreInfo='For more details, type: config_info gpp'
        # adding what you want here
        self.header_paths  = []
        self.library_paths = []
        self.version      = ''


    def PrintDisableMessage(self):
        self.logger.warning('g++ compiler not found. Please install it before using MadAnalysis 5.')

        
    def AutoDetection(self):
        msg=''
        
        # Which
        result = ShellCommand.Which('g++',all=False,mute=True)
        if len(result)==0:
            msg = 'g++ compiler not found. ' +\
                  'Please install it before using MadAnalysis 5.'
            return DetectStatusType.UNFOUND, msg
        if self.debug:
            self.logger.debug("  which:         " + str(result[0]))

        # Ok
        return DetectStatusType.FOUND,msg


    def ExtractInfo(self):

        # Which all
        if self.debug:
            result = ShellCommand.Which('g++',all=True,mute=True)
            if len(result)==0:
                self.logger.error('g++ compiler not found. Please install it before ' + \
                                  'using MadAnalysis 5')
                return False
            self.logger.debug("  which-all:     ")
            for file in result:
                self.logger.debug("    - "+str(file))

        # Getting the version
        ok, out, err = ShellCommand.ExecuteWithCapture(['g++','-dumpversion'],'./')
        if not ok:
            self.logger.error('g++ compiler not found. Please install it before ' + \
                              'using MadAnalysis 5')
            return False
        out=out.lstrip()
        out=out.rstrip()
        self.version = str(out)
        if self.debug:
            self.logger.debug("  version:       " + self.version)


        # Getting include path
        ok, out, err = ShellCommand.ExecuteWithCapture(['g++','-E','-x','c++','-','-v'],'./',stdin=True)
        if not ok:
            self.logger.warning('unexpected error with g++')
            return True
        toKeep=False
        self.header_paths  = []
        self.library_paths = []
        for line in out.split('\n'):
            line = line.lstrip()
            line = line.rstrip()
            if line.startswith('#include <...>'):
                toKeep=True
                continue
            elif line.startswith('End of search list'):
                toKeep=False
            if toKeep:
                if os.path.isdir(line):
                    self.header_paths.append(os.path.normpath(line))
            if line.startswith('LIBRARY_PATH='):
                paths=line[13:].split(':')
                for path in paths:
                    if os.path.isdir(path):
                        self.library_paths.append(os.path.normpath(path))
                    
        if self.debug:
            self.logger.debug("  search path for headers:")
            for line in self.header_paths:
                self.logger.debug('    - '+line)
            self.logger.debug("  search path for libraries:")
            for line in self.library_paths:
                self.logger.debug('    - '+line)

        # Ok
        return True


    def SaveInfo(self):
        self.archi_info.gcc_version = self.version
        self.session_info.gcc_header_search_path  = self.header_paths
        self.session_info.gcc_library_search_path = self.library_paths

        return True


