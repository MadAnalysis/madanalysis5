################################################################################
#
#  Copyright (C) 2012-2025 Jack Araz, Eric Conte & Benjamin Fuks
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


from __future__ import absolute_import

import logging
import os
import re
import subprocess

from shell_command import ShellCommand

from madanalysis.enumeration.detect_status_type import DetectStatusType

log = logging.getLogger("MA5")


class DetectFastjet:
    def __init__(self, archi_info, user_info, session_info, debug):
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

    def IsItVetoed(self):
        if self.user_info.fastjet_veto:
            log.debug("user setting: veto on FastJet")
            return True
        else:
            log.debug("no user veto")
            return False

    def ManualDetection(self):
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

    def ToolsDetection(self):
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

    def AutoDetection(self):
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

        try:
            result = subprocess.run(
                ["./fastjet-config", "--config"],
                capture_output=True,
                text=True,
                check=True,
            )
        except subprocess.CalledProcessError:
            return DetectStatusType.UNFOUND, "Unable to run fastjet-config."

        match = re.search(r"CXX=([^'\s]+)", result.stdout)
        if match:
            fastjet_cxx = match.group(1)
            log.debug("FastJet CXX = %s", fastjet_cxx)
        else:
            return (
                DetectStatusType.UNFOUND,
                "Unable to determine fastjet compilation properties",
            )

        if self.archi_info.has_root and self.archi_info.root_compiler != "":
            if fastjet_cxx != self.archi_info.root_compiler:
                return (
                    DetectStatusType.UNFOUND,
                    "FastJet is compiled with a different compiler than MadAnalysis. Please rebuild FastJet through MadAnalysis.",
                )

        # Debug mode
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

    def ExtractInfo(self):
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
        if match:
            fastjet_cxx = match.group(1)
            log.debug("FastJet CXX = %s", fastjet_cxx)
        else:
            return False

        if self.archi_info.has_root and self.archi_info.root_compiler != "":
            if fastjet_cxx != self.archi_info.root_compiler:
                log.error(
                    "FastJet is compiled with a different compiler than MadAnalysis."
                    "This might be due to the ROOT installation which requires specific compiler."
                    "Please rebuild FastJet through MadAnalysis."
                )
                return False

        # Ok
        return True

    def SaveInfo(self):
        # archi_info
        self.archi_info.has_fastjet = True
        self.archi_info.fastjet_priority = self.force
        self.archi_info.fastjet_bin_path = self.bin_path
        self.archi_info.fastjet_original_bins = [self.bin_file]
        self.archi_info.fastjet_lib_paths = self.lib_paths

        # Ok
        return True
