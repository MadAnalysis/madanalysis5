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

from ma5_validation.system.exceptions import InvalidScript
from .mode_handler import MA5Mode
from .path_handler import PathHandler


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

    def __init__(self, filename: Text = None, name: Text = None):
        if name is None:
            self.name = os.path.basename(filename).split(".ma5")[0]
        else:
            self.name = name
        self.filename = filename
        self._mode = None
        self.title = None
        self._ma5_commands = []

    @staticmethod
    def _modifier(line: Text) -> Text:
        """
        Modifies the input string's keywords

        Parameters
        ----------
        line : Text
            input string

        Returns
        -------
        Text:
            modified line
        """
        modifications = {
            "$MA5PATH": PathHandler.MA5PATH,
            "$SMP_PATH": PathHandler.SMP_PATH,
            "$LOGPATH": PathHandler.LOGPATH,
            "$PARTON_LEVEL_PATH": PathHandler.PARTON_LEVEL_PATH,
            "$HADRON_LEVEL_PATH": PathHandler.HADRON_LEVEL_PATH,
            "$RECO_LEVEL_PATH": PathHandler.RECO_LEVEL_PATH,
            "$EXPERT_LEVEL_PATH": PathHandler.EXPERT_LEVEL_PATH,
            "$FASTJET_INTERFACE_PATH": PathHandler.FASTJET_INTERFACE_PATH,
        }
        for key, path in modifications.items():
            line = line.replace(key, path)
        return line

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
                    self.title = " ".join(line[6:-1].split())
                    continue
                elif line.startswith("#MODE"):
                    self.mode = line.split()[1]

                    if self.IsExpert:
                        # initialize Expert mode attributes
                        for attr in ["cpp", "header", "sample", "expert_name", "command_line"]:
                            setattr(self, attr, None)

                    continue

                if None in [self.mode, self.title]:
                    raise InvalidScript(
                        f"First two lines of the script needs to include #TITLE and #MODE: {filename}"
                    )

                # Read location of the cpp and header files
                if self.IsExpert:
                    if line.startswith("#CPP"):
                        self.cpp = self._modifier(line.split()[1])
                    elif line.startswith("#HEADER"):
                        self.header = self._modifier(line.split()[1])
                    elif line.startswith("#COMMANDLINE"):
                        self.command_line = " ".join(line.split("#COMMANDLINE")[1:])

                if not line.startswith("#") and not line.startswith("\n"):
                    line = self._modifier(line)
                    if line.startswith("import"):
                        if not self.IsExpert:
                            script_lines.append(line)
                        else:
                            sample = line
                            if "*" in sample:
                                from glob import glob

                                self.sample = glob(sample)
                            else:
                                self.sample = [sample]

                    elif "submit" in line and not self.IsExpert:
                        script_lines.append(
                            f"submit {os.path.join(PathHandler.LOGPATH, self.name + '_' + self.mode.name)}\n"
                        )
                    else:
                        script_lines.append(line)

        if None in [self.mode, self.title]:
            raise InvalidScript(
                f"Script does not have mode or title. Please check the script: {filename}"
            )

        if self.IsExpert:
            assert None not in [self.cpp, self.header], "CPP and Header files are not defined"
            assert (
                os.path.basename(self.cpp).split(".cpp")[0]
                == os.path.basename(self.header).split(".h")[0]
            ), "Invalid expert analysis decleration"
            assert self.sample is not None, "No sample has been provided."
            assert os.path.isfile(self.cpp), f"Can't find {self.cpp}"
            assert os.path.isfile(self.header), f"Can't find {self.header}"
            self.expert_name = os.path.basename(self.cpp).split(".cpp")[0]
            self.command_line = "" if self.command_line is None else self.command_line

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
        """
        Get or set MadAnalysis 5 Mode

        Parameters
        ----------
        mode : Text
            Mode indicator

        Raises
        ------
        InvalidMode: If Mode has not been implemented in validation suite
        """
        return self._mode

    @mode.setter
    def mode(self, mode: Text) -> None:
        """
        Set MadAnalysis 5 mode

        Parameters
        ----------
        mode : Text
            Mode indicator

        Raises
        ------
        InvalidMode: If Mode has not been implemented in validation suite
        """
        self._mode = MA5Mode.get_mode(mode.upper())

    @property
    def IsExpert(self) -> bool:
        """
        Is the script tagged as expert mode.

        Returns
        -------
        Bool
        """
        return "EXPERT" in self.mode.name

    def mode_flag(self) -> Text:
        """
        Return commandline execution mode

        Returns
        -------
        Text: ma5 execution mode
        """
        return MA5Mode.get_flag(self.mode)
