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


"""Installation of FastJet (``install fastjet``)."""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from madanalysis.core.main import Main

import logging
import os

from shell_command import ShellCommand

from madanalysis.install.install_service import InstallService

log = logging.getLogger("MA5")


class InstallFastjet:
    """Installer of FastJet (``install fastjet``).

    The methods are called by
    :meth:`madanalysis.install.install_manager.InstallManager.Execute` in the following
    order (only when defined): ``Detect``/``Remove``, ``GetNcores``,
    ``CreatePackageFolder``, ``CreateTmpFolder``, ``Download``, ``Unpack``, ``Configure``,
    ``Build``, ``PreCheck``, ``Clean``, ``Install``, ``Check`` and ``NeedToRestart``.

    FastJet 3.5.1 is downloaded from fastjet.fr and installed in ``tools/fastjet`` (all plugins enabled).
    """
    def __init__(self, main: Main) -> None:
        """Prepare the installation of FastJet (folders, download URLs).

        Args:
            main (``Main``): session state.
        """
        self.main = main
        self.installdir = os.path.normpath(
            self.main.archi_info.ma5dir + "/tools/fastjet/"
        )
        self.toolsdir = os.path.normpath(self.main.archi_info.ma5dir + "/tools")
        self.tmpdir = self.main.session_info.tmpdir
        self.downloaddir = self.main.session_info.downloaddir
        self.untardir = os.path.normpath(self.tmpdir + "/MA5_fastjet/")
        self.ncores = 1
        self.files = {"fastjet.tar.gz": "https://fastjet.fr/repo/fastjet-3.5.1.tar.gz"}

    def Detect(self) -> bool:
        """Check whether FastJet is already installed.

        Returns:
            ``bool``:
            ``True`` if the installation folder exists.
        """
        if not os.path.isdir(self.toolsdir):
            log.debug("The folder '" + self.toolsdir + "' is not found")
            return False
        if not os.path.isdir(self.installdir):
            log.debug("The folder " + self.installdir + "' is not found")
            return False
        return True

    def Remove(self, question: bool = True) -> tuple[bool, bool]:
        """Remove the previous installation of FastJet.

        Args:
            question (``bool``, default ``True``): ask the user for confirmation.

        Returns:
            ``tuple[bool, bool]``:
            Result of :meth:`~madanalysis.IOinterface.folder_writer.FolderWriter.RemoveDirectory`:
            whether the operation succeeded and whether the folder was removed/kept on the
            user's request.
        """
        from madanalysis.IOinterface.folder_writer import FolderWriter

        return FolderWriter.RemoveDirectory(self.installdir, question)

    def GetNcores(self) -> None:
        """Ask the number of cores used for the compilation (all cores in forced mode)."""
        self.ncores = InstallService.get_ncores(
            self.main.archi_info.ncores, self.main.forced
        )

    def CreatePackageFolder(self) -> bool:
        """Create the installation folder of FastJet.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        if not InstallService.create_tools_folder(self.toolsdir):
            return False
        if not InstallService.create_package_folder(self.toolsdir, "fastjet"):
            return False
        return True

    def CreateTmpFolder(self) -> bool:
        """Create (clean) the temporary unpacking folder and the download folder.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        ok = InstallService.prepare_tmp(self.untardir, self.downloaddir)
        if ok:
            self.tmpdir = self.untardir
        return ok

    def Download(self) -> bool:
        """Download the source files of FastJet.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Checking connection with MA5 web site
        if not InstallService.check_ma5site():
            return False
        # Launching wget
        logname = os.path.normpath(self.installdir + "/wget.log")
        if not InstallService.wget(
            self.files,
            logname,
            self.downloaddir,
            headers={
                "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
                "Accept-Language": "en-US,en;q=0.5",
                "Referer": "https://fastjet.fr/",
                "Connection": "keep-alive",
            },
        ):
            return False
        # Ok
        return True

    def Unpack(self) -> bool:
        """Unpack the downloaded files of FastJet.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Logname
        logname = os.path.normpath(self.installdir + "/unpack.log")
        # Unpacking the tarball
        ok, packagedir = InstallService.untar(
            logname, self.downloaddir, self.tmpdir, "fastjet.tar.gz"
        )
        if not ok:
            return False
        # Ok: returning the good folder
        self.tmpdir = packagedir
        return True

    def Configure(self) -> bool:
        """Configure FastJet before compilation.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Input
        initial_env = os.environ.get("CXXFLAGS", "")
        os.environ["CXXFLAGS"] = initial_env + " -std=c++11 -fPIC "
        theCommands = [
            "./configure",
            "--prefix=" + self.installdir,
            "--enable-allplugins",
        ]
        log.debug("Configuring fastjet with prefix %s", self.installdir)
        log.debug("Configuring fastjet with CXXFLAGS %s", os.environ["CXXFLAGS"])
        logname = os.path.normpath(self.installdir + "/configuration.log")
        # Execute
        log.debug("shell command: " + " ".join(theCommands))
        ok, out = ShellCommand.ExecuteWithLog(
            theCommands, logname, self.tmpdir, silent=False
        )
        # reset CXXFLAGS
        if initial_env == "":
            os.environ.pop("CXXFLAGS")
        else:
            os.environ["CXXFLAGS"] = initial_env
        # return result
        if not ok:
            log.error(
                "impossible to configure the project. For more details, see the log file:"
            )
            log.error(logname)
        return ok

    def Build(self) -> bool:
        """Compile FastJet.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Input
        theCommands = ["make", "-j" + str(self.ncores)]
        logname = os.path.normpath(self.installdir + "/compilation.log")
        # Execute
        log.debug("shell command: " + " ".join(theCommands))
        ok, out = ShellCommand.ExecuteWithLog(
            theCommands, logname, self.tmpdir, silent=False
        )
        # return result
        if not ok:
            log.error(
                "impossible to build the project. For more details, see the log file:"
            )
            log.error(logname)
        return ok

    def Install(self) -> bool:
        """Install FastJet in its definitive folder.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Input
        theCommands = ["make", "install"]
        logname = os.path.normpath(self.installdir + "/installation.log")
        # Execute
        log.debug("shell command: " + " ".join(theCommands))
        ok, out = ShellCommand.ExecuteWithLog(
            theCommands, logname, self.tmpdir, silent=False
        )
        # return result
        if not ok:
            # NOTE: the message says 'build' although this is the installation step.
            log.error(
                "impossible to build the project. For more details, see the log file:"
            )
            log.error(logname)
        return ok

    def Check(self) -> bool:
        """Check that FastJet has been properly installed.

        Returns:
            ``bool``:
            ``True`` if the expected files are present.
        """
        # Check folders
        dirs = [
            self.installdir + "/include",
            self.installdir + "/lib",
            self.installdir + "/bin",
        ]
        for dir in dirs:
            if not os.path.isdir(dir):
                log.error("folder " + dir + " is missing.")
                self.display_log()
                return False

        # Check fastjet executable
        if not os.path.isfile(self.installdir + "/bin/fastjet-config"):
            log.error("binary labeled 'fastjet-config' is missing.")
            self.display_log()
            return False

        # Check one header file
        if not os.path.isfile(self.installdir + "/include/fastjet/PseudoJet.hh"):
            log.error("header labeled 'include/fastjet/PseudoJet.hh' is missing.")
            self.display_log()
            return False

        if (not os.path.isfile(self.installdir + "/lib/libfastjet.so")) and (
            not os.path.isfile(self.installdir + "/lib/libfastjet.a")
        ):
            log.error("library labeled 'libfastjet.so' or 'libfastjet.a' is missing.")
            self.display_log()
            return False

        return True

    def display_log(self) -> None:
        """Log the paths of the installation log files."""
        log.error("More details can be found into the log files:")
        log.error(" - " + os.path.normpath(self.installdir + "/wget.log"))
        log.error(" - " + os.path.normpath(self.installdir + "/unpack.log"))
        log.error(" - " + os.path.normpath(self.installdir + "/configuration.log"))
        log.error(" - " + os.path.normpath(self.installdir + "/compilation.log"))
        log.error(" - " + os.path.normpath(self.installdir + "/installation.log"))

    def NeedToRestart(self) -> bool:
        """Tell whether MadAnalysis 5 must be restarted after the installation.

        Returns:
            ``bool``:
            ``True`` if a restart (new configuration check and library build) is needed.
        """
        return True
