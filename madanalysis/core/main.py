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


"""Global state of a MadAnalysis 5 session.

The :class:`Main` object created in :func:`madanalysis.core.launcher.MainSession` is the
single container of the session state. It is passed to (and mutated by) virtually
every component: the interpreter commands, the job writers, the layout/report
generators and the recasting machinery. It also owns the logic checking the system
configuration (:meth:`Main.CheckConfig`, :meth:`Main.CheckConfig2`) and building the
SampleAnalyzer C++ libraries (:meth:`Main.BuildLibrary`).
"""

from __future__ import absolute_import
from __future__ import annotations
from madanalysis.multiparticle.multiparticle_collection import MultiParticleCollection
from madanalysis.dataset.dataset_collection import DatasetCollection
from madanalysis.selection.selection import Selection
from madanalysis.interpreter.cmd_base import CmdBase
from madanalysis.region.region_collection import RegionCollection
from madanalysis.fastsim.fastsim import SuperFastSim
from madanalysis.jet_clustering.jet_collection import JetCollection
from madanalysis.system.session_info import SessionInfo
from madanalysis.system.architecture_info import ArchitectureInfo
from madanalysis.core.library_builder import LibraryBuilder
from madanalysis.IOinterface.library_writer import LibraryWriter
from madanalysis.IOinterface.madgraph_interface import MadGraphInterface
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.enumeration.stacking_method_type import StackingMethodType
from madanalysis.enumeration.normalize_type import NormalizeType
from madanalysis.enumeration.graphic_render_type import GraphicRenderType
from madanalysis.observable.observable_manager import ObservableManager
from madanalysis.configuration.recast_configuration import RecastConfiguration
from madanalysis.configuration.fastsim_configuration import FastsimConfiguration
from madanalysis.configuration.fom_configuration import FomConfiguration
from madanalysis.configuration.isolation_configuration import IsolationConfiguration
from madanalysis.configuration.merging_configuration import MergingConfiguration
from madanalysis.system.checkup import CheckUp
import logging
import os
import sys
from six.moves import range
import traceback as tb

# pylint: disable=too-many-instance-attributes,import-outside-toplevel, import-error,consider-using-enumerate,logging-not-lazy,logging-fstring-interpolation


