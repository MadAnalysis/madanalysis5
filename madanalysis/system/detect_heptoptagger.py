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


from __future__ import absolute_import
import logging, os
from madanalysis.enumeration.detect_status_type import DetectStatusType


class DetectHEPTopTagger:
    def __init__(self, archi_info, user_info, session_info, debug):
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

    def IsItVetoed(self):
        if self.user_info.fastjet_veto:
            self.logger.debug("user setting: veto on FastJet")
            return True
        else:
            self.logger.debug("no user veto")
            return False

    def detect_files(self):
        """
        Detect HEPTopTagger files
        """
        msg = ""

        if not self.archi_info.has_fastjet:
            logging.getLogger("MA5").debug(f" -> no fastjet")
            return DetectStatusType.UNFOUND, "FastJet does not exist"
        if not self.archi_info.has_fjcontrib:
            logging.getLogger("MA5").debug(f" -> no fastjet contrib.")
            return DetectStatusType.UNFOUND, "FastJet contrib does not exist"

        if not os.path.isdir(os.path.join(self.archi_info.ma5dir, "tools", "HEPTopTagger")):
            logging.getLogger("MA5").debug(
                f" -> {os.path.join(self.archi_info.ma5dir, 'tools', 'HEPTopTagger')} does not exist."
            )
            return (
                DetectStatusType.UNFOUND,
                f"{os.path.join(self.archi_info.ma5dir, 'tools', 'HEPTopTagger')} does not exist.",
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

    def ManualDetection(self):
        return self.detect_files()

    def ToolsDetection(self):
        return self.detect_files()

    def AutoDetection(self):
        return self.detect_files()

    def ExtractInfo(self):
        return True

    def SaveInfo(self):
        # archi_info
        self.archi_info.has_heptoptagger = True

        # Ok
        return True
