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

from ma5_validation.system.exceptions import MadAnalysis5Error
from .path_handler import PathHandler

from .reader import ScriptReader


class JobHandler:
    def __init__(self, script: ScriptReader, paths: PathHandler = None):
        assert isinstance(script, ScriptReader), f"Unknown input type: {type(script)}"
        self.script = script

        if paths is None:
            self.ma5_path = PathHandler.MA5PATH
            self.log_path = PathHandler.LOGPATH
        else:
            self.ma5_path = paths.MA5PATH
            self.log_path = paths.LOGPATH
        self.log_file = os.path.join(self.log_path, self.script.name + ".log")

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
            "&>",
            self.log_file,
        ]

        print("   * Running MadAnalysis 5: " + self.script.title)
        os.system(" ".join(commands))
        return True


    def analyze(self):
        endTag = False
        errorTag = False
        log_file = None
        with open(self.log_file, "r", encoding="utf-8") as log:
            for line in file:
                if line.find("ma5>#END") != -1:
                    endTag = True
                if endTag:
                    if line.find("ERROR") != -1 or line.find("MA5-ERROR") != -1:
                        errorTag = True
                        break
            if errorTag:
                log_file = log.read()

        if errorTag:
            print(log_file)
            raise MadAnalysis5Error(
                f"MadAnalysis has raised an error. For details, please see: {self.log_file}"
            )