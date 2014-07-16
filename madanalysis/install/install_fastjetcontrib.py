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


from madanalysis.install.install_service import InstallService
from shell_command import ShellCommand
import os
import sys
import logging

class InstallFastjetContrib:

    def __init__(self,main):
        self.main       = main
        self.installdir = os.path.normpath(self.main.archi_info.ma5dir+'/tools/fastjet/')
        self.bindir     = os.path.normpath(self.installdir+'/bin/fastjet-config')
        self.toolsdir   = os.path.normpath(self.main.archi_info.ma5dir+'/tools')
        self.tmpdir     = self.main.session_info.tmpdir
        self.downloaddir = self.main.session_info.downloaddir
        self.untardir = os.path.normpath(self.tmpdir + '/MA5_fastjetcontrib/')
        self.ncores     = 1
        self.files = {"fastjetcontrib.tar.gz" : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/WikiStart/fjcontrib-1.012.tar.gz"}


    def GetNcores(self):
        self.ncores = InstallService.get_ncores(self.main.archi_info.ncores,\
                                                self.main.forced)


    def CreateTmpFolder(self):
        ok = InstallService.prepare_tmp(self.untardir, self.downloaddir)
        if ok:
            self.tmpdir=self.untardir
        return ok
        
    def Download(self):
        # Checking connection with MA5 web site
        if not InstallService.check_ma5site():
            return False
        # Launching wget
        logname = os.path.normpath(self.installdir+'/wget_contrib.log')
        if not InstallService.wget(self.files,logname,self.downloaddir):
            return False
        # Ok
        return True


    def Unpack(self):
        # Logname
        logname = os.path.normpath(self.installdir+'/unpack_contrib.log')
        # Unpacking the tarball
        ok, packagedir = InstallService.untar(logname, self.tmpdir,'fastjetcontrib.tar.gz')
        if not ok:
            return False
        # Ok: returning the good folder
        self.tmpdir=packagedir
        return True


    def Configure(self):
        # Input
        theCommands=['./configure','--fastjet-config='+self.bindir]
        logname=os.path.normpath(self.installdir+'/configuration_contrib.log')
        # Execute
        logging.debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             self.tmpdir,\
                                             silent=False)
        # return result
        if not ok:
            logging.error('impossible to configure the project. For more details, see the log file:')
            logging.error(logname)
        return ok

        
    def Build(self):
        # Input
        theCommands=['make','-j'+str(self.ncores)]
        logname=os.path.normpath(self.installdir+'/compilation_contrib.log')
        # Execute
        logging.debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             self.tmpdir,\
                                             silent=False)
        # return result
        if not ok:
            logging.error('impossible to build the project. For more details, see the log file:')
            logging.error(logname)
        return ok


    def Install(self):
        # Input
        theCommands=['make','install']
        logname=os.path.normpath(self.installdir+'/installation_contrib.log')
        # Execute
        logging.debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             self.tmpdir,\
                                             silent=False)
        # return result
        if not ok:
            logging.error('impossible to build the project. For more details, see the log file:')
            logging.error(logname)
        return ok


    def Check(self):
        # Check folders
        dirs = [self.installdir+"/include/fastjet/contrib",\
                self.installdir+"/lib",\
                self.installdir+"/bin"]
        for dir in dirs:
            if not os.path.isdir(dir):
                logging.error('folder '+dir+' is missing.')
                self.display_log()
                return False

        # Check fastjet executable
        if not os.path.isfile(self.installdir+'/bin/fastjet-config'):
            logging.error("binary labeled 'fastjet-config' is missing.")
            self.display_log()
            return False

        # Check one header file
        if not os.path.isfile(self.installdir+'/include/fastjet/contrib/Nsubjettiness.hh'):
            logging.error("header labeled 'include/fastjet/contrib/Nsubjettiness.hh' is missing.")
            self.display_log()
            return False

        if (not os.path.isfile(self.installdir+'/lib/libNsubjettiness.so')) and \
           (not os.path.isfile(self.installdir+'/lib/libNsubjettiness.a')):
            logging.error("library labeled 'libNsubjettiness.so' or 'libNsubjettiness.a' is missing.")
            self.display_log()
            return False
        
        return True

    def display_log(self):
        logging.error("More details can be found into the log files:")
        logging.error(" - "+os.path.normpath(self.installdir+"/wget_contrib.log"))
        logging.error(" - "+os.path.normpath(self.installdir+"/unpack_contrib.log"))
        logging.error(" - "+os.path.normpath(self.installdir+"/configuration_contrib.log"))
        logging.error(" - "+os.path.normpath(self.installdir+"/compilation_contrib.log"))
        logging.error(" - "+os.path.normpath(self.installdir+"/installation_contrib.log"))

    def NeedToRestart(self):
        return True
    
        
