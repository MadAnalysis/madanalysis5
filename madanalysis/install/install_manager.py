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
import sys

from string_tools                                       import StringTools
class InstallManager():

    def __init__(self,main):
        self.main=main

    def Execute(self, rawpackage):

        # Selection of the package
        package=rawpackage.lower()
        if package=='zlib':
            from madanalysis.install.install_zlib import InstallZlib
            installer=InstallZlib(self.main)
        elif package=='fastjet':
            from madanalysis.install.install_fastjet import InstallFastjet
            installer=InstallFastjet(self.main)
        elif package=='fastjet-contrib':
            from madanalysis.install.install_fastjetcontrib import InstallFastjetContrib
            installer=InstallFastjetContrib(self.main)
        elif package=='delphes':
            from madanalysis.install.install_delphes import InstallDelphes
            installer=InstallDelphes(self.main)
        elif package=='delphesma5tune':
            from madanalysis.install.install_delphesMA5tune import InstallDelphesMA5tune
            installer=InstallDelphesMA5tune(self.main)
        elif package=='samples':
            from madanalysis.install.install_samples import InstallSamples
            installer=InstallSamples(self.main)
        elif package=='gnuplot':
            from madanalysis.install.install_gnuplot import InstallGnuplot
            installer=InstallGnuplot(self.main)
        elif package=='matplotlib':
            from madanalysis.install.install_matplotlib import InstallMatplotlib
            installer=InstallMatplotlib(self.main)
        elif package=='root':
            from madanalysis.install.install_root import InstallRoot
            installer=InstallRoot(self.main)
        elif package=='numpy':
            from madanalysis.install.install_numpy import InstallNumpy
            installer=InstallNumpy(self.main)
        elif package=='recastingtools':
            from madanalysis.install.install_recastingtools import InstallRecastingTools
            installer=InstallRecastingTools(self.main)
        else:
            logging.error('the package "'+rawpackage+'" is unknown')
            return False

        # Writing the Makefiles
        logging.info("")
        logging.info("   **********************************************************")
        logging.info("   "+StringTools.Center('Installing '+rawpackage,57))
        logging.info("   **********************************************************")

        # Get list of the methods of the installer class
        # If the method does not exist, the method is not called
        methods = dir(installer)

        # 0. Detecting previous installation
        if 'Detect' in methods:
            logging.info("   Detecting a previous installation ...")
            if installer.Detect():
                logging.info("   => found")
                logging.info("   Removing the previous installation ...")
                ok1, ok2 = installer.Remove(question=True)
                if not ok1 and not ok2:
                    self.PrintBad()
                    return False
                elif not ok1 and ok2:
                    self.PrintSkip()
                    return True
            else:
                logging.info("   => not found. OK")

        # 1. Asking for number of cores
        if 'GetNcores' in methods:
            installer.GetNcores()
            logging.info("   **********************************************************")

        # 2. Creating a folder
        if 'CreatePackageFolder' in methods:
            logging.info("   Creating a devoted folder ...")
            if not installer.CreatePackageFolder():
                self.PrintBad()
                return False

        # 3. Creating a temporary folder
        if 'CreateTmpFolder' in methods:
            logging.info("   Creating a temporary folder ...")
            if not installer.CreateTmpFolder():
                self.PrintBad()
                return False

        # 4. Downloading
        if 'Download' in methods:
            if self.main.session_info.has_web:
                logging.info("   Downloading the package ...")
                if not installer.Download():
                    self.PrintBad()
                    return False
            else:
                logging.warning("   Download is not allowed because the internet access is disabled.")

        # 5. Unpacking
        if 'Unpack' in methods:
            logging.info("   Unpacking the package ...")
            if not installer.Unpack():
                self.PrintBad()
                return False

        # 6. Configuring
        if 'Configure' in methods:
            logging.info("   Configuring the package ...")
            if not installer.Configure():
                self.PrintBad()
                return False

        # 7. Compiling
        if 'Build' in methods:
            logging.info("   Building the package ...")
            if not installer.Build():
                self.PrintBad()
                return False

        # 8. Checking
        if 'PreCheck' in methods:
            logging.info("   Checking the building ...")
            if not installer.PreCheck():
                self.PrintBad()
                return False

        # 9. Clean
        if 'Clean' in methods:
            logging.info("   Cleaning the building ...")
            if not installer.Clean():
                self.PrintBad()
                return False

        # 9. Install
        if 'Install' in methods:
            logging.info("   Transfering the data from the temporary to the definitive folder ...")
            if not installer.Install():
                self.PrintBad()
                return False

        # 10. Checking (again)
        if 'Check' in methods:
            logging.info("   Checking the installation ...")
            if not installer.Check():
                self.PrintBad()
                return False

        # 11. End: restart MA5 session?
        self.PrintGood()

        if installer.NeedToRestart():
            return 'restart'
        else:
            return True

    def PrintGood(self):
        logging.info("   Installation complete.")
        sys.stdout.write("   => Status:")
        sys.stdout.write('\x1b[32m'+'[OK]'+'\x1b[0m'+'\n')
        sys.stdout.flush()
        logging.info("   **********************************************************")
        logging.info("")

    def PrintSkip(self):
        logging.info("   Installation skipped.")
        sys.stdout.write("   => Status:")
        sys.stdout.write('\x1b[35m'+'[SKIPPED]'+'\x1b[0m'+'\n')
        sys.stdout.flush()
        logging.info("   **********************************************************")
        logging.info("")

    def PrintBad(self):
        logging.info("   Installation NOT complete.")
        sys.stdout.write("   => Status:")
        sys.stdout.write('\x1b[31m'+'[FAILURE]'+'\x1b[0m'+'\n')
        sys.stdout.flush()
        logging.info("   **********************************************************")
        logging.info("")
