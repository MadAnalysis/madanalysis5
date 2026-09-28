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


"""Detection of ``dvipdf`` (conversion of DVI reports into PDF)."""

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


class DetectDvipdf:
    """Detector of ``dvipdf`` (conversion of DVI reports into PDF).

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
        """Create the detector of ``dvipdf`` (conversion of DVI reports into PDF).

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
        self.name      = 'dvipdf'
        self.mandatory = False
        self.log       = []
        self.logger    = logging.getLogger('MA5')
        # adding what you want here


    def PrintDisableMessage(self) -> None:
        """Log the consequences of ``dvipdf`` (conversion of DVI reports into PDF) being unavailable.
        """
        self.logger.warning("dvipdf disabled. DVI reports will not be converted to pdf files.")
        

    def IsItVetoed(self) -> bool:
        """Check whether ``dvipdf`` (conversion of DVI reports into PDF) has been vetoed by the user (``installation_options.dat``).

        Returns:
            ``bool``:
            ``True`` if vetoed.
        """
        if self.user_info.dvipdf_veto:
            self.logger.debug("user setting: veto on dvipdf")
            return True
        else:
            self.logger.debug("no user veto")
            return False

        
    def AutoDetection(self) -> tuple[int, str]:
        """Look for ``dvipdf`` (conversion of DVI reports into PDF) on the system.

        Returns:
            ``tuple[int, str]``:
            Detection status (:class:`~madanalysis.enumeration.detect_status_type.DetectStatusType`)
            and a message.
        """
        # Which
        result = ShellCommand.Which('dvipdf',all=False,mute=True)
        if len(result)==0:
            # FIXME: a bare status is returned instead of a (status, message) tuple; DetectManager
            # unpacks two values (TypeError). Same below.
            return DetectStatusType.UNFOUND
        if self.debug:
            self.logger.debug("  which:         " + str(result[0]))

        # Ok
        return DetectStatusType.FOUND


    def ExtractInfo(self) -> bool:
        """Extract detailed information about the detected ``dvipdf`` (conversion of DVI reports into PDF) (version, paths, ...).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Which all
        if self.debug:
            result = ShellCommand.Which('dvipdf',all=True,mute=True)
            if len(result)==0:
                return False
            self.logger.debug("  which-all:     ")
            for file in result:
                self.logger.debug("    - "+str(file))

        # Ok
        return True


    def SaveInfo(self) -> bool:
        """Store the information about ``dvipdf`` (conversion of DVI reports into PDF) in the architecture/session information.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        self.session_info.has_dvipdf = True

        # Consisntency with the latex option
        if not self.session_info.has_latex:
            self.session_info.has_dvipdf = False

        # Ok
        return True


