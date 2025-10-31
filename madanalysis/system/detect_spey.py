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
from importlib.util import find_spec
from importlib.metadata import version

from madanalysis.enumeration.detect_status_type import DetectStatusType

log = logging.getLogger("MA5")


class DetectSpey:
    def __init__(self, archi_info, user_info, session_info, debug):
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

    def IsItVetoed(self):
        return False

    def AutoDetection(self):
        spey_check = find_spec("spey") is not None
        spey_pyhf_check = find_spec("spey_pyhf") is not None
        if not spey_check:
            log.debug("Spey is not available")
        if not spey_pyhf_check:
            log.debug("Spey-pyhf plug-in is not available")

        if not spey_pyhf_check or not spey_check:
            return DetectStatusType.UNFOUND, ""

        # Checking release
        log.debug("  release = %s", version("spey"))
        # log.debug("  where? = " + spey.__file__)

        # Ok
        return DetectStatusType.FOUND, ""

    def SaveInfo(self):
        self.session_info.has_spey = True
        self.archi_info.has_spey = True
        return True
