###############################################################################
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


"""Installation of the example event samples (``install samples``)."""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from madanalysis.core.main import Main

import logging
import os

from madanalysis.install.install_service import InstallService


class InstallSamples:
    """Installer of the example event samples (``install samples``).

    The methods are called by
    :meth:`madanalysis.install.install_manager.InstallManager.Execute` in the following
    order (only when defined): ``Detect``/``Remove``, ``GetNcores``,
    ``CreatePackageFolder``, ``CreateTmpFolder``, ``Download``, ``Unpack``, ``Configure``,
    ``Build``, ``PreCheck``, ``Clean``, ``Install``, ``Check`` and ``NeedToRestart``.

    The samples are downloaded into ``<ma5dir>/samples``.
    """

    def __init__(self,main: Main) -> None:
        """Prepare the installation of the example event samples (folders, download URLs).

        Args:
            main (``Main``): session state.
        """
        self.main       = main
        self.installdir = os.path.normpath(self.main.archi_info.ma5dir+'/samples')
        self.files = {
            "ttbar_fh.lhe.gz"    : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/samples/ttbar_fh.lhe.gz", \
            "ttbar_sl_1.lhe.gz"  : "https://github.com/MadAnalysis/samples/raw/refs/heads/main/ttbar_sl_1.lhe.gz", \
            "ttbar_sl_2.lhe.gz"  : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/samples/ttbar_sl_2.lhe.gz", \
            "ttbar_sl_1.lhe"     : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/samples/ttbar_sl_1.lhe", \
            "ttbar_sl_2.lhe"     : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/samples/ttbar_sl_2.lhe", \
            "zz.lhe.gz"          : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/samples/zz.lhe.gz", \
            "mg5_ttbar2l.lhco"   : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/samples/mg5_ttbar2l.lhco", \
            "mg5_zll.lhco"       : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/samples/mg5_zll.lhco", \
            "wplus_prod.hepmc.gz": "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/samples/wplus_prod.hepmc.gz", \
            "MinBias.pileup"     : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/samples/MinBias.pileup", \
            "lljj.lhe.gz"        : "https://github.com/MadAnalysis/samples/raw/refs/heads/main/lljj.lhe.gz"
        }


    def Detect(self) -> bool:
        """Check whether the example event samples is already installed.

        Returns:
            ``bool``:
            ``True`` if the installation folder exists.
        """
        if not os.path.isdir(self.installdir):
            logging.getLogger('MA5').debug("The folder "+self.installdir+"' is not found")
            return False
        return True


    def Remove(self,question: bool = True) -> tuple[bool, bool]:
        """Remove the previous installation of the example event samples.

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
        """Create the installation folder of the example event samples.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        return InstallService.create_package_folder(self.main.archi_info.ma5dir,'samples')


    def Download(self) -> bool:
        """Download the source files of the example event samples.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Checking connection with MA5 web site
        if not InstallService.check_ma5site():
            return False
        # Launching wget
        logname = os.path.normpath(self.installdir+'/wget.log')
        if not InstallService.wget(self.files,logname,self.installdir):
            return False
        # Ok
        return True


    def Check(self) -> bool:
        """Check that the example event samples has been properly installed.

        Returns:
            ``bool``:
            ``True`` if the expected files are present.
        """

        filesToCheck = list(self.files.keys())
        ok=True
        for item in filesToCheck:
            logging.getLogger('MA5').debug('checking file: '+item+ ' ...')
            filename=os.path.normpath(self.installdir+'/'+item)
            if not os.path.isfile(filename):
                logging.getLogger('MA5').error('file called "'+filename+'" is not found')
                ok=False
        return ok

    def NeedToRestart(self) -> bool:
        """Tell whether MadAnalysis 5 must be restarted after the installation.

        Returns:
            ``bool``:
            ``True`` if a restart (new configuration check and library build) is needed.
        """
        return False
    
        
