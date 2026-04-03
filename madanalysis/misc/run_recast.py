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

from __future__ import absolute_import

import copy
import json
import logging
import math
import os
import shutil
import time
from pathlib import Path
from typing import Union

import numpy as np
from shell_command import ShellCommand  # pylint: disable=import-error
from six.moves import range
from string_tools import StringTools  # pylint: disable=import-error

from madanalysis.configuration.delphes_configuration import DelphesConfiguration
from madanalysis.configuration.delphesMA5tune_configuration import (
    DelphesMA5tuneConfiguration,
)
from madanalysis.core.main import Main
from madanalysis.dataset.dataset_collection import DatasetCollection
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
from madanalysis.misc.utils import (
    clean_region_name,
    edit_recasting_card,
    get_runs,
    read_xsec,
)

# pylint: disable=logging-fstring-interpolation,import-outside-toplevel

log = logging.getLogger("MA5")


class RunRecast:
    """
    One-line summary
    Initialize the RunRecast controller holding runtime state for recasting runs.

    Extended summary
    Stores references to the main application object, directory paths and
    internal configuration derived from the architecture and recasting
    settings. Prepares PAD <-> detector mapping and Delphes include paths.

    Args:
        main (``Main``): The main MadAnalysis application object providing configuration.
        dirname (``str``): Base working directory used for the recasting jobs.
    """

    def __init__(self, main: Main, dirname: str):
        self.dirname: str = dirname
        self.main: Main = main
        self.delphes_runcard = []
        self.analysis_runcard = []
        self.forced = self.main.forced
        self.detector = ""
        self.pad = ""
        self.first11 = True
        self.first12 = True
        self.pyhf_config = {}  # initialize and configure histfactory
        self.cov_config = {}
        self.TACO_output = self.main.recasting.TACO_output
        self.pad_dict = {}

        if self.main.recasting.ma5tune:
            self.pad_dict.update({"v1.1": ("PADForMA5tune", "delphesMA5tune")})
        if self.main.recasting.delphes:
            self.pad_dict.update({"v1.2": ("PAD", "delphes")})
        if self.main.archi_info.has_fastjet:
            self.pad_dict.update({"vSFS": ("PADForSFS", "fastjet")})

        self.delphes_inc_pths = []
        if len(self.main.archi_info.delphes_inc_paths) != 0:
            self.delphes_inc_pths = self.main.archi_info.delphes_inc_paths
            self.delphes_inc_pths.append(
                next((p for p in self.delphes_inc_pths if Path(p).stem == "delphes"), "")
                + "/modules"
            )

    def init(self) -> bool:
        """
        One-line summary
        Prepare and validate the recasting runs by editing the recasting card and collecting runs.

        Extended summary
        Optionally opens an editor for the recasting card (unless forced or in script mode),
        then obtains the list of delphes and analysis runs from the recasting card and
        verifies there is work to do.

        Returns:
            ``bool``:
            True if at least one delphes run was found and initialization succeeded, False otherwise.
        """
        ### First, the analyses to take care off
        log.debug("  Inviting the user to edit the recasting card...")
        if not self.forced or not self.main.script:
            edit_recasting_card(self.main.session_info.editor, self.dirname)
        ### Getting the list of analyses to recast
        log.info("   Getting the list of delphes simulation to be performed...")
        self.delphes_runcard, self.analysis_runcard = get_runs(self.dirname)
        ### Check if we have anything to do
        if len(self.delphes_runcard) == 0:
            log.warning("No recasting to do... Please check the recasting card")
            return False

        ### Exit
        return True

    ################################################
    ### GENERAL METHODS
    ################################################

    ## Running the machinery
    def execute(self) -> bool:
        """
        One-line summary
        Execute all configured PAD runs for the collected delphes runcard entries.

        Extended summary
        Iterates over the configured delphes runs, maps version to PAD/detector, runs the
        analysis for each entry and performs cleanup. Restores main.forced at exit.

        Returns:
            ``bool``:
            True if execution completed successfully for all runs, False on error or unsupported version.
        """
        self.main.forced = True
        for version, card in self.delphes_runcard:
            ## Extracting run infos and checks
            if not self.pad_dict.get(version, False):
                self.main.forced = self.forced
                return False
            pad, self.detector = self.pad_dict[version]
            self.pad = f"{self.main.archi_info.ma5dir}/tools/{pad}"
            self.main.fastsim.package = self.detector

            ## Running the analyses
            if not self.analysis_single(version, card):
                self.main.forced = self.forced
                return False

            ## Cleaning
            pth = Path(os.path.normpath(self.dirname + "_RecastRun"))
            if not self.main.developer_mode:
                if not FolderWriter.RemoveDirectory(str(pth)):
                    log.error("Cannot remove directory: %s", str(pth))
            else:
                log.debug("Analysis kept in %s folder.", str(pth))

        # exit
        self.main.forced = self.forced
        return True

    ################################################
    ### FastSim RUN
    ################################################
    def run_delphes_analysis(
        self, dataset: DatasetCollection, card: str, analysislist: list[str]
    ) -> bool:
        """
        One-line summary
        Build, compile and run a PAD-based Delphes analysis for a dataset.

        Extended summary
        Prepares the run directory, writes analyzer sources and Makefiles, patches main.cpp,
        fixes pileup references, compiles, links and executes the SampleAnalyzer job and moves
        any produced Delphes events back to the main output layout.

        Args:
            dataset (``DatasetCollection``): Dataset collection to process.
            card (``str``): Delphes card filename (relative to PAD Input).
            analysislist (``list[str]``): List of analyzer names to include in the run.

        Returns:
            ``bool``:
            True on success, False on any failure during preparation, compilation or execution.
        """
        # Preparing the run
        self.main.recasting.status = "off"
        self.main.fastsim.package = self.detector
        self.main.fastsim.clustering = 0

        pad_name = "PAD" if self.detector == "delphes" else "PADForMA5tune"
        card_path = Path(f"../../../../tools/{pad_name}/Input/Cards/{card}")
        version = ""
        if self.detector == "delphesMA5tune":
            version = "v1x1"
            self.main.fastsim.delphes = 0
            self.main.fastsim.delphesMA5tune = DelphesMA5tuneConfiguration()
            self.main.fastsim.delphesMA5tune.card = str(card_path)
        elif self.detector == "delphes":
            self.main.fastsim.delphesMA5tune = 0
            self.main.fastsim.delphes = DelphesConfiguration()
            self.main.fastsim.delphes.card = str(card_path)
            version = "v1x2"

        recast_path = Path(self.dirname + "_RecastRun").absolute()
        org_rel = Path("Build/SampleAnalyzer/User/Analyzer")
        jobber = JobWriter(self.main, str(recast_path))

        log.info("   Creating folder '%s'", recast_path.stem)
        if not jobber.Open():
            return False
        log.info("   Copying 'SampleAnalyzer' source files...")
        if not jobber.CopyLHEAnalysis():
            return False
        if not jobber.CreateBldDir(
            analysisName="DelphesRun", outputName="DelphesRun.saf"
        ):
            return False
        if not jobber.WriteSelectionHeader(self.main):
            return False
        if not jobber.WriteSelectionSource(self.main):
            return False

        # remove default user selection files if present
        try:
            (recast_path / org_rel / "user.h").unlink(missing_ok=True)
            (recast_path / org_rel / "user.cpp").unlink(missing_ok=True)
        except Exception as err:
            log.debug("Could not remove user files: %s", err)

        log.info("   Writing the list of datasets...")
        jobber.WriteDatasetList(dataset)
        log.info("   Creating Makefiles...")
        if not jobber.WriteMakefiles(ma5_fastjet_mode=False):
            return False

        # Build analysisList.h and copy analyzer files from the pad
        analysis_list_path = recast_path / org_rel / "analysisList.h"
        pad_path = Path(self.pad)
        pad_org = pad_path / org_rel
        recast_org = recast_path / org_rel

        try:
            with analysis_list_path.open("w", encoding="utf-8") as f:
                for ana in analysislist:
                    f.write(f'#include "SampleAnalyzer/User/Analyzer/{ana}.h"\n')
                f.write('#include "SampleAnalyzer/Process/Analyzer/AnalyzerManager.h"\n')
                f.write('#include "SampleAnalyzer/Commons/Service/LogStream.h"\n\n')
                f.write(
                    "// -----------------------------------------------------------------------------\n"
                )
                f.write("// BuildUserTable\n")
                f.write(
                    "// -----------------------------------------------------------------------------\n"
                )
                f.write("void BuildUserTable(MA5::AnalyzerManager& manager)\n{\n")
                f.write("  using namespace MA5;\n")
                for ana in analysislist:
                    pad_cpp = pad_org / f"{ana}.cpp"
                    pad_h = pad_org / f"{ana}.h"
                    rec_cpp = recast_org / f"{ana}.cpp"
                    rec_h = recast_org / f"{ana}.h"
                    # require header, cpp may be optional (but typically present)
                    if not pad_h.exists():
                        log.error("Missing analysis header in PAD: %s", pad_h)
                        return False
                    shutil.copyfile(str(pad_h), str(rec_h))
                    if pad_cpp.exists():
                        shutil.copyfile(str(pad_cpp), str(rec_cpp))
                    else:
                        log.debug(
                            "No .cpp for %s in PAD; continuing with header only.", ana
                        )
                    f.write(f'  manager.Add("{ana}", new {ana});\n')
                f.write("}\n")
        except Exception as err:
            log.error("Cannot prepare analysisList.h: %s", err)
            return False

        # Update main executable: backup and create modified main.cpp
        main_base = recast_path / "Build" / "Main" / "main"
        main_cpp = main_base.with_suffix(".cpp")
        main_bak = main_base.with_suffix(".bak")
        try:
            shutil.move(str(main_cpp), str(main_bak))
        except Exception as err:
            log.error("Cannot backup main.cpp: %s", err)
            return False

        try:
            with main_bak.open("r", encoding="utf-8") as infile:
                lines = infile.readlines()
        except Exception as err:
            log.error("Cannot read main.bak: %s", err)
            return False

        new_lines = []
        ignore = False
        for line in lines:
            if "// Getting pointer to the analyzer" in line:
                ignore = True
                new_lines.append(line)
                for analysis in analysislist:
                    new_lines.append(
                        f"  std::map<std::string, std::string> param_{analysis};\n"
                    )
                    new_lines.append(f"  AnalyzerBase* analyzer_{analysis}=\n")
                    new_lines.append(
                        f'    manager.InitializeAnalyzer("{analysis}", "{analysis}.saf", param_{analysis});\n'
                    )
                    new_lines.append(f"  if (analyzer_{analysis}==0) return 1;\n\n")
                continue
            if (
                "// Post initialization (creates the new output directory structure)"
                in line
                and self.TACO_output != ""
            ):
                new_lines.append(line)
                new_lines.append(
                    f'    std::ofstream out;\n      out.open("../Output/{self.TACO_output}");\n'
                )
                new_lines.append("      manager.HeadSR(out);\n      out << std::endl;\n")
                continue
            if "//Getting pointer to fast-simulation package" in line:
                ignore = False
                new_lines.append(line)
                continue
            if "!analyzer1" in line and not ignore:
                ignore = True
                for analysis in analysislist:
                    new_lines.append(
                        f"      if (!analyzer_{analysis}->Execute(mySample,myEvent)) continue;\n"
                    )
                if self.TACO_output != "":
                    new_lines.append("\n      manager.DumpSR(out);\n")
                continue
            if "    }" in line:
                new_lines.append(line)
                ignore = False
                continue
            if "manager.Finalize(mySamples,myEvent);" in line and self.TACO_output != "":
                new_lines.append(line)
                new_lines.append("  out.close();\n")
                continue
            if not ignore:
                new_lines.append(line)

        try:
            with main_cpp.open("w", encoding="utf-8") as outfile:
                outfile.writelines(new_lines)
        except Exception as err:
            log.error("Cannot write new main.cpp: %s", err)
            return False

        # Fix pileup in the card copied into the run folder
        if not self.fix_pileup(str(recast_path / "Input" / card)):
            return False

        # Compile / Link / Run
        log.info("   Compiling 'SampleAnalyzer'...")
        if not jobber.CompileJob():
            log.error("job submission aborted.")
            return False
        log.info("   Linking 'SampleAnalyzer'...")
        if not jobber.LinkJob():
            log.error("job submission aborted.")
            return False

        log.info("   Running 'SampleAnalyzer' over dataset '%s'...", dataset.name)
        log.info("    *******************************************************")
        if not jobber.RunJob(dataset):
            log.error("run over '%s' aborted.", dataset.name)
            return False
        log.info("    *******************************************************")

        # Restoring the run
        self.main.recasting.status = "on"
        self.main.fastsim.package = "none"

        event_path = next(
            (
                x
                for x in (recast_path / f"Output/SAF/_{dataset.name}").iterdir()
                if "RecoEvents" in str(x)
            ),
            None,
        )
        if event_path is not None:
            root_path = event_path / "DelphesEvents.root"
            if root_path.is_file():
                main_event_path = (
                    Path(self.dirname) / f"Output/SAF/{dataset.name}/RecoEvents"
                )
                main_event_path.mkdir(parents=True, exist_ok=True)
                moved_smp = (
                    main_event_path
                    / f"RecoEvents_{version}_{card.replace('.tcl', '')}.root"
                )
                shutil.move(str(root_path), str(moved_smp))

        return True

    def run_SimplifiedFastSim(
        self, dataset: DatasetCollection, card: str, analysislist: list[str]
    ) -> bool:
        """
        One-line summary
        Run the Simplified Fast Simulation (SFS) workflow for a dataset.

        Extended summary
        Rejects already reconstructed inputs, loads the analysis card into the interpreter,
        prepares a SFS run directory, writes analyzers from PAD, patches main, compiles, links
        and runs the analysis and moves produced outputs into the main Output/SAF layout.

        Args:
            dataset (``DatasetCollection``): Dataset collection to process.
            card (``str``): Path to the analysis card to load.
            analysislist (``list[str]``): List of analyzers to include.

        Returns:
            ``bool``:
            True on success, False on error.
        """
        # Reject already-reconstructed inputs
        if any(
            any(x.endswith(ext) for ext in ("root", "lhco", "lhco.gz"))
            for x in dataset.filenames
        ):
            log.error("   Dataset can not contain reconstructed file type.")
            return False

        # Load the analysis card and configure interpreter / fastsim temporarily
        from madanalysis.core.script_stack import ScriptStack

        ScriptStack.AddScript(card)

        self.main.recasting.status = "off"
        self.main.superfastsim.Reset()

        old_script_mode = self.main.script
        self.main.script = True
        try:
            from madanalysis.interpreter.interpreter import Interpreter

            interpreter = Interpreter(self.main)
            interpreter.load(verbose=self.main.developer_mode)
        except Exception as err:
            log.debug(err)
            self.main.script = old_script_mode
            return False
        finally:
            self.main.script = old_script_mode

        old_fastsim = self.main.fastsim.package
        self.main.fastsim.package = "fastjet"

        output_name = None
        if self.main.recasting.store_events:
            output_name = "SFS_events.lhe"
            if self.main.archi_info.has_zlib:
                output_name += ".gz"
            log.debug("   Setting the output LHE file: %s", output_name)

        run_dir = Path(self.dirname + "_SFSRun")
        jobber = JobWriter(self.main, str(run_dir))

        # Prepare build/run directory and analysis sources
        log.info("   Creating folder '%s'...", Path(self.dirname).name)
        if not jobber.Open():
            self.main.fastsim.package = old_fastsim
            return False
        log.info("   Copying 'SampleAnalyzer' source files...")
        if not jobber.CopyLHEAnalysis():
            self.main.fastsim.package = old_fastsim
            return False
        if not jobber.CreateBldDir(analysisName="SFSRun", outputName="SFSRun.saf"):
            self.main.fastsim.package = old_fastsim
            return False
        if not jobber.WriteSelectionHeader(self.main):
            self.main.fastsim.package = old_fastsim
            return False
        # remove potentially generated user files safely
        (run_dir / "Build/SampleAnalyzer/User/Analyzer/user.h").unlink(missing_ok=True)
        if not jobber.WriteSelectionSource(self.main):
            self.main.fastsim.package = old_fastsim
            return False
        (run_dir / "Build/SampleAnalyzer/User/Analyzer/user.cpp").unlink(missing_ok=True)

        log.info("   Writing the list of datasets...")
        jobber.WriteDatasetList(dataset)
        log.info("   Creating Makefiles...")
        if not jobber.WriteMakefiles():
            self.main.fastsim.package = old_fastsim
            return False

        # Create analysisList.h and copy analyzer files from PAD
        analysis_list_path = run_dir / "Build/SampleAnalyzer/User/Analyzer/analysisList.h"
        pad_analyzer_dir = Path(self.pad) / "Build/SampleAnalyzer/User/Analyzer"
        analyzer_dest = run_dir / "Build/SampleAnalyzer/User/Analyzer"
        try:
            analyzer_dest.mkdir(parents=True, exist_ok=True)
            with analysis_list_path.open("w", encoding="utf-8") as f:
                for ana in analysislist:
                    f.write(f'#include "SampleAnalyzer/User/Analyzer/{ana}.h"\n')
                f.write('#include "SampleAnalyzer/Process/Analyzer/AnalyzerManager.h"\n')
                f.write('#include "SampleAnalyzer/Commons/Service/LogStream.h"\n\n')
                if self.main.superfastsim.isTaggerOn():
                    f.write('#include "new_tagger.h"\n')
                if self.main.superfastsim.isNewSmearerOn():
                    f.write('#include "new_smearer_reco.h"\n')
                f.write(
                    "// -----------------------------------------------------------------------------\n"
                )
                f.write("// BuildUserTable\n")
                f.write(
                    "// -----------------------------------------------------------------------------\n"
                )
                f.write("void BuildUserTable(MA5::AnalyzerManager& manager)\n{\n")
                f.write("  using namespace MA5;\n")
                for ana in analysislist:
                    src_cpp = pad_analyzer_dir / f"{ana}.cpp"
                    src_h = pad_analyzer_dir / f"{ana}.h"
                    dst_cpp = analyzer_dest / f"{ana}.cpp"
                    dst_h = analyzer_dest / f"{ana}.h"
                    if not src_h.exists():
                        log.error("Missing analysis header in PAD: %s", src_h)
                        return False
                    shutil.copyfile(str(src_h), str(dst_h))
                    if src_cpp.exists():
                        shutil.copyfile(str(src_cpp), str(dst_cpp))
                    else:
                        log.debug(
                            "No .cpp for %s in PAD; continuing with header only.", ana
                        )
                    f.write(f'  manager.Add("{ana}", new {ana});\n')
                f.write("}\n")
        except Exception as err:
            log.error("Error preparing analysisList.h or copying analyzers: %s", err)
            self.main.fastsim.package = old_fastsim
            return False

        # Modify main to register analyzers / optional writer / TACO output
        try:
            main_path = run_dir / "Build/Main"
            main_cpp = main_path / "main.cpp"
            main_bak = main_path / "main.bak"
            shutil.move(str(main_cpp), str(main_bak))
            with main_bak.open("r", encoding="utf-8") as infile, main_cpp.open(
                "w", encoding="utf-8"
            ) as outfile:
                ignore = False
                for line in infile:
                    if "// Getting pointer to the analyzer" in line:
                        ignore = True
                        outfile.write(line)
                        for analysis in analysislist:
                            outfile.write(
                                f"  std::map<std::string, std::string> prm{analysis};\n"
                            )
                            outfile.write(f"  AnalyzerBase* analyzer_{analysis}=\n")
                            outfile.write(
                                f'    manager.InitializeAnalyzer("{analysis}","{analysis}.saf",prm{analysis});\n'
                            )
                            outfile.write(f"  if (analyzer_{analysis}==0) return 1;\n\n")
                        if output_name:
                            outfile.write("  //Getting pointer to the writer\n")
                            outfile.write("  WriterBase* writer1 = \n")
                            outfile.write(
                                f'      manager.InitializeWriter("lhe","{output_name}");\n'
                            )
                            outfile.write("  if (writer1==0) return 1;\n\n")
                        continue
                    if (
                        "// Post initialization (creates the new output directory structure)"
                        in line
                        and self.TACO_output != ""
                    ):
                        outfile.write(line)
                        outfile.write(
                            f'    std::ofstream out;\n      out.open("../Output/{self.TACO_output}");\n'
                        )
                        outfile.write(
                            "      manager.HeadSR(out);\n      out << std::endl;\n"
                        )
                        continue
                    if "//Getting pointer to the clusterer" in line:
                        ignore = False
                        outfile.write(line)
                        continue
                    if "!analyzer1" in line and not ignore:
                        ignore = True
                        if output_name:
                            outfile.write(
                                "      writer1->WriteEvent(myEvent,mySample);\n"
                            )
                        for analysis in analysislist:
                            outfile.write(
                                f"      if (!analyzer_{analysis}->Execute(mySample,myEvent)) continue;\n"
                            )
                        if self.TACO_output != "":
                            outfile.write("\n      manager.DumpSR(out);\n")
                        continue
                    if "    }" in line:
                        outfile.write(line)
                        ignore = False
                        continue
                    if (
                        "manager.Finalize(mySamples,myEvent);" in line
                        and self.TACO_output != ""
                    ):
                        outfile.write(line)
                        outfile.write("  out.close();\n")
                        continue
                    if not ignore:
                        outfile.write(line)
        except Exception as err:
            log.error("Cannot update main.cpp: %s", err)
            self.main.fastsim.package = old_fastsim
            return False

        # Restore recasting status and fastsim package
        self.main.recasting.status = "on"
        self.main.fastsim.package = old_fastsim

        # Compile, link and run
        log.info("   Compiling 'SampleAnalyzer'...")
        if not jobber.CompileJob():
            log.error("job submission aborted.")
            return False
        log.info("   Linking 'SampleAnalyzer'...")
        if not jobber.LinkJob():
            log.error("job submission aborted.")
            return False

        log.info("   Running 'SampleAnalyzer' over dataset '%s'...", dataset.name)
        log.info("    *******************************************************")
        if not jobber.RunJob(dataset):
            log.error("run over '%s' aborted.", dataset.name)
            return False
        log.info("    *******************************************************")

        # Move produced SAF/cutflows/histograms/events into main Output/SAF layout
        out_base = Path(self.dirname) / "Output/SAF" / dataset.name
        out_base.mkdir(parents=True, exist_ok=True)

        sfs_out_base = run_dir / "Output/SAF" / f"_{dataset.name}"
        for analysis in analysislist:
            dest_analysis = out_base / analysis
            (dest_analysis / "Cutflows").mkdir(parents=True, exist_ok=True)
            (dest_analysis / "Histograms").mkdir(parents=True, exist_ok=True)
            if self.main.recasting.store_events:
                (dest_analysis / "RecoEvents").mkdir(parents=True, exist_ok=True)

            src_analysis_dir = sfs_out_base / f"{analysis}_0"
            # Cutflows
            src_cutflows = src_analysis_dir / "Cutflows"
            if src_cutflows.is_dir():
                for src in src_cutflows.iterdir():
                    shutil.move(str(src), str(dest_analysis / "Cutflows" / src.name))
            # Histograms
            src_histos = src_analysis_dir / "Histograms"
            if src_histos.is_dir():
                for src in src_histos.iterdir():
                    shutil.move(str(src), str(dest_analysis / "Histograms" / src.name))

            # Move event file if any
            if self.main.recasting.store_events:
                src_event_dir = sfs_out_base / "lheEvents0_0"
                if src_event_dir.is_dir():
                    # move first event file found
                    try:
                        first = next(src_event_dir.iterdir())
                        shutil.move(
                            str(first),
                            str(dest_analysis / "RecoEvents" / first.name),
                        )
                    except StopIteration:
                        pass

        # Move dataset .saf if produced
        saf_src = sfs_out_base / f"_{dataset.name}.saf"
        if saf_src.exists():
            shutil.move(str(saf_src), str(out_base / f"{dataset.name}.saf"))

        # Move TACO_output if requested
        if self.TACO_output:
            taco_src = run_dir / "Output" / self.TACO_output
            if taco_src.exists():
                filename = (
                    ".".join(self.TACO_output.split(".")[:-1])
                    + "_"
                    + Path(card).name.replace("ma5", "")
                    + self.TACO_output.split(".")[-1]
                )
                shutil.move(str(taco_src), str(out_base / filename))

        # Cleanup the SFS run directory unless in developer mode
        if not self.main.developer_mode:
            if not FolderWriter.RemoveDirectory(str(run_dir)):
                log.error("Cannot remove directory: %s", run_dir)
        else:
            log.debug("Analysis kept in %s folder.", run_dir)

        return True

    ################################################
    ### ANALYSIS EXECUTION
    ################################################
    def analysis_single(self, version: str, card: str) -> bool:
        """
        One-line summary
        Perform a single analysis version/card recasting including PAD execution and CLs.

        Extended summary
        Selects the appropriate detector, prepares analyzer list for the given card,
        executes the PAD or SFS runs over all datasets, manages eventfile postprocessing
        and optionally triggers CLs computation.

        Args:
            version (``str``): PAD version identifier (e.g. 'v1.2').
            card (``str``): Analysis card name.

        Returns:
            ``bool``:
            True on success for all datasets and CLs calculations, False otherwise.
        """
        ## Init and header
        self.analysis_header(version, card)

        # Activating the right delphes
        detector_handler = DetectorManager(self.main)
        if not detector_handler.manage(self.detector):
            log.error("Problem with the activation of delphesMA5tune")
            return False

        ## Getting the analyses associated with the given card
        analyses = [ana for v, ana in self.analysis_runcard if version == v]
        for del_card, ana_list in self.main.recasting.DelphesDic.items():
            if card == del_card:
                analyses = [x for x in analyses if x in ana_list]
                break

        # Executing the PAD
        for myset in self.main.datasets:
            if not self.main.recasting.stat_only_mode:
                if version in ["v1.1", "v1.2"]:
                    if not self.run_delphes_analysis(myset, card, analyses):
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
                        log.error(f"The file called {eventfile} is not found...")
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
                        f"{self.main.archi_info.ma5dir}/tools/PADForSFS/Input/Cards/{card}",
                        analyses,
                    ):
                        return False
                    if self.main.recasting.store_root:
                        log.warning(
                            "Simplified-FastSim does not use root, hence file will not be stored."
                        )

                if myset.xsection == 0.0:
                    myset.xsection = read_xsec(
                        f"{self.dirname}/Output/SAF/{myset.name}/{myset.name}.saf"
                    )
                    log.debug(f"Cross-section has been set to {myset.xsection} pb.")
            else:
                self.dirname = self.main.recasting.stat_only_dir
            ## Running the CLs exclusion script (if available)
            if not self.main.recasting.analysis_only_mode:
                log.debug(f"Compute CLs exclusion for {myset.name}")
                if not self.compute_cls(analyses, myset):
                    self.main.forced = self.forced
                    return False

        # Exit
        return True

    def analysis_header(self, version: str, card: str) -> None:
        """
        One-line summary
        Log a standardized header for a PAD run.

        Extended summary
        Prints a nicely formatted banner with the PAD version and card name to the log.

        Args:
            version (``str``): PAD version string.
            card (``str``): Card filename or identifier.

        Returns:
            ``None``:
            Pure logging side-effect.
        """
        ## Printing
        log.info("   **********************************************************")
        log.info(
            "   "
            + StringTools.Center(
                version + " running of the PAD" + " on events generated with", 57
            )
        )
        log.info("   " + StringTools.Center(card, 57))
        log.info("   **********************************************************")

    def update_pad_main(self, analysislist: list[str]) -> bool:
        """
        One-line summary
        Update the PAD main.cpp for a set of analyzers and copy required analyzer sources.

        Extended summary
        Creates/overwrites the analysisList.h and a modified main.cpp inside the RecastRun
        directory to register the specified analyzers. Also copies analyzer headers and
        sources from the PAD into the run directory.

        Args:
            analysislist (``list[str]``): List of analyzer names to include in the PAD run.

        Returns:
            ``bool``:
            True on success, False if required files are missing or on I/O errors.
        """
        ## Migrating the necessary files to the working directory
        log.info("   Writing the PAD analyses")
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

    def make_pad(self) -> bool:
        """
        One-line summary
        Compile the PAD library within the RecastRun build directory.

        Extended summary
        Invokes 'make' with an appropriate core count and captures the compilation log.
        Returns False if compilation fails.

        Returns:
            ``bool``:
            True if make succeeded, False otherwise.
        """
        # Initializing the compiler
        log.info("   Compiling the PAD located in %s_RecastRun", self.dirname)
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
            log.error(
                "Impossible to compile the PAD. For more details, see the log file:"
            )
            log.error(logfile)
            return False
        return True

    def save_output(
        self, eventfile: str, setname: str, analyses: list[str], card: str
    ) -> bool:
        """
        One-line summary
        Save and merge produced SAF outputs and move analyzer outputs to the main Output/SAF.

        Extended summary
        If the target SAF doesn't exist, moves the produced SAF file; otherwise merges
        file entries. Moves analyzer-specific directories and TACO outputs if requested.

        Args:
            eventfile (``str``): Event file path string (may include quotes).
            setname (``str``): Dataset name.
            analyses (``list[str]``): List of analyses to move into Output/SAF.
            card (``str``): Card name used to generate TACO outputs.

        Returns:
            ``bool``:
            True when outputs were moved/merged successfully.
        """
        outfile = self.dirname + "/Output/SAF/" + setname + "/" + setname + ".saf"
        if not os.path.isfile(outfile):
            shutil.move(
                self.dirname + f"_RecastRun/Output/SAF/_{setname}/_{setname}.saf", outfile
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
                self.dirname + f"_RecastRun/Output/SAF/_{setname}/" + analysis + "_0",
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

    def compute_cls(self, analyses: list[str], dataset: DatasetCollection) -> bool:
        """
        One-line summary
        Compute CLs exclusion limits for the provided analyses and dataset.

        Extended summary
        Validates XML parsing support, writes bibliography, iterates over requested
        extrapolated luminosities and analyses, parses analysis info files, reads cutflows,
        constructs statistical models and computes limits. Writes results into CLs output files.

        Args:
            analyses (``list[str]``): List of analysis names to compute CLs for.
            dataset (``DatasetCollection``): Dataset metadata used for the computation.

        Returns:
            ``bool``:
            True on success for all computations, False on any encountered error.
        """
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

        log.info("\033[1m   * Exclusion limit computation uses Spey package\033[0m")
        log.info("\033[1m     Please cite arXiv:2307.06996 [hep-ph]\033[0m")

        # Bibliography
        bibfile = os.path.join(self.dirname, "bibliography.bib")
        print_gl_citation = self.main.recasting.global_likelihoods_switch
        with open(bibfile, "w") as bib:
            try:
                bib.write(spey.cite() + "\n")
            except Exception as err:
                log.debug(err)
                pass
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
            log.info(
                "\033[1m   * Using Uncertainties and Higher-Luminosity Estimates\033[0m"
            )
            log.info("\033[1m     Please cite arXiv:1910.11418 [hep-ph]\033[0m")

        ## Running over all luminosities to extrapolate
        for extrapolated_lumi in [
            "default"
        ] + self.main.recasting.extrapolated_luminosities:
            log.info(
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
                log.debug(f"Running CLs exclusion calculation for {analysis}")
                # Getting the info file information (possibly rescaled)
                lumi, regions, regiondata = self.parse_info_file(
                    ET, analysis, extrapolated_lumi
                )
                log.debug(f"lumi = {str(lumi)}")
                log.debug(f"regions = {str(regions)}")
                log.debug(f"regiondata = {str(regiondata)}")
                if lumi == -1 or regions == -1 or regiondata == -1:
                    log.warning(
                        f"Info file for {analysis} missing or corrupted. Skipping the CLs calculation."
                    )
                    return False

                # Citation notifications for Global Likelihoods
                if (self.cov_config or self.pyhf_config) and print_gl_citation:
                    print_gl_citation = False
                    log.info(
                        "\033[1m   * Using global likelihoods to improve CLs calculations\033[0m"
                    )
                    log.info("\033[1m     Please cite arXiv:2206.14870 [hep-ph]\033[0m")
                    if self.pyhf_config != {}:
                        log.info(
                            "\033[1m                 pyhf DOI:10.5281/zenodo.1169739\033[0m"
                        )
                        log.info(
                            "\033[1m                 For more details see https://scikit-hep.org/pyhf/\033[0m"
                        )
                        if (
                            self.main.recasting.simplify_likelihoods
                            and self.main.session_info.has_simplify
                        ):
                            log.info(
                                "\033[1m                 using simplify: ATL-PHYS-PUB-2021-038\033[0m"
                            )
                            log.info(
                                "\033[1m                 For more details see https://github.com/eschanet/simplify\033[0m"
                            )
                    elif self.cov_config:
                        log.info("\033[1m                 CMS-NOTE-2017-001\033[0m")

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
                    log.warning(
                        f"Info file for {analysis} corrupted. Skipping the CLs calculation."
                    )
                    return False

                if dataset.xsection == 0.0:
                    log.error(
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
        """
        One-line summary
        Determine an XML parsing module to use (lxml or xml.etree.ElementTree).

        Extended summary
        Tries to import lxml.etree first and falls back to xml.etree.ElementTree.
        Logs and returns False if neither is available.

        Returns:
            ``module``:
            The imported XML module on success, or False on failure.
        """
        ## Checking XML parsers
        try:
            from lxml import ET
        except ImportError as err:
            log.debug(str(err))
            try:
                import xml.etree.ElementTree as ET
            except ImportError as err2:
                log.warning("lxml or xml not available... the CLs module cannot be used")
                log.debug(str(err2))
                return False
        # exit
        return ET

    def parse_info_file(
        self, etree, analysis: str, extrapolated_lumi: Union[str, float]
    ) -> tuple[float, list, dict]:
        """
        One-line summary
        Parse an analysis .info XML file and extract header information.

        Extended summary
        Opens and parses the analysis.info file using the provided etree module and
        delegates extraction to header_info_file. Returns (-1,-1,-1) on errors.

        Args:
            etree (``module``): XML parsing module (e.g. xml.etree.ElementTree).
            analysis (``str``): Analyzer name (without extension) to parse.
            extrapolated_lumi (``str`` or ``float``): 'default' or a numeric extrapolated luminosity.

        Returns:
            ``tuple``:
            (lumi (float), regions (list), regiondata (dict)) on success or (-1,-1,-1) on error.
        """
        ## Is file existing?
        filename = (
            self.pad + "/Build/SampleAnalyzer/User/Analyzer/" + analysis + ".info"
        )  # ERIC
        if not os.path.isfile(filename):
            log.warning("Info " + filename + " does not exist...")
            return -1, -1, -1
        ## Getting the XML information
        try:
            with open(filename, "r") as info_input:
                info_tree = etree.parse(info_input)
        except Exception as err:
            log.warning("Error during XML parsing: " + str(err))
            log.warning("Cannot parse the info file")
            return -1, -1, -1

        try:
            results = self.header_info_file(info_tree, analysis, extrapolated_lumi)
            return results
        except Exception as err:
            log.warning("Error during extracting header info file: " + str(err))
            log.warning("Cannot parse the info file")
            return -1, -1, -1

    def fix_pileup(self, filename: str) -> bool:
        """
        One-line summary
        Ensure Delphes card references point to local PAD pileup files.

        Extended summary
        Backs up the provided tcl card, scans for 'set PileUpFile' directives and rewrites
        the referenced path to point to the PAD/Input/Pileup directory inside the MA5 installation.
        Verifies that the referenced pileup files exist after modification.

        Args:
            filename (``str``): Path to the Delphes .tcl card file to fix.

        Returns:
            ``bool``:
            True if pileup entries were fixed and referenced files exist, False on error.
        """
        # x
        filename = str(filename)
        log.debug("delphes card is here: %s", filename)

        # Container for pileup
        FoundPileup = []

        # Safe
        if not os.path.isfile(filename):
            log.error("internal error: file %s is not found", filename)
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

    def header_info_file(
        self, etree, analysis: str, extrapolated_lumi: Union[str, float]
    ):
        """
        One-line summary
        Extract header-level information from a parsed analysis info XML tree.

        Extended summary
        Validates root tags, extracts the analysis luminosity and region definitions, handles
        covariance and pyhf blocks, rescales rates for extrapolated luminosities and
        returns (lumi, regions, regiondata). Performs extensive validation and returns -1 triplet on error.

        Args:
            etree (``xml.etree.ElementTree.ElementTree``): Parsed XML tree (root accessible via getroot()).
            analysis (``str``): Analyzer name (for logging and validation).
            extrapolated_lumi (``str`` or ``float``): 'default' or numeric target luminosity for rescaling.

        Returns:
            ``tuple``:
            (lumi (float), regions (list), regiondata (dict)) on success or (-1,-1,-1) on failure.
        """
        log.debug("Reading info from the file related to %s...", analysis)
        ## checking the header of the file
        info_root = etree.getroot()
        if info_root.tag != "analysis":
            log.warning("Invalid info file (%s): <analysis> tag.", analysis)
            return -1, -1, -1
        if info_root.attrib["id"].lower() != analysis.lower():
            log.warning("Invalid info file (%s): <analysis id> tag.", analysis)
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
        if self.main.recasting.global_likelihoods_switch and self.cov_config == {}:
            try:
                self.pyhf_config = self.pyhf_info_file(info_root)
                log.debug(str(self.pyhf_config))
            except Exception as err:
                log.debug("Check pyhf_info_file function!\n" + str(err))
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
                    log.warning("Invalid info file (" + analysis + "): ill-defined lumi")
                    log.debug(str(err))
                    return -1, -1, -1
                log.debug("The luminosity of " + analysis + " is " + str(lumi) + " fb-1.")
            # regions
            if child.tag == "region" and (
                "type" not in child.attrib or child.attrib["type"] == "signal"
            ):
                if "id" not in child.attrib:
                    log.warning("Invalid info file (" + analysis + "): <region id> tag.")
                    return -1, -1, -1
                if child.attrib["id"] in regions:
                    log.warning(
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
                    # log.debug(rchild.tag)
                    # log.debug(str(lumi)+' '+str(regions)+ ' '+str(regiondata))
                    try:
                        myval = float(rchild.text)
                    except ValueError as err:
                        log.warning(
                            "Invalid info file ("
                            + analysis
                            + "): region data ill-defined."
                        )
                        log.debug(str(err))
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
                        log.warning(
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

    def pyhf_info_file(self, info_root) -> dict:
        """
        One-line summary
        Extract and validate pyhf-related configuration from an analysis info XML root.

        Extended summary
        If <pyhf> blocks are present, attempts to import spey_pyhf, constructs the
        HistFactory dictionary and validates likelihood profiles. Returns an empty dict
        if pyhf support is missing or validation fails.

        Args:
            info_root (``xml.etree.ElementTree.Element``): Root element of the parsed info file.

        Returns:
            ``dict``:
            A validated pyhf configuration dictionary, or {} if none or invalid.
        """
        self.pyhf_config = {}  # reset
        if any(x.tag == "pyhf" for x in info_root):
            # pyhf_path = os.path.join(self.main.archi_info.ma5dir, 'tools/pyhf/pyhf-master/src')
            try:
                # if os.path.isdir(pyhf_path) and pyhf_path not in sys.path:
                #     sys.path.insert(0, pyhf_path)
                import spey_pyhf

                log.debug("spey_pyhf v" + str(spey_pyhf.__version__))
                log.debug(
                    "spey_pyhf has been imported from " + " ".join(spey_pyhf.__path__)
                )
            except ImportError:
                log.warning(
                    "To use the global likelihood machinery, please install spey-pyhf"
                )
                return {}
            except Exception as err:
                log.debug("Problem with pyhf_info_file function!!")
                log.debug(str(err))
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
                log.debug(
                    'Likelihood profile "' + str(likelihood_profile) + '" is valid.'
                )
            else:
                log.warning(
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

    def write_cls_header(self, xs: float, out) -> None:
        """
        One-line summary
        Write the header of a CLs output file depending on whether signal xsec is known.

        Extended summary
        Produces a human-readable header describing columns written in CLs output .dat files.
        If systematics are configured, the header includes corresponding columns.

        Args:
            xs (``float``): Signal cross section, used to select header format.
            out (``file``): Open file-like object to write the header into.

        Returns:
            ``None``:
            Writes into the provided file object.
        """
        if xs <= 0:
            log.info(
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

    def read_cutflows(self, path: str, regions: list[str], regiondata: dict) -> dict:
        """
        One-line summary
        Read per-region SAF cutflow files and populate regiondata with initial and final counts.

        Extended summary
        For each requested signal region (or combined regions), opens the corresponding .saf
        file, extracts initial and final sums of weights and updates regiondata with N0 and Nf.
        Returns -1 on any validation or parsing error.

        Args:
            path (``str``): Directory containing region .saf cutflow files.
            regions (``list[str]``): List of region identifiers to read.
            regiondata (``dict``): Pre-initialized region data dictionary to update.

        Returns:
            ``dict``:
            Updated regiondata on success, or -1 on failure.
        """
        log.debug("Read the cutflow from the files:")
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
                log.debug("+ " + filename)
                if not os.path.isfile(filename):
                    log.warning(
                        "Cannot find a cutflow for the region "
                        + regiontocombine
                        + " in "
                        + path
                    )
                    log.warning("Skipping the CLs calculation.")
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
                    log.warning(
                        "Invalid cutflow for the region "
                        + reg
                        + "("
                        + regname
                        + ") in "
                        + path
                    )
                    log.warning("Skipping the CLs calculation.")
                    return -1
                Nf += myNf
                N0 += myN0
            if Nf == 0 and N0 == 0:
                log.warning(
                    "Invalid cutflow for the region "
                    + reg
                    + "("
                    + regname
                    + ") in "
                    + path
                )
                log.warning("Skipping the CLs calculation.")
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
        """
        One-line summary
        Compute CLs and related quantities for each region using provided statistical models.

        Extended summary
        Uses the different statistical model containers (uncorrelated, simplified, pyhf)
        to compute rSR, CLs and mark the best region(s). Also handles covariant subsets and pyhf
        results. Returns the enriched regiondata dictionary.

        Args:
            regiondata (``dict``): Per-region data with N0/Nf and other entries.
            stat_models (``dict``): Statistical model objects keyed by model type.
            xsection (``float``): Signal cross section used to compute expected counts.
            lumi (``float``): Luminosity used to scale expected signals.
            is_extrapolated (``bool``): Whether the computation is for an extrapolated luminosity.

        Returns:
            ``dict``:
            Updated regiondata with CLs, rSR, best flags and related fields.
        """
        from .statistical_models import APRIORI, OBSERVED

        log.debug("Compute CLs...")
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
    ) -> None:
        """
        One-line summary
        Write final CLs tabulated output for each region and optional global results.

        Extended summary
        Formats efficiency, statistical and systematic bands, global likelihoods (SL/pyhf)
        and writes them into the provided summary file object. When in developer_mode,
        optionally dumps json debug files for pyhf.

        Args:
            analysis (``str``): Analysis name.
            regions (``list[str]``): Ordered list of regions to write.
            regiondata (``dict``): Computed per-region results (CLs, N0, Nf, etc.).
            errordata (``dict``): Error-variation results keyed by variation names.
            summary (``file``): Open file-like object to append the CLs results.
            xsflag (``bool``): Indicates whether signal x-section is undefined (True) or present (False).
            lumi (``float``): Luminosity used in the computation (fb^-1).

        Returns:
            ``None``:
            Writes formatted results to 'summary'.
        """
        log.debug("Write CLs...")
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
            log.debug("Saving dictionary : " + name)
            with open(name, "w+") as results:
                json.dump(to_save, results, indent=4)
            ###################################################################################
            # @Jack : For debugging purposes in the future. This slice of code
            #         prints the Json file for signal WITH XSEC=1 !!!
            if self.pyhf_config != {}:
                iterator = copy.deepcopy(list(self.pyhf_config.items()))
                for n, (likelihood_profile, config) in enumerate(iterator):
                    if not regiondata.get("pyhf", {}).get(likelihood_profile, False):
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
            eff = max(regiondata[reg]["Nf"] / regiondata[reg]["N0"], 0.0)
            stat = round(
                math.sqrt(eff * (1 - eff) / (abs(regiondata[reg]["N0"]) * lumi)), 10
            )
            syst = []
            if len(self.main.recasting.systematics) > 0:
                for unc in self.main.recasting.systematics:
                    syst.append(round(0.5 * (unc[0] + unc[1]) * eff, 8))
            else:
                syst = [0]
            myeff = f"{eff:.7f}"
            mystat = f"{stat:.7f}"
            mysyst = [f"{x:.7f}" for x in syst]
            myxsexp = regiondata[reg]["s95exp"]
            if "s95obs" in list(regiondata[reg].keys()):
                myxsobs = regiondata[reg]["s95obs"]
            else:
                myxsobs = "-1"
            if not xsflag:
                mycls = f"{regiondata[reg]['CLs']:.7f}"
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
                            + (f"{min(band):.4f}")
                            + ", "
                            + (f"{max(band):.4f}")
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
                log.debug(str(pyhf_data))
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
