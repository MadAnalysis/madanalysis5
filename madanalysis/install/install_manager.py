################################################################################
#  
#  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://launchpad.net/madanalysis5>
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


import logging

class InstallManager():

    def __init__(self,main):
        self.main=main

    def Execute(self, rawpackage):

        # Selection of the package
        package=rawpackage.lower()
        if package=='zlib':
            installer=InstallZlib(self.main)
        elif package=='fastjet':
            installer=InstallFastjet(self.main)
        elif package=='delphes':
            from madanalysis.install.install_delphes import InstallDelphes
            installer=InstallDelphes(self.main)
        elif package=='delphesma5tune':
            installer=InstallDelphesMA5tune(self.main)
        elif package=='samples':
            installer=InstallSamples(self.main)
        else:
            logging.error('the package "'+rawpackage+'" is unknown')
            return False

        # Get list of the methods of the installer class
        # If the method does not exist, the method is not called
        methods = dir(installer)

        # 1. Asking for number of cores
        if 'GetNcores' in methods:
            ncores = installer.GetNcores()

        # 2. Creating a folder
        installdir=''
        if 'CreatePackageFolder' in methods:
            logging.info("Creating a devoted folder ...")
            ok, installdir = installer.CreatePackageFolder()
            if not ok:
                return False
            logging.debug('install folder: '+installdir)

        # 3. Creating a temporary folder
        tmpdir=''
        if 'CreateTmpFolder' in methods:
            logging.info("Creating a temporary folder ...")
            ok, tmpdir = installer.get_tmp()
            if not ok:
                return False
            logging.debug('temporary folder: '+tmpdir)

        # Choose the working path
        if tmpdir!='':
            workdir=tmpdir
        else:
            workdir=installdir

        # 4. Downloading
        if 'Download' in methods:
            logging.info("Downloading the package ...")
            if not installer.Download(workdir):
                return False

        # 5. Unpacking
        # workdir could be modified. Example:
        # /tmp/econte/ -> /tmp/econte/fastjet3.4/
        if 'Unpack' in methods:
            logging.info("Unpacking the package ...")
            ok, workdir = installer.Unpack(workdir)
            if not ok:
                return False

        # 6. Configuring
        if 'Configure' in methods:
            logging.info("Configuring the package ...")
            if not installer.Configure(workdir):
                return False

        # 7. Compiling
        if 'Build' in methods:
            logging.info("Building the package ...")
            if not installer.Build(workdir):
                return False

        # 8. Checking
        if 'Check' in methods:
            logging.info("Checking the compilation ...")
            if not installer.Check(workdir):
                return False
        
        # Transfering the data : workdir -> installdir
        # 2 possibilities:
        #   - tmpdir -> installdir
        #   - installdir/package454.5 -> installdir/
        if 'Install' in methods:
            logging.info("Transfering the data form the temporary to the definitive folder ...")
            if not installer.Install(workdir,installdir):
                return False

        # 9. Checking (again)
        if 'Check' in methods and installdir!=workdir:
            logging.info("Checking the installation ...")
            if not installer.Check(installdir):
                return False

        # 10. End: restart MA5 session?
        logging.info("Installation complete.")
        if installer.NeedRestart():
            return 'restart'
        else:
            return True
