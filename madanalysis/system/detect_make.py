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


"""Detection of GNU Make (mandatory)."""

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


class DetectMake:
    """Detector of GNU Make (mandatory).

    The methods are called by
    :meth:`madanalysis.system.detect_manager.DetectManager.Execute` in the following
    order (only when defined): ``IsItVetoed``, ``AreDependenciesInstalled``,
    ``ManualDetection``, ``ToolsDetection``, ``AutoDetection``, ``ExtractInfo`` and
    ``SaveInfo``.

    Attributes:
        name (``str``): name displayed in the configuration check.
        mandatory (``bool``): whether MadAnalysis 5 can run without the package.
    """

    def __init__(self, archi_info: Any, user_info: Any, session_info: Any, debug: bool) -> None:
        """Create the detector of GNU Make (mandatory).

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
        self.name      = 'GNU Make'
        self.mandatory = True
        self.log       = []
        self.logger    = logging.getLogger('MA5')
        self.moreInfo='For more details, type: config_info make'
        # adding what you want here
        self.version = ""


    def PrintDisableMessage(self) -> None:
        """Log the consequences of GNU Make (mandatory) being unavailable."""
        self.logger.warning("GNU Make not found. Please install it before using MadAnalysis 5")
        

    def AutoDetection(self) -> tuple[int, str]:
        """Look for GNU Make (mandatory) on the system.

        Returns:
            ``tuple[int, str]``:
            Detection status (:class:`~madanalysis.enumeration.detect_status_type.DetectStatusType`)
            and a message.
        """
        msg = ''
        
        # Which
        result = ShellCommand.Which('make',all=False,mute=True)
        if len(result)==0:
            msg = "GNU Make not found. Please install it before using MadAnalysis 5"
            return DetectStatusType.UNFOUND, msg
        if self.debug:
            self.logger.debug("  which:         " + str(result[0]))

        # Ok
        return DetectStatusType.FOUND, msg


    def ExtractInfo(self) -> bool:
        """Extract detailed information about the detected GNU Make (mandatory) (version, paths, ...).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Which all
        if self.debug:
            result = ShellCommand.Which('make',all=True,mute=True)
            if len(result)==0:
                self.logger.error('GNU Make not found. Please install it before ' + \
                                  'using MadAnalysis 5')
                return False
            self.logger.debug("  which-all:     ")
            for file in result:
                self.logger.debug("    - "+str(file))

        # Getting the version
        ok, out, err = ShellCommand.ExecuteWithCapture(['make','--version'],'./')
        if not ok:
            self.logger.error('GNU Make not found. Please install it before ' + \
                              'using MadAnalysis 5')
            return False
        lines=out.split('\n')
        if len(lines)==0:
             self.logger.error('command "make --version" seems to not give the GNU Make version')
             return False
        firstline=lines[0]
        firstline=firstline.lstrip()
        firstline=firstline.rstrip()
        self.version = str(firstline)
        if self.debug:
            self.logger.debug("  version:       " + self.version)


        # Ok
        return True


    def SaveInfo(self) -> bool:
        """Store the information about GNU Make (mandatory) in the architecture/session information.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        self.archi_info.make_version = self.version
        return True


