################################################################################
#
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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


"""Detection of HEPTopTagger (``tools/HEPTopTagger``)."""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any
import logging, os
from madanalysis.enumeration.detect_status_type import DetectStatusType


class DetectHEPTopTagger:
    """Detector of HEPTopTagger (``tools/HEPTopTagger``).

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
        """Create the detector of HEPTopTagger (``tools/HEPTopTagger``).

        Args:
            archi_info (``ArchitectureInfo``): system configuration, filled by :meth:`SaveInfo`.
            user_info (``UserInfo``): user options (vetoes, forced paths).
            session_info (``SessionInfo``): session information, filled by :meth:`SaveInfo`.
            debug (``bool``): print detailed information.
        """
        self.archi_info = archi_info
        self.user_info = user_info
        self.session_info = session_info
        self.debug = debug
        self.name = "HEPTopTagger"
        self.mandatory = False
        self.force = False
        self.lib_paths = []
        self.bin_file = ""
        self.bin_path = ""
        self.version = ""
        self.logger = logging.getLogger("MA5")

    def IsItVetoed(self) -> bool:
        """Check whether HEPTopTagger (``tools/HEPTopTagger``) has been vetoed by the user (``installation_options.dat``).

        Returns:
            ``bool``:
            ``True`` if vetoed.
        """
        if self.user_info.fastjet_veto:
            self.logger.debug("user setting: veto on FastJet")
            return True
        else:
            self.logger.debug("no user veto")
            return False

    def detect_files(self) -> tuple[int, str]:
        """Check that FastJet, FastJet contrib and the HEPTopTagger sources are available.

        Returns:
            ``tuple[int, str]``:
            Detection status and message.
        """
        msg = ""

        if not self.archi_info.has_fastjet:
            logging.getLogger("MA5").debug(f" -> FastJet not found")
            return DetectStatusType.UNFOUND, "FastJet not found."
        if not self.archi_info.has_fjcontrib:
            logging.getLogger("MA5").debug(f" -> FastJet contrib not found.")
            return DetectStatusType.UNFOUND, "FastJet contrib not found."

        if not os.path.isdir(os.path.join(self.archi_info.ma5dir, "tools", "HEPTopTagger")):
            logging.getLogger("MA5").debug(
                f" -> The {os.path.join(self.archi_info.ma5dir, 'tools', 'HEPTopTagger')} folder does not exist."
            )
            return (
                DetectStatusType.UNFOUND,
                f"The {os.path.join(self.archi_info.ma5dir, 'tools', 'HEPTopTagger')} folder does not exist.",
            )

        # Check HTT files
        for htt_file in ["HEPTopTagger.hh", "HEPTopTagger.cc"]:
            if not os.path.isfile(
                os.path.join(self.archi_info.ma5dir, "tools", "HEPTopTagger", htt_file)
            ):
                logging.getLogger("MA5").debug(f" -> {htt_file} is missing.")
                return DetectStatusType.UNFOUND, f"{htt_file} is missing."
        # Ok
        return DetectStatusType.FOUND, msg

    def ManualDetection(self) -> tuple[int, str]:
        """Look for HEPTopTagger (``tools/HEPTopTagger``) in the location given by the user (``installation_options.dat``).

        Returns:
            ``tuple[int, str]``:
            Detection status (:class:`~madanalysis.enumeration.detect_status_type.DetectStatusType`)
            and a message.
        """
        return self.detect_files()

    def ToolsDetection(self) -> tuple[int, str]:
        """Look for HEPTopTagger (``tools/HEPTopTagger``) in the ``tools`` folder of MadAnalysis 5 (local installation).

        Returns:
            ``tuple[int, str]``:
            Detection status (:class:`~madanalysis.enumeration.detect_status_type.DetectStatusType`)
            and a message.
        """
        return self.detect_files()

    def AutoDetection(self) -> tuple[int, str]:
        """Look for HEPTopTagger (``tools/HEPTopTagger``) on the system.

        Returns:
            ``tuple[int, str]``:
            Detection status (:class:`~madanalysis.enumeration.detect_status_type.DetectStatusType`)
            and a message.
        """
        return self.detect_files()

    def ExtractInfo(self) -> bool:
        """Extract detailed information about the detected HEPTopTagger (``tools/HEPTopTagger``) (version, paths, ...).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        return True

    def SaveInfo(self) -> bool:
        """Store the information about HEPTopTagger (``tools/HEPTopTagger``) in the architecture/session information.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # archi_info
        self.archi_info.has_heptoptagger = True

        # Ok
        return True
