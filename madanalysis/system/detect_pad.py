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


"""Detection of a Public Analysis Database (PAD, PADForMA5tune or PADForSFS)."""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any
from shell_command import ShellCommand
from madanalysis.enumeration.detect_status_type import DetectStatusType
from madanalysis.system.config_checker import ConfigChecker
import logging, os


class DetectPAD:
    """Detector of a Public Analysis Database (PAD, PADForMA5tune or PADForSFS).

    The methods are called by
    :meth:`madanalysis.system.detect_manager.DetectManager.Execute` in the following
    order (only when defined): ``IsItVetoed``, ``AreDependenciesInstalled``,
    ``ManualDetection``, ``ToolsDetection``, ``AutoDetection``, ``ExtractInfo`` and
    ``SaveInfo``.

    Attributes:
        name (``str``): name displayed in the configuration check.
        mandatory (``bool``): whether MadAnalysis 5 can run without the package.
    """
    def __init__(self, archi_info: Any, user_info: Any, session_info: Any, debug: bool, padtype: str = "") -> None:
        """Create the detector of a PAD.

        Args:
            archi_info (``ArchitectureInfo``): system configuration.
            user_info (``UserInfo``): user options.
            session_info (``SessionInfo``): session information, filled by :meth:`SaveInfo`.
            debug (``bool``): print detailed information.
            padtype (``str``, default ``""``): ``""`` (PAD), ``"ma5"`` (PADForMA5tune) or
                ``"sfs"`` (PADForSFS).
        """
        self.archi_info = archi_info
        self.user_info = user_info
        self.session_info = session_info
        self.debug = debug
        self.ma5tune = padtype == "ma5"
        self.sfs = padtype == "sfs"
        if self.ma5tune:
            self.name = "PADForMA5tune"
        elif self.sfs:
            self.name = "PADForSFS"
        else:
            self.name = "PAD"
        self.mandatory = False
        self.force = False
        self.build_file = ""
        self.build_path = ""
        self.version = ""
        self.logger = logging.getLogger("MA5")

    def IsItVetoed(self) -> bool:
        """Check whether a Public Analysis Database (PAD, PADForMA5tune or PADForSFS) has been vetoed by the user (``installation_options.dat``).

        Returns:
            ``bool``:
            ``True`` if vetoed.
        """
        if self.ma5tune:
            if self.user_info.padma5_veto:
                self.logger.debug("user setting: veto on PADForMA5Tune")
                return True
            else:
                self.logger.debug("no user veto")
                return False
        elif self.sfs:
            if self.user_info.padsfs_veto:
                self.logger.debug("user setting: veto on PADForSFS")
                return True
            else:
                self.logger.debug("no user veto")
                return False
        else:
            if self.user_info.pad_veto:
                self.logger.debug("user setting: veto on PAD")
                return True
            else:
                self.logger.debug("no user veto")
                return False

    def AreDependenciesInstalled(self) -> bool:
        """Check whether the dependencies of a Public Analysis Database (PAD, PADForMA5tune or PADForSFS) are available.

        Returns:
            ``bool``:
            ``True`` if all dependencies are available.
        """
        checker = ConfigChecker(
            self.archi_info, self.user_info, self.session_info, False, False
        )
        if self.ma5tune:
            if not checker.checkDelphesMA5tune(True):
                self.logger.debug("dependency 'DelphesMA5tune' is not installed")
                return False
        elif not self.sfs:
            if not checker.checkDelphes(True):
                self.logger.debug("dependency 'Delphes' is not installed")
                return False
        return True

    def ManualDetection(self) -> tuple[int, str]:
        """Look for a Public Analysis Database (PAD, PADForMA5tune or PADForSFS) in the location given by the user (``installation_options.dat``).

        Returns:
            ``tuple[int, str]``:
            Detection status (:class:`~madanalysis.enumeration.detect_status_type.DetectStatusType`)
            and a message.
        """
        msg = ""

        if self.ma5tune:
            # User setting
            if self.user_info.padma5_build_path == None:
                return DetectStatusType.UNFOUND, msg

            self.logger.debug("User setting: PADForMA5Tune build path is specified.")

            # Folder name
            folder = os.path.normpath(self.user_info.padma5_build_path)

        # FIXME: 'if' instead of 'elif': for the PADForMA5tune, the else-branch below is also
        # executed and the PAD build path overrides the PADForMA5tune one.
        if self.sfs:
            # User setting
            if (
                self.user_info.padsfs_build_path is None
                or not self.archi_info.has_fastjet
            ):
                return DetectStatusType.UNFOUND, msg

            self.logger.debug("User setting: PADForSFS build path is specified.")

            # Folder name
            folder = os.path.normpath(self.user_info.padsfs_build_path)

        else:
            # User setting
            if self.user_info.pad_build_path is None:
                return DetectStatusType.UNFOUND, msg

            self.logger.debug("User setting: PAD build path is specified.")

            # Folder name
            folder = os.path.normpath(self.user_info.pad_build_path)

        filename = folder + "/MadAnalysis5job"

        # Detection of the PAD exectuable
        self.logger.debug(
            "Detecting MadAnalysis5job in the path specified by the user ..."
        )
        if not os.path.isfile(filename) and not self.sfs:
            logging.getLogger("MA5").debug("-> not found")
            msg = "MadAnalysis5job program is not found in folder: " + folder + "\n"
            msg += "Please check that " + self.name + " is properly installed."
            return DetectStatusType.UNFOUND, msg

        self.build_file = filename
        self.build_path = folder

        self.logger.debug("MadAnalysis5job program found in: %s", self.build_path)

        # Ok
        return DetectStatusType.FOUND, msg

    def ToolsDetection(self) -> tuple[int, str]:
        """Look for a Public Analysis Database (PAD, PADForMA5tune or PADForSFS) in the ``tools`` folder of MadAnalysis 5 (local installation).

        Returns:
            ``tuple[int, str]``:
            Detection status (:class:`~madanalysis.enumeration.detect_status_type.DetectStatusType`)
            and a message.
        """
        msg = ""
        if not self.session_info.has_spey:
            msg = (
                "Spey is not installed. Please install it before using " + self.name + "."
            )
            return DetectStatusType.UNFOUND, msg

        if self.ma5tune:
            thefolder = "PADForMA5tune"
        elif self.sfs:
            thefolder = "PADForSFS"
            if not self.archi_info.has_fastjet:
                return DetectStatusType.UNFOUND, msg
        else:
            thefolder = "PAD"

        filename = os.path.normpath(
            self.archi_info.ma5dir + "/tools/" + thefolder + "/Build/MadAnalysis5job"
        )
        if self.sfs:
            filename = os.path.normpath(
                self.archi_info.ma5dir
                + "/tools/"
                + thefolder
                + "/Build/SampleAnalyzer/User/Analyzer/analysisList.h"
            )

        self.logger.debug(
            "Look for " + self.name + " in the folder here :" + filename + " ..."
        )
        if os.path.isfile(filename):
            self.logger.debug("-> found")
            self.build_file = filename
            self.build_path = os.path.dirname(self.build_file)
        else:
            self.logger.debug("-> not found")
            return DetectStatusType.UNFOUND, msg

        return DetectStatusType.FOUND, msg

    def ExtractInfo(self) -> bool:
        """Extract detailed information about the detected a Public Analysis Database (PAD, PADForMA5tune or PADForSFS) (version, paths, ...).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        if self.sfs:
            return True
        theCommands = [self.build_file, "--info"]
        ok, out, err = ShellCommand.ExecuteWithCapture(theCommands, "./")
        if not ok:
            self.logger.debug("->ERROR: MadAnalyis5job program does not work properly.")
            self.logger.debug(str(out))
            self.logger.debug(str(err))
            return False
        lines = out.split("\n")
        ok = False
        nbAnalysis = 0
        for line in lines:
            line = line.lstrip()
            line = line.rstrip()
            if line.startswith("BEGIN "):
                self.logger.debug("  MA5 stamp found!")
                ok = True
                continue
            if ok:
                nbAnalysis += 1
            if line.endswith("END "):
                break
        if self.debug:
            self.logger.debug("  number of recast analyses: " + str(nbAnalysis))

        # Ok
        return ok

    def SaveInfo(self) -> bool:
        """Store the information about a Public Analysis Database (PAD, PADForMA5tune or PADForSFS) in the architecture/session information.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # archi_info
        if self.ma5tune:
            self.session_info.has_padma5 = True
            self.session_info.padma5_build_path = self.build_path
            self.session_info.padma5_original_bins = [self.build_file]
        elif self.sfs:
            self.session_info.has_padsfs = True
            self.session_info.padsfs_build_path = self.build_path
            self.session_info.padsfs_original_bins = [self.build_file]
        else:
            self.session_info.has_pad = True
            self.session_info.pad_build_path = self.build_path
            self.session_info.pad_original_bins = [self.build_file]

        # Ok
        return True
