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


"""Installation of the FastJet contrib libraries (``install fastjet-contrib``)."""

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

class InstallFastjetContrib:
    """Installer of the FastJet contrib libraries (``install fastjet-contrib``).

    The methods are called by
    :meth:`madanalysis.install.install_manager.InstallManager.Execute` in the following
    order (only when defined): ``Detect``/``Remove``, ``GetNcores``,
    ``CreatePackageFolder``, ``CreateTmpFolder``, ``Download``, ``Unpack``, ``Configure``,
    ``Build``, ``PreCheck``, ``Clean``, ``Install``, ``Check`` and ``NeedToRestart``.

    fjcontrib is installed into the local FastJet installation (``tools/fastjet``).
    """

    def __init__(self,main: Main) -> None:
        """Prepare the installation of the FastJet contrib libraries (folders, download URLs).

        Args:
            main (``Main``): session state.
        """
        self.main       = main
        self.installdir = os.path.normpath(self.main.archi_info.ma5dir+'/tools/fastjet/')
        self.bindir     = os.path.normpath(self.installdir+'/bin/fastjet-config')
        self.toolsdir   = os.path.normpath(self.main.archi_info.ma5dir+'/tools')
        self.tmpdir     = self.main.session_info.tmpdir
        self.downloaddir = self.main.session_info.downloaddir
        self.untardir = os.path.normpath(self.tmpdir + '/MA5_fastjetcontrib/')
        self.ncores     = 1
        self.files = {"fastjetcontrib.tar.gz" : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/WikiStart/fjcontrib-1.052.tar.gz"}

    def GetNcores(self) -> None:
        """Ask the number of cores used for the compilation (all cores in forced mode)."""
        self.ncores = InstallService.get_ncores(self.main.archi_info.ncores,\
                                                self.main.forced)


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
        """Download the source files of the FastJet contrib libraries.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Checking connection with MA5 web site
        if not InstallService.check_ma5site():
            return False
        # Launching wget
        logname = os.path.normpath(self.installdir+'/wget_contrib.log')
        if not InstallService.wget(self.files,logname,self.downloaddir):
            return False
        # Ok
        return True


    def Unpack(self) -> bool:
        """Unpack the downloaded files of the FastJet contrib libraries.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Logname
        logname = os.path.normpath(self.installdir+'/unpack_contrib.log')
        # Unpacking the tarball
        ok, packagedir = InstallService.untar(logname, self.downloaddir, self.tmpdir,'fastjetcontrib.tar.gz')
        if not ok:
            return False
        # Ok: returning the good folder
        self.tmpdir=packagedir
        return True


    def Configure(self) -> bool:
        """Configure the FastJet contrib libraries before compilation.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Input
        # TODO: figure out how to give `-std=c++11 -fPIC` together to CXXFLAGS
        # using " or ' doesn't work on linux systems
        theCommands = ['./configure', '--fastjet-config=' + self.bindir, 'CXXFLAGS=-fPIC']
        if self.main.archi_info.has_root and self.main.archi_info.root_compiler:
            theCommands.append("CXX=" + self.main.archi_info.root_compiler)
        logname=os.path.normpath(self.installdir+'/configuration_contrib.log')
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

        
    def Build(self) -> bool:
        """Compile the FastJet contrib libraries.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Input
        theCommands=['make','-j'+str(self.ncores)]
        logname=os.path.normpath(self.installdir+'/compilation_contrib.log')
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
        """Install the FastJet contrib libraries in its definitive folder.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Input
        theCommands=['make','install']
        logname=os.path.normpath(self.installdir+'/installation_contrib.log')
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
        """Check that the FastJet contrib libraries has been properly installed.

        Returns:
            ``bool``:
            ``True`` if the expected files are present.
        """
        # Check folders
        dirs = [self.installdir+"/include/fastjet/contrib",\
                self.installdir+"/lib",\
                self.installdir+"/bin"]
        for dir in dirs:
            if not os.path.isdir(dir):
                logging.getLogger('MA5').error('folder '+dir+' is missing.')
                self.display_log()
                return False

        # Check fastjet executable
        if not os.path.isfile(self.installdir+'/bin/fastjet-config'):
            logging.getLogger('MA5').error("binary labeled 'fastjet-config' is missing.")
            self.display_log()
            return False

        # Check one header file
        if not os.path.isfile(self.installdir+'/include/fastjet/contrib/Nsubjettiness.hh'):
            logging.getLogger('MA5').error("header labeled 'include/fastjet/contrib/Nsubjettiness.hh' is missing.")
            self.display_log()
            return False

        if (not os.path.isfile(self.installdir+'/lib/libNsubjettiness.so')) and \
           (not os.path.isfile(self.installdir+'/lib/libNsubjettiness.a')):
            logging.getLogger('MA5').error("library labeled 'libNsubjettiness.so' or 'libNsubjettiness.a' is missing.")
            self.display_log()
            return False
        
        return True

    def display_log(self) -> None:
        """Log the paths of the installation log files."""
        logging.getLogger('MA5').error("More details can be found into the log files:")
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/wget_contrib.log"))
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/unpack_contrib.log"))
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/configuration_contrib.log"))
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/compilation_contrib.log"))
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/installation_contrib.log"))

    def NeedToRestart(self) -> bool:
        """Tell whether MadAnalysis 5 must be restarted after the installation.

        Returns:
            ``bool``:
            ``True`` if a restart (new configuration check and library build) is needed.
        """
        return True
    
        
