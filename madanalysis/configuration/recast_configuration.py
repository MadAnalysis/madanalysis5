################################################################################
#
#  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


"""Configuration of the recasting mode (``set main.recast = on`` and ``set main.recast.*``).

In recasting mode, the events are processed by one or several detector simulations
(Delphes, Delphes-MA5tune or SFS) and by the corresponding analyses of the Public
Analysis Database (PAD, PADForMA5tune, PADForSFS). The analyses to run are listed in a
*recasting card* (``Input/recasting_card.dat`` of the job directory) whose lines read::

    <analysis>  <v1.1|v1.2|vSFS>  <on|off>  <detector card>  # description

where ``v1.1`` refers to the PADForMA5tune, ``v1.2`` to the PAD and ``vSFS`` to the
PADForSFS. The detector card associated with each analysis is given in
``tools/<PAD>/Input/recast_config.dat``.
"""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any

import glob
import logging
import os
import shutil

from madanalysis.enumeration.ma5_running_type import MA5RunningType

# pylint: disable=logging-fstring-interpolation, logging-not-lazy


class RecastConfiguration:
    """Settings of the recasting mode.

    Attributes:
        status (``str``): ``"on"`` or ``"off"``.
        delphes / ma5tune (``bool``): whether Delphes / Delphes-MA5tune are available.
        pad / padtune / padsfs (``bool``): whether the PAD / PADForMA5tune / PADForSFS are
            installed.
        card_path (``str``): user-provided recasting card (empty = generated card).
        store_root / store_events (``bool``): keep the (reconstructed) event files.
        TACO_output (``str``): output file of the TACO mode (empty = disabled).
        global_likelihoods_switch (``bool``): use full/simplified likelihoods when available.
        simplify_likelihoods (``bool``): simplify full likelihoods (``simplify`` package).
        systematics (``list[tuple[float, float]]``): relative (up, down) systematic
            uncertainties on the signal.
        extrapolated_luminosities (``list[float]``): luminosities (fb^-1) to extrapolate to.
        THerror_combination (``str``): ``"linear"`` or ``"quadratic"`` combination of
            theory uncertainties.
        error_extrapolation (``str | list[float]``): ``"linear"``, ``"sqrt"`` or
            user-defined ``[syst, stat]`` relative background uncertainties for the
            extrapolation.
        stat_only_mode / stat_only_dir: recompute the statistics from an existing job
            directory (currently disabled in the interface).
        analysis_only_mode (``bool``): only compute the signal-region efficiencies.
        DelphesDic (``dict[str, list[str]]``): detector card -> analyses, read from the
            ``recast_config.dat`` files of the installed PADs.
        description (``dict[str, str]``): analysis -> description, read from the
            ``analysis_description.dat`` files.
        ma5dir (``str``): MadAnalysis 5 installation folder.
    """

    userVariables = {
        "status": ["on", "off"],
        "card_path": "",
        "store_root": ["True", "False"],
        "store_events": ["True", "False"],
        "THerror_combination": ["quadratic", "linear"],
        "error_extrapolation": ["linear", "sqrt"],
        "global_likelihoods": ["on", "off"],
        "simplify_likelihoods": ["True", "False"],
        "analysis_only_mode": ["True", "False"],
        #        "stat_only_mode": "",
        "TACO_output": "",
    }

    def __init__(self) -> None:
        """Initialise a disabled recasting configuration and read the PAD metadata.

        The files ``tools/<PAD>/Input/recast_config.dat`` and
        ``tools/<PAD>/Input/analysis_description.dat`` of every installed PAD are parsed to
        fill :attr:`DelphesDic` and :attr:`description`.
        """
        self.status = "off"
        self.delphes = False
        self.ma5tune = False
        self.pad = False
        self.padtune = False
        self.padsfs = False
        self.store_root = False
        self.store_events = False
        self.TACO_output = ""
        self.global_likelihoods_switch = True
        self.simplify_likelihoods = False
        self.systematics = []
        self.extrapolated_luminosities = []
        self.THerror_combination = "linear"
        self.error_extrapolation = "linear"
        self.stat_only_mode = False
        self.analysis_only_mode = False
        self.stat_only_dir = None
        self.DelphesDic = {}
        self.description = {}
        self.ma5dir = os.path.abspath(
            os.path.join(
                os.path.dirname(os.path.realpath(__file__)), os.pardir, os.pardir
            )
        )
        for mypad in ["PAD", "PADForMA5tune", "PADForSFS"]:
            if os.path.isfile(
                os.path.join(self.ma5dir, "tools", mypad, "Input", "recast_config.dat")
            ):
                dico_file = open(
                    os.path.join(
                        self.ma5dir, "tools", mypad, "Input", "recast_config.dat"
                    ),
                    "r",
                )
                for line in dico_file:
                    if line.strip().startswith("#"):
                        continue
                    self.DelphesDic[line.split("|")[0].strip()] = line.split("|")[
                        1
                    ].split()
                dico_file.close()
            if os.path.isfile(
                os.path.join(
                    self.ma5dir, "tools", mypad, "Input", "analysis_description.dat"
                )
            ):
                dico_file = open(
                    os.path.join(
                        self.ma5dir, "tools", mypad, "Input", "analysis_description.dat"
                    ),
                    "r",
                )
                for line in dico_file:
                    if line.strip().startswith("#"):
                        continue
                    self.description[line.split("|")[0].strip()] = line.split("|")[1][:-1]
                dico_file.close()

        self.card_path = ""
        self.logger = logging.getLogger("MA5")

    def Display(self) -> None:
        """Log the recasting settings."""
        self.user_DisplayParameter("status")
        if self.status == "on":
            self.user_DisplayParameter("delphes")
            self.user_DisplayParameter("ma5tune")
            self.user_DisplayParameter("pad")
            self.user_DisplayParameter("padtune")
            self.user_DisplayParameter("padsfs")
            self.user_DisplayParameter("card_path")
            self.user_DisplayParameter("store_events")
            self.user_DisplayParameter("TACO_output")
            # FIXME: user_DisplayParameter handles 'extrapolated_luminosity' (singular): nothing is displayed.
            self.user_DisplayParameter("extrapolated_luminosities")
            self.user_DisplayParameter("systematics")
            self.user_DisplayParameter("THerror_combination")
            self.user_DisplayParameter("error_extrapolation")
            self.user_DisplayParameter("global_likelihoods")
            #            self.user_DisplayParameter("stat_only_mode")
            self.user_DisplayParameter("analysis_only_mode")

    def user_DisplayParameter(self, parameter: str) -> None:
        """Log the value of one setting.

        Args:
            parameter (``str``): name of the setting.
        """
        if parameter == "status":
            self.logger.info(f" recasting mode: {self.status}")
        elif parameter == "delphes":
            self.logger.info(
                f"   * analyses based on delphes    : {('not '*(not self.delphes)) + 'allowed'}"
            )
        elif parameter == "ma5tune":
            self.logger.info(
                f"   * analyses based on the ma5tune: {('not '*(not self.ma5tune)) + 'allowed'}"
            )
        elif parameter == "pad":
            self.logger.info(
                f"   * the PAD is                   : {('not '*(not self.pad)) + 'available'}"
            )
        elif parameter == "padtune":
            self.logger.info(
                f"   * the PADForMa5tune is         : {('not '*(not self.padtune)) + 'available'}"
            )
        elif parameter == "padsfs":
            self.logger.info(
                f"   * the PADForSFS is             : {('not '*(not self.padsfs)) + 'available'}"
            )
        elif parameter == "card_path":
            self.logger.info("   * Path to a recasting card: " + str(self.card_path))
        elif parameter in ["store_root", "store_events"]:
            self.logger.info(
                "   * Keeping the event files: "
                + str(self.store_root or self.store_events)
            )
        elif parameter == "TACO_output":
            self.logger.info(
                "   * Running in TACO mode and storing the results at "
                + str(self.TACO_output)
            )
        elif parameter == "systematics":
            if len(self.systematics) > 0:
                for idx, syst in enumerate(self.systematics):
                    up, dn = syst
                    self.logger.info(f"   * Systematics {idx}: [+{up:.1%}, -{dn:.1%}]")
        elif parameter == "extrapolated_luminosity":
            if len(self.extrapolated_luminosities) > 0:
                tmp = [
                    "{:.1f}".format(x) + " fb^{-1}"
                    for x in self.extrapolated_luminosities
                ]
                self.logger.info(
                    "   * Results extrapolated for the luminosities: " + ", ".join(tmp)
                )
        elif parameter == "THerror_combination":
            self.logger.info(
                "   * Theory errors (if provided) are combined in a "
                + self.THerror_combination
                + " way"
            )
        elif parameter == "error_extrapolation":
            if isinstance(self.error_extrapolation, str):
                self.logger.info(
                    "   * Errors on the background extrapolated "
                    + self.error_extrapolation
                    + "ly (if necessary)"
                )
            else:
                if self.error_extrapolation[1] == 0:
                    self.logger.info(
                        "   * Relative error on the extrapolated background taken as"
                        + " {:.1%}".format(self.error_extrapolation[0])
                    )
                else:
                    self.logger.info(
                        "   * Relative error on the extrapolated background Nb taken as"
                        + " sqrt({:.2f}^2 + ({:.2f}/Nb)^2)".format(
                            self.error_extrapolation[0], self.error_extrapolation[1]
                        )
                    )
        elif parameter == "global_likelihoods":
            self.logger.info(
                "   * Global-Likelihoods will"
                + (not self.global_likelihoods_switch) * " not"
                + " be calculated"
                + (self.global_likelihoods_switch) * ", if available"
                + "."
            )
        elif parameter == "simplify_likelihoods":
            if self.simplify_likelihoods:
                self.logger.info(
                    "   * Simplified profile likelihoods will be used when available."
                )
        #         elif parameter == "stat_only_mode":
        #             if self.stat_only_mode:
        #                 self.logger.info(
        #                     "   * Test statistics will be computed for the given analysis."
        #                 )
        elif parameter == "analysis_only_mode":
            if self.analysis_only_mode:
                self.logger.info(
                    "   * MadAnalysis 5 will only compute the various signal region efficiencies (no statistical treatment)."
                )

        return

    def user_SetParameter(
        self, parameters: str | list[str], values: str | list[str], level: int, archi_info: Any, session_info: Any, datasets: Any
    ) -> None:
        """Set a recasting setting (``set main.recast[.<parameter>] = <value>``).

        Switching the recasting on requires the reco mode and Spey, and at least one of
        the PAD/Delphes, PADForMA5tune/Delphes-MA5tune or PADForSFS combinations.
        Switching it off is refused if hadron-level (HEP/HepMC) datasets are imported.
        The ``add`` keyword appends systematic uncertainties
        (``set main.recast.add.systematics = up [down]``) or extrapolated luminosities
        (``set main.recast.add.extrapolated_luminosity = L1 [L2 ...]``).

        Args:
            parameters (``str | list[str]``): name of the setting, or the list of dotted
                name components after ``main.recast`` (to support ``add``).
            values (``str | list[str]``): value, or list of value tokens.
            level (``int``): running mode.
            archi_info (``ArchitectureInfo``): detected configuration.
            session_info (``SessionInfo``): session information (PAD availability).
            datasets (``DatasetCollection``): datasets of the session.
        """
        # Make sure that previous features are unchanged:  the 'add' keyword is properly dealt with
        if isinstance(parameters, list):
            parameter = parameters[0]
            value = values[0]
        else:
            parameter = parameters
            value = values

        if parameter != "status" and self.status != "on":
            self.logger.error("Please first set the recasting mode to 'on'.")
            return

        # algorithm
        if parameter == "status":
            # Switch on the clustering
            if value == "on":

                # Only in reco mode
                if level != MA5RunningType.RECO:
                    self.logger.error("recasting is only available in the RECO mode")
                    return

                # # Only if ROOT is install
                # if not archi_info.has_root:
                #     self.logger.error("recasting is only available if ROOT is installed")
                #     return

                canrecast = False
                # Delphes and the PAD?
                if archi_info.has_root and archi_info.has_delphes:
                    self.delphes = True
                if archi_info.has_root and session_info.has_pad:
                    self.pad = True
                if (
                    not archi_info.has_delphes
                    or not session_info.has_pad
                    or not session_info.has_spey
                ):
                    self.logger.warning(
                        "Delphes and/or the PAD are not installed (or deactivated): "
                        + "the corresponding analyses will be unavailable"
                    )
                else:
                    canrecast = True

                if not session_info.has_spey:
                    self.logger.warning("Recast module requires Spey package.")
                    self.logger.warning(
                        "Installation instructions can be found at https://spey.readthedocs.io/"
                    )
                    canrecast = False

                # DelphesMA5tune and the PADFor MA5TUne?
                if archi_info.has_root and archi_info.has_delphesMA5tune:
                    self.ma5tune = True
                if archi_info.has_root and session_info.has_padma5:
                    self.padtune = True
                if (
                    not archi_info.has_delphesMA5tune
                    or not session_info.has_padma5
                    or not session_info.has_spey
                ):
                    self.logger.warning(
                        "DelphesMA5tune and/or the PADForMA5tune are not installed "
                        + "(or deactivated): the corresponding analyses will be unavailable"
                    )
                else:
                    canrecast = True

                # PADForSFS?
                if session_info.has_padsfs:
                    self.padsfs = True
                if not self.padsfs:
                    self.logger.warning(
                        "PAD for Simplified-FastSim is not installed: "
                        + "the corresponding analyses will be unavailable"
                    )
                else:
                    canrecast = True

                # can we use the recasting mode
                if canrecast:
                    self.status = "on"
                else:
                    self.logger.error(
                        "The recasting modules (PAD/Delphes, PADForMA5tune/DelphesMa5tune, Spey) "
                        + "are not available. The recasting mode cannot be activated"
                    )
                    return

            elif value == "off":
                test = True
                for dataset in datasets:
                    if not test:
                        break
                    for file in dataset.filenames:
                        if (
                            file.endswith("hep")
                            or file.endswith("hep.gz")
                            or file.endswith("hepmc")
                            or file.endswith("hepmc.gz")
                        ):
                            test = False
                            break
                if not test:
                    self.logger.error(
                        "some datasets have a hadronic file format. "
                        + "The recasting mode cannot be switched off."
                    )
                    return
                self.status = "off"
            else:
                self.logger.error("Recasting can only be set to 'on' or 'off'.")

        # path to a recasting card
        elif parameter == "card_path":
            if os.path.isfile(value):
                self.card_path = value
            else:
                self.logger.error("Invalid path to a recasting card.")
                return

        # Keeping the root files
        elif parameter == "store_root" or parameter == "store_events":
            if value == "True":
                self.store_root = True
                self.store_events = True
            elif value == "False":
                self.store_root = False
                self.store_events = False
            else:
                self.logger.error("Do the root files need to be stored? (True/False)")
                return

        # Running in TACO mode
        elif parameter == "TACO_output":
            self.TACO_output = value

        # Systematic uncertainties and Luminosity extrapolation
        elif parameter == "add":
            ## Checking the values
            try:
                vals = [float(x) for x in values if x != ","]
            except:
                self.logger.error(
                    "Values for the systematic uncertainties and extrapolated luminosities should be real"
                )
                return
            ## Systematics
            if len(parameters) > 1 and parameters[1] == "systematics":
                if len(vals) == 1 and vals[0] >= 0.0 and vals[0] <= 1.0:
                    self.systematics.append((vals[0], vals[0]))
                elif (
                    len(vals) == 2
                    and vals[0] >= 0.0
                    and vals[0] <= 1.0
                    and vals[1] >= 0.0
                    and vals[1] <= 1.0
                ):
                    self.systematics.append((vals[0], vals[1]))
                else:
                    self.logger.error(
                        "Invalid syntax for adding systematics uncertainties."
                    )
                    return
            ## Extrapolated lumis
            elif len(parameters) > 1 and parameters[1] == "extrapolated_luminosity":
                if len(vals) >= 1:
                    self.extrapolated_luminosities += vals
                else:
                    self.logger.error(
                        "Invalid syntax for adding extrapolated luminosities."
                    )
                    return
            ## protection
            else:
                self.logger.error("Invalid syntax with the 'add' keyword")
                return

        # Error combination
        elif parameter == "THerror_combination":
            if value in ["quadratic", "linear"]:
                self.THerror_combination = value
            else:
                self.logger.error("Theoretical uncertainties can only be combined")
                self.logger.error("linearly [linear] or quadratically [quadratic].")
                return

        # Error extrapolation
        elif parameter == "error_extrapolation":

            def error_message() -> None:
                """Log the allowed syntaxes of ``error_extrapolation``."""
                self.logger.error(
                    "When extrapolating to different luminosities, uncertainties"
                )
                self.logger.error(
                    "can only be extrapolated linearly [linear], sqrtly [sqrt], "
                )
                self.logger.error("overwriten by a single user-defined value (systs)")
                self.logger.error(
                    "or taken as two comma-separated user-defined values (systs, stats)"
                )

            if value in ["linear", "sqrt"]:
                self.error_extrapolation = value
            else:
                all_values = [x for x in values if x != ","]
                if len(all_values) > 2:
                    error_message()
                    return
                try:
                    if len(all_values) == 1:
                        self.error_extrapolation = [float(value), 0]
                    else:
                        self.error_extrapolation = [float(x) for x in all_values]
                except ValueError:
                    error_message()
                    return

        # Switch to turn off the global likelihood calculations
        elif parameter == "global_likelihoods":
            if value.lower() in ["on", "off"]:
                self.global_likelihoods_switch = value.lower() == "on"
            else:
                self.logger.error(
                    "You can only switch the global-likelihood machinery to 'on' or 'off'."
                )
                return

        # Set simplified likelihoods
        elif parameter == "simplify_likelihoods":
            if value.lower() in ["true", "false"]:
                self.simplify_likelihoods = value.lower() == "true"
                if self.simplify_likelihoods:
                    self.logger.warning(
                        "Please note that this method is currently under "
                        + "development and relies on third party software."
                    )
            else:
                self.logger.error("Please type either True or False.")
                return

        elif parameter == "stat_only_mode":
            # FIXME: after 'false', execution continues to the directory test below and logs an error.
            if value.lower() == "false":
                self.stat_only_mode = False
            if os.path.isdir(os.path.join(value, "Output/SAF")):
                self.stat_only_dir = value
                self.stat_only_mode = True
            else:
                # FIXME: missing f-prefix: '{value}' is printed literally.
                self.logger.error("{value} is not a valid directory.")
                return
        elif parameter == "analysis_only_mode":
            if value.lower() == "false":
                self.analysis_only_mode = False
            elif value.lower() == "true":
                self.analysis_only_mode = True
            else:
                self.logger.error(
                    "analysis_only_mode can only be set to 'True' or 'False'."
                )
                return

        # other rejection if no algo specified
        else:
            self.logger.error(f"The recast module has no parameter called '{parameter}'")
            return

    def user_GetParameters(self, var: str = "") -> list[str]:
        """Get the names of the settable parameters (tab completion).

        Args:
            var (``str``, default ``""``): ``"add"`` to get the parameters of the ``add``
                keyword.

        Returns:
            ``list[str]``:
            Parameter names (empty if the recasting mode is off).
        """
        if self.status == "on":
            if var == "add":
                table = ["extrapolated_luminosity", "systematics"]
            else:
                table = [
                    "card_path",
                    "store_events",
                    "TACO_output",
                    "add",
                    "THerror_combination",
                    "error_extrapolation",
                    "global_likelihoods",
                    #                    "stat_only_mode",
                    "analysis_only_mode",
                ]  # , "simplify_likelihoods"
        else:
            table = []
        return table

    def user_GetValues(self, variable: str) -> list[str]:
        """Get suggested values of a parameter (tab completion).

        Args:
            variable (``str``): name of the parameter.

        Returns:
            ``list[str]``:
            Suggested values, or an empty list.
        """
        table = []
        if variable in RecastConfiguration.userVariables:
            table.extend(RecastConfiguration.userVariables[variable])
        return table

    def CreateCard(self, dirname: str, write: bool = True) -> bool:
        """Create (or check and copy) the recasting card of a job.

        Without a user card, a card listing all analyses of the installed PADs (all
        switched ``on``) is written; otherwise the user card is checked with
        :meth:`CheckCard` and copied.

        Args:
            dirname (``str``): job directory.
            write (``bool``, default ``True``): whether the generated card is written.

        Returns:
            ``bool``:
            ``True`` on success, ``False`` if the user card is missing or invalid.
        """
        # using an existing card
        if self.card_path == "":
            if self.padtune and self.ma5tune:
                self.CreateMyCard(dirname, "PADForMA5tune", write)
            if self.pad and self.delphes:
                self.CreateMyCard(dirname, "PAD", write)
            if self.padsfs:
                self.CreateMyCard(dirname, "PADForSFS", write)
            return True
        # using and checking an existing card
        else:
            if not os.path.isfile(self.card_path):
                self.logger.error("Invalid path to a recasting card.")
                return False
            if not self.CheckCard(dirname):
                self.logger.error("Invalid recasting card")
                return False
        return True

    def CheckCard(self, dirname: str) -> bool:
        """Validate a user-provided recasting card and copy it into the job directory.

        Each analysis must exist in the relevant PAD (read from ``Build/Main/main.cpp``
        for the PAD and PADForMA5tune, from the analysis files for the PADForSFS) and be
        associated with the detector card defined in ``recast_config.dat``.

        Args:
            dirname (``str``): job directory.

        Returns:
            ``bool``:
            ``True`` if the card is valid and has been copied to
            ``<dirname>/Input/recasting_card.dat``.
        """
        self.logger.info("   Checking the recasting card...")
        ToLoopOver = []
        padlist = []
        tunelist = []
        sfslist = []
        if self.pad:
            padfile = open(
                os.path.normpath(
                    os.path.join(self.ma5dir, "tools/PAD/Build/Main/main.cpp")
                ),
                "r",
            )
            ToLoopOver.append([padfile, padlist])
        if self.padtune:
            tunefile = open(
                os.path.normpath(
                    os.path.join(self.ma5dir, "tools/PADForMA5tune/Build/Main/main.cpp")
                ),
                "r",
            )
            ToLoopOver.append([tunefile, tunelist])
        if self.padsfs:
            # get the analysis list that is available in the folder
            sfs_path = os.path.normpath(
                os.path.join(
                    self.ma5dir, "tools/PADForSFS/Build/SampleAnalyzer/User/Analyzer"
                )
            )
            analysislist = [
                x.split("/")[-1].split(".cpp")[0] for x in glob.glob(sfs_path + "/*.cpp")
            ]
            # should check corresponding headers, keep only the analyses with headers
            headerlist = [
                x.split("/")[-1].split(".h")[0]
                for x in glob.glob(sfs_path + "/*.h")
                if not x.startswith("analysisList")
            ]
            analysislist = [i for i in analysislist if i in headerlist]
            # getting the list of available detector cards
            sfs_path = os.path.normpath(
                os.path.join(self.ma5dir, "tools/PADForSFS/Input/Cards")
            )
            cardlist = [x.split("/")[-1] for x in glob.glob(sfs_path + "/*.ma5")]
            # final list with analyses
            for ma5card, analysis in self.DelphesDic.items():
                for ana in analysis:
                    if ana in analysislist and ma5card in cardlist:
                        sfslist.append([ana, ma5card])
        for myfile, mylist in ToLoopOver:
            for line in myfile:
                if "manager.InitializeAnalyzer" in line:
                    analysis = str(line.split('"')[1])
                    mydelphes = "UNKNOWN"
                    for mycard, alist in self.DelphesDic.items():
                        if analysis in alist:
                            mydelphes = mycard
                            break
                    mylist.append([analysis, mydelphes])
        if self.pad:
            padfile.close()
        if self.padtune:
            tunefile.close()
        usercard = open(self.card_path)
        for line in usercard:
            if len(line.strip()) == 0:
                continue
            if line.lstrip()[0] == "#":
                continue
            myline = line.split()
            myana = myline[0]
            myver = myline[1]
            # NOTE: IndexError for card lines with fewer than 4 fields.
            mydelphes = myline[3]
            # checking the presence of the analysis and the delphes card
            if myver == "v1.2":
                if not myana in [x[0] for x in padlist]:
                    self.logger.error(
                        "Recasting card: invalid analysis (not present in the PAD): "
                        + myana
                    )
                    return False
                if not os.path.isfile(
                    os.path.normpath(
                        os.path.join(self.ma5dir, "tools/PAD/Input/Cards", mydelphes)
                    )
                ):
                    self.logger.error(
                        "Recasting card: PAD analysis linked to an invalid delphes card: "
                        + myana
                        + " - "
                        + mydelphes
                    )
                    return False
            elif myver == "v1.1":
                if not myana in [x[0] for x in tunelist]:
                    self.logger.error(
                        "Recasting card: invalid analysis (not present in the PADForMA5tune): "
                        + myana
                    )
                    return False
                if not os.path.isfile(
                    os.path.normpath(
                        os.path.join(
                            self.ma5dir, "tools/PADForMA5tune/Input/Cards", mydelphes
                        )
                    )
                ):
                    self.logger.error(
                        "Recasting card: PADForMA5tune analysis linked to an invalid delphes card: "
                        + myana
                        + " - "
                        + mydelphes
                    )
                    return False
            elif myver == "vSFS":
                if not myana in [x[0] for x in sfslist]:
                    self.logger.error(
                        "Recasting card: invalid analysis (not present in PADForSFS): "
                        + myana
                    )
                    return False
                if not os.path.isfile(
                    os.path.normpath(
                        os.path.join(
                            self.ma5dir, "tools/PADForSFS/Input/Cards", mydelphes
                        )
                    )
                ):
                    self.logger.error(
                        "Recasting card: PADForSFS analysis linked to an invalid SFS card: "
                        + myana
                        + " - "
                        + mydelphes
                    )
                    return False
            else:
                self.logger.error(
                    "Recasting card: invalid analysis (not present in the PAD, PADForMA5tune and PADForSFS): "
                    + myana
                )
                return False
            # checking the matching between the delphes card and the analysis
            for mycard, alist in self.DelphesDic.items():
                if myana in alist and myver != "vSFS":
                    if mydelphes != mycard:
                        self.logger.error(
                            "Invalid delphes card associated with the analysis: " + myana
                        )
                        return False
                    break
        usercard.close()
        try:
            shutil.copy(self.card_path, dirname + "/Input/recasting_card.dat")
        except:
            self.logger.error(
                "impossible to copy the recasting card to the working directory"
            )
            return False
        return True

    def CreateMyCard(self, dirname: str, padtype: str, write: bool = True) -> list[str] | None:
        """Generate the recasting-card lines of one PAD.

        Args:
            dirname (``str``): job directory.
            padtype (``str``): ``"PAD"``, ``"PADForMA5tune"`` or ``"PADForSFS"``.
            write (``bool``, default ``True``): append the lines to
                ``<dirname>/Input/recasting_card.dat`` (with a header if the file does not
                exist yet) instead of returning them.

        Returns:
            ``list[str] | None``:
            The sorted card lines if ``write`` is ``False``, ``None`` otherwise.
        """
        thecard = []
        if write:
            exist = os.path.isfile(dirname + "/Input/recasting_card.dat")
            if not exist and write:
                thecard.append(
                    "# Detector cards must be located in the PAD(ForMA5tune/ForSFS) directory"
                )
                thecard.append("# Switches must be on or off")
                thecard.append(
                    "# AnalysisName               PADType    Switch     DetectorCard"
                )
        if padtype in ["PAD", "PADForMA5tune"]:
            mainfile = open(
                os.path.normpath(
                    os.path.join(self.ma5dir, "tools", padtype, "Build/Main/main.cpp")
                ),
                "r",
            )
            if padtype == "PAD":
                mytype = "v1.2"
            else:
                mytype = "v1.1"
            for line in mainfile:
                if "manager.InitializeAnalyzer" in line:
                    analysis = str(line.split('"')[1])
                    mydelphes = "UNKNOWN"
                    descr = "UNKNOWN"
                    for mycard, alist in self.DelphesDic.items():
                        if analysis in alist:
                            mydelphes = mycard
                            break
                    for myana, mydesc in self.description.items():
                        if analysis == myana:
                            descr = mydesc
                            break
                    thecard.append(
                        analysis.ljust(30, " ")
                        + mytype.ljust(12, " ")
                        + "on    "
                        + mydelphes.ljust(50, " ")
                        + " # "
                        + descr
                    )
            mainfile.close()
        elif padtype == "PADForSFS":
            sfs_path = os.path.normpath(
                os.path.join(
                    self.ma5dir, "tools/PADForSFS/Build/SampleAnalyzer/User/Analyzer"
                )
            )
            analysislist = [
                x.split("/")[-1].split(".cpp")[0] for x in glob.glob(sfs_path + "/*.cpp")
            ]
            for mycard, alist in self.DelphesDic.items():
                # it the analysis name is the same skip the one which has delphes card
                if mycard.endswith("tcl"):
                    continue
                for analysis in alist:
                    if analysis not in analysislist:
                        continue
                    descr = "UNKNOWN"
                    if analysis in list(self.description.keys()):
                        descr = self.description[analysis]
                    thecard.append(
                        analysis.ljust(30, " ")
                        + "vSFS        on    "
                        + mycard.ljust(50, " ")
                        + " # "
                        + descr
                    )
        thecard.sort()
        if write:
            card = open(dirname + "/Input/recasting_card.dat", "a")
            # NOTE: the last line has no newline, so '#' is appended to it (inside its comment).
            card.write("\n".join(thecard))
            card.write("#\n")
            card.close()
        else:
            return thecard

    def CheckFile(self, dirname: str, dataset: Any) -> bool:
        """Check that the CLs output of a dataset has been produced.

        Args:
            dirname (``str``): job directory (replaced by :attr:`stat_only_dir` in
                stat-only mode).
            dataset (``Dataset``): the dataset.

        Returns:
            ``bool``:
            ``True`` if ``Output/SAF/<dataset>/CLs_output.dat`` exists.
        """
        dirname = self.stat_only_dir if self.stat_only_mode else dirname
        filename = os.path.normpath(
            dirname + "/Output/SAF/" + dataset.name + "/CLs_output.dat"
        )
        self.logger.debug('Check file "' + filename + '"...')
        if not os.path.isfile(filename):
            self.logger.error(
                "The file '"
                + dirname
                + "/Output/SAF/"
                + dataset.name
                + '/CLs_output.dat" has not been found.'
            )
            return False
        return True

    def collect_outputs(self, dirname: str, datasets: Any) -> None:
        """Merge the CLs outputs of all datasets into ``Output/SAF/CLs_output_summary.dat``.

        Args:
            dirname (``str``): job directory (replaced by :attr:`stat_only_dir` in
                stat-only mode).
            datasets (``DatasetCollection``): datasets of the session.
        """
        dirname = self.stat_only_dir if self.stat_only_mode else dirname
        filename = os.path.normpath(
            os.path.join(dirname, "Output/SAF/CLs_output_summary.dat")
        )
        self.logger.debug('Check summary file "' + filename + '"...')
        with open(filename, "w", encoding="utf-8") as out:
            counter = 1
            for item in datasets:
                cls_pth = os.path.normpath(
                    os.path.join(dirname, "Output", "SAF", item.name, "CLs_output.dat")
                )
                if os.path.isfile(cls_pth):
                    with open(cls_pth, "r", encoding="utf-8") as outset:
                        for line in outset.readlines():
                            if counter == 1 and "# analysis name" in line:
                                out.write("# dataset name".ljust(30) + line[2:])
                                counter += 1
                            if len(line.lstrip()) == 0:
                                continue
                            if line.lstrip()[0] == "#":
                                continue
                            out.write(item.name.ljust(30) + line)
                        out.write("\n")
