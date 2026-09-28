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


"""Detection of Spey and spey-pyhf (statistics of the recasting mode)."""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any

import logging
from importlib.util import find_spec
from importlib.metadata import version

from madanalysis.enumeration.detect_status_type import DetectStatusType

log = logging.getLogger("MA5")


class DetectSpey:
    """Detector of Spey and spey-pyhf (statistics of the recasting mode).

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
        """Create the detector of Spey and spey-pyhf (statistics of the recasting mode).

        Args:
            archi_info (``ArchitectureInfo``): system configuration, filled by :meth:`SaveInfo`.
            user_info (``UserInfo``): user options (vetoes, forced paths).
            session_info (``SessionInfo``): session information, filled by :meth:`SaveInfo`.
            debug (``bool``): print detailed information.
        """
        # mandatory options
        self.archi_info = archi_info
        self.user_info = user_info
        self.session_info = session_info
        self.debug = debug
        self.name = "Spey"
        self.mandatory = False
        self.log = []
        self.moreInfo = "For more details see https://spey.readthedocs.io"
        # adding what you want here

    def IsItVetoed(self) -> bool:
        """Check whether Spey and spey-pyhf (statistics of the recasting mode) has been vetoed by the user (``installation_options.dat``).

        Returns:
            ``bool``:
            ``True`` if vetoed.
        """
        return False

    def AutoDetection(self) -> tuple[int, str]:
        """Look for Spey and spey-pyhf (statistics of the recasting mode) on the system.

        Returns:
            ``tuple[int, str]``:
            Detection status (:class:`~madanalysis.enumeration.detect_status_type.DetectStatusType`)
            and a message.
        """
        spey_check = find_spec("spey") is not None
        # FIXME: only ModuleNotFoundError is caught: any other exception raised while importing
        # spey/spey_pyhf (e.g. a broken jax backend) aborts MadAnalysis 5 at startup.
        spey_pyhf_check = find_spec("spey_pyhf") is not None
        if not spey_check:
            log.debug("Spey is not available")
        if not spey_pyhf_check:
            log.debug("Spey-pyhf plug-in is not available")

        if not spey_pyhf_check or not spey_check:
            return DetectStatusType.UNFOUND, ""

        # Checking release
        log.debug("  release = %s", version("spey"))
        log.debug(self.moreInfo)
        # log.debug("  where? = " + spey.__file__)

        # Ok
        return DetectStatusType.FOUND, ""

    def SaveInfo(self) -> bool:
        """Store the information about Spey and spey-pyhf (statistics of the recasting mode) in the architecture/session information.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        self.session_info.has_spey = True
        return True
