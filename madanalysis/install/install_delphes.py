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


from madanalysis.install.install_base import InstallBase
import logging

class InstallDelphes(InstallBase):

    def __init__(self,main):
        InstallBase.__init__(self,main)
        self.files      = {"delphes.tar.gz" : "http://cp3.irmp.ucl.ac.be/downloads/Delphes-3.1.1.tar.gz"}
        self.installdir = os.path.normpath(self.main.archi_info.ma5dir+'/tools/delphes/')

    def Main(self):
        # Asking for number of cores
        ncores = self.get_ncores()

        # Creating a temporary folder
        logging.info("Creating a temporary folder ...")
        ok, tmpdir = self.get_tmp()
        if not ok:
            return False
        logging.debug('temporary folder: '+tmpdir)

        # Downloading
        logging.info("Downloading the package ...")
        if not self.Download(tmpdir):
            return False

        # Unpacking
        logging.info("Unpacking the package ...")
        ok, tmpdir = self.Unpack(tmpdir)
        if not ok:
            return False

        # Configuring
        logging.info("Configuring the package ...")
        if not self.Configure():
            return False

        # Compiling
        logging.info("Building the package ...")
        if not self.Build():
            return False

        # Checking
        logging.info("Checking the compilation ...")
        if not self.Check(tmpdir):
            return False

        # Creating a folder
        logging.info("Creating a devoted folder in 'tools' ...")
        if not self.CreateFolder():
            return False
        
        # Transfering the data
        logging.info("Transfering the data form the temporary to the definitive folder ...")
        if not self.Install():
            return False

        # Checking
        logging.info("Checking the installation ...")
        if not self.Check():
            return False

        # End
        logging.info("Installation complete.")
        return 'restart'


    def CreateFolder(self):
        # Creating tools folder
        if not self.create_tools_folder():
            return False

        # Creating package folder
        if not self.create_package_folder('delphes'):
            return False

        
    def Download(self,path):
        # Checking connection with MA5 web site
        if not self.check_ma5site():
            return False

        # Launching wget
        if not self.wget(files,'delphes',path):
            return False


    def Unpack(self,path):
        # Unpacking the tarball
        ok, packagedir = self.untar(path,'delphes.tar.gz','delphes')
        if not ok:
            return False,''

        # Returning the good folder
        return True, packagedir


    def Configure(self):
        os.system("cd "+packagedir+" ; ./configure > "+self.main.archi_info.ma5dir+"/tools/delphes/"+"configuration.log 2>&1")

        
    def Build(self):
        os.system("cd "+packagedir+" ; make -j"+str(ncores)+" > "+self.main.archi_info.ma5dir+"/tools/delphes/"+"compilation.log 2>&1")


    def Check(self):
        pathname=os.path.normpath(self.main.archi_info.ma5dir+"/tools/delphes/modules")
        logging.debug('Look for '+pathname+' ...')
        if (not os.path.isdir(pathname)):
            logging.error("folder labeled '"+pathname+"' is missing.")
            self.display_log(self.installdir)
            return False

        filename=os.path.normpath(self.installdir+'/modules/ParticlePropagator.h')
        logging.debug('Look for '+filename+' ...')
        if not os.path.isfile(filename):
            logging.error("header labeled '"+filename+"' is missing.")
            self.display_log(self.installdir)
            return False

        filename=os.path.normpath(self.installdir+'/libDelphes.so')
        logging.debug('Look for '+filename+' ...')
        if not os.path.isfile(filename):
            logging.error("library labeled '"+filename+"' is missing.")
            self.display_log(self.installdir)
            return False

        return True
        
