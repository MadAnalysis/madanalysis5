################################################################################
#  
#  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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
from madanalysis.IOinterface.folder_writer import FolderWriter
from shell_command import ShellCommand
import os
import sys
import logging

class InstallRoot:

    def __init__(self,main):
        self.main       = main
        self.installdir = os.path.normpath(self.main.archi_info.ma5dir+'/tools/root/')
        self.toolsdir   = os.path.normpath(self.main.archi_info.ma5dir+'/tools')
        self.tmpdir     = self.main.session_info.tmpdir
        self.downloaddir= os.path.normpath(self.tmpdir + '/MA5_downloads/')
        self.untardir = os.path.normpath(self.tmpdir + '/MA5_root/')
        self.ncores     = 1
#        self.files = {"root.tar.gz" : "ftp://root.cern.ch/root/root_v5.34.18.source.tar.gz"}
        self.files = {"root.tar.gz" : "https://root.cern.ch/download/root_v6.04.08.source.tar.gz"}
        self.logger = logging.getLogger('MA5')

    def Detect(self):
        if not os.path.isdir(self.toolsdir):
            logging.getLogger('MA5').debug("The folder '"+self.toolsdir+"' is not found")
            return False
        if not os.path.isdir(self.installdir):
            logging.getLogger('MA5').debug("The folder "+self.installdir+"' is not found")
            return False
        return True


    def Remove(self,question=True):
        from madanalysis.IOinterface.folder_writer import FolderWriter
        return FolderWriter.RemoveDirectory(self.installdir,question)


    def CreatePackageFolder(self):
        if not InstallService.create_tools_folder(self.toolsdir):
            return False
        if not InstallService.create_package_folder(self.toolsdir,'root'):
            return False
        return True


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
        logname = os.path.normpath(self.installdir+'/wget.log')
        if not InstallService.wget(self.files,logname,self.downloaddir):
            return False
        # Ok
        return True


    def Unpack(self):
        # Logname
        logname = os.path.normpath(self.installdir+'/unpack.log')
        # Unpacking the tarball
        ok, packagedir = InstallService.untar(logname, self.downloaddir, self.tmpdir,'root.tar.gz')
        if not ok:
            return False
        # Ok: returning the good folder
        self.tmpdir=packagedir
        return True


    def GetNcores(self):
        self.ncores = InstallService.get_ncores(self.main.archi_info.ncores,\
                                                self.main.forced)

    def Configure(self):
        # Input
        theCommands=['./configure','--prefix='+self.installdir,'--disable-gfal','--disable-python']
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


    def Install(self):
        # Input
        theCommands=['make', 'install']
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


    def Build(self):
        # Input
        theCommands=['make','-j'+str(self.ncores)]
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


    def Check(self):
        # Check folders
        dirs = [self.installdir+"/bin",\
                self.installdir+"/lib", \
                self.installdir+"/include"]
        for dir in dirs:
            if not os.path.isdir(dir):
                logging.getLogger('MA5').error('folder '+dir+' is missing.')
                self.display_log()
                return False
            else:
                # Checking root executable
                self.logger.debug('Checking that bin folder is there...')
                if dir == self.installdir+"/bin":
                    self.logger.debug('Checking that root executable is there...')
                    path = os.path.join(os.path.join(self.installdir, "bin"), "root")
                    if not os.path.isfile(path) :
                        logging.getLogger('MA5').error("Root executable doesn't exist.")
                        self.display_log()
                        return False
                    elif os.path.isfile(path) and not os.access(path, os.X_OK):
                        logging.getLogger('MA5').error("You don't have the permission to execute root.")
                        self.display_log()
                        return False
                    self.logger.debug('Checking that root-config executable is there...')
                    path = os.path.join(os.path.join(self.installdir, "bin"), "root-config")
                    if not os.path.isfile(path) :
                        logging.getLogger('MA5').error("Root-config executable doesn't exist.")
                        self.display_log()
                        return False
                    elif os.path.isfile(path) and not os.access(path, os.X_OK):
                        logging.getLogger('MA5').error("You don't have the permission to execute root-config.")
                        self.display_log()
                        return False

                # Checking libraries
                    self.logger.debug('Checking that lib folder is there...')
                elif dir == self.installdir+"/lib":
                    path = os.path.join(os.path.join(self.installdir, "lib"), "root")
                    listdir = os.listdir(path)
                    libs = ["libHist.",   "libCore.", "libGraf3d.", "libMathCore.",\
                            "libMatrix.", "libRIO.",  "libNet.",    "libGraf.",       "libThread.", \
                            "libGpad.",   "libTree.", "libRint.",   "libPostscript.", "libPhysics."]
                    globaltest = True
                    for ref in libs:
                        test=False
                        self.logger.debug('Checking if there is the lib: '+ref+'* ...')
                        for item in listdir:
                            if item.startswith(ref):
                                test=True
                                break
                        if not test:
                            logging.getLogger('MA5').debug('--> NOT found')
                            globaltest=False
                        else:
                            logging.getLogger('MA5').debug('--> found')

                    if not globaltest:
                        logging.getLogger('MA5').error('Libraries are missing. Please reinstall root.')
                        self.display_log()
                        return False

                # Checking headers
                elif dir == self.installdir+"/include":
                    path = os.path.join(os.path.join(self.installdir, "include"), "root")
                    listdir = os.listdir(path)
                    includes = ["TFrame.h", "TROOT.h", "TBenchmark.h", "TString.h", "TText.h",\
                                "TSystem.h", "TInterpreter.h", "TFile.h", "TPaveLabel.h", \
                                "TPaveText.h", "TCanvas.h", "TH1.h", "TStyle.h", "TImage.h", \
                                "TApplication.h", "TLegend.h"]
                    samefiles = list(set(includes).intersection(set(listdir)))
                    if len(samefiles) != len(includes):
                        logging.getLogger('MA5').error('Headers are missing. Please reinstall root.')
                        self.display_log()
                        return False                
        return True

    def display_log(self):
        logging.getLogger('MA5').error("More details can be found into the log files:")
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/wget.log"))
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/unpack.log"))
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/configuration.log"))
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/compilation.log"))
        logging.getLogger('MA5').error(" - "+os.path.normpath(self.installdir+"/installation.log"))

    def NeedToRestart(self):
        return True


