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

from .reader import ScriptReader


class JobHandler:
    def __init__(self, script: ScriptReader, ma5_path: Text, log_path: Text = None):
        self.script = script
        if log_path is None:
            self.log_path = os.path.dirname(os.path.realpath(__file__))
            for _ in range(3):
                self.log_path = os.path.split(self.log_path)[0]
            self.log_path = os.path.join(self.log_path, "scripts", "log")
            if not os.path.isdir(self.log_path):
                os.mkdir(self.log_path)
        else:
            if not os.path.isdir(log_path):
                os.mkdir(log_path)
            self.log_path = log_path
        if not os.path.isdir(ma5_path):
            raise InvalidMadAnalysisPath(f"Invalid path: {ma5_path}")
        if not os.path.isdir(os.path.join(ma5_path, "tools/ReportGenerator/Services")):
            raise InvalidMadAnalysisPath(f"Invalid path: {ma5_path}")

        self.ma5_path = os.path.normpath(ma5_path)

    def install(self, packages: Union[Text, Sequence[Text]] = None):
        if packages is None:
            packages = []
        if isinstance(packages, str):
            packages = [packages]
        commands = []
        for package in packages:
            commands.append(f"install {package}")
        self.write_ma5script("\n".join(commands), "installation.ma5")

        commands = [
            self.ma5_path + "/bin/ma5",
            "--forced",
            "--script",
            "--debug",
            os.path.join(self.log_path, "installation.ma5"), "&>",
            os.path.join(self.log_path, "installation.log")
        ]
        print("   * Installing packages...")
        os.system(" ".join(commands))

    def write_ma5script(self, commands, name: Text = None):
        script_name = os.path.join(
            self.log_path, name if name is not None else self.script.name + ".ma5"
        )
        with open(script_name, "w") as script:
            script.write(commands)

    def execute(self):
        self.write_ma5script(
            self.script.commands + f"\n\nsubmit {os.path.join(self.log_path, self.script.name)}"
        )

        commands = [
            self.ma5_path + "/bin/ma5",
            "--forced",
            "--script",
            "--debug",
            self.script.mode,
            os.path.join(self.log_path, self.script.name + ".ma5"),
        ]

        print("   * Running MadAnalysis 5: " + self.script.title)
        os.system(" ".join(commands))
        return True
