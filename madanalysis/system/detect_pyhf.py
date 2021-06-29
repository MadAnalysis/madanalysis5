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
import logging, os, sys
from madanalysis.enumeration.detect_status_type import DetectStatusType

class Detectpyhf:

    def __init__(self, archi_info, user_info, session_info, debug):
        # mandatory options
        self.archi_info   = archi_info
        self.user_info    = user_info
        self.session_info = session_info
        self.debug        = debug
        self.name         = 'pyhf'
        self.mandatory    = False
        self.log          = []
        self.logger       = logging.getLogger('MA5')
        self.moreInfo     = 'For more details see https://scikit-hep.org/pyhf/'
        # adding what you want here


    def IsItVetoed(self):
        if self.user_info.pyhf_veto:
            self.logger.debug("user setting: veto on pyhf")
            return True
        else:
            self.logger.debug("no user veto")
            return False


    def AutoDetection(self):

        # Checking if scipy is installed on the system
        pyhf_path = os.path.join(self.archi_info.ma5dir,'tools/pyhf'+(sys.version_info[0] > 2)*'/src')
        if os.path.isdir(pyhf_path) and pyhf_path not in sys.path:
            sys.path.insert(0, pyhf_path)
        try:
            import pyhf
            self.logger.debug("pyhf has been imported from "+" ".join(pyhf.__path__))
        except ModuleNotFoundError as err:
            self.logger.debug(str(err))
            return DetectStatusType.UNFOUND,''

        # Checking release
        if self.debug:
            # pyhf version
            self.logger.debug("  release = "+pyhf.__version__)
            # pyhf location
            self.logger.debug("  where? = "+pyhf.__file__)

        # Ok
        return DetectStatusType.FOUND,''


    def SaveInfo(self):
        self.session_info.has_pyhf = True
        return True
