################################################################################
#
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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


import os
from typing import Text

from ma5_validation.system.exceptions import InvalidMadAnalysisPath


class PathHandler:
    LOGPATH = os.path.dirname(os.path.realpath(__file__))
    for _ in range(3):
        LOGPATH = os.path.split(LOGPATH)[0]
    LOGPATH = os.path.join(LOGPATH, "scripts", "log")
    if not os.path.isdir(LOGPATH):
        os.mkdir(LOGPATH)

    MA5PATH = os.path.dirname(os.path.realpath(__file__))
    for _ in range(4):
        MA5PATH = os.path.split(MA5PATH)[0]

    MA5_SCRIPTPATH = os.path.join(
        os.path.split(os.path.dirname(os.path.realpath(__file__)))[0], "ma5_scripts"
    )

    @staticmethod
    def set_ma5path(ma5path: Text):
        if not os.path.isdir(ma5_path):
            raise InvalidMadAnalysisPath(f"Invalid path: {ma5_path}")
        if not os.path.isdir(os.path.join(ma5_path, "tools/ReportGenerator/Services")):
            raise InvalidMadAnalysisPath(f"Invalid path: {ma5_path}")
        PathHandler.MA5PATH = os.path.normpath(ma5_path)

    @staticmethod
    def set_logpath(logpath: Text):
        if not os.path.isdir(logpath):
            os.mkdir(logpath)
        PathHandler.LOGPATH = logpath
