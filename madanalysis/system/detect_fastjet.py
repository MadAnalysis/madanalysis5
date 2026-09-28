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


"""Detection of FastJet."""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any

import logging
import os
import re
import subprocess
from pathlib import Path

from shell_command import ShellCommand

from madanalysis.enumeration.detect_status_type import DetectStatusType

log = logging.getLogger("MA5")


class DetectFastjet:
    """Detector of FastJet.

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
        """Create the detector of FastJet.

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
        self.name = "FastJet"
        self.mandatory = False
        self.force = False
        self.lib_paths = []
        self.bin_file = ""
        self.bin_path = ""
        self.version = ""

    def IsItVetoed(self) -> bool:
        """Check whether FastJet has been vetoed by the user (``installation_options.dat``).

        Returns:
            ``bool``:
            ``True`` if vetoed.
        """
        if self.user_info.fastjet_veto:
            log.debug("user setting: veto on FastJet")
            return True
        else:
            log.debug("no user veto")
            return False

    def ManualDetection(self) -> tuple[int, str]:
        """Look for FastJet in the location given by the user (``installation_options.dat``).

        Returns:
            ``tuple[int, str]``:
            Detection status (:class:`~madanalysis.enumeration.detect_status_type.DetectStatusType`)
            and a message.
        """
        msg = ""

        # User setting
        if self.user_info.fastjet_bin_path is None:
            return DetectStatusType.UNFOUND, msg

        log.debug("User setting: fastjet bin path is specified.")

        # File & folder name
        folder = os.path.normpath(self.user_info.fastjet_bin_path)
        filename = folder + "/fastjet-config"

        # Detection of fastjet-config
        log.debug("Detecting fastjet-config in the path specified by the user ...")
        if not os.path.isfile(filename):
            logging.getLogger("MA5").debug("-> not found")
            msg = "fastjet-config program is not found in folder: " + folder + "\n"
            msg += "Please check that FastJet is properly installed."
            return DetectStatusType.UNFOUND, msg

        self.bin_file = filename
        self.bin_path = folder

        log.debug("fastjet-config program found in: %s", self.bin_path)

        # Ok
        return DetectStatusType.FOUND, msg

    def ToolsDetection(self) -> tuple[int, str]:
        """Look for FastJet in the ``tools`` folder of MadAnalysis 5 (local installation).

        Returns:
            ``tuple[int, str]``:
            Detection status (:class:`~madanalysis.enumeration.detect_status_type.DetectStatusType`)
            and a message.
        """
        msg = ""

        filename = os.path.normpath(
            self.archi_info.ma5dir + "/tools/fastjet/bin/fastjet-config"
        )
        log.debug("Look for FastJet in the folder here:%s...", filename)
        if os.path.isfile(filename):
            log.debug("-> found")
            self.bin_file = filename
            self.bin_path = os.path.dirname(self.bin_file)
        else:
            log.debug("-> not found")
            return DetectStatusType.UNFOUND, msg

        return DetectStatusType.FOUND, msg

    def AutoDetection(self) -> tuple[int, str]:
        """Look for FastJet on the system.

        Returns:
            ``tuple[int, str]``:
            Detection status (:class:`~madanalysis.enumeration.detect_status_type.DetectStatusType`)
            and a message.
        """
        msg = ""

        # Trying to call fastjet-config with which
        result = ShellCommand.Which("fastjet-config", mute=True)
        if len(result) == 0:
            msg = "The FastJet package is not found."
            return DetectStatusType.UNFOUND, msg
        islink = os.path.islink(result[0])
        if not islink:
            self.bin_file = os.path.normpath(result[0])
        else:
            self.bin_file = os.path.normpath(os.path.realpath(result[0]))
        self.bin_path = os.path.dirname(self.bin_file)

        # NOTE: FileNotFoundError is not caught if fastjet-config cannot be executed.
        try:
            result = subprocess.run([self.bin_file, "--config"], capture_output=True, text=True, check=True)
        except subprocess.CalledProcessError:
            return DetectStatusType.UNFOUND, "Unable to run fastjet-config."

        match = re.search(r"CXX=([^'\s]+)", result.stdout)
        fastjet_cxx = None
        if match:
            fastjet_cxx = match.group(1)
            log.debug("FastJet CXX = %s", fastjet_cxx)
        else:
            log.warning("FastJet does not report its compiler; compatibility will be checked by the SampleAnalyzer build test.")

        if fastjet_cxx is not None:
            if self.archi_info.has_root and self.archi_info.root_compiler != "":
                if Path(fastjet_cxx).stem != Path(self.archi_info.root_compiler).stem:
                    log.warning(
                        "FastJet is compiled with a different compiler than MadAnalysis."
                        "This might be due to the ROOT installation which requires specific compiler."
                        "Please rebuild FastJet through MadAnalysis."
                    )

        # Debug mode
        # FIXME: 'result' has been overwritten by the subprocess result above: result[0] raises a
        # TypeError in debug mode.
        if self.debug:
            log.debug(
                "  which:         %s [is it a link? %s]", str(result[0]), str(islink)
            )
            if islink:
                log.debug("                 -> %s", os.path.realpath(result[0]))

        # Which all
        if self.debug:
            result = ShellCommand.Which("fastjet-config", all=True, mute=True)
            if len(result) == 0:
                msg = "The FastJet package is not found."
                return DetectStatusType.UNFOUND, msg
            log.debug("  which-all:     ")
            for file in result:
                log.debug("    - %s", str(file))
        return DetectStatusType.FOUND, msg

    def ExtractInfo(self) -> bool:
        """Extract detailed information about the detected FastJet (version, paths, ...).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        theCommands = [self.bin_path + "/fastjet-config", "--version"]
        ok, out, err = ShellCommand.ExecuteWithCapture(theCommands, "./")
        if not ok:
            msg = "fastjet-config program does not work properly."
            return False  # ,msg
        out = out.lstrip()
        out = out.rstrip()
        self.version = str(out)
        if self.debug:
            log.debug("  version:       %s", self.version)

        # Using fastjet-config for getting lib and header paths
        log.debug("Trying to get library and header paths ...")
        theCommands = [self.bin_path + "/fastjet-config", "--libs", "--plugins"]
        ok, out, err = ShellCommand.ExecuteWithCapture(theCommands, "./")
        if not ok:
            msg = "fastjet-config program does not work properly."
            return False  # ,msg

        # Extracting FastJet library and header path
        out = out.lstrip()
        out = out.rstrip()
        log.debug("  Lib flags:     %s", str(out))
        words = out.split()
        for word in words:
            if word.startswith("-L") and not word[2:] in self.lib_paths:
                self.lib_paths.append(word[2:])
        if self.debug:
            log.debug("  Lib path:      %s", str(self.lib_paths))

        commands = [self.bin_path + "/fastjet-config", "--config"]
        ok, out, err = ShellCommand.ExecuteWithCapture(commands, "./")
        if not ok:
            return False  # ,msg

        match = re.search(r"CXX=([^'\s]+)", out)
        fastjet_cxx = None
        if match:
            fastjet_cxx = match.group(1)
            log.debug("FastJet CXX = %s", fastjet_cxx)

        if fastjet_cxx is not None:
            if self.archi_info.has_root and self.archi_info.root_compiler != "":
                if Path(fastjet_cxx).stem != Path(self.archi_info.root_compiler).stem:
                    log.warning(
                        "FastJet is compiled with a different compiler than MadAnalysis."
                        "This might be due to the ROOT installation which requires specific compiler."
                        "Please rebuild FastJet through MadAnalysis."
                    )
        else:
            log.warning("FastJet does not report its compiler; compatibility will be checked by the SampleAnalyzer build test.")

        # Ok
        return True

    def SaveInfo(self) -> bool:
        """Store the information about FastJet in the architecture/session information.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # archi_info
        self.archi_info.has_fastjet = True
        self.archi_info.fastjet_priority = self.force
        self.archi_info.fastjet_bin_path = self.bin_path
        self.archi_info.fastjet_original_bins = [self.bin_file]
        self.archi_info.fastjet_lib_paths = self.lib_paths

        # Ok
        return True
