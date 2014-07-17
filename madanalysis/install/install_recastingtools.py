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

class InstallRecastingTools:

    def __init__(self,main):
        self.main        = main
        self.installdir  = os.path.normpath(self.main.archi_info.ma5dir+'/tools/RecastingTools/')
        self.toolsdir    = os.path.normpath(self.main.archi_info.ma5dir+'/tools')
        self.tmpdir      = self.main.session_info.tmpdir
        self.downloaddir = self.main.session_info.downloaddir
        self.untardir    = os.path.normpath(self.tmpdir + '/MA5_RecastingTools/')
        self.ncores      = 1
        self.files = {"exclusion_CLs.py" : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/PhysicsAnalysisDatabase/exclusion_CLs.py"}


    def Detect(self):
        if not os.path.isdir(self.toolsdir):
            logging.debug("The folder '"+self.toolsdir+"' is not found")
            return False
        if not os.path.isdir(self.installdir):
            logging.debug("The folder "+self.installdir+"' is not found")
            return False
        return True


    def Remove(self,question=True):
        from madanalysis.IOinterface.folder_writer import FolderWriter
        return FolderWriter.RemoveDirectory(self.installdir,question)


    def GetNcores(self):
      return 1

    def CreatePackageFolder(self):
        if not InstallService.create_tools_folder(self.toolsdir):
            return False
        if not InstallService.create_package_folder(self.toolsdir,'RecastingTools'):
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
        # copying the file (no unpacking necessary here) + chmod
        import shutil
        source      = os.path.normpath(self.downloaddir+'/exclusion_CLs.py')
        destination = os.path.normpath(self.installdir +'/exclusion_CLs.py')
        try:
            shutil.copy(source,destination)
        except:
            logging.error('impossible to copy "'+source+'" to "'+destination+'"')
            return False, ''
        import stat
        try:
            st = os.stat(destination)
            os.chmod(destination, st.st_mode | stat.S_IEXEC)
        except:
            logging.error('impossible to make executable the file "'+destination+'"')
            return False, ''
        return True


    def Configure(self):
        return True

    def Build(self):
        return True

    def Install(self):
        return True

    def Check(self):
        if not os.path.isfile(self.installdir+'/exclusion_CLs.py'):
            logging.error("recasting tools are missing.")
            self.display_log()
            return False
        return True

    def display_log(self):
        logging.error("More details can be found into the log files:")
        logging.error(" - "+os.path.normpath(self.installdir+"/wget.log"))
        logging.error(" - "+os.path.normpath(self.installdir+"/copy.log"))

    def NeedToRestart(self):
        return True  # if not, the script will be not copied in the working folder
