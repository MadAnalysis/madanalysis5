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
from typing import Text, Sequence, Union

from ma5_validation.system.exceptions import InvalidMadAnalysisPath
from .path_handler import PathHandler
from .script_handler import ScriptReader


class InstallationHandler(ScriptReader):
    _installable = ["fastjet", "delphes", "root", "PAD", "PADForSFS", "PADForMA5Tune", "samples"]

    def __init__(self, packages: Union[Text, Sequence[Text]], paths: PathHandler = None):
        super(InstallationHandler, self).__init__(name="install")

        if isinstance(packages, str):
            packages = [packages]

        self.title = "Package Installation: " + ", ".join(packages)

        self._ma5_commands = []
        for package in packages:
            if package in InstallationHandler._installable:
                self._ma5_commands.append(f"install {package}\n")
