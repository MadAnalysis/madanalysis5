################################################################################
#  
#  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


"""Detection of the C++ compiler g++ (mandatory)."""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any
import logging
import glob
import os
import sys
import re
import platform
from shell_command  import ShellCommand
from madanalysis.enumeration.detect_status_type import DetectStatusType


class DetectGpp:
    """Detector of the C++ compiler g++ (mandatory).

    The methods are called by
    :meth:`madanalysis.system.detect_manager.DetectManager.Execute` in the following
    order (only when defined): ``IsItVetoed``, ``AreDependenciesInstalled``,
    ``ManualDetection``, ``ToolsDetection``, ``AutoDetection``, ``ExtractInfo`` and
    ``SaveInfo``.

    Attributes:
        name (``str``): name displayed in the configuration check.
        mandatory (``bool``): whether MadAnalysis 5 can run without the package.

    The compiler version is checked (GCC >= 8, or clang >= 9 on macOS) and the supported
    C++ standards (``cpp11``, ``cpp14``) are stored in the architecture information.
    """

    def __init__(self, archi_info: Any, user_info: Any, session_info: Any, debug: bool) -> None:
        """Create the detector of the C++ compiler g++ (mandatory).

        Args:
            archi_info (``ArchitectureInfo``): system configuration, filled by :meth:`SaveInfo`.
            user_info (``UserInfo``): user options (vetoes, forced paths).
            session_info (``SessionInfo``): session information, filled by :meth:`SaveInfo`.
            debug (``bool``): print detailed information.
        """
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


    def PrintDisableMessage(self) -> None:
        """Log the consequences of the C++ compiler g++ (mandatory) being unavailable."""
        self.logger.warning('g++ compiler not found. Please install it before using MadAnalysis 5.')

        
    def AutoDetection(self) -> tuple[int, str]:
        """Look for the C++ compiler g++ (mandatory) on the system.

        Returns:
            ``tuple[int, str]``:
            Detection status (:class:`~madanalysis.enumeration.detect_status_type.DetectStatusType`)
            and a message.
        """
        msg=''
        
        # Which
        result = ShellCommand.Which('g++',all=False,mute=True)
        if len(result)==0:
            msg = 'g++ compiler not found. ' +\
                  'Please install it before using MadAnalysis 5.'
            return DetectStatusType.UNFOUND, msg
        if self.debug:
            self.logger.debug("  which:         " + str(result[0]))

        # Check GCC version
        try:
            # NOTE: temporary files are written in the MadAnalysis 5 folder.
            command = ["g++ -dumpversion > .gcc_version"]
            result = ShellCommand.Execute(command, self.archi_info.ma5dir, shell=True)
            with open(os.path.join(self.archi_info.ma5dir, ".gcc_version"), "r") as f:
                result = f.read()
            os.remove(os.path.join(self.archi_info.ma5dir, ".gcc_version"))
            gcc_version = [int(x) for x in result.split(".")]
            if (gcc_version[0] < 8 and not self.archi_info.isMac) or (gcc_version[0] < 9 and self.archi_info.isMac):
                msg = "MadAnalysis 5 requires " + self.archi_info.isMac*"clang version 9 " + \
                      (not self.archi_info.isMac)*"GCC version 8 " + "or above." + \
                      f" Current version is " + ".".join([str(x) for x in gcc_version])
                self.logger.error(msg)
                return DetectStatusType.UNFOUND, msg
        except Exception as err:
            self.logger.debug("Problem with compiler version detection:: " + str(err))

        # Check C++ version
        try:
            with open(os.path.join(self.archi_info.ma5dir, "cxxtest.cc"), 'w') as f:
                f.write("int main() { return 0; }\n")
            command = lambda cxx_version: [
                f"g++ -std=c++{cxx_version} "
                f"{os.path.join(self.archi_info.ma5dir, 'cxxtest.cc')} "
                f"-o {os.path.join(self.archi_info.ma5dir, 'cxxtest')}"
            ]
            for version in [11,14]: # ,17,20]: for the future
                result = ShellCommand.Execute(command(version), self.archi_info.ma5dir, shell=True)
                if result:
                    setattr(self.archi_info, "cpp"+str(version), True)
            os.remove(os.path.join(self.archi_info.ma5dir, "cxxtest.cc"))
            os.remove(os.path.join(self.archi_info.ma5dir, "cxxtest"))
        except Exception as err:
            self.logger.debug(f"Unexpected {err}, {type(err)}")


        if not self.archi_info.cpp11:
            return DetectStatusType.UNFOUND, "Please update C++ compiler. " + \
                                             "Old compilers are no longer supported."

        # Ok
        return DetectStatusType.FOUND,msg


    def ExtractInfo(self) -> bool:
        """Extract detailed information about the detected the C++ compiler g++ (mandatory) (version, paths, ...).

        Returns:
            ``bool``:
            ``True`` on success.
        """

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


    def SaveInfo(self) -> bool:
        """Store the information about the C++ compiler g++ (mandatory) in the architecture/session information.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        self.archi_info.gcc_version = self.version
        self.session_info.gcc_header_search_path  = self.header_paths
        self.session_info.gcc_library_search_path = self.library_paths

        return True


