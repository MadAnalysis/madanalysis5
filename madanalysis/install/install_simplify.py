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


"""Installation of the ``simplify`` package (likelihood simplification) (``install simplify``).
"""

from __future__                          import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from madanalysis.core.main import Main
from madanalysis.install.install_service import InstallService
import os, sys, logging

class InstallSimplify:
    """Installer of the ``simplify`` package (likelihood simplification) (``install simplify``).

    The methods are called by
    :meth:`madanalysis.install.install_manager.InstallManager.Execute` in the following
    order (only when defined): ``Detect``/``Remove``, ``GetNcores``,
    ``CreatePackageFolder``, ``CreateTmpFolder``, ``Download``, ``Unpack``, ``Configure``,
    ``Build``, ``PreCheck``, ``Clean``, ``Install``, ``Check`` and ``NeedToRestart``.
    """
    def __init__(self,main: Main) -> None:
        """Prepare the installation of the ``simplify`` package (likelihood simplification) (folders, download URLs).

        Args:
            main (``Main``): session state.
        """
        self.main        = main
        self.installdir  = os.path.normpath(self.main.archi_info.ma5dir+'/tools/simplify/')
        self.toolsdir    = os.path.normpath(self.main.archi_info.ma5dir+'/tools')
        self.tmpdir      = self.main.session_info.tmpdir
        self.downloaddir = self.main.session_info.downloaddir
        self.untardir    = os.path.normpath(self.tmpdir + '/MA5_simplify/')
        self.ncores      = 1
        self.files = {
            "master.zip" : "https://github.com/eschanet/simplify/archive/refs/heads/master.zip"
        }
        self.simplify_version= None # there is no version info

    def Detect(self) -> bool:
        """Check whether the ``simplify`` package (likelihood simplification) is already installed.

        Returns:
            ``bool``:
            ``True`` if the installation folder exists.
        """
        if not os.path.isdir(self.toolsdir):
            logging.getLogger('MA5').debug("The folder '"+self.toolsdir+"' is not found")
            return False
        if not os.path.isdir(self.installdir):
            logging.getLogger('MA5').debug("The folder "+self.installdir+"' is not found")
            return False
        return True

    def Remove(self,question: bool = True) -> tuple[bool, bool]:
        """Remove the previous installation of the ``simplify`` package (likelihood simplification).

        Args:
            question (``bool``, default ``True``): ask the user for confirmation.

        Returns:
            ``tuple[bool, bool]``:
            Result of :meth:`~madanalysis.IOinterface.folder_writer.FolderWriter.RemoveDirectory`:
            whether the operation succeeded and whether the folder was removed/kept on the
            user's request.
        """
        from madanalysis.IOinterface.folder_writer import FolderWriter
        return FolderWriter.RemoveDirectory(self.installdir,question)

    def CreatePackageFolder(self) -> bool:
        """Create the installation folder of the ``simplify`` package (likelihood simplification).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        if not InstallService.create_tools_folder(self.toolsdir):
            return False
        if not InstallService.create_package_folder(self.toolsdir,'simplify'):
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
            self.tmpdir=self.untardir
        return ok

    def Download(self) -> bool:
        """Download the source files of the ``simplify`` package (likelihood simplification).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Checking connection with MA5 web site
        if not InstallService.check_ma5site():
            return False
        # Launching wget
        logname = os.path.normpath(self.installdir+'/wget.log')
        if not InstallService.wget(self.files,logname,self.downloaddir):
            return False
        # Ok
        return True

    def Unpack(self) -> bool:
        """Unpack the downloaded files of the ``simplify`` package (likelihood simplification).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Logname
        logname = os.path.normpath(self.installdir+'/unpack.log')
        # Unpacking the tarball
        for key in self.files.keys():
            ok, packagedir = InstallService.untar(logname, self.downloaddir, self.installdir, key)
            if not ok:
                return False
        # Ok: returning the good folder
        self.tmpdir=packagedir
        return True

    def Build(self) -> bool:
        """Compile the ``simplify`` package (likelihood simplification).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # all checks are done in Check function.
        return True

    def Install(self) -> bool:
        """Install the ``simplify`` package (likelihood simplification) in its definitive folder.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        return True

    def Check(self) -> bool:
        """Check that the ``simplify`` package (likelihood simplification) has been properly installed.

        Returns:
            ``bool``:
            ``True`` if the expected files are present.
        """
        if sys.version_info[0] == 2:
            return False
        try:
            if os.path.isdir(self.installdir) and not self.installdir in sys.path:
                sys.path.insert(0, self.installdir+'/simplify-master/src/')
            import simplify
            logging.getLogger('MA5').debug("simplify has been imported from "+" ".join(simplify.__path__))

        except ImportError as err:
            logging.getLogger('MA5').error("The simplify module cannot be used. "+\
                                           "Please check that all requirements are available and (re-)install it.")
            logging.getLogger('MA5').debug(err)
            self.display_log()
            return False
        return True

    def display_log(self) -> None:
        """Log the paths of the installation log files."""
        logging.getLogger('MA5').error("More details can be found into the log files:")
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/wget.log"))
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/unpack.log"))

    def NeedToRestart(self) -> bool:
        """Tell whether MadAnalysis 5 must be restarted after the installation.

        Returns:
            ``bool``:
            ``True`` if a restart (new configuration check and library build) is needed.
        """
        return False
