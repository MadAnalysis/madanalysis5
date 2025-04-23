################################################################################
#
#  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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

import copy
import json
import logging
import math
import os
import shutil
import sys
import time

import numpy as np
from shell_command import ShellCommand
from six.moves import input, range
from string_tools import StringTools

from madanalysis.configuration.delphes_configuration import DelphesConfiguration
from madanalysis.configuration.delphesMA5tune_configuration import (
    DelphesMA5tuneConfiguration,
)
from madanalysis.install.detector_manager import DetectorManager
from madanalysis.IOinterface.folder_writer import FolderWriter
from madanalysis.IOinterface.job_writer import JobWriter
from madanalysis.IOinterface.library_writer import LibraryWriter
from madanalysis.misc.histfactory_reader import (
    HF_Background,
    HF_Signal,
    construct_histfactory_dictionary,
)
from madanalysis.misc.theoretical_error_setup import error_dict_setup

# pylint: disable=logging-fstring-interpolation,import-outside-toplevel


class RunRecast:
    def __init__(self, main, dirname):
        self.dirname = dirname
        self.main = main
        self.delphes_runcard = []
        self.analysis_runcard = []
        self.forced = self.main.forced
        self.detector = ""
        self.pad = ""
        self.first11 = True
        self.first12 = True
        self.pyhf_config = {}  # initialize and configure histfactory
        self.cov_config = {}
        self.logger = logging.getLogger("MA5")
        self.TACO_output = self.main.recasting.TACO_output

    def init(self):
        ### First, the analyses to take care off
        logging.getLogger("MA5").debug(
            "  Inviting the user to edit the recasting card..."
        )
        self.edit_recasting_card()
        ### Getting the list of analyses to recast
        self.logger.info("   Getting the list of delphes simulation to be performed...")
        self.get_runs()
        ### Check if we have anything to do
        if len(self.delphes_runcard) == 0:
            self.logger.warning("No recasting to do... Please check the recasting card")
            return False

        ### Exit
        return True

    ################################################
    ### GENERAL METHODS
    ################################################

    ## Running the machinery
    def execute(self):
        self.main.forced = True
        for delphescard in list(set(sorted(self.delphes_runcard))):
            ## Extracting run infos and checks
            version = delphescard[:4]
            card = delphescard[5:]
            if not self.check_run(version):
                self.main.forced = self.forced
                return False

            ## Running the fastsim
            if not self.fastsim_single(version, card):
                self.main.forced = self.forced
                return False
            self.main.fastsim.package = self.detector

            ## Running the analyses
            if not self.analysis_single(version, card):
                self.main.forced = self.forced
                return False

            ## Cleaning
            if not FolderWriter.RemoveDirectory(
                os.path.normpath(self.dirname + "_RecastRun")
            ):
                return False

        # exit
        self.main.forced = self.forced
        return True

    ## Prompt to edit the recasting card
    def edit_recasting_card(self):
        if self.forced or self.main.script:
            return
        self.logger.info("Would you like to edit the recasting Card ? (Y/N)")
        allowed_answers = ["n", "no", "y", "yes"]
        answer = ""
        while answer not in allowed_answers:
            answer = input("Answer: ")
            answer = answer.lower()
        if answer in ["no", "n"]:
            return
        else:
            err = os.system(
                self.main.session_info.editor
                + " "
                + self.dirname
                + "/Input/recasting_card.dat"
            )

        return

    ## Checking the recasting card to get the analysis to run
    def get_runs(self):
        del_runs = []
        ana_runs = []
        ## decoding the card
        runcard = open(self.dirname + "/Input/recasting_card.dat", "r")
        for line in runcard:
            if len(line.strip()) == 0 or line.strip().startswith("#"):
                continue
            myline = line.split()
            if myline[2].lower() == "on" and myline[3] not in del_runs:
                del_runs.append(myline[1] + "_" + myline[3])
            if myline[2].lower() == "on":
                ana_runs.append(myline[1] + "_" + myline[0])
        ## saving the information and exti
        self.delphes_runcard = del_runs
        self.analysis_runcard = ana_runs
        return

    def check_run(self, version):
        ## setup
        check = False
        if version == "v1.1":
            self.detector = "delphesMA5tune"
            self.pad = self.main.archi_info.ma5dir + "/tools/PADForMA5tune"
            check = self.main.recasting.ma5tune
        elif version == "v1.2":
            self.detector = "delphes"
            self.pad = self.main.archi_info.ma5dir + "/tools/PAD"
            check = self.main.recasting.delphes
        elif version == "vSFS":
            self.detector = "fastjet"
            self.pad = self.main.archi_info.ma5dir + "/tools/PADForSFS"
            check = True
        ## Check and exit
        if not check:
            self.logger.error(
                "The %s library is not present -> the associated analyses cannot be used",
                self.detector,
            )
            return False
        return True

    ################################################
    ### DELPHES RUN
    ################################################
    def fastsim_single(self, version, delphescard):
        self.logger.debug(
            "Launch a bunch of fastsim with the delphes card: %s", delphescard
        )

        # Init and header
        self.fastsim_header(version)

        # Activating the right delphes
        if self.detector != "fastjet":
            self.logger.debug("Activating the detector (switch delphes/delphesMA5tune)")
            self.main.fastsim.package = self.detector
            detector_handler = DetectorManager(self.main)
            if not detector_handler.manage(self.detector):
                self.logger.error("Problem with the activation of delphesMA5tune")
                return False

        # Checking whether events have already been generated and if not, event generation
        self.logger.debug("Loop over the datasets...")
        evtfile = None
        for item in self.main.datasets:
            if self.detector == "delphesMA5tune":
                evtfile = (
                    self.dirname
                    + "/Output/SAF/"
                    + item.name
                    + "/RecoEvents/RecoEvents_v1x1_"
                    + delphescard.replace(".tcl", "")
                    + ".root"
                )
            elif self.detector == "delphes":
                evtfile = (
                    self.dirname
                    + "/Output/SAF/"
                    + item.name
                    + "/RecoEvents/RecoEvents_v1x2_"
                    + delphescard.replace(".tcl", "")
                    + ".root"
                )
            elif self.detector == "fastjet":
                return True

            self.logger.debug("- applying fastsim and producing %s ...", evtfile)
            if not os.path.isfile(os.path.normpath(evtfile)):
                if not self.generate_events(item, delphescard):
                    return False

        # Exit
        return True

    def fastsim_header(self, version):
        ## Gettign the version dependent stuff
        to_print = False
        tag = None
        if version == "v1.1" and self.first11:
            to_print = True
            tag = version
            self.first11 = False
        elif version != "v1.1" and self.first12:
            to_print = True
            tag = "v1.2+"
            self.first12 = False
        ## Printing
        if to_print:
            self.logger.info(
                "   **********************************************************"
            )
            self.logger.info(
                "   %s", StringTools.Center(f"{tag} detector simulations", 57)
            )
            self.logger.info(
                "   **********************************************************"
            )

    def run_delphes(self, dataset, card):
        # Initializing the JobWriter
        if os.path.isdir(self.dirname + "_RecastRun"):
            if not FolderWriter.RemoveDirectory(
                os.path.normpath(self.dirname + "_RecastRun")
            ):
                return False
        jobber = JobWriter(self.main, self.dirname + "_RecastRun")

        # Writing process
        self.logger.info(
            "   Creating folder '" + self.dirname.split("/")[-1] + "_RecastRun'..."
        )
        if not jobber.Open():
            return False
        self.logger.info("   Copying 'SampleAnalyzer' source files...")
        if not jobber.CopyLHEAnalysis():
            return False
        if not jobber.CreateBldDir():
            return False
        self.logger.info("   Inserting your selection into 'SampleAnalyzer'...")
        if not jobber.WriteSelectionHeader(self.main):
            return False
        if not jobber.WriteSelectionSource(self.main):
            return False
        self.logger.info("   Writing the list of datasets...")
        jobber.WriteDatasetList(dataset)
        self.logger.info("   Creating Makefiles...")
        if not jobber.WriteMakefiles():
            return False
        self.logger.debug("   Fixing the pileup path...")
        self.fix_pileup(self.dirname + "_RecastRun/Input/" + card)

        # Creating executable
        self.logger.info("   Compiling 'SampleAnalyzer'...")
        if not jobber.CompileJob():
            return False
        self.logger.info("   Linking 'SampleAnalyzer'...")
        if not jobber.LinkJob():
            return False

        # Running
        self.logger.info(
            "   Running 'SampleAnalyzer' over dataset '" + dataset.name + "'..."
        )
        self.logger.info("    *******************************************************")
        if not jobber.RunJob(dataset):
            self.logger.error("run over '" + dataset.name + "' aborted.")
        self.logger.info("    *******************************************************")

        # Exit
        return True

    def run_SimplifiedFastSim(self, dataset, card, analysislist):
        """

        Parameters
        ----------
        dataset : MA5 Dataset
            one of the datasets from self.main.dataset
        card : SFS Run Card
            SFS description for the detector simulation
        analysislist : LIST of STR
            list of analysis names

        Returns
        -------
        bool
            SFS run correctly (True), there was a mistake (False)

        """
        if any(
            any(x.endswith(y) for y in ["root", "lhco", "lhco.gz"])
            for x in dataset.filenames
        ):
            self.logger.error("   Dataset can not contain reconstructed file type.")
            return False
        # Load the analysis card
        from madanalysis.core.script_stack import ScriptStack

        ScriptStack.AddScript(card)
        self.main.recasting.status = "off"
        self.main.superfastsim.Reset()
        script_mode = self.main.script
        self.main.script = True
        from madanalysis.interpreter.interpreter import Interpreter

        interpreter = Interpreter(self.main)
        interpreter.load(verbose=self.main.developer_mode)
        self.main.script = script_mode
        old_fastsim = self.main.fastsim.package
        self.main.fastsim.package = "fastjet"
        if self.main.recasting.store_events:
            output_name = "SFS_events.lhe"
            if self.main.archi_info.has_zlib:
                output_name += ".gz"
            self.logger.debug("   Setting the output LHE file :" + output_name)

        # Initializing the JobWriter
        jobber = JobWriter(self.main, self.dirname + "_SFSRun")

        # Writing process
        self.logger.info("   Creating folder '" + self.dirname.split("/")[-1] + "'...")
        if not jobber.Open():
            return False
        self.logger.info("   Copying 'SampleAnalyzer' source files...")
        if not jobber.CopyLHEAnalysis():
            return False
        if not jobber.CreateBldDir(analysisName="SFSRun", outputName="SFSRun.saf"):
            return False
        if not jobber.WriteSelectionHeader(self.main):
            return False
        os.remove(self.dirname + "_SFSRun/Build/SampleAnalyzer/User/Analyzer/user.h")
        if not jobber.WriteSelectionSource(self.main):
            return False
        os.remove(self.dirname + "_SFSRun/Build/SampleAnalyzer/User/Analyzer/user.cpp")
        #######
        self.logger.info("   Writing the list of datasets...")
        jobber.WriteDatasetList(dataset)
        self.logger.info("   Creating Makefiles...")
        if not jobber.WriteMakefiles():
            return False
        # Copying the analysis files
        analysisList = open(
            self.dirname + "_SFSRun/Build/SampleAnalyzer/User/Analyzer/analysisList.h",
            "w",
        )
        for ana in analysislist:
            analysisList.write('#include "SampleAnalyzer/User/Analyzer/' + ana + '.h"\n')
        analysisList.write(
            '#include "SampleAnalyzer/Process/Analyzer/AnalyzerManager.h"\n'
        )
        analysisList.write('#include "SampleAnalyzer/Commons/Service/LogStream.h"\n\n')
        analysisList.write(
            "// -----------------------------------------------------------------------------\n"
        )
        analysisList.write("// BuildUserTable\n")
        analysisList.write(
            "// -----------------------------------------------------------------------------\n"
        )
        analysisList.write("void BuildUserTable(MA5::AnalyzerManager& manager)\n")
        analysisList.write("{\n")
        analysisList.write("  using namespace MA5;\n")
        try:
            for ana in analysislist:
                shutil.copyfile(
                    self.pad + "/Build/SampleAnalyzer/User/Analyzer/" + ana + ".cpp",
                    self.dirname
                    + "_SFSRun/Build/SampleAnalyzer/User/Analyzer/"
                    + ana
                    + ".cpp",
                )
                shutil.copyfile(
                    self.pad + "/Build/SampleAnalyzer/User/Analyzer/" + ana + ".h",
                    self.dirname
                    + "_SFSRun/Build/SampleAnalyzer/User/Analyzer/"
                    + ana
                    + ".h",
                )
                analysisList.write('  manager.Add("' + ana + '", new ' + ana + ");\n")
        except Exception as err:
            self.logger.debug(str(err))
            self.logger.error("Cannot copy the analysis: " + ana)
            self.logger.error(
                "Please make sure that corresponding analysis downloaded propoerly."
            )
            return False
        analysisList.write("}\n")
        analysisList.close()

        # Update Main
        self.logger.info("   Updating the main executable")
        shutil.move(
            self.dirname + "_SFSRun/Build/Main/main.cpp",
            self.dirname + "_SFSRun/Build/Main/main.bak",
        )
        mainfile = open(self.dirname + "_SFSRun/Build/Main/main.bak", "r")
        newfile = open(self.dirname + "_SFSRun/Build/Main/main.cpp", "w")
        ignore = False
        for line in mainfile:
            if "// Getting pointer to the analyzer" in line:
                ignore = True
                newfile.write(line)
                for analysis in analysislist:
                    newfile.write(
                        "  std::map<std::string, std::string> prm" + analysis + ";\n"
                    )
                    newfile.write("  AnalyzerBase* analyzer_" + analysis + "=\n")
                    newfile.write(
                        '    manager.InitializeAnalyzer("'
                        + analysis
                        + '","'
                        + analysis
                        + '.saf",'
                        + "prm"
                        + analysis
                        + ");\n"
                    )
                    newfile.write("  if (analyzer_" + analysis + "==0) return 1;\n\n")
                if self.main.recasting.store_events:
                    newfile.write("  //Getting pointer to the writer\n")
                    newfile.write("  WriterBase* writer1 = \n")
                    newfile.write(
                        '      manager.InitializeWriter("lhe","' + output_name + '");\n'
                    )
                    newfile.write("  if (writer1==0) return 1;\n\n")
            elif (
                "// Post initialization (creates the new output directory structure)"
                in line
                and self.TACO_output != ""
            ):
                newfile.write(
                    '    std::ofstream out;\n      out.open("../Output/'
                    + self.TACO_output
                    + '");\n'
                )
                newfile.write("\n      manager.HeadSR(out);\n      out << std::endl;\n")
            elif "//Getting pointer to the clusterer" in line:
                ignore = False
                newfile.write(line)
            elif "!analyzer1" in line and not ignore:
                ignore = True
                if self.main.recasting.store_events:
                    newfile.write("      writer1->WriteEvent(myEvent,mySample);\n")
                for analysis in analysislist:
                    newfile.write(
                        "      if (!analyzer_"
                        + analysis
                        + "->Execute(mySample,myEvent)) continue;\n"
                    )
                if self.TACO_output != "":
                    newfile.write("\n      manager.DumpSR(out);\n")
            elif "    }" in line:
                newfile.write(line)
                ignore = False
            elif (
                "manager.Finalize(mySamples,myEvent);" in line and self.TACO_output != ""
            ):
                newfile.write(line)
                newfile.write("  out.close();\n")
            elif not ignore:
                newfile.write(line)
        mainfile.close()
        newfile.close()
        # restore
        self.main.recasting.status = "on"
        self.main.fastsim.package = old_fastsim
        # Creating executable
        self.logger.info("   Compiling 'SampleAnalyzer'...")
        if not jobber.CompileJob():
            self.logger.error("job submission aborted.")
            return False
        self.logger.info("   Linking 'SampleAnalyzer'...")
        if not jobber.LinkJob():
            self.logger.error("job submission aborted.")
            return False
        # Running
        self.logger.info(
            "   Running 'SampleAnalyzer' over dataset '" + dataset.name + "'..."
        )
        self.logger.info("    *******************************************************")
        if not jobber.RunJob(dataset):
            self.logger.error("run over '" + dataset.name + "' aborted.")
            return False
        self.logger.info("    *******************************************************")

        if not os.path.isdir(self.dirname + "/Output/SAF/" + dataset.name):
            os.mkdir(self.dirname + "/Output/SAF/" + dataset.name)
        for analysis in analysislist:
            if not os.path.isdir(
                self.dirname + "/Output/SAF/" + dataset.name + "/" + analysis
            ):
                os.mkdir(self.dirname + "/Output/SAF/" + dataset.name + "/" + analysis)
            if not os.path.isdir(
                self.dirname
                + "/Output/SAF/"
                + dataset.name
                + "/"
                + analysis
                + "/CutFlows"
            ):
                os.mkdir(
                    self.dirname
                    + "/Output/SAF/"
                    + dataset.name
                    + "/"
                    + analysis
                    + "/Cutflows"
                )
            if not os.path.isdir(
                self.dirname
                + "/Output/SAF/"
                + dataset.name
                + "/"
                + analysis
                + "/Histograms"
            ):
                os.mkdir(
                    self.dirname
                    + "/Output/SAF/"
                    + dataset.name
                    + "/"
                    + analysis
                    + "/Histograms"
                )
            if (
                not os.path.isdir(
                    self.dirname
                    + "/Output/SAF/"
                    + dataset.name
                    + "/"
                    + analysis
                    + "/RecoEvents"
                )
                and self.main.recasting.store_events
            ):
                os.mkdir(
                    self.dirname
                    + "/Output/SAF/"
                    + dataset.name
                    + "/"
                    + analysis
                    + "/RecoEvents"
                )
            cutflow_list = os.listdir(
                self.dirname
                + "_SFSRun/Output/SAF/_"
                + dataset.name
                + "/"
                + analysis
                + "_0/Cutflows"
            )
            histogram_list = os.listdir(
                self.dirname
                + "_SFSRun/Output/SAF/_"
                + dataset.name
                + "/"
                + analysis
                + "_0/Histograms"
            )
            # Copy dataset info file
            if os.path.isfile(
                self.dirname
                + "_SFSRun/Output/SAF/_"
                + dataset.name
                + "/_"
                + dataset.name
                + ".saf"
            ):
                shutil.move(
                    self.dirname
                    + "_SFSRun/Output/SAF/_"
                    + dataset.name
                    + "/_"
                    + dataset.name
                    + ".saf",
                    self.dirname
                    + "/Output/SAF/"
                    + dataset.name
                    + "/"
                    + dataset.name
                    + ".saf",
                )
            for cutflow in cutflow_list:
                shutil.move(
                    self.dirname
                    + "_SFSRun/Output/SAF/_"
                    + dataset.name
                    + "/"
                    + analysis
                    + "_0/Cutflows/"
                    + cutflow,
                    self.dirname
                    + "/Output/SAF/"
                    + dataset.name
                    + "/"
                    + analysis
                    + "/Cutflows/"
                    + cutflow,
                )
            for histos in histogram_list:
                shutil.move(
                    self.dirname
                    + "_SFSRun/Output/SAF/_"
                    + dataset.name
                    + "/"
                    + analysis
                    + "_0/Histograms/"
                    + histos,
                    self.dirname
                    + "/Output/SAF/"
                    + dataset.name
                    + "/"
                    + analysis
                    + "/Histograms/"
                    + histos,
                )
            if self.main.recasting.store_events:
                event_list = os.listdir(
                    self.dirname
                    + "_SFSRun/Output/SAF/_"
                    + dataset.name
                    + "/lheEvents0_0/"
                )
                if len(event_list) > 0:
                    shutil.move(
                        self.dirname
                        + "_SFSRun/Output/SAF/_"
                        + dataset.name
                        + "/lheEvents0_0/"
                        + event_list[0],
                        self.dirname
                        + "/Output/SAF/"
                        + dataset.name
                        + "/"
                        + analysis
                        + "/RecoEvents/"
                        + event_list[0],
                    )
            if self.TACO_output != "":
                filename = (
                    ".".join(self.TACO_output.split(".")[:-1])
                    + "_"
                    + card.split("/")[-1].replace("ma5", "")
                    + self.TACO_output.split(".")[-1]
                )
                shutil.move(
                    self.dirname + "_SFSRun/Output/" + self.TACO_output,
                    self.dirname + "/Output/SAF/" + dataset.name + "/" + filename,
                )

        if not self.main.developer_mode:
            # Remove the analysis folder
            if not FolderWriter.RemoveDirectory(
                os.path.normpath(self.dirname + "_SFSRun")
            ):
                self.logger.error("Cannot remove directory: " + self.dirname + "_SFSRun")
        else:
            self.logger.debug("Analysis kept in " + self.dirname + "_SFSRun folder.")

        return True

    def generate_events(self, dataset, card):
        # Preparing the run
        self.main.recasting.status = "off"
        self.main.fastsim.package = self.detector
        self.main.fastsim.clustering = 0
        if self.detector == "delphesMA5tune":
            self.main.fastsim.delphes = 0
            self.main.fastsim.delphesMA5tune = DelphesMA5tuneConfiguration()
            self.main.fastsim.delphesMA5tune.card = os.path.normpath(
                "../../../../tools/PADForMA5tune/Input/Cards/" + card
            )
        elif self.detector == "delphes":
            self.main.fastsim.delphesMA5tune = 0
            self.main.fastsim.delphes = DelphesConfiguration()
            self.main.fastsim.delphes.card = os.path.normpath(
                "../../../../tools/PAD/Input/Cards/" + card
            )
        # Execution
        if not self.run_delphes(dataset, card):
            self.logger.error(
                "The " + self.detector + " problem with the running of the fastsim"
            )
            return False
        # Restoring the run
        self.main.recasting.status = "on"
        self.main.fastsim.package = "none"
        ## Saving the output
        if not os.path.isdir(self.dirname + "/Output/SAF/" + dataset.name):
            os.mkdir(self.dirname + "/Output/SAF/" + dataset.name)
        if not os.path.isdir(
            self.dirname + "/Output/SAF/" + dataset.name + "/RecoEvents"
        ):
            os.mkdir(self.dirname + "/Output/SAF/" + dataset.name + "/RecoEvents")
        if self.detector == "delphesMA5tune":
            shutil.move(
                self.dirname
                + "_RecastRun/Output/SAF/_"
                + dataset.name
                + "/RecoEvents0_0/DelphesMA5tuneEvents.root",
                self.dirname
                + "/Output/SAF/"
                + dataset.name
                + "/RecoEvents/RecoEvents_v1x1_"
                + card.replace(".tcl", "")
                + ".root",
            )
        elif self.detector == "delphes":
            shutil.move(
                self.dirname
                + "_RecastRun/Output/SAF/_"
                + dataset.name
                + "/RecoEvents0_0/DelphesEvents.root",
                self.dirname
                + "/Output/SAF/"
                + dataset.name
                + "/RecoEvents/RecoEvents_v1x2_"
                + card.replace(".tcl", "")
                + ".root",
            )
        ## Exit
        return True

    ################################################
    ### ANALYSIS EXECUTION
    ################################################
    def analysis_single(self, version, card):
        ## Init and header
        self.analysis_header(version, card)

        # Activating the right delphes
        detector_handler = DetectorManager(self.main)
        if not detector_handler.manage(self.detector):
            self.logger.error("Problem with the activation of delphesMA5tune")
            return False

        ## Getting the analyses associated with the given card
        analyses = [
            x.replace(version + "_", "") for x in self.analysis_runcard if version in x
        ]
        for del_card, ana_list in self.main.recasting.DelphesDic.items():
            if card == del_card:
                analyses = [x for x in analyses if x in ana_list]
                break

        # Executing the PAD
        for myset in self.main.datasets:
            if not self.main.recasting.stat_only_mode:
                if version in ["v1.1", "v1.2"]:
                    ## Preparing the PAD
                    self.update_pad_main(analyses)
                    if not self.make_pad():
                        self.main.forced = self.forced
                        return False
                    ## Getting the file name corresponding to the events
                    eventfile = os.path.normpath(
                        self.dirname
                        + "/Output/SAF/"
                        + myset.name
                        + "/RecoEvents/RecoEvents_"
                        + version.replace(".", "x")
                        + "_"
                        + card.replace(".tcl", "")
                        + ".root"
                    )
                    if not os.path.isfile(eventfile):
                        self.logger.error(f"The file called {eventfile} is not found...")
                        return False
                    ## Running the PAD
                    if not self.run_pad(eventfile):
                        self.main.forced = self.forced
                        return False
                    ## Saving the output and cleaning
                    if not self.save_output(
                        '"' + eventfile + '"', myset.name, analyses, card
                    ):
                        self.main.forced = self.forced
                        return False
                    if not self.main.recasting.store_root:
                        os.remove(eventfile)
                    else:
                        time.sleep(1.0)
                else:
                    # Run SFS
                    if not self.run_SimplifiedFastSim(
                        myset,
                        self.main.archi_info.ma5dir
                        + "/tools/PADForSFS/Input/Cards/"
                        + card,
                        analyses,
                    ):
                        return False
                    if self.main.recasting.store_root:
                        self.logger.warning(
                            "Simplified-FastSim does not use root, hence file will not be stored."
                        )
            else:
                self.dirname = self.main.recasting.stat_only_dir
            ## Running the CLs exclusion script (if available)
            if not self.main.recasting.analysis_only_mode:
                self.logger.debug(f"Compute CLs exclusion for {myset.name}")
                if not self.compute_cls(analyses, myset):
                    self.main.forced = self.forced
                    return False

        # Exit
        return True

    def analysis_header(self, version, card):
        ## Printing
        self.logger.info("   **********************************************************")
        self.logger.info(
            "   "
            + StringTools.Center(
                version + " running of the PAD" + " on events generated with", 57
            )
        )
        self.logger.info("   " + StringTools.Center(card, 57))
        self.logger.info("   **********************************************************")

    def update_pad_main(self, analysislist):
        ## Migrating the necessary files to the working directory
        self.logger.info("   Writing the PAD analyses")
        ## Safety (for backwards compatibility)
        if not os.path.isfile(self.pad + "/Build/Main/main.bak"):
            shutil.copy(
                self.pad + "/Build/Main/main.cpp", self.pad + "/Build/Main/main.bak"
            )
        mainfile = open(self.pad + "/Build/Main/main.bak", "r")
        newfile = open(self.dirname + "_RecastRun/Build/Main/main.cpp", "w")
        # Clean the analyzer folder
        if not FolderWriter.RemoveDirectory(
            os.path.normpath(
                self.dirname + "_RecastRun/Build/SampleAnalyzer/User/Analyzer"
            )
        ):
            return False
        os.mkdir(
            os.path.normpath(
                self.dirname + "_RecastRun/Build/SampleAnalyzer/User/Analyzer"
            )
        )
        # Including the necessary analyses
        analysisList = open(
            self.dirname + "_RecastRun/Build/SampleAnalyzer/User/Analyzer/analysisList.h",
            "w",
        )
        analysisList_header = (
            '#include "SampleAnalyzer/Process/Analyzer/AnalyzerManager.h"\n'
            + '#include "SampleAnalyzer/Commons/Service/LogStream.h"\n'
        )
        analysisList_body = (
            "\n// -----------------------------------------------------------------------------\n"
            + "//                                 BuildTable\n"
            + "// -----------------------------------------------------------------------------\n"
            + "void BuildUserTable(MA5::AnalyzerManager& manager)\n{\n    using namespace MA5;\n"
        )
        for analysis in analysislist:
            analysisList_header += (
                '#include "SampleAnalyzer/User/Analyzer/' + analysis + '.h"\n'
            )
            analysisList_body += (
                '    manager.Add("' + analysis + '",new ' + analysis + ");\n"
            )
            shutil.copy(
                self.pad + "/Build/SampleAnalyzer/User/Analyzer/" + analysis + ".cpp",
                self.dirname
                + "_RecastRun/Build/SampleAnalyzer/User/Analyzer/"
                + analysis
                + ".cpp",
            )
            shutil.copy(
                self.pad + "/Build/SampleAnalyzer/User/Analyzer/" + analysis + ".h",
                self.dirname
                + "_RecastRun/Build/SampleAnalyzer/User/Analyzer/"
                + analysis
                + ".h",
            )
        # Finalisation
        analysisList_body += "}\n"
        analysisList.write(analysisList_header)
        analysisList.write(analysisList_body)
        analysisList.close()
        ignore = False

        ## creating the main file with the desired analyses inside
        for line in mainfile:
            if "// Getting pointer to the analyzer" in line:
                ignore = True
                newfile.write(line)
                for analysis in analysislist:
                    newfile.write(
                        "  std::map<std::string, std::string> prm" + analysis + ";\n"
                    )
                    newfile.write("  AnalyzerBase* analyzer_" + analysis + "=\n")
                    newfile.write(
                        '    manager.InitializeAnalyzer("'
                        + analysis
                        + '","'
                        + analysis
                        + '.saf",'
                        + "prm"
                        + analysis
                        + ");\n"
                    )
                    newfile.write("  if (analyzer_" + analysis + "==0) return 1;\n\n")
            elif (
                "// Post initialization (creates the new output directory structure)"
                in line
            ):
                ignore = False
                newfile.write(line)
                if self.TACO_output != "":
                    newfile.write(
                        '      std::ofstream out;\n      out.open("../Output/'
                        + self.TACO_output
                        + '");\n'
                    )
                    newfile.write("      manager.HeadSR(out);\n      out << std::endl;\n")
            elif "!analyzer_" in line and not ignore:
                ignore = True
                for analysis in analysislist:
                    newfile.write(
                        "      if (!analyzer_"
                        + analysis
                        + "->Execute(mySample,myEvent)) continue;\n"
                    )
            elif "!analyzer1" in line:
                if self.TACO_output != "":
                    newfile.write("\nmanager.DumpSR(out);\n")
                ignore = False
            elif (
                "manager.Finalize(mySamples,myEvent);" in line and self.TACO_output != ""
            ):
                newfile.write(line)
                newfile.write("  out.close();\n")
            elif not ignore:
                newfile.write(line)

        ## exit
        mainfile.close()
        newfile.close()
        time.sleep(1.0)
        return True

    def make_pad(self):
        # Initializing the compiler
        self.logger.info("   Compiling the PAD located in " + self.dirname + "_RecastRun")
        compiler = LibraryWriter("lib", self.main)
        ncores = compiler.get_ncores2()
        # compiling
        command = ["make"]
        strcores = ""  # ERIC
        if ncores > 1:
            strcores = "-j" + str(ncores)
            command.append(strcores)
        logfile = self.dirname + "_RecastRun/Build/Log/PADcompilation.log"
        result, out = ShellCommand.ExecuteWithLog(
            command, logfile, self.dirname + "_RecastRun/Build"
        )
        time.sleep(1.0)
        # Checks and exit
        if not result:
            self.logger.error(
                "Impossible to compile the PAD. For more details, see the log file:"
            )
            self.logger.error(logfile)
            return False
        return True

    def run_pad(self, eventfile):
        ## input file
        if os.path.isfile(self.dirname + "_RecastRun/Input/PADevents.list"):
            os.remove(self.dirname + "_RecastRun/Input/PADevents.list")
        infile = open(self.dirname + "_RecastRun/Input/PADevents.list", "w")
        infile.write(eventfile)
        infile.close()
        ## cleaning the output directory
        if os.path.isdir(
            os.path.normpath(self.dirname + "_RecastRun/Output/SAF/PADevents")
        ):
            if not FolderWriter.RemoveDirectory(
                os.path.normpath(self.dirname + "_RecastRun/Output/SAF/PADevents")
            ):
                return False
        ## running
        command = ["./MadAnalysis5job", "../Input/PADevents.list"]
        ok = ShellCommand.Execute(command, self.dirname + "_RecastRun/Build")
        ## checks
        if not ok:
            self.logger.error("Problem with the run of the PAD on the file: " + eventfile)
            return False
        os.remove(self.dirname + "_RecastRun/Input/PADevents.list")
        ## exit
        time.sleep(1.0)
        return True

    def save_output(self, eventfile, setname, analyses, card):
        outfile = self.dirname + "/Output/SAF/" + setname + "/" + setname + ".saf"
        if not os.path.isfile(outfile):
            shutil.move(
                self.dirname + "_RecastRun/Output/SAF/PADevents/PADevents.saf", outfile
            )
        else:
            inp = open(outfile, "r")
            out = open(outfile + ".2", "w")
            intag = False
            stack = []
            maxl = len(eventfile)
            for line in inp:
                if "<FileInfo>" in line:
                    out.write(line)
                    intag = True
                elif "</FileInfo>" in line:
                    for i in range(len(stack)):
                        out.write(
                            stack[i].ljust(maxl)
                            + " # file "
                            + str(i + 1)
                            + "/"
                            + str(len(stack) + 1)
                            + "\n"
                        )
                    out.write(
                        eventfile.ljust(maxl)
                        + " # file "
                        + str(len(stack) + 1)
                        + "/"
                        + str(len(stack) + 1)
                        + "\n"
                    )
                    out.write(line)
                    intag = False
                elif intag:
                    stack.append(line.strip().split("#")[0])
                    maxl = max(maxl, len(line.strip().split("#")[0]))
                else:
                    out.write(line)
            inp.close()
            out.close()
            shutil.move(outfile + ".2", outfile)
        for analysis in analyses:
            shutil.move(
                self.dirname + "_RecastRun/Output/SAF/PADevents/" + analysis + "_0",
                self.dirname + "/Output/SAF/" + setname + "/" + analysis,
            )
        if self.TACO_output != "":
            filename = (
                ".".join(self.TACO_output.split(".")[:-1])
                + "_"
                + card.replace("tcl", "")
                + self.TACO_output.split(".")[-1]
            )
            shutil.move(
                self.dirname + "_RecastRun/Output/" + self.TACO_output,
                self.dirname + "/Output/SAF/" + setname + "/" + filename,
            )
        return True

    ################################################
    ### CLS CALCULATIONS AND OUTPUT
    ################################################

    def compute_cls(self, analyses, dataset):
        import spey
        from spey.system.webutils import get_bibtex

        from .statistical_models import (
            compute_poi_upper_limits,
            initialise_statistical_models,
        )

        ## Checking whether the CLs module can be used
        ET = self.check_xml_scipy_methods()
        if not ET:
            return False

        self.logger.info(
            "\033[1m   * Exclusion limit computation uses Spey package\033[0m"
        )
        self.logger.info("\033[1m     Please cite arXiv:2307.06996 [hep-ph]\033[0m")

        # Bibliography
        bibfile = os.path.join(self.dirname, "bibliography.bib")
        print_gl_citation = self.main.recasting.global_likelihoods_switch
        with open(bibfile, "w") as bib:
            bib.write(spey.cite() + "\n")
            if self.pyhf_config:
                pyhfbib = spey.get_backend_bibtex("pyhf")
                for _, item in pyhfbib.items():
                    for it in item:
                        bib.write(it + "\n")
            try:
                for arxiv in [
                    "1910.11418",
                    "2303.03427",
                    "2206.14870",
                    "2112.05163",
                    "2006.09387",
                ]:
                    bib.write(get_bibtex("inspire/arxiv", arxiv) + "\n")
            except Exception:
                pass
        if (
            len(self.main.recasting.extrapolated_luminosities) > 0
            or any(
                x is not None
                for x in [
                    dataset.scaleup,
                    dataset.scaledn,
                    dataset.pdfup,
                    dataset.pdfdn,
                ]
            )
            or any(a + b > 0.0 for a, b in self.main.recasting.systematics)
        ):
            self.logger.info(
                "\033[1m   * Using Uncertainties and Higher-Luminosity Estimates\033[0m"
            )
            self.logger.info("\033[1m     Please cite arXiv:1910.11418 [hep-ph]\033[0m")

        ## Running over all luminosities to extrapolate
        for extrapolated_lumi in [
            "default"
        ] + self.main.recasting.extrapolated_luminosities:
            self.logger.info(
                f"   Calculation of the exclusion CLs for a lumi of {extrapolated_lumi}"
            )
            ## Preparing the output file and checking whether a cross section has been defined
            outext = (
                ""
                if extrapolated_lumi == "default"
                else "_lumi_{:.3f}".format(extrapolated_lumi)
            )
            outfile = os.path.join(
                self.dirname, "Output/SAF", dataset.name, "CLs_output" + outext + ".dat"
            )
            if os.path.isfile(outfile):
                mysummary = open(outfile, "a+")
                mysummary.write("\n")
            else:
                mysummary = open(outfile, "w")
                self.write_cls_header(dataset.xsection, mysummary)

            ## running over all analysis
            for analysis in analyses:
                self.logger.debug(f"Running CLs exclusion calculation for {analysis}")
                # Getting the info file information (possibly rescaled)
                lumi, regions, regiondata = self.parse_info_file(
                    ET, analysis, extrapolated_lumi
                )
                self.logger.debug(f"lumi = {str(lumi)}")
                self.logger.debug(f"regions = {str(regions)}")
                self.logger.debug(f"regiondata = {str(regiondata)}")
                if lumi == -1 or regions == -1 or regiondata == -1:
                    self.logger.warning(
                        f"Info file for {analysis} missing or corrupted. Skipping the CLs calculation."
                    )
                    return False

                # Citation notifications for Global Likelihoods
                if (self.cov_config or self.pyhf_config) and print_gl_citation:
                    print_gl_citation = False
                    self.logger.info(
                        "\033[1m   * Using global likelihoods to improve CLs calculations\033[0m"
                    )
                    self.logger.info(
                        "\033[1m     Please cite arXiv:2206.14870 [hep-ph]\033[0m"
                    )
                    if self.pyhf_config != {}:
                        self.logger.info(
                            "\033[1m                 pyhf DOI:10.5281/zenodo.1169739\033[0m"
                        )
                        self.logger.info(
                            "\033[1m                 For more details see https://scikit-hep.org/pyhf/\033[0m"
                        )
                        if (
                            self.main.recasting.simplify_likelihoods
                            and self.main.session_info.has_simplify
                        ):
                            self.logger.info(
                                "\033[1m                 using simplify: ATL-PHYS-PUB-2021-038\033[0m"
                            )
                            self.logger.info(
                                "\033[1m                 For more details see https://github.com/eschanet/simplify\033[0m"
                            )
                    elif self.cov_config:
                        self.logger.info(
                            "\033[1m                 CMS-NOTE-2017-001\033[0m"
                        )

                ## Reading the cutflow information
                regiondata = self.read_cutflows(
                    self.dirname
                    + "/Output/SAF/"
                    + dataset.name
                    + "/"
                    + analysis
                    + "/Cutflows",
                    regions,
                    regiondata,
                )
                if regiondata == -1:
                    self.logger.warning(
                        f"Info file for {analysis} corrupted. Skipping the CLs calculation."
                    )
                    return False

                if dataset.xsection <= 0:
                    self.logger.error(
                        f"Cross section for {dataset.name} is not defined. Skipping the CLs calculation."
                    )
                    return False
                # Setup statistical models
                statistical_models = initialise_statistical_models(
                    regiondata=regiondata,
                    regions=regions,
                    xsection=dataset.xsection,
                    lumi=lumi,
                    simplified_model_config=self.cov_config if self.cov_config else None,
                    full_statistical_model_config=self.pyhf_config
                    if self.pyhf_config
                    else None,
                )

                ## Performing the CLS calculation
                model_types = [
                    "uncorrelated_background",
                    "simplified_likelihoods",
                    "full_likelihoods",
                ]
                for model_type, record in zip(model_types, [None, "cov_subset", "pyhf"]):
                    models = statistical_models[model_type]
                    if models:
                        regiondata = compute_poi_upper_limits(
                            regiondata,
                            models,
                            dataset.xsection,
                            is_extrapolated=extrapolated_lumi != "default",
                            record_to=record,
                        )

                xsflag = True
                if dataset.xsection > 0:
                    xsflag = False
                    regiondata = self.extract_cls(
                        regiondata,
                        statistical_models,
                        dataset.xsection,
                        lumi,
                        is_extrapolated=extrapolated_lumi != "default",
                    )

                ## Uncertainties on the rates
                Error_dict = error_dict_setup(
                    dataset=dataset,
                    systematics=self.main.recasting.systematics,
                    linear_comb=self.main.recasting.THerror_combination == "linear",
                )

                ## Computation of the uncertainties on the limits
                regiondata_errors = {}
                if dataset.xsection > 0.0 and any(x != 0 for x in Error_dict.values()):
                    for error_key, error_value in Error_dict.items():
                        varied_xsec = max(
                            round(dataset.xsection * (1.0 + error_value), 10), 0.0
                        )
                        if varied_xsec > 0:
                            xsflag = False
                            varied_statistical_models = initialise_statistical_models(
                                regiondata=regiondata,
                                regions=regions,
                                xsection=varied_xsec,
                                lumi=lumi,
                                simplified_model_config=self.cov_config
                                if self.cov_config
                                else None,
                                full_statistical_model_config=self.pyhf_config
                                if self.pyhf_config
                                else None,
                            )
                            regiondata_errors[error_key] = copy.deepcopy(regiondata)
                            if error_value != 0.0:
                                regiondata_errors[error_key] = self.extract_cls(
                                    regiondata_errors[error_key],
                                    varied_statistical_models,
                                    varied_xsec,
                                    lumi,
                                    is_extrapolated=extrapolated_lumi != "default",
                                )

                ## writing the output file
                self.write_cls_output(
                    analysis,
                    regions,
                    regiondata,
                    regiondata_errors,
                    mysummary,
                    xsflag,
                    lumi,
                )
                mysummary.write("\n")

            ## Closing the output file
            mysummary.close()
        return True

    def check_xml_scipy_methods(self):
        ## Checking XML parsers
        try:
            from lxml import ET
        except ImportError as err:
            self.logger.debug(str(err))
            try:
                import xml.etree.ElementTree as ET
            except ImportError as err2:
                self.logger.warning(
                    "lxml or xml not available... the CLs module cannot be used"
                )
                self.logger.debug(str(err2))
                return False
        # exit
        return ET

    def parse_info_file(self, etree, analysis, extrapolated_lumi):
        ## Is file existing?
        filename = (
            self.pad + "/Build/SampleAnalyzer/User/Analyzer/" + analysis + ".info"
        )  # ERIC
        if not os.path.isfile(filename):
            self.logger.warning("Info " + filename + " does not exist...")
            return -1, -1, -1
        ## Getting the XML information
        try:
            with open(filename, "r") as info_input:
                info_tree = etree.parse(info_input)
        except Exception as err:
            self.logger.warning("Error during XML parsing: " + str(err))
            self.logger.warning("Cannot parse the info file")
            return -1, -1, -1

        try:
            results = self.header_info_file(info_tree, analysis, extrapolated_lumi)
            return results
        except Exception as err:
            self.logger.warning("Error during extracting header info file: " + str(err))
            self.logger.warning("Cannot parse the info file")
            return -1, -1, -1

    def fix_pileup(self, filename):
        # x
        self.logger.debug("delphes card is here: " + filename)

        # Container for pileup
        FoundPileup = []

        # Safe
        if not os.path.isfile(filename):
            self.logger.error("internal error: file " + filename + " is not found")
            return False

        # Estimate the newpath of pileup
        if self.detector == "delphesMA5tune":
            newpath = self.main.archi_info.ma5dir + "/tools/PADForMA5tune/Input/Pileup"
        else:
            newpath = self.main.archi_info.ma5dir + "/tools/PAD/Input/Pileup"

        # Safe copy
        shutil.copyfile(filename, filename + ".original")
        input = open(filename + ".original", "r")
        output = open(filename, "w")

        # Loop on lines
        for line in input:
            line2 = line.lstrip()
            line2 = line2.rstrip()
            words = line2.split()
            if len(words) >= 3 and words[0] == "set" and words[1] == "PileUpFile":
                pileup = words[2].split("/")[-1]
                newfilename = os.path.normpath(newpath + "/" + pileup)
                output.write(line.replace(words[2], newfilename))
                FoundPileup.append(newfilename)
            else:
                output.write(line)

        # Close
        input.close()
        output.close()

        # Found pileup?
        logging.getLogger("MA5").debug(
            str(len(FoundPileup)) + " pile-up samples has been declared"
        )
        for item in FoundPileup:
            if not os.path.isfile(item):
                logging.getLogger("MA5").warning(
                    "Problem with Delphes card: pile-up sample is not found: " + item
                )
                return False

        return True

    def header_info_file(self, etree, analysis, extrapolated_lumi):
        self.logger.debug("Reading info from the file related to " + analysis + "...")
        ## checking the header of the file
        info_root = etree.getroot()
        if info_root.tag != "analysis":
            self.logger.warning("Invalid info file (" + analysis + "): <analysis> tag.")
            return -1, -1, -1
        if info_root.attrib["id"].lower() != analysis.lower():
            self.logger.warning(
                "Invalid info file (" + analysis + "): <analysis id> tag."
            )
            return -1, -1, -1
        ## extracting the information
        lumi = 0
        lumi_scaling = 1.0
        regions = []
        self.cov_config = {}
        self.pyhf_config = {}
        regiondata = {}
        # Getting the description of the subset of SRs having covariances
        # Now the cov_switch is activated here
        if (
            "cov_subset" in info_root.attrib
            and self.main.recasting.global_likelihoods_switch
        ):
            self.cov_config[info_root.attrib["cov_subset"]] = dict(
                cov_regions=[], covariance=[]
            )
        # activate pyhf
        if (
            self.main.recasting.global_likelihoods_switch
            and self.main.session_info.has_pyhf
            and self.cov_config == {}
        ):
            try:
                self.pyhf_config = self.pyhf_info_file(info_root)
                self.logger.debug(str(self.pyhf_config))
            except Exception as err:
                self.logger.debug("Check pyhf_info_file function!\n" + str(err))
                self.pyhf_config = {}

        ## first we need to get the number of regions
        for child in info_root:
            # Luminosity
            if child.tag == "lumi":
                try:
                    lumi = float(child.text)
                    if extrapolated_lumi != "default":
                        lumi_scaling = round(extrapolated_lumi / lumi, 8)
                        lumi = lumi * lumi_scaling
                except Exception as err:
                    self.logger.warning(
                        "Invalid info file (" + analysis + "): ill-defined lumi"
                    )
                    self.logger.debug(str(err))
                    return -1, -1, -1
                self.logger.debug(
                    "The luminosity of " + analysis + " is " + str(lumi) + " fb-1."
                )
            # regions
            if child.tag == "region" and (
                "type" not in child.attrib or child.attrib["type"] == "signal"
            ):
                if "id" not in child.attrib:
                    self.logger.warning(
                        "Invalid info file (" + analysis + "): <region id> tag."
                    )
                    return -1, -1, -1
                if child.attrib["id"] in regions:
                    self.logger.warning(
                        "Invalid info file (" + analysis + "): doubly-defined region."
                    )
                    return -1, -1, -1
                regions.append(child.attrib["id"])
                # If one covariance entry is found, the covariance switch is turned on
                if self.main.recasting.global_likelihoods_switch:
                    for grand_child in child.findall("covariance"):
                        if "cov_subset" in info_root.attrib:
                            if grand_child.attrib.get("cov_subset", "default") in [
                                info_root.attrib["cov_subset"],
                                "default",
                            ]:
                                if (
                                    child.attrib["id"]
                                    not in self.cov_config[
                                        info_root.attrib["cov_subset"]
                                    ]["cov_regions"]
                                ):
                                    self.cov_config[info_root.attrib["cov_subset"]][
                                        "cov_regions"
                                    ].append(child.attrib["id"])
                        else:
                            if grand_child.attrib.get("cov_subset", False):
                                subsetID = grand_child.attrib["cov_subset"]
                                if subsetID not in self.cov_config.keys():
                                    self.cov_config[subsetID] = dict(
                                        cov_regions=[], covariance=[]
                                    )
                                if (
                                    child.attrib["id"]
                                    not in self.cov_config[subsetID]["cov_regions"]
                                ):
                                    self.cov_config[subsetID]["cov_regions"].append(
                                        child.attrib["id"]
                                    )

        if self.cov_config:
            for cov_subset, subset in self.cov_config.items():
                length = len(subset["cov_regions"])
                self.cov_config[cov_subset]["covariance"] = [
                    [0.0 for i in range(length)] for j in range(length)
                ]

        ## getting the region information
        for child in info_root:
            if child.tag == "region" and (
                "type" not in child.attrib or child.attrib["type"] == "signal"
            ):
                nobs, nb, deltanb, syst, stat = [-1] * 5
                for rchild in child:
                    # self.logger.debug(rchild.tag)
                    # self.logger.debug(str(lumi)+' '+str(regions)+ ' '+str(regiondata))
                    try:
                        myval = float(rchild.text)
                    except ValueError as err:
                        self.logger.warning(
                            "Invalid info file ("
                            + analysis
                            + "): region data ill-defined."
                        )
                        self.logger.debug(str(err))
                        return -1, -1, -1
                    if rchild.tag == "nobs":
                        nobs = myval
                    elif rchild.tag == "nb":
                        nb = myval
                    elif rchild.tag == "deltanb":
                        deltanb = myval
                    elif rchild.tag == "deltanb_syst":
                        syst = myval
                    elif rchild.tag == "deltanb_stat":
                        stat = myval
                    elif rchild.tag == "covariance":
                        if self.cov_config:
                            for cov_subset, item in self.cov_config.items():
                                if (
                                    child.attrib["id"] not in item["cov_regions"]
                                    or rchild.attrib["region"] not in item["cov_regions"]
                                ):
                                    continue
                                i = item["cov_regions"].index(child.attrib["id"])
                                j = item["cov_regions"].index(rchild.attrib["region"])
                                self.cov_config[cov_subset]["covariance"][i][j] = myval
                    else:
                        self.logger.warning(
                            "Invalid info file (" + analysis + "): unknown region subtag."
                        )
                        return -1, -1, -1
                if syst == -1 and stat == -1:
                    if self.main.recasting.error_extrapolation == "sqrt":
                        deltanb = round(deltanb * math.sqrt(lumi_scaling), 8)
                    elif self.main.recasting.error_extrapolation == "linear":
                        deltanb = round(deltanb * lumi_scaling, 8)
                    else:
                        nb_new = nb * lumi_scaling
                        deltanb = round(
                            math.sqrt(
                                self.main.recasting.error_extrapolation[0] ** 2
                                * nb_new**2
                                + self.main.recasting.error_extrapolation[1] ** 2 * nb_new
                            ),
                            8,
                        )
                else:
                    if syst == -1:
                        syst = 0.0
                    if stat == -1:
                        stat = 0.0
                    deltanb = round(
                        math.sqrt(
                            (syst / nb) ** 2
                            + (stat / (nb * math.sqrt(lumi_scaling))) ** 2
                        )
                        * nb
                        * lumi_scaling,
                        8,
                    )
                regiondata[child.attrib["id"]] = {
                    "nobs": nobs * lumi_scaling,
                    "nb": nb * lumi_scaling,
                    "deltanb": deltanb,
                }

        tmp = {}
        for cov_subset, item in self.cov_config.items():
            if item["covariance"] != []:
                cov = np.array(item["covariance"])
                sigma = np.sqrt(np.diag(cov))
                invsigma = np.linalg.inv(np.diag(sigma))
                corr = invsigma @ cov @ invsigma

                if self.main.recasting.error_extrapolation == "sqrt":
                    new_sigma = round(math.sqrt(sigma) * lumi_scaling, 8)
                elif self.main.recasting.error_extrapolation == "linear":
                    new_sigma = sigma * lumi_scaling**2
                else:
                    new_sigma = (
                        sigma
                        * lumi_scaling**2
                        * self.main.recasting.error_extrapolation[0] ** 2
                        + np.sqrt(sigma)
                        * lumi_scaling
                        * self.main.recasting.error_extrapolation[1] ** 2
                    )

                new_sigma_matrix = np.diag(new_sigma)
                new_cov = new_sigma_matrix @ corr @ new_sigma_matrix
                item["covariance"] = new_cov.tolist()
                tmp[cov_subset] = item
        self.cov_config = tmp

        return lumi, regions, regiondata

    def pyhf_info_file(self, info_root):
        """In order to make use of HistFactory, we need some pieces of information. First,
        the location of the specific background-only likelihood json files that are given
        in the info file. The collection of SR contributing to a given profile must be
        provided. One can process multiple likelihood profiles dedicated to different sets
        of SRs.
        """
        self.pyhf_config = {}  # reset
        if any(x.tag == "pyhf" for x in info_root):
            # pyhf_path = os.path.join(self.main.archi_info.ma5dir, 'tools/pyhf/pyhf-master/src')
            try:
                # if os.path.isdir(pyhf_path) and pyhf_path not in sys.path:
                #     sys.path.insert(0, pyhf_path)
                import spey_pyhf

                self.logger.debug("spey_pyhf v" + str(spey_pyhf.__version__))
                self.logger.debug(
                    "spey_pyhf has been imported from " + " ".join(spey_pyhf.__path__)
                )
            except ImportError:
                self.logger.warning(
                    "To use the global likelihood machinery, please install spey-pyhf"
                )
                return {}
            except Exception as err:
                self.logger.debug("Problem with pyhf_info_file function!!")
                self.logger.debug(str(err))
                return {}
        else:
            return {}
        analysis = info_root.attrib["id"]
        pyhf_config, to_remove = construct_histfactory_dictionary(info_root, self)
        # validate
        for likelihood_profile, config in pyhf_config.items():
            if likelihood_profile in to_remove:
                continue
            # validat pyhf config
            background = HF_Background(config)
            signal = HF_Signal(
                config, {}, xsection=1.0, background=background, validate=True
            )

            if signal.hf != []:
                self.logger.debug(
                    'Likelihood profile "' + str(likelihood_profile) + '" is valid.'
                )
            else:
                self.logger.warning(
                    "Invalid profile in "
                    + analysis
                    + " ignoring :"
                    + str(likelihood_profile)
                )
                to_remove.append(likelihood_profile)
        # remove invalid profiles
        for rm in to_remove:
            pyhf_config.pop(rm)

        return pyhf_config

    def write_cls_header(self, xs, out):
        if xs <= 0:
            self.logger.info(
                "   Signal xsection not defined. The 95% excluded xsection will be calculated."
            )
            out.write(
                "# analysis name".ljust(30, " ")
                + "signal region".ljust(60, " ")
                + "sig95(exp)".ljust(15, " ")
                + "sig95(obs)".ljust(10, " ")
                + "        ||    "
                + "efficiency".ljust(15, " ")
                + "stat".ljust(15, " ")
            )
            for i in range(0, len(self.main.recasting.systematics)):
                out.write(
                    (
                        "syst"
                        + str(i + 1)
                        + "("
                        + str(self.main.recasting.systematics[i][0] * 100)
                        + "%)"
                    ).ljust(15, " ")
                )
            out.write("\n")
        else:
            out.write(
                "# analysis name".ljust(30, " ")
                + "signal region".ljust(60, " ")
                + "best?".ljust(10, " ")
                + "sig95(exp)".ljust(20, " ")
                + "sig95(obs)".ljust(20, " ")
                + "1-CLs".ljust(10, " ")
                + "     ||    "
                + "efficiency".ljust(15, " ")
                + "stat".ljust(15, " ")
            )
            for i in range(0, len(self.main.recasting.systematics)):
                out.write(
                    (
                        "syst"
                        + str(i + 1)
                        + "("
                        + str(self.main.recasting.systematics[i][0] * 100)
                        + "%)"
                    ).ljust(15, " ")
                )
            out.write("\n")

    def read_cutflows(self, path, regions, regiondata):
        self.logger.debug("Read the cutflow from the files:")
        for reg in regions:
            regname = clean_region_name(reg)
            ## getting the initial and final number of events
            IsInitial = False
            IsCounter = False
            N0 = 0.0
            Nf = 0.0
            ## checking if regions must be combined
            theregs = regname.split(";")
            for regiontocombine in theregs:
                filename = path + "/" + regiontocombine + ".saf"
                self.logger.debug("+ " + filename)
                if not os.path.isfile(filename):
                    self.logger.warning(
                        "Cannot find a cutflow for the region "
                        + regiontocombine
                        + " in "
                        + path
                    )
                    self.logger.warning("Skipping the CLs calculation.")
                    return -1
                mysaffile = open(filename)
                myN0 = -1
                myNf = -1
                for line in mysaffile:
                    if "<InitialCounter>" in line:
                        IsInitial = True
                        continue
                    elif "</InitialCounter>" in line:
                        IsInitial = False
                        continue
                    elif "<Counter>" in line:
                        IsCounter = True
                        continue
                    elif "</Counter>" in line:
                        IsCounter = False
                        continue
                    if IsInitial and "sum of weights" in line and not "^2" in line:
                        myN0 = float(line.split()[0]) + float(line.split()[1])
                    if IsCounter and "sum of weights" in line and not "^2" in line:
                        myNf = float(line.split()[0]) + float(line.split()[1])
                mysaffile.close()
                if myNf == -1 or myN0 == -1:
                    self.logger.warning(
                        "Invalid cutflow for the region "
                        + reg
                        + "("
                        + regname
                        + ") in "
                        + path
                    )
                    self.logger.warning("Skipping the CLs calculation.")
                    return -1
                Nf += myNf
                N0 += myN0
            if Nf == 0 and N0 == 0:
                self.logger.warning(
                    "Invalid cutflow for the region "
                    + reg
                    + "("
                    + regname
                    + ") in "
                    + path
                )
                self.logger.warning("Skipping the CLs calculation.")
                return -1
            regiondata[reg]["N0"] = N0
            regiondata[reg]["Nf"] = Nf
        return regiondata

    def extract_cls(
        self,
        regiondata: dict,
        stat_models: dict,
        xsection: float,
        lumi: float,
        is_extrapolated: bool,
    ) -> dict:
        from .statistical_models import APRIORI, OBSERVED

        self.logger.debug("Compute CLs...")
        ## computing fi a region belongs to the best expected ones, and derive the CLs in all cases
        idx = 2 if is_extrapolated else 0
        expected = APRIORI if is_extrapolated else OBSERVED
        bestreg = []
        rMax = -1
        for reg, stat_model in stat_models["uncorrelated_background"].items():
            nsignal = (
                xsection * lumi * 1000.0 * regiondata[reg]["Nf"] / regiondata[reg]["N0"]
            )
            if nsignal <= 0:
                rSR = -1
                myCLs = 0
            else:
                n95 = (
                    float(regiondata[reg]["s95exp"])
                    * lumi
                    * 1000.0
                    * regiondata[reg]["Nf"]
                    / regiondata[reg]["N0"]
                )
                rSR = nsignal / n95
                myCLs = stat_model.exclusion_confidence_level(expected=expected)[idx]
            regiondata[reg]["rSR"] = rSR
            regiondata[reg]["CLs"] = myCLs
            if rSR > rMax:
                regiondata[reg]["best"] = 1
                for mybr in bestreg:
                    regiondata[mybr]["best"] = 0
                bestreg = [reg]
                rMax = rSR
            else:
                regiondata[reg]["best"] = 0

        if self.cov_config:
            minsig95, bestreg = 1e99, []
            for cov_subset, stat_model in stat_models["simplified_likelihoods"].items():
                CLs = stat_model.exclusion_confidence_level(expected=expected)
                s95 = float(regiondata["cov_subset"][cov_subset]["s95exp"])
                regiondata["cov_subset"][cov_subset]["CLs"] = CLs[idx]
                if expected != OBSERVED:
                    regiondata["cov_subset"][cov_subset]["full_CLs_output"] = CLs
                if 0.0 < s95 < minsig95:
                    regiondata["cov_subset"][cov_subset]["best"] = 1
                    for mybr in bestreg:
                        regiondata["cov_subset"][mybr]["best"] = 0
                    bestreg = [cov_subset]
                    minsig95 = s95
                else:
                    regiondata["cov_subset"][cov_subset]["best"] = 0

        if self.pyhf_config:
            minsig95, bestreg = 1e99, []
            for llhd_profile, stat_model in stat_models["full_likelihoods"].items():
                CLs = stat_model.exclusion_confidence_level(expected=expected)
                regiondata["pyhf"][llhd_profile]["CLs"] = CLs[idx]
                if expected != OBSERVED:
                    regiondata["pyhf"][llhd_profile]["full_CLs_output"] = CLs
                s95 = float(regiondata["pyhf"][llhd_profile]["s95exp"])
                if 0.0 < s95 < minsig95:
                    regiondata["pyhf"][llhd_profile]["best"] = 1
                    for mybr in bestreg:
                        regiondata["pyhf"][mybr]["best"] = 0
                    bestreg = [llhd_profile]
                    minsig95 = s95
                else:
                    regiondata["pyhf"][llhd_profile]["best"] = 0

        return regiondata

    def write_cls_output(
        self, analysis, regions, regiondata, errordata, summary, xsflag, lumi
    ):
        self.logger.debug("Write CLs...")
        if self.main.developer_mode:
            to_save = {analysis: {"regiondata": regiondata, "errordata": errordata}}
            name = summary.name.split(".dat")[0] + ".json"
            if os.path.isfile(name):
                with open(name, "r") as json_file:
                    past = json.load(json_file)
                for key, item in [
                    (k, i) for k, i in past.items() if k not in list(to_save.keys())
                ]:
                    to_save[key] = item
            self.logger.debug("Saving dictionary : " + name)
            with open(name, "w+") as results:
                json.dump(to_save, results, indent=4)
            ###################################################################################
            # @Jack : For debugging purposes in the future. This slice of code
            #         prints the Json file for signal WITH XSEC=1 !!!
            if self.pyhf_config != {}:
                iterator = copy.deepcopy(list(self.pyhf_config.items()))
                for n, (likelihood_profile, config) in enumerate(iterator):
                    if regiondata.get("pyhf", {}).get(likelihood_profile, False) == False:
                        continue
                    signal = HF_Signal(config, regiondata, xsection=1.0)
                    name = summary.name.split(".dat")[0]
                    with open(
                        name + "_" + likelihood_profile + "_sig.json", "w+"
                    ) as out_file:
                        json.dump(signal(lumi), out_file, indent=4)
            ###################################################################################
        err_sets = [
            ["scale_up", "scale_dn", "Scale var."],
            ["TH_up", "TH_dn", "TH   error"],
        ]
        for reg in regions:
            eff = regiondata[reg]["Nf"] / regiondata[reg]["N0"]
            if eff < 0:
                eff = 0
            stat = round(
                math.sqrt(eff * (1 - eff) / (abs(regiondata[reg]["N0"]) * lumi)), 10
            )
            syst = []
            if len(self.main.recasting.systematics) > 0:
                for unc in self.main.recasting.systematics:
                    syst.append(round(0.5 * (unc[0] + unc[1]) * eff, 8))
            else:
                syst = [0]
            myeff = "%.7f" % eff
            mystat = "%.7f" % stat
            mysyst = ["%.7f" % x for x in syst]
            myxsexp = regiondata[reg]["s95exp"]
            if "s95obs" in list(regiondata[reg].keys()):
                myxsobs = regiondata[reg]["s95obs"]
            else:
                myxsobs = "-1"
            if not xsflag:
                mycls = "%.10f" % regiondata[reg]["CLs"]
                summary.write(
                    analysis.ljust(30, " ")
                    + reg.ljust(60, " ")
                    + str(regiondata[reg]["best"]).ljust(10, " ")
                    + myxsexp.ljust(20, " ")
                    + myxsobs.ljust(20, " ")
                    + mycls.ljust(10, " ")
                    + "   ||    "
                    + myeff.ljust(15, " ")
                    + mystat.ljust(15, " ")
                )
                for onesyst in mysyst:
                    summary.write(onesyst.ljust(15, " "))
                summary.write("\n")
                band = []
                for error_set in err_sets:
                    if len([x for x in error_set if x in list(errordata.keys())]) == 2:
                        band = band + [
                            errordata[error_set[0]][reg]["CLs"],
                            errordata[error_set[1]][reg]["CLs"],
                            regiondata[reg]["CLs"],
                        ]
                        if len(set(band)) == 1:
                            continue
                        summary.write(
                            "".ljust(90, " ")
                            + error_set[2]
                            + " band:         ["
                            + ("%.4f" % min(band))
                            + ", "
                            + ("%.4f" % max(band))
                            + "]\n"
                        )
                for i, sys in enumerate(self.main.recasting.systematics):
                    error_set = ["sys" + str(i) + "_up", "sys" + str(i) + "_dn"]
                    if len([x for x in error_set if x in list(errordata.keys())]) == 2:
                        band = band + [
                            errordata[error_set[0]][reg]["CLs"],
                            errordata[error_set[1]][reg]["CLs"],
                            regiondata[reg]["CLs"],
                        ]
                        if len(set(band)) == 1:
                            continue
                        up, dn = sys
                        summary.write(
                            "".ljust(90, " ")
                            + "+{:.1f}% -{:.1f}% syst:".format(
                                up * 100.0, dn * 100.0
                            ).ljust(25, " ")
                            + "["
                            + ("%.4f" % min(band))
                            + ", "
                            + ("%.4f" % max(band))
                            + "]\n"
                        )
            else:
                summary.write(
                    analysis.ljust(30, " ")
                    + reg.ljust(60, " ")
                    + myxsexp.ljust(20, " ")
                    + myxsobs.ljust(20, " ")
                    + " ||    "
                    + myeff.ljust(15, " ")
                    + mystat.ljust(15, " ")
                )
                if syst != [0]:
                    for onesyst in mysyst:
                        summary.write(onesyst.ljust(15, " "))
                summary.write("\n")
        # Adding the global CLs from simplified likelihood
        for cov_subset in self.cov_config:
            if not xsflag:
                myxsexp = regiondata["cov_subset"][cov_subset].get("s95exp", "-1")
                myxsobs = regiondata["cov_subset"][cov_subset].get("s95obs", "-1")
                best = str(regiondata["cov_subset"][cov_subset].get("best", 0))
                myglobalcls = "%.4f" % regiondata["cov_subset"][cov_subset]["CLs"]
                description = "[SL]-" + cov_subset
                summary.write(
                    analysis.ljust(30, " ")
                    + description.ljust(60, " ")
                    + best.ljust(10, " ")
                    + myxsexp.ljust(15, " ")
                    + myxsobs.ljust(15, " ")
                    + myglobalcls.ljust(7, " ")
                    + "   ||    \n"
                )
                band = []
                for error_set in err_sets:
                    if len([x for x in error_set if x in list(errordata.keys())]) == 2:
                        band = band + [
                            errordata[error_set[0]]["cov_subset"][cov_subset]["CLs"],
                            errordata[error_set[1]]["cov_subset"][cov_subset]["CLs"],
                            regiondata["cov_subset"][cov_subset]["CLs"],
                        ]
                        if len(set(band)) == 1:
                            continue
                        summary.write(
                            "".ljust(90, " ")
                            + error_set[2]
                            + " band:         ["
                            + ("%.4f" % min(band))
                            + ", "
                            + ("%.4f" % max(band))
                            + "]\n"
                        )
                for i, sys in enumerate(self.main.recasting.systematics):
                    error_set = ["sys" + str(i) + "_up", "sys" + str(i) + "_dn"]
                    if len([x for x in error_set if x in list(errordata.keys())]) == 2:
                        band = band + [
                            errordata[error_set[0]]["cov_subset"][cov_subset]["CLs"],
                            errordata[error_set[1]]["cov_subset"][cov_subset]["CLs"],
                            regiondata["cov_subset"][cov_subset]["CLs"],
                        ]
                        if len(set(band)) == 1:
                            continue
                        up, dn = sys
                        summary.write(
                            "".ljust(90, " ")
                            + "+{:.1f}% -{:.1f}% syst:".format(
                                up * 100.0, dn * 100.0
                            ).ljust(25, " ")
                            + "["
                            + ("%.4f" % min(band))
                            + ", "
                            + ("%.4f" % max(band))
                            + "]\n"
                        )
            else:
                myxsexp = regiondata["cov_subset"][cov_subset]["s95exp"]
                myxsobs = regiondata["cov_subset"][cov_subset]["s95obs"]
                description = "[SL]-" + cov_subset
                summary.write(
                    analysis.ljust(30, " ")
                    + description.ljust(60, " ")
                    + myxsexp.ljust(15, " ")
                    + myxsobs.ljust(15, " ")
                    + " ||    \n"
                )

        # pyhf results
        pyhf_data = regiondata.get("pyhf", {})
        for likelihood_profile in list(self.pyhf_config.keys()):
            if likelihood_profile not in list(pyhf_data.keys()):
                continue
            myxsexp = pyhf_data.get(likelihood_profile, {}).get("s95exp", "-1")
            myxsobs = pyhf_data.get(likelihood_profile, {}).get("s95obs", "-1")
            if not xsflag:
                self.logger.debug(str(pyhf_data))
                mycls = "{:.4f}".format(
                    pyhf_data.get(likelihood_profile, {}).get("CLs", 0.0)
                )
                best = str(pyhf_data.get(likelihood_profile, {}).get("best", 0))
                summary.write(
                    analysis.ljust(30, " ")
                    + ("[pyhf]-" + likelihood_profile + "-profile").ljust(60, " ")
                    + best.ljust(10, " ")
                    + myxsexp.ljust(15, " ")
                    + myxsobs.ljust(15, " ")
                    + mycls.ljust(7, " ")
                    + "   ||    "
                    + "".ljust(15, " ")
                    + "".ljust(15, " ")
                )
                summary.write("\n")
                band = []
                for error_set in err_sets:
                    if len([x for x in error_set if x in list(errordata.keys())]) == 2:
                        band = band + [
                            errordata[error_set[0]]
                            .get("pyhf", {})
                            .get(likelihood_profile, {})
                            .get("CLs", 0.0),
                            errordata[error_set[1]]
                            .get("pyhf", {})
                            .get(likelihood_profile, {})
                            .get("CLs", 0.0),
                            pyhf_data.get(likelihood_profile, {}).get("CLs", 0.0),
                        ]
                        if len(set(band)) == 1:
                            continue
                        summary.write(
                            "".ljust(90, " ")
                            + error_set[2]
                            + " band:         ["
                            + ("%.4f" % min(band))
                            + ", "
                            + ("%.4f" % max(band))
                            + "]\n"
                        )
                for i, sys in enumerate(self.main.recasting.systematics):
                    error_set = ["sys" + str(i) + "_up", "sys" + str(i) + "_dn"]
                    if len([x for x in error_set if x in list(errordata.keys())]) == 2:
                        band = band + [
                            errordata[error_set[0]]
                            .get("pyhf", {})
                            .get(likelihood_profile, {})
                            .get("CLs", 0.0),
                            errordata[error_set[1]]
                            .get("pyhf", {})
                            .get(likelihood_profile, {})
                            .get("CLs", 0.0),
                            pyhf_data.get(likelihood_profile, {}).get("CLs", 0.0),
                        ]
                        if len(set(band)) == 1:
                            continue
                        up, dn = sys
                        summary.write(
                            "".ljust(90, " ")
                            + "+{:.1f}% -{:.1f}% syst:".format(
                                up * 100.0, dn * 100.0
                            ).ljust(25, " ")
                            + "["
                            + ("%.4f" % min(band))
                            + ", "
                            + ("%.4f" % max(band))
                            + "]\n"
                        )
            else:
                summary.write(
                    analysis.ljust(30, " ")
                    + ("[pyhf]-" + likelihood_profile + "-profile").ljust(60, " ")
                    + myxsexp.ljust(15, " ")
                    + myxsobs.ljust(15, " ")
                    + " ||    "
                    + "".ljust(15, " ")
                    + "".ljust(15, " ")
                )
                summary.write("\n")


def clean_region_name(mystr):
    newstr = mystr.replace("/", "_slash_")
    newstr = newstr.replace("->", "_to_")
    newstr = newstr.replace(">=", "_greater_than_or_equal_to_")
    newstr = newstr.replace(">", "_greater_than_")
    newstr = newstr.replace("<=", "_smaller_than_or_equal_to_")
    newstr = newstr.replace("<", "_smaller_than_")
    newstr = newstr.replace(" ", "_")
    newstr = newstr.replace(",", "_")
    newstr = newstr.replace("+", "_")
    newstr = newstr.replace("-", "_")
    newstr = newstr.replace("(", "_lp_")
    newstr = newstr.replace(")", "_rp_")
    return newstr