class Main:
    """Session state of MadAnalysis 5 (the ``main`` object of the interpreter).

    The attributes that can be modified by the user with ``set main.<attribute> =
    <value>`` are listed in :attr:`userVariables`. The most important attributes are:

    * ``mode`` (:class:`~madanalysis.enumeration.ma5_running_type.MA5RunningType`):
      parton, hadron or reco running mode;
    * ``archi_info`` / ``session_info``: detected system configuration;
    * ``datasets``, ``multiparticles``, ``regions``, ``selection``: user-defined
      datasets, (multi)particles, signal regions and plots/cuts;
    * ``fastsim`` / ``superfastsim`` / ``jet_collection``: detector simulation
      (FastJet, Delphes, SFS) and extra jet collections;
    * ``recasting``, ``merging``, ``isolation``, ``fom``: sub-configurations;
    * ``lumi``, ``normalize``, ``stack``, ``graphic_render``, ``output``,
      ``random_seed``: global settings of the analysis and of the reports;
    * ``forced`` / ``script`` / ``developer_mode`` / ``expertmode``: running options;
    * ``repeatSession``: set to ``True`` to request a restart of the session;
    * ``lastjob_name`` / ``lastjob_status``: last submitted job (for ``resubmit``).

    .. note::
        ``forced``, ``version`` and ``date`` also exist as **class** attributes;
        ``Main.forced`` is read by components that do not have access to the instance.
    """
    userVariables = {
        "currentdir": [],
        "normalize": ["none", "lumi", "lumi_weight"],
        "graphic_render": ["root", "matplotlib", "none"],
        "lumi": [],
        "stacking_method": ["stack", "superimpose", "normalize2one"],
        "outputfile": ['"output.lhe.gz"', '"output.lhco.gz"'],
        "recast": ["on", "off"],
        "random_seed": ["47"],
    }

    forced = False
    version = ""
    date = ""

    def __init__(self) -> None:
        """Initialise a session in parton-level mode with default settings.

        .. note::
            ``currentdir`` is a property: the assignment below calls
            :meth:`set_currentdir`, which changes directory (to the current one) and
            logs the current directory.
        """
        # NOTE: 'currentdir' is a property (see set_currentdir): this assignment calls os.chdir.
        self.currentdir = os.getcwd()
        self.firstdir = os.getcwd()
        self.archi_info = ArchitectureInfo()
        self.session_info = SessionInfo()
        self.mode = MA5RunningType.PARTON
        self.forced = False
        self.multiparticles = MultiParticleCollection()
        self.datasets = DatasetCollection()
        self.regions = RegionCollection()
        self.selection = Selection()
        self.script = False
        self.observables = ObservableManager(self.mode)
        self.expertmode = False
        self.repeatSession = False
        self.developer_mode = False
        self.recast = "off"
        self.ResetParameters()
        self.madgraph = MadGraphInterface()
        self.logger = logging.getLogger("MA5")
        self.redirectSAlogger = False
        self.random_seed = None

    def ResetParameters(self) -> None:
        r"""Reset the sub-configurations and global analysis settings to their defaults.

        Recreates the merging, fastsim, SFS, jet collection, recasting, figure-of-merit
        and isolation configurations, and resets the luminosity (10 fb\ :sup:`-1`), the
        stacking method, the output file, the graphical renderer and the
        normalisation (``none`` in reco mode, ``lumi_weight`` otherwise). Datasets,
        multiparticles, regions and the selection are **not** reset.
        """
        self.merging = MergingConfiguration()
        self.fastsim = FastsimConfiguration()
        self.superfastsim = SuperFastSim()
        self.jet_collection = JetCollection()
        self.recasting = RecastConfiguration()
        self.fom = FomConfiguration()
        self.lumi = 10
        self.lastjob_name = ""
        self.lastjob_status = False
        self.random_seed = None
        self.stack = StackingMethodType.STACK
        self.isolation = IsolationConfiguration()
        self.output = ""
        self.graphic_render = GraphicRenderType.NONE
        if self.mode == MA5RunningType.RECO:
            self.normalize = NormalizeType.NONE
        else:
            self.normalize = NormalizeType.LUMI_WEIGHT
        self.superfastsim.InitObservables(self.observables)

    def InitObservables(self, mode: int) -> None:
        """Rebuild the list of observables available for a given running mode.

        Args:
            mode (``int``): running mode, a value of
                :class:`~madanalysis.enumeration.ma5_running_type.MA5RunningType`.
        """
        self.observables = ObservableManager(mode)
        self.superfastsim.InitObservables(self.observables)

    def IsGoodFormat(self, file: str) -> bool:
        """Check whether an event file can be read in the current session.

        Args:
            file (``str``): name of the event file.

        Returns:
            ``bool``:
            ``True`` if the file extension is among the allowed ones of
            :meth:`GetSampleFormat`, ``False`` otherwise.
        """
        allowed, forbidden = self.GetSampleFormat()
        for item in allowed:
            if file.endswith(item):
                return True
        return False

    def PrintErrorFormat(self, file: str) -> str:
        """Get the reason why an event file cannot be read in the current session.

        Args:
            file (``str``): name of the event file.

        Returns:
            ``str``:
            The error message associated with the (forbidden) file extension, or
            ``"The file format is unknown"`` if the extension is not referenced.
        """
        allowed, forbidden = self.GetSampleFormat()
        for item in forbidden:
            if file.endswith(item[0]):
                return item[1]
        return "The file format is unknown"

    def GetSampleFormat(self) -> tuple[list[str], list[list[str]]]:
        """Get the event-file extensions allowed and forbidden in the current session.

        The result depends on the running mode, on the recasting status, on the
        selected fast-simulation package and on the availability of zlib (``.gz``
        files) and Delphes/ROOT (``.root`` files). Every extension is also accepted
        with the ``.fifo`` suffix (named pipes).

        Returns:
            ``tuple[list[str], list[list[str]]]``:
            The list of allowed extensions, and the list of ``[extension,
            error_message]`` pairs for the forbidden ones.
        """

        # Initializing containers
        allowed = []
        forbidden = []
        # NOTE: 'errormsg' is never used.
        errormsg = []

        # Adding format according to MA5 level mode
        if self.mode in [MA5RunningType.PARTON, MA5RunningType.HADRON]:
            allowed.append(".lhe")
            allowed.append(".hep")
            allowed.append(".hepmc")
            forbidden.append(
                [
                    ".root",
                    "ROOT format is only available at the reconstructed level of MA5",
                ]
            )
            forbidden.append(
                [
                    ".lhco",
                    "LHCO format is only available at the reconstructed level of MA5",
                ]
            )
        else:
            if self.recasting.status == "on":
                allowed.append(".hep")
                allowed.append(".hepmc")
                forbidden.append([".lhe", "LHE format cannot be used for recasting"])
                forbidden.append([".lhco", "LHCO format cannot be used for recasting"])
                if self.archi_info.has_delphes or self.archi_info.has_delphesMA5tune:
                    allowed.append(".root")
                else:
                    forbidden.append([".root", "ROOT format requires the package ROOT"])
            elif self.fastsim.package == "none":
                allowed.append(".lhco")
                forbidden.append(
                    [
                        ".lhe",
                        "LHE format is only available at the parton or hadron level of MA5",
                    ]
                )
                forbidden.append(
                    [
                        ".hep",
                        "HEP format is only available at the parton or hadron level of MA5",
                    ]
                )
                forbidden.append(
                    [
                        ".hepmc",
                        "HEPMC format is only available at the parton or hadron level of MA5",
                    ]
                )
                if self.archi_info.has_delphes or self.archi_info.has_delphesMA5tune:
                    allowed.append(".root")
                else:
                    forbidden.append(
                        [
                            ".root",
                            "ROOT format is not supported. The ROOT package is required",
                        ]
                    )
            else:
                allowed.append(".lhe")
                allowed.append(".hep")
                allowed.append(".hepmc")
                forbidden.append(
                    [".root", "ROOT format cannot be used when fastim package is applied"]
                )
                forbidden.append(
                    [".lhco", "LHCO format cannot be used when fastim package is applied"]
                )

        # Adding gzip file
        if self.archi_info.has_zlib:
            zipsamples = []
            for item in allowed:
                zipsamples.append(item + ".gz")
            allowed.extend(zipsamples)
            zipsamples = []
            for item in forbidden:
                zipsamples.append([item[0] + ".gz", item[1]])
            forbidden.extend(zipsamples)
        else:
            zipsamples = []
            for item in allowed:
                zipsamples.append(
                    [
                        item + ".gz",
                        "GZ format is not supported. The Zlib package is required",
                    ]
                )
            for item in forbidden:
                zipsamples.append(
                    [
                        item[0] + ".gz",
                        "GZ format is not supported. The Zlib package is required",
                    ]
                )
            forbidden.extend(zipsamples)

        # fifo format
        fifosamples = []
        for item in allowed:
            fifosamples.append(item + ".fifo")
        allowed.extend(fifosamples)
        fifosamples = []
        for item in forbidden:
            fifosamples.append([item[0] + ".fifo", item[1]])
        forbidden.extend(fifosamples)

        return allowed, forbidden

    def Display(self) -> None:
        """Log all the settings of the session (``display main`` command)."""
        self.logger.info(" *********************************")
        self.logger.info("            main program          ")
        self.logger.info(" *********************************")
        self.user_DisplayParameter("currentdir")
        self.user_DisplayParameter("graphic_render")
        self.user_DisplayParameter("normalize")
        self.user_DisplayParameter("lumi")
        self.user_DisplayParameter("outputfile")
        self.fom.Display()
        self.logger.info(" *********************************")
        allowed, forbidden = self.GetSampleFormat()
        forbidden2 = []
        for item in forbidden:
            forbidden2.append(item[0])
        self.logger.info(" File extension readable in this session: " + " ".join(allowed))
        self.logger.info(
            " File extension NOT readable in this session: " + " ".join(forbidden2)
        )
        self.logger.info(" *********************************")
        if self.archi_info.has_fastjet:
            self.merging.Display()
        self.fastsim.Display()
        self.isolation.Display()
        self.logger.info(" *********************************")
        self.recasting.Display()
        self.logger.info(" *********************************")

    def user_DisplayParameter(self, parameter: str) -> None:
        """Log the value of one ``main`` parameter (``display main.<parameter>``).

        Args:
            parameter (``str``): name of the parameter (``currentdir``,
                ``stacking_method``, ``normalize``, ``graphic_render``, ``outputfile``,
                ``lumi`` or ``recast``).
        """
        if parameter == "currentdir":
            self.logger.info(" currentdir = " + self.get_currentdir())
        elif parameter == "stacking_method":
            sentence = " stacking methode for histograms = "
            if self.stack == StackingMethodType.STACK:
                sentence += "stack"
            elif self.stack == StackingMethodType.SUPERIMPOSE:
                sentence += "superimpose"
            else:
                sentence += "normalize2one"
            self.logger.info(sentence)
        elif parameter == "normalize":
            word = ""
            if self.normalize == NormalizeType.NONE:
                word = "none"
            elif self.normalize == NormalizeType.LUMI:
                word = "lumi"
            elif self.normalize == NormalizeType.LUMI_WEIGHT:
                word = "lumi_weight"
            self.logger.info(" histogram normalization mode = " + word)
        elif parameter == "graphic_render":
            word = ""
            if self.graphic_render == GraphicRenderType.NONE:
                word = "none"
            elif self.graphic_render == GraphicRenderType.ROOT:
                word = "root"
            elif self.graphic_render == GraphicRenderType.MATPLOTLIB:
                word = "matplotlib"
            self.logger.info(" graphic renderer = " + word)
        elif parameter == "outputfile":
            if self.output == "":
                msg = "none"
            else:
                msg = '"' + self.output + '"'
            self.logger.info(" output file = " + msg)
        elif parameter == "lumi":
            self.logger.info(" integrated luminosity = " + str(self.lumi) + " fb^{-1}")
        elif parameter == "recast":
            self.logger.info(' Recasting mode = "' + self.recasting.status + '"')
        else:
            self.logger.error("'main' has no parameter called '" + parameter + "'")

    def user_GetValues(self, variable: str) -> list[str]:
        """Get the possible values of a ``main`` parameter (used for tab completion).

        Args:
            variable (``str``): name of the parameter.

        Returns:
            ``list[str]``:
            Possible values (directories for ``currentdir``), or an empty list if
            the parameter is unknown or takes free values.
        """
        if variable == "currentdir":
            return CmdBase.directory_complete()
        else:
            try:
                return Main.userVariables[variable]
            except:
                return []

    def user_GetParameters(self) -> list[str]:
        """Get the names of the parameters that can be set with ``set main.<name>``.

        Returns:
            ``list[str]``:
            Keys of :attr:`userVariables`.
        """
        return list(Main.userVariables.keys())

    def user_SetParameter(self, parameter: str, value: str) -> bool | None:
        """Set a ``main`` parameter (``set main.<parameter> = <value>``).

        Supported parameters: ``currentdir``, ``random_seed``, ``stacking_method``,
        ``normalize``, ``graphic_render``, ``lumi`` and ``outputfile`` (``.lhe``/
        ``.lhco``, optionally gzipped and quoted). Other ``main`` sub-objects
        (``fastsim``, ``recast``, ``merging``, ...) are handled in
        :mod:`madanalysis.interpreter.cmd_set`.

        Args:
            parameter (``str``): name of the parameter.
            value (``str``): value as typed by the user.

        Returns:
            ``bool | None``:
            ``False`` for most errors, ``None`` on success (and for some errors, see
            the FIXME notes in the code).
        """
        # currentdir
        if parameter == "currentdir":
            self.set_currentdir(value)
        elif parameter == "random_seed":
            try:
                tmp = int(value)
            except ValueError as err:
                self.logger.debug(tb.format_exc())
                self.logger.error("Random seed has have an integer value.")
                return False
            self.random_seed = tmp
            self.logger.debug(f"Random seed has been set to {self.random_seed}")

        # stacked
        elif parameter == "stacking_method":
            if value == "stack":
                self.stack = StackingMethodType.STACK
            elif value == "superimpose":
                self.stack = StackingMethodType.SUPERIMPOSE
            elif value == "normalize2one":
                self.stack = StackingMethodType.NORMALIZE2ONE
            else:
                self.logger.error(
                    "'stack' possible values are : 'stack', 'superimpose', 'normalize2one'"
                )
                return False

        # normalize
        elif parameter == "normalize":
            if value == "none":
                self.normalize = NormalizeType.NONE
            elif value == "lumi":
                self.normalize = NormalizeType.LUMI
            elif value == "lumi_weight":
                self.normalize = NormalizeType.LUMI_WEIGHT
            else:
                self.logger.error(
                    "'normalize' possible values are : 'none', 'lumi', 'lumi_weight'"
                )
                return False

        # graphic_render
        elif parameter == "graphic_render":
            if value == "none":
                self.graphic_render = GraphicRenderType.NONE
            elif value == "root":
                if self.session_info.has_root:
                    self.graphic_render = GraphicRenderType.ROOT
                else:
                    self.logger.error(
                        "Sorry but the Root package is not detected by MadAnalysis"
                    )
                    return False
            elif value == "matplotlib":
                if self.session_info.has_matplotlib:
                    self.graphic_render = GraphicRenderType.MATPLOTLIB
                else:
                    self.logger.error(
                        "Sorry but the Matplotlib package is not detected by MadAnalysis"
                    )
                    return False
            else:
                self.logger.error(
                    "'graphic_render' possible values are : 'none', 'root', 'matplotlib'"
                )
                return False

        # lumi
        elif parameter == "lumi":
            try:
                tmp = float(value)
            # FIXME: returns None on error while other parameters return False.
            except:
                self.logger.error("'lumi' is a positive float value")
                return
            if tmp > 0:
                self.lumi = tmp
            else:
                # FIXME: returns None on error while other parameters return False.
                self.logger.error("'lumi' is a positive float value")
                return

        # output
        elif parameter == "outputfile":
            quoteTag = False
            if value.startswith("'") and value.endswith("'"):
                quoteTag = True
            if value.startswith('"') and value.endswith('"'):
                quoteTag = True
            if quoteTag:
                value = value[1:-1]
            valuemin = value.lower()

            # Compressed file
            if valuemin.endswith(".gz") and not self.archi_info.has_zlib:
                self.logger.error(
                    "Compressed formats (*.gz) are not available. "
                    + "Please install zlib with the command line:"
                )
                self.logger.error(" install zlib")
                return False

            # LHE
            if valuemin.endswith(".lhe") or valuemin.endswith(".lhe.gz"):
                self.output = value
                return

            # LHCO
            elif valuemin.endswith(".lhco") or valuemin.endswith(".lhco.gz"):
                if self.mode == MA5RunningType.RECO:
                    self.output = value
                    return
                elif self.mode == MA5RunningType.PARTON:
                    self.logger.error("LHCO format is not available in PARTON mode.")
                    return False
                # FIXME: dead branch; the fast-simulation package can only be set in RECO mode, so an
                # LHCO output is always refused in HADRON mode.
                elif self.mode == MA5RunningType.HADRON:
                    if self.fastsim.package == "none":
                        self.logger.error(
                            "Please select a fast-simulation package before requesting a LHCO file output."
                        )
                        self.logger.error("Command: set main.fastsim.package = ... ")
                        return False
                    else:
                        self.output = value
                        return

            else:
                self.logger.error(
                    "Output format is not available. Extension allowed: "
                    + ".lhe .lhe.gz .lhco .lhco.gz"
                )
                return False

        # other
        # FIXME: returns None for an unknown parameter while other errors return False.
        else:
            self.logger.error("'main' has no parameter called '" + parameter + "'")

    def get_currentdir(self) -> str:
        """Getter of the :attr:`currentdir` property.

        Returns:
            ``str``:
            The current working directory of the Python process.
        """
        return os.getcwd()

    def set_currentdir(self, dir: str) -> None:
        """Setter of the :attr:`currentdir` property: change the working directory.

        An error is logged (but no exception raised) if the directory is not
        accessible. The (possibly unchanged) current directory is then logged.

        Args:
            dir (``str``): new working directory (``~`` is expanded).
        """
        theDir = os.path.expanduser(dir)
        try:
            os.chdir(theDir)
        except:
            self.logger.error("Impossible to access the directory : " + theDir)
        try:
            self.user_DisplayParameter("currentdir")
        except:
            pass

    currentdir = property(get_currentdir, set_currentdir)

    def AutoSetGraphicalRenderer(self) -> None:
        """Select the graphical renderer automatically.

        ROOT is preferred, then Matplotlib; if none of them is available, no figure
        is produced.
        """
        self.logger.debug("Function AutoSetGraphicalRenderer:")
        self.logger.debug("   - ROOT is there:       " + str(self.session_info.has_root))
        self.logger.debug(
            "   - Matplotlib is there: " + str(self.session_info.has_matplotlib)
        )
        if self.session_info.has_root:
            self.graphic_render = GraphicRenderType.ROOT
        elif self.session_info.has_matplotlib:
            self.graphic_render = GraphicRenderType.MATPLOTLIB
        else:
            self.graphic_render = GraphicRenderType.NONE
        self.logger.info(
            "Package used for graphical rendering: "
            + "\x1b[32m"
            + GraphicRenderType.convert2string(self.graphic_render)
            + "\x1b[0m"
        )

    def CheckConfig(self, debug: bool = False) -> bool:
        """First configuration check, performed before building SampleAnalyzer.

        Checks the architecture, reads the user options
        (``madanalysis/input/installation_options.dat``), the session information,
        the mandatory packages (Python, compiler, make) and the optional data-processing
        packages (zlib, FastJet, ROOT, Delphes, ...), then creates the temporary
        folders.

        Args:
            debug (``bool``, default ``False``): enable debug printouts.

        Returns:
            ``bool``:
            ``True`` if all checks succeeded, ``False`` otherwise.
        """
        checkup = CheckUp(self.archi_info, self.session_info, debug, self.script)

        if not checkup.CheckArchitecture():
            return False
        if not checkup.ReadUserOptions():
            return False
        if not checkup.CheckSessionInfo():
            return False
        if not checkup.CheckMandatoryPackages():
            return False
        if not checkup.CheckOptionalProcessingPackages():
            return False
        if not checkup.SetFolder():
            return False
        return True

    def CheckConfig2(self, debug: bool = False) -> bool:
        """Second configuration check, performed after building SampleAnalyzer.

        Checks the optional reinterpretation packages (SciPy, Spey, pyhf, PADs) and
        graphical packages (ROOT, Matplotlib, LaTeX), selects the graphical renderer
        and checks whether a newer MadAnalysis 5 release is available.

        Args:
            debug (``bool``, default ``False``): enable debug printouts.

        Returns:
            ``bool``:
            ``True`` if all checks succeeded, ``False`` otherwise.
        """
        checkup = CheckUp(self.archi_info, self.session_info, debug, self.script)

        # Read user options
        if not checkup.ReadUserOptions():
            return False

        # Reinterpretation packages
        if not checkup.CheckOptionalReinterpretationPackages():
            return False

        # Graphical packages
        if not checkup.CheckOptionalGraphicalPackages():
            return False
        self.AutoSetGraphicalRenderer()

        checkup.check_updates()

        # Ok
        return True

    def BuildLibrary(self, forced: bool = False) -> bool:
        """Build (if needed) the SampleAnalyzer libraries and test programs.

        A rebuild is triggered when the libraries are missing, when the system
        configuration differs from the one saved in ``tools/architecture.ma5``, when
        the test program ``TestSampleAnalyzer`` fails, or when ``forced`` is ``True``.
        The list of components depends on the detected packages (zlib, FastJet,
        fjcontrib/substructure, HEPTopTagger, Delphes, Delphes-MA5tune, ROOT). For
        each component, the project is cleaned, compiled, linked and checked, and
        test programs are executed.

        As a side effect, the environment variable ``FASTJET_FLAG`` is set
        (``-DMA5_FASTJET_MODE`` when FastJet is available).

        Args:
            forced (``bool``, default ``False``): force the rebuild.

        Raises:
            ``SystemExit``: if any building step fails.

        Returns:
            ``bool``:
            ``True`` when the libraries are ready.
        """
        builder = LibraryBuilder(self.archi_info)
        UpdateNeed = False
        FirstUse, Missing = builder.checkMA5()
        if not FirstUse and not Missing:
            UpdateNeed = not builder.compare()

        rebuild = forced or FirstUse or UpdateNeed or Missing

        if not rebuild:
            if not os.path.isfile(
                self.archi_info.ma5dir + "/tools/SampleAnalyzer/Lib/libprocess_for_ma5.so"
            ):
                FirstUse = True
            rebuild = forced or FirstUse or UpdateNeed or Missing

        if not rebuild:
            self.logger.info("  => MadAnalysis libraries found.")

            # NOTE: FirstUse is set when the test program is missing, but the test is still run below.
            # Test the program
            if not os.path.isfile(
                self.archi_info.ma5dir + "/tools/SampleAnalyzer/Bin/TestSampleAnalyzer"
            ):
                FirstUse = True

            precompiler = LibraryWriter("lib", self)
            if not precompiler.Run(
                "TestSampleAnalyzer",
                [
                    self.archi_info.ma5dir
                    + "/tools/SampleAnalyzer/Test/Process/dummy_list.txt"
                ],
                self.archi_info.ma5dir + "/tools/SampleAnalyzer/Bin/",
                silent=True,
            ):
                UpdateNeed = True

            if not precompiler.CheckRun(
                "TestSampleAnalyzer",
                self.archi_info.ma5dir + "/tools/SampleAnalyzer/Bin/",
                silent=True,
            ):
                UpdateNeed = True
            rebuild = forced or FirstUse or UpdateNeed or Missing

        if not rebuild:
            self.logger.info("  => MadAnalysis test program works.")
            return True

        # Compile library
        if FirstUse:
            self.logger.info("  => First use of MadAnalysis (or the library is missing).")
        elif Missing:
            self.logger.info(
                "  => Libraries are missing or system configuration has changed. Need to rebuild the library."
            )
        elif UpdateNeed:
            self.logger.info(
                "  => System configuration has changed since the last use. Need to rebuild the library."
            )
        elif forced:
            self.logger.info("  => The user forces to rebuild the library.")
        # Initializing the JobWriter
        compiler = LibraryWriter("lib", self)

        # Dumping architecture
        if not self.archi_info.save(self.archi_info.ma5dir + "/tools/architecture.ma5"):
            sys.exit()

        # Library to compiles
        # NOTE: element [2] below is the component keyword understood by LibraryWriter
        # (Makefile name / folder selection).
        # |- [0] = unique name
        # |- [1] = title of the library to display
        # |- [2] =
        # |- [3] = output file to cross-check
        # |- [4] = folder
        # |- [5] = False=Library, True=Executable
        libraries = []
        libraries.append(
            [
                "configuration",
                "SampleAnalyzer configuration",
                "configuration",
                self.archi_info.ma5dir + "/tools/SampleAnalyzer/Bin/PortabilityCheckup",
                self.archi_info.ma5dir + "/tools/SampleAnalyzer/Configuration",
                True,
            ]
        )
        libraries.append(
            [
                "commons",
                "SampleAnalyzer commons",
                "commons",
                self.archi_info.ma5dir
                + "/tools/SampleAnalyzer/Lib/libcommons_for_ma5.so",
                self.archi_info.ma5dir + "/tools/SampleAnalyzer/Commons",
                False,
            ]
        )
        libraries.append(
            [
                "test_commons",
                "SampleAnalyzer commons",
                "test_commons",
                self.archi_info.ma5dir + "/tools/SampleAnalyzer/Bin/TestCommons",
                self.archi_info.ma5dir + "/tools/SampleAnalyzer/Test/",
                True,
            ]
        )
        # Zlib
        if self.archi_info.has_zlib:
            libraries.append(
                [
                    "zlib",
                    "interface to zlib",
                    "zlib",
                    self.archi_info.ma5dir
                    + "/tools/SampleAnalyzer/Lib/libzlib_for_ma5.so",
                    self.archi_info.ma5dir + "/tools/SampleAnalyzer/Interfaces",
                    False,
                ]
            )
            libraries.append(
                [
                    "test_zlib",
                    "interface to zlib",
                    "test_zlib",
                    self.archi_info.ma5dir + "/tools/SampleAnalyzer/Bin/TestZlib",
                    self.archi_info.ma5dir + "/tools/SampleAnalyzer/Test/",
                    True,
                ]
            )

        # Fastjet
        if self.archi_info.has_fastjet:
            os.environ["FASTJET_FLAG"] = "-DMA5_FASTJET_MODE"
            self.logger.debug("setting environment: FASTJET_FLAG=%s", os.environ["FASTJET_FLAG"])
            libraries.append(['FastJet', 'interface to FastJet', 'fastjet',
                              self.archi_info.ma5dir + '/tools/SampleAnalyzer/Lib/libfastjet_for_ma5.so',
                              self.archi_info.ma5dir + '/tools/SampleAnalyzer/Interfaces', False])
            libraries.append(['test_fastjet', 'interface to Fastjet', 'test_fastjet',
                              self.archi_info.ma5dir + '/tools/SampleAnalyzer/Bin/TestFastjet',
                              self.archi_info.ma5dir + '/tools/SampleAnalyzer/Test/', True])
            if self.archi_info.has_fjcontrib:
                libraries.append(['substructure', 'interface to Jet Substructure module', 'substructure',
                                  self.archi_info.ma5dir + '/tools/SampleAnalyzer/Lib/libsubstructure_for_ma5.so',
                                  self.archi_info.ma5dir + '/tools/SampleAnalyzer/Interfaces', False])
                libraries.append(['test_substructure', 'interface to Jet Substructure module', 'test_substructure',
                                  self.archi_info.ma5dir + '/tools/SampleAnalyzer/Bin/TestSubstructure',
                                  self.archi_info.ma5dir + '/tools/SampleAnalyzer/Test/', True])
                if self.archi_info.has_heptoptagger:
                    libraries.append(['HEPTopTagger', 'interface to HEPTopTagger module', 'HEPTopTagger',
                                      self.archi_info.ma5dir + '/tools/SampleAnalyzer/Lib/libHEPTopTagger_for_ma5.so',
                                      self.archi_info.ma5dir + '/tools/SampleAnalyzer/Interfaces', False])
                    libraries.append(['test_htt', 'interface to HEPTopTagger module', 'test_htt',
                                      self.archi_info.ma5dir + '/tools/SampleAnalyzer/Bin/TestHEPTopTagger',
                                      self.archi_info.ma5dir + '/tools/SampleAnalyzer/Test/', True])
        else:
            os.environ["FASTJET_FLAG"] = ""
        # Delphes
        if self.archi_info.has_delphes:
            libraries.append(
                [
                    "Delphes",
                    "interface to Delphes",
                    "delphes",
                    self.archi_info.ma5dir
                    + "/tools/SampleAnalyzer/Lib/libdelphes_for_ma5.so",
                    self.archi_info.ma5dir + "/tools/SampleAnalyzer/Interfaces",
                    False,
                ]
            )
            libraries.append(
                [
                    "test_delphes",
                    "interface to Delphes",
                    "test_delphes",
                    self.archi_info.ma5dir + "/tools/SampleAnalyzer/Bin/TestDelphes",
                    self.archi_info.ma5dir + "/tools/SampleAnalyzer/Test/",
                    True,
                ]
            )
        # DelphesMA5tune
        if self.archi_info.has_delphesMA5tune:
            libraries.append(
                [
                    "Delphes-MA5tune",
                    "interface to Delphes-MA5tune",
                    "delphesMA5tune",
                    self.archi_info.ma5dir
                    + "/tools/SampleAnalyzer/Lib/libdelphesMA5tune_for_ma5.so",
                    self.archi_info.ma5dir + "/tools/SampleAnalyzer/Interfaces",
                    False,
                ]
            )
            libraries.append(
                [
                    "test_delphesMA5tune",
                    "interface to DelphesMA5tune",
                    "test_delphesMA5tune",
                    self.archi_info.ma5dir
                    + "/tools/SampleAnalyzer/Bin/TestDelphesMA5tune",
                    self.archi_info.ma5dir + "/tools/SampleAnalyzer/Test/",
                    True,
                ]
            )

        # Root
        if self.archi_info.has_root:
            libraries.append(
                [
                    "Root",
                    "interface to Root",
                    "root",
                    self.archi_info.ma5dir
                    + "/tools/SampleAnalyzer/Lib/libroot_for_ma5.so",
                    self.archi_info.ma5dir + "/tools/SampleAnalyzer/Interfaces",
                    False,
                ]
            )
            libraries.append(
                [
                    "test_root",
                    "interface to Root",
                    "test_root",
                    self.archi_info.ma5dir + "/tools/SampleAnalyzer/Bin/TestRoot",
                    self.archi_info.ma5dir + "/tools/SampleAnalyzer/Test/",
                    True,
                ]
            )

        # Process
        libraries.append(
            [
                "process",
                "SampleAnalyzer core",
                "process",
                self.archi_info.ma5dir
                + "/tools/SampleAnalyzer/Lib/libprocess_for_ma5.so",
                self.archi_info.ma5dir + "/tools/SampleAnalyzer/Process",
                False,
            ]
        )
        libraries.append(
            [
                "test_process",
                "SampleAnalyzer core",
                "test_process",
                self.archi_info.ma5dir + "/tools/SampleAnalyzer/Bin/TestSampleAnalyzer",
                self.archi_info.ma5dir + "/tools/SampleAnalyzer/Test/",
                True,
            ]
        )

        # Writing the Makefiles
        self.logger.info("")
        self.logger.info("   **********************************************************")
        self.logger.info("                Building SampleAnalyzer libraries     ")
        self.logger.info("   **********************************************************")

        # Getting number of cores
        ncores = compiler.get_ncores2()

        # Chronometer start
        from chronometer import Chronometer

        chrono = Chronometer()
        chrono.Start()

        # Writing the main Makefile
        from madanalysis.build.makefile_writer import MakefileWriter

        options = MakefileWriter.UserfriendlyMakefileOptions()
        options.has_commons = True
        options.has_process = True
        options.has_test = True
        options.has_zlib = self.archi_info.has_zlib
        options.has_fastjet = self.archi_info.has_fastjet
        options.has_delphes = self.archi_info.has_delphes
        options.has_delphesMA5tune = self.archi_info.has_delphesMA5tune
        # MakefileWriter.UserfriendlyMakefileForSampleAnalyzer(self.archi_info.ma5dir+'/tools/SampleAnalyzer/Makefile',options)

        # Writing the setup
        self.logger.info("   Writing the setup files ...")
        from madanalysis.build.setup_writer import SetupWriter

        # @Jack: git doesn't like empty folders so these have to be created before compilation
        if not os.path.isdir(
            os.path.join(self.archi_info.ma5dir, "tools/SampleAnalyzer/", "Bin")
        ):
            os.mkdir(os.path.join(self.archi_info.ma5dir, "tools/SampleAnalyzer/", "Bin"))

        SetupWriter.WriteSetupFile(
            True, self.archi_info.ma5dir + "/tools/SampleAnalyzer/", self.archi_info
        )
        SetupWriter.WriteSetupFile(
            False, self.archi_info.ma5dir + "/tools/SampleAnalyzer/", self.archi_info
        )
        # Writing the makefile
        self.logger.info("   Writing all the Makefiles ...")
        for ind in range(0, len(libraries)):
            if not compiler.WriteMakefileForInterfaces(libraries[ind][2]):
                self.logger.error("library building aborted.")
                sys.exit()
        if not compiler.WriteMakefileForInterfaces("test"):
            self.logger.error("test program building aborted.")
            sys.exit()

        # Compiling the libraries
        for ind in range(0, len(libraries)):

            isLibrary = not libraries[ind][5]
            if isLibrary:
                product = "library"
            else:
                product = "test program"

            self.logger.info(
                "   **********************************************************"
            )
            self.logger.info(f"   Component {ind + 1}/{len(libraries)} - {product}: {libraries[ind][1]}")

            # Cleaning the project
            self.logger.info(
                f"     - Cleaning the project before building the {product} ..."
            )
            if not compiler.MrProper(libraries[ind][2], libraries[ind][4]):
                self.logger.error(f"The {product} building aborted.")
                sys.exit()

            # Compiling
            self.logger.info("     - Compiling the source files ...")
            if not compiler.Compile(ncores, libraries[ind][2], libraries[ind][4]):
                self.logger.error(f"The {product} building aborted.")
                sys.exit()

            # Linking
            self.logger.info("     - Linking the " + product + " ...")
            if not compiler.Link(libraries[ind][2], libraries[ind][4]):
                self.logger.error("The " + product + " building aborted.")
                sys.exit()

            # Checking
            self.logger.info(
                "     - Checking that the " + product + " is properly built ..."
            )
            if not os.path.isfile(libraries[ind][3]):
                self.logger.error(
                    "The " + product + " '" + libraries[ind][3] + "' is not produced."
                )
                sys.exit()

            # Cleaning the project
            self.logger.info(
                "     - Cleaning the project after building the " + product + " ..."
            )
            if not compiler.Clean(libraries[ind][2], libraries[ind][4]):
                self.logger.error("library building aborted.")
                sys.exit()

            if not isLibrary:

                # Running the program test
                self.logger.info("     - Running the test program ...")
                program = libraries[ind][3].split("/")[-1]

                argv = []
                if program == "TestSampleAnalyzer":
                    argv = [
                        self.archi_info.ma5dir
                        + "/tools/SampleAnalyzer/Test/Process/dummy_list.txt"
                    ]
                if not compiler.Run(
                    program, argv, self.archi_info.ma5dir + "/tools/SampleAnalyzer/Bin/"
                ):
                    self.logger.error("the test failed.")
                    sys.exit()

                # Checking the program output
                self.logger.info("     - Checking the program output...")
                if libraries[ind][0] == "configuration":
                    if not compiler.CheckRunConfiguration(
                        program, self.archi_info.ma5dir + "/tools/SampleAnalyzer/Bin/"
                    ):
                        self.logger.error("the test failed.")
                        sys.exit()
                else:
                    if not compiler.CheckRun(
                        program, self.archi_info.ma5dir + "/tools/SampleAnalyzer/Bin/"
                    ):
                        self.logger.error("the test failed.")
                        sys.exit()

            # Print Ok
            self.logger.info("      => Status: \x1b[32m" + "[OK]" + "\x1b[0m")

        self.logger.info("   **********************************************************")

        # Chrono end
        chrono.Stop()
        self.logger.info("   Elapsed time = " + chrono.Display())

        self.logger.info("   **********************************************************")
        self.logger.info("")

        return True
