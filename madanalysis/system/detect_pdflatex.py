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


"""Detection of ``pdflatex`` (PDF reports)."""

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


class DetectPdflatex:
    """Detector of ``pdflatex`` (PDF reports).

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
        """Create the detector of ``pdflatex`` (PDF reports).

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
        self.name      = 'pdflatex'
        self.mandatory = False
        self.log       = []
        self.logger    = logging.getLogger('MA5')

        # adding what you want here


    def PrintDisableMessage(self) -> None:
        """Log the consequences of ``pdflatex`` (PDF reports) being unavailable."""
        self.logger.warning("pdflatex disabled. Reports under the pdf format will not be compiled.")
        

    def IsItVetoed(self) -> bool:
        """Check whether ``pdflatex`` (PDF reports) has been vetoed by the user (``installation_options.dat``).

        Returns:
            ``bool``:
            ``True`` if vetoed.
        """
        if self.user_info.pdflatex_veto:
            self.logger.debug("user setting: veto on pdflatex")
            return True
        else:
            self.logger.debug("no user veto")
            return False

        
    def AutoDetection(self) -> tuple[int, str]:
        """Look for ``pdflatex`` (PDF reports) on the system.

        Returns:
            ``tuple[int, str]``:
            Detection status (:class:`~madanalysis.enumeration.detect_status_type.DetectStatusType`)
            and a message.
        """
        # Which
        result = ShellCommand.Which('pdflatex',all=False,mute=True)
        if len(result)==0:
            return DetectStatusType.UNFOUND,''
        if self.debug:
            self.logger.debug("  which:         " + str(result[0]))

        # Ok
        return DetectStatusType.FOUND,''


    def ExtractInfo(self) -> bool:
        """Extract detailed information about the detected ``pdflatex`` (PDF reports) (version, paths, ...).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Which all
        if self.debug:
            result = ShellCommand.Which('pdflatex',all=True,mute=True)
            if len(result)==0:
                return False
            self.logger.debug("  which-all:     ")
            for file in result:
                self.logger.debug("    - "+str(file))

        # Ok
        return True


    def SaveInfo(self) -> bool:
        """Store the information about ``pdflatex`` (PDF reports) in the architecture/session information.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        self.session_info.has_pdflatex = True
        return True


