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


"""Detection of gnuplot."""

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


class DetectGnuPlot:
    """Detector of gnuplot.

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
        """Create the detector of gnuplot.

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
        self.name      = 'gnuplot'
        self.mandatory = False
        self.log       = []
        self.logger    = logging.getLogger('MA5')

        # adding what you want here


    def PrintDisableMessage(self) -> None:
        """Log the consequences of gnuplot being unavailable."""
        self.logger.warning("gnuplot disabled. Plots in gnuplot format file will not be produced.")
        

    def IsItVetoed(self) -> bool:
        """Check whether gnuplot has been vetoed by the user (``installation_options.dat``).

        Returns:
            ``bool``:
            ``True`` if vetoed.
        """
        if self.user_info.gnuplot_veto:
            self.logger.debug("user setting: veto on gnuplot")
            return True
        else:
            self.logger.debug("no user veto")
            return False

        
    def AutoDetection(self) -> tuple[int, str]:
        """Look for gnuplot on the system.

        Returns:
            ``tuple[int, str]``:
            Detection status (:class:`~madanalysis.enumeration.detect_status_type.DetectStatusType`)
            and a message.
        """
        # Which
        result = ShellCommand.Which('gnuplot',all=False,mute=True)
        if len(result)==0:
            return DetectStatusType.UNFOUND,''
        if self.debug:
            self.logger.debug("  which:         " + str(result[0]))

        # Ok
        return DetectStatusType.FOUND,''


    def ExtractInfo(self) -> bool:
        """Extract detailed information about the detected gnuplot (version, paths, ...).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Which all
        if self.debug:
            result = ShellCommand.Which('gnuplot',all=True,mute=True)
            if len(result)==0:
                return False
            self.logger.debug("  which-all:     ")
            for item in result:
                self.logger.debug("    - "+str(item))

        # Version
        ok, out, err = ShellCommand.ExecuteWithCapture(['gnuplot','--version'],'./')
        if not ok:
            self.logger.warning('gnuplot is not found. Please install it if you would like to use gnuplot')
            return False
        out=out.lstrip()
        out=out.rstrip()
        self.version = str(out)
        if self.debug:
            self.logger.debug("  version:       " + self.version)

        # Ok
        return True


    def SaveInfo(self) -> bool:
        """Store the information about gnuplot in the architecture/session information.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        self.session_info.has_gnuplot = True
        return True


