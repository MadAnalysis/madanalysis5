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
from enum import Enum, auto
import os
from typing import Text

from ma5_validation.system.exceptions import InvalidScript, InvalidMode
from .path_handler import PathHandler

class _AutoName(Enum):
    def _generate_next_value_(name, start, count, last_values):
        return name

class MA5Mode(_AutoName):
    PARTON = auto()
    HADRON = auto()
    RECO = auto()
    RECOFAC = auto()

    @staticmethod
    def get_mode(mode: Text):
        if mode == "PARTON":
            return MA5Mode.PARTON
        elif mode == "HADRON":
            return MA5Mode.HADRON
        elif mode == "RECO":
            return MA5Mode.RECO
        elif mode == "RECOFRAC":
            return MA5Mode.RECOFAC
        else:
            raise InvalidMode(
                f"Unknown mode: {mode}. Available modes are: " +\
                ", ".join(MA5Mode._member_names_)
            )


class ScriptReader:
    """
    Validation script reader.

    Format of the valiation script:
    ```
    #TITLE Script title
    #MODE Ma5 run mode

    plot MET
    select MET > 20
    import $MA5PATH/samples/*lhe*
    ```
    `$MA5PATH` indicates madanalysis path which has been in use with the current validation run.

    Parameters
    ----------
    filename : Text
        script name or full path.
    """

    _modes = MA5Mode._member_names_

    def __init__(self, filename: Text = None, name: Text = None, paths: PathHandler = None):
        if name is None:
            self.name = os.path.basename(filename).split(".ma5")[0]
        else:
            self.name = name
        self.filename = filename
        self._mode = None
        self.title = None
        self._ma5_commands = []
        if paths is None:
            self.ma5_path = PathHandler.MA5PATH
        else:
            self.ma5_path = paths.MA5PATH

    def decode(self) -> None:
        """
        Decode MadAnalysis 5 validation script. Format:

        ```
        #TITLE Script title
        #MODE Ma5 run mode
        #SUBMIT Submission folder name i.e. sfs_test

        plot MET
        select MET > 20
        ```
        """
        if self.filename is None:
            raise InvalidScript("File name is not defined.")

        if len(self.filename.split("/")) > 1:
            if os.path.isfile(self.filename):
                filename = self.filename
            else:
                raise FileNotFoundError(f"Can't find ma5 script: {self.filename}")
        else:
            filename = os.path.join(
                os.path.split(os.path.dirname(os.path.realpath(__file__)))[0],
                "ma5_scripts",
                self.filename,
            )
            if not os.path.isfile(filename):
                raise FileNotFoundError(f"Can't find ma5 script: {filename}")

        with open(filename, "r", encoding="utf-8") as script:
            script_lines = []
            for line in script:
                if line.startswith("#TITLE"):
                    self.title = line[6:-1]
                elif line.startswith("#MODE"):
                    mode = line.split()[1].upper()
                    if mode in ScriptReader._modes:
                        self._mode = MA5Mode.get_mode(line.split()[1].upper())
                    else:
                        raise InvalidMode(f"Unknown MadAnalysis 5 mode: {mode}")
                if not line.startswith("#") and not line.startswith("\n") and "submit" not in line:
                    if line.startswith("import"):
                        script_lines.append(
                            line.replace("$MA5PATH", os.path.normpath(self.ma5_path))
                        )
                    else:
                        script_lines.append(line)

        if None in [self._mode, self.title]:
            raise InvalidScript(
                f"Script does not have mode or title. Please check the script: {filename}"
            )

        self._ma5_commands = script_lines

    @property
    def commands(self) -> Text:
        """
        Write commands list to text format

        Returns
        -------
        Text: MadAnalysis 5 commands
        """
        return "".join(self._ma5_commands) + "\n#END\n"

    @property
    def mode(self):
        return self._mode

    def mode_flag(self) -> Text:
        """
        Return commandline execution mode

        Returns
        -------
        Text: ma5 execution mode
        """
        if self._mode == MA5Mode.PARTON:
            return "--partonlevel"
        elif self._mode == MA5Mode.HADRON:
            return "--hadronlevel"
        elif self._mode == MA5Mode.RECO:
            return "--recolevel"
        elif self._mode == MA5Mode.RECOFAC:
            return "--FAC --recolevel"
        else:
            return ""
