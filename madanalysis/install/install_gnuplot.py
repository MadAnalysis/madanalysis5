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


"""Installation of gnuplot 4.6.5 (legacy) (``install gnuplot``)."""

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

class InstallGnuplot:
    """Installer of gnuplot 4.6.5 (legacy) (``install gnuplot``).

    The methods are called by
    :meth:`madanalysis.install.install_manager.InstallManager.Execute` in the following
    order (only when defined): ``Detect``/``Remove``, ``GetNcores``,
    ``CreatePackageFolder``, ``CreateTmpFolder``, ``Download``, ``Unpack``, ``Configure``,
    ``Build``, ``PreCheck``, ``Clean``, ``Install``, ``Check`` and ``NeedToRestart``.
    """

    def __init__(self,main: Main) -> None:
        """Prepare the installation of gnuplot 4.6.5 (legacy) (folders, download URLs).

        Args:
            main (``Main``): session state.
        """
        self.main       = main
        self.installdir = os.path.normpath(self.main.archi_info.ma5dir+'/tools/gnuplot/')
        self.toolsdir   = os.path.normpath(self.main.archi_info.ma5dir+'/tools')
        self.tmpdir     = self.main.session_info.tmpdir
        # NOTE: the session download folder is not used here.
        self.downloaddir= os.path.normpath(self.tmpdir + '/MA5_downloads/')
        self.untardir = os.path.normpath(self.tmpdir + '/MA5_gnuplot/')
        self.ncores     = 1
        self.files = {"gnuplot.tar.gz" : "http://sourceforge.net/projects/gnuplot/files/gnuplot/4.6.5/gnuplot-4.6.5.tar.gz"}


    def Detect(self) -> bool:
        """Check whether gnuplot 4.6.5 (legacy) is already installed.

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
        """Remove the previous installation of gnuplot 4.6.5 (legacy).

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

    def GetNcores(self) -> None:
        """Ask the number of cores used for the compilation (all cores in forced mode)."""
        self.ncores = InstallService.get_ncores(self.main.archi_info.ncores,\
                                                self.main.forced)

    def CreatePackageFolder(self) -> bool:
        """Create the installation folder of gnuplot 4.6.5 (legacy).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        if not InstallService.create_tools_folder(self.toolsdir):
            return False
        if not InstallService.create_package_folder(self.toolsdir,'gnuplot'):
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
        """Download the source files of gnuplot 4.6.5 (legacy).

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
        """Unpack the downloaded files of gnuplot 4.6.5 (legacy).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Logname
        logname = os.path.normpath(self.installdir+'/unpack.log')
        # Unpacking the tarball
        ok, packagedir = InstallService.untar(logname, self.downloaddir, self.tmpdir,'gnuplot.tar.gz')
        if not ok:
            return False
        # Ok: returning the good folder
        self.tmpdir=packagedir
        return True


    def Configure(self) -> bool:
        """Configure gnuplot 4.6.5 (legacy) before compilation.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Input
        theCommands=['./configure','--prefix='+self.installdir, '--with-png', '--with-jpeg', '--with-pdf']
        logname=os.path.normpath(self.installdir+'/configuration.log')
        # Execute
        logging.getLogger('MA5').debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             self.tmpdir,\
                                             silent=False)
        # return result
        if not ok:
            logging.getLogger('MA5').error('impossible to configure the project. For more details, see the log file:')
            logging.getLogger('MA5').error(logname)
        return ok


    def Install(self) -> bool:
        """Install gnuplot 4.6.5 (legacy) in its definitive folder.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Input
        theCommands=['make', 'install', '-j'+str(self.ncores)]
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
        """Check that gnuplot 4.6.5 (legacy) has been properly installed.

        Returns:
            ``bool``:
            ``True`` if the expected files are present.
        """
        # Check folders
        dirs = [self.installdir+"/bin",\
                self.installdir+"/libexec",\
                self.installdir+"/share"]
        for dir in dirs:
            if not os.path.isdir(dir):
                logging.getLogger('MA5').error('folder '+dir+' is missing.')
                self.display_log()
                return False

        # Check gnuplot executable
        if not os.path.isfile(self.installdir+'/bin/gnuplot'):
            logging.getLogger('MA5').error("binary labeled 'gnuplot' is missing.")
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


