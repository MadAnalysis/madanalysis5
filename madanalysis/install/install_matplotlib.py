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


"""Installation of Matplotlib 1.3.1 (legacy) (``install matplotlib``)."""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from madanalysis.core.main import Main
from madanalysis.install.install_service import InstallService
from shell_command import ShellCommand
import os
import sys
import logging

class InstallMatplotlib:
    """Installer of Matplotlib 1.3.1 (legacy) (``install matplotlib``).

    The methods are called by
    :meth:`madanalysis.install.install_manager.InstallManager.Execute` in the following
    order (only when defined): ``Detect``/``Remove``, ``GetNcores``,
    ``CreatePackageFolder``, ``CreateTmpFolder``, ``Download``, ``Unpack``, ``Configure``,
    ``Build``, ``PreCheck``, ``Clean``, ``Install``, ``Check`` and ``NeedToRestart``.
    """

    def __init__(self,main: Main) -> None:
        """Prepare the installation of Matplotlib 1.3.1 (legacy) (folders, download URLs).

        Args:
            main (``Main``): session state.
        """
        self.main       = main
        self.installdir = os.path.normpath(self.main.archi_info.ma5dir+'/tools/matplotlib/')
        self.toolsdir   = os.path.normpath(self.main.archi_info.ma5dir+'/tools')
        self.tmpdir     = self.main.session_info.tmpdir
        self.downloaddir = self.main.session_info.downloaddir
        self.untardir = os.path.normpath(self.tmpdir + '/MA5_matplotlib/')
        self.ncores     = 1
        self.files = {"matplotlib.tar.gz" : "http://sourceforge.net/projects/matplotlib/files/matplotlib/"+\
                                            "matplotlib-1.3.1/matplotlib-1.3.1.tar.gz"}


    def Detect(self) -> bool:
        """Check whether Matplotlib 1.3.1 (legacy) is already installed.

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
        """Remove the previous installation of Matplotlib 1.3.1 (legacy).

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
        """Create the installation folder of Matplotlib 1.3.1 (legacy).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        if not InstallService.create_tools_folder(self.toolsdir):
            return False
        if not InstallService.create_package_folder(self.toolsdir,'matplotlib'):
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
        """Download the source files of Matplotlib 1.3.1 (legacy).

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
        """Unpack the downloaded files of Matplotlib 1.3.1 (legacy).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Logname
        logname = os.path.normpath(self.installdir+'/unpack.log')
        # Unpacking the tarball
        ok, packagedir = InstallService.untar(logname, self.downloaddir,self.tmpdir,'matplotlib.tar.gz')
        if not ok:
            return False
        # Ok: returning the good folder
        self.tmpdir=packagedir
        return True


    def Build(self) -> bool:
        """Compile Matplotlib 1.3.1 (legacy).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Input
        theCommands=[sys.executable,'setup.py','build']
        logname=os.path.normpath(self.installdir+'/compilation.log')
        # Execute
        logging.getLogger('MA5').debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             self.tmpdir,\
                                             silent=False)
        # return result
        if not ok:
            logging.getLogger('MA5').error('impossible to build the project. For more details, see the log file:')
            logging.getLogger('MA5').error(logname)
        return ok


    def Install(self) -> bool:
        """Install Matplotlib 1.3.1 (legacy) in its definitive folder.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Input
        # FIXME: 'python' is hard-coded (sys.executable is used in Build); obsolete Python 2-era recipe.
        theCommands=['python','setup.py','install','--home='+self.installdir]
        logname=os.path.normpath(self.installdir+'/compilation.log')
        # Execute
        logging.getLogger('MA5').debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             self.tmpdir,\
                                             silent=False)
        # return result
        if not ok:
            logging.getLogger('MA5').error('impossible to build the project. For more details, see the log file:')
            logging.getLogger('MA5').error(logname)
        return ok


    def Check(self) -> bool:
        """Check that Matplotlib 1.3.1 (legacy) has been properly installed.

        Returns:
            ``bool``:
            ``True`` if the expected files are present.
        """
        # Check matplotlib downloaded version is in use
        try:
            import matplotlib
            # FIXME: only Matplotlib 1.3.1 is accepted: the check fails with any modern version.
            if str(matplotlib.__version__) != "1.3.1":
                logging.getLogger('MA5').error("Not using the right version of Matplotlib.")
                self.display_log()
                return False
        except:
            logging.getLogger('MA5').error("Cannot use Matplotlib. Please install it.")
            self.display_log()
            return False
        return True

    def display_log(self) -> None:
        """Log the paths of the installation log files."""
        logging.getLogger('MA5').error("More details can be found into the log files:")
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/wget.log"))
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/unpack.log"))
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/configuration.log"))
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/compilation.log"))
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/installation.log"))

    def NeedToRestart(self) -> bool:
        """Tell whether MadAnalysis 5 must be restarted after the installation.

        Returns:
            ``bool``:
            ``True`` if a restart (new configuration check and library build) is needed.
        """
        return True


