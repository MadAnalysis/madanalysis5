################################################################################
#
#  Copyright (C) 2012-2020 Jack Araz, Eric Conte & Benjamin Fuks
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


from __future__ import absolute_import
import logging, os
from shell_command import ShellCommand
from madanalysis.enumeration.detect_status_type import DetectStatusType


class DetectFastjetContrib:
    def __init__(self, archi_info, user_info, session_info, debug):
        self.archi_info = archi_info
        self.user_info = user_info
        self.session_info = session_info
        self.debug = debug
        self.name = "FastJet Contrib"
        self.mandatory = False
        self.force = False
        self.lib_paths = []
        self.bin_file = ""
        self.bin_path = ""
        self.version = ""
        self.logger = logging.getLogger("MA5")
        self.required = ["RecursiveTools", "Nsubjettiness", "VariableR", "EnergyCorrelator"]

    def IsItVetoed(self):
        if self.user_info.fastjet_veto:
            self.logger.debug("user setting: veto on FastJet")
            return True
        else:
            self.logger.debug("no user veto")
            return False

    def detect_libs(self, fjconfig):
        """
        Detect fastjet contrib libraries
        """
        msg = ""

        theCommands = [fjconfig, "--libs"]
        ok, out, err = ShellCommand.ExecuteWithCapture(theCommands, "./")
        if not ok:
            return False, "The fastjet-config executable does not work properly."

        libpath = [x[2:] for x in out.split() if x.startswith("-L")][0]

        not_found = []
        for libs in self.required:
            if not any(
                [f"lib{libs}{ext}" in os.listdir(libpath) for ext in [".a", ".dylib", ".so"]]
            ):
                not_found.append(libs)
        if len(not_found) != 0:
            return (
                DetectStatusType.UNFOUND,
                "The following FJContrib libraries cannot be found: " + ", ".join(not_found),
            )

        # Ok
        return DetectStatusType.FOUND, msg

    def ManualDetection(self):
        msg = ""

        # User setting
        if self.user_info.fastjet_bin_path == None:
            return DetectStatusType.UNFOUND, msg
        # Ok
        return self.detect_libs(self.user_info.fastjet_bin_path + "/fastjet-config")

    def ToolsDetection(self):
        msg = ""

        filename = os.path.normpath(self.archi_info.ma5dir + "/tools/fastjet/bin/fastjet-config")
        self.logger.debug("Look for FastJet in the folder here:" + filename + " ...")
        if os.path.isfile(filename):
            self.logger.debug("-> found")
            self.bin_file = filename
            self.bin_path = os.path.dirname(self.bin_file)
        else:
            self.logger.debug("-> not found")
            return DetectStatusType.UNFOUND, msg

        return self.detect_libs(self.archi_info.ma5dir + "/tools/fastjet/bin/fastjet-config")

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

        # Debug mode
        if self.debug:
            self.logger.debug(
                "  which:         " + str(result[0]) + " [is it a link? " + str(islink) + "]"
            )
            if islink:
                self.logger.debug("                 -> " + os.path.realpath(result[0]))

        # Which all
        if self.debug:
            result = ShellCommand.Which("fastjet-config", all=True, mute=True)
            if len(result) == 0:
                msg = "The FastJet package is not found."
                return DetectStatusType.UNFOUND, msg
            self.logger.debug("  which-all:     ")
            for file in result:
                self.logger.debug("    - " + str(file))

        return self.detect_libs("fastjet-config")

    def ExtractInfo(self):
        return True

    def SaveInfo(self):
        # archi_info
        self.archi_info.has_fjcontrib = True

        # Ok
        return True
