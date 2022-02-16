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


import os, sys
from typing import Text

from ma5_validation.system.exceptions import InvalidMadAnalysisPath


class PathHandler:
    """
    Class includes all the paths defined within the validation suite

    LOGPATH: Location for the outputs of the validation scripts will be exported. Can be changed
        via `PathHandler.set_logpath(PATH)`
    MA5PATH: MadAnalysis 5 path. By default the path is the one that hosts validation suite.
        Can be changed via `PathHandler.set_ma5path(PATH)`.
    MA5_SCRIPTPATH: Main path for the validation scripts.
    PARTON_LEVEL_PATH: Main path for the parton level validation scripts.
    HADRON_LEVEL_PATH: Main path for the hadron level validation scripts.
    RECO_LEVEL_PATH: Main path for the reco level validation scripts.
    EXPERT_LEVEL_PATH: Main path for the expert level validation scripts.
    FASTJET_INTERFACE_PATH: Main path for the fastjet interface validation scripts.
    """

    # Set defult log path
    LOGPATH = os.path.dirname(os.path.realpath(__file__))
    for _ in range(3):
        LOGPATH = os.path.split(LOGPATH)[0]
    LOGPATH = os.path.join(LOGPATH, "scripts", "log")
    if not os.path.isdir(LOGPATH):
        os.mkdir(LOGPATH)

    # Default MA5 path
    MA5PATH = os.path.dirname(os.path.realpath(__file__))
    for _ in range(4):
        MA5PATH = os.path.split(MA5PATH)[0]

    # Sample path
    SMP_PATH = os.path.join(MA5PATH, "samples")

    # Main path for the validation scripts
    MA5_SCRIPTPATH = os.path.join(
        os.path.split(os.path.dirname(os.path.realpath(__file__)))[0], "ma5_scripts"
    )

    # These paths defines the location of the validation scripts
    PARTON_LEVEL_PATH = os.path.join(MA5_SCRIPTPATH, "parton_level")
    HADRON_LEVEL_PATH = os.path.join(MA5_SCRIPTPATH, "hadron_level")
    RECO_LEVEL_PATH = os.path.join(MA5_SCRIPTPATH, "reco_level")
    EXPERT_LEVEL_PATH = os.path.join(MA5_SCRIPTPATH, "expert_level")
    FASTJET_INTERFACE_PATH = os.path.join(MA5_SCRIPTPATH, "fastjet_interface")

    @staticmethod
    def set_ma5path(ma5_path: Text) -> None:
        """
        Set MadAnalysis path
        Parameters
        ----------
        ma5_path : Text
            A valid path for MadAnalysis 5 package
        """
        if not os.path.isdir(ma5_path):
            raise InvalidMadAnalysisPath(f"Invalid path: {ma5_path}")
        if not os.path.isdir(os.path.join(ma5_path, "tools/ReportGenerator/Services")):
            raise InvalidMadAnalysisPath(f"Invalid path: {ma5_path}")
        PathHandler.MA5PATH = os.path.normpath(ma5_path)
        SMP_PATH = os.path.join(PathHandler.MA5PATH, "samples")

    @staticmethod
    def set_logpath(logpath: Text) -> None:
        """
        Set log path for validation suite
        Parameters
        ----------
        logpath : Text
            The path where log file should be
        """
        if not os.path.isdir(logpath):
            os.mkdir(logpath)
        PathHandler.LOGPATH = logpath

    @staticmethod
    def check_ma5_setup():
        """
        Check current MadAnalysis Architecture

        Returns
        -------
        session and architecture info
        """
        if not os.path.isdir(PathHandler.MA5PATH):
            raise InvalidMadAnalysisPath(
                "Detected MadAnalysis 5 general folder is not correct: " + PathHandler.MA5PATH
            )
        os.environ["MA5_BASE"] = PathHandler.MA5PATH
        sys.path.insert(0, PathHandler.MA5PATH)
        servicedir = os.path.join(PathHandler.MA5PATH, "tools/ReportGenerator/Services")
        if not os.path.isdir(servicedir):
            raise InvalidMadAnalysisPath(
                "Detected MadAnalysis 5 service folder is not correct: " + PathHandler.MA5PATH
            )
        sys.path.insert(0, servicedir)
        from madanalysis.core.main import Main as ma5_main

        curdir = os.getcwd()
        os.chdir(os.path.join(PathHandler.MA5PATH, "bin"))
        main = ma5_main()
        main.archi_info.ma5dir = PathHandler.MA5PATH
        main.CheckConfig(debug=True)
        main.CheckConfig2(debug=True)
        main.Display()
        os.chdir(curdir)
        return main.session_info, main.archi_info
