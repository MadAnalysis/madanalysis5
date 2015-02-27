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
import glob
import shutil

class InstallPad:

    def __init__(self,main):
        self.main        = main
        self.installdir  = os.path.normpath(self.main.archi_info.ma5dir+'/PAD')
        self.tmpdir      = self.main.session_info.tmpdir
        self.downloaddir = self.installdir + "/Build/SampleAnalyzer/User/Analyzer"
        self.untardir    = ""
        self.ncores      = 1
        self.analyses    = {"cms_sus_13_012", "cms_sus_13_016", "atlas_sus_13_05", "atlas_susy_2013_11"}
        self.files = { 
    "cms_sus_13_011.cpp" : "http://inspirehep.net/record/1301484/files/cms_sus_13_011.cpp",
    "cms_sus_13_011.h"   : "http://inspirehep.net/record/1301484/files/cms_sus_13_011.h",
    "cms_sus_13_011.info": "http://inspirehep.net/record/1301484/files/cms_sus_13_011.info",
    "cms_sus_13_016.cpp" : "http://inspirehep.net/record/1305194/files/cms_sus_13_016.cpp",
    "cms_sus_13_016.h"   : "http://inspirehep.net/record/1305194/files/cms_sus_13_016.h",
    "cms_sus_13_016.info": "http://inspirehep.net/record/1305194/files/cms_sus_13_016.info",
    "cms_sus_13_012.cpp" : "http://inspirehep.net/record/1305458/files/cms_sus_13_012.cpp",
    "cms_sus_13_012.h"   : "http://inspirehep.net/record/1305458/files/cms_sus_13_012.h",
#    "cms_sus_13_012.info": "http://inspirehep.net/record/1305458/files/cms_sus_13_012.info",
    "atlas_sus_13_05.cpp"    : "http://inspirehep.net/record/1325001/files/atlas_sus_13_05.cpp",
    "atlas_sus_13_05.h"      : "http://inspirehep.net/record/1325001/files/atlas_sus_13_05.h",
    "atlas_sus_13_05.info"   : "http://inspirehep.net/record/1325001/files/atlas_sus_13_05.info",
    "atlas_susy_2013_11.cpp" : "http://inspirehep.net/record/1326686/files/atlas_susy_2013_11.cpp",
    "atlas_susy_2013_11.h"   : "http://inspirehep.net/record/1326686/files/atlas_susy_2013_11.h",
    "atlas_susy_2013_11.info": "http://inspirehep.net/record/1326686/files/atlas_susy_2013_11.info"}


    def Detect(self):
        if not os.path.isdir(self.installdir):
            logging.debug("The folder "+self.installdir+"' is not found")
            return False
        return True


    def Remove(self,question=True):
        import time
        bkpname = "pad-v" + time.strftime("%Y%m%d-%Hh%M") + ".tgz"
        logging.info("     => Backuping the previous installation: " + bkpname)
        logname = os.path.normpath(self.main.archi_info.ma5dir+'/pad-backup.log')
        TheCommand = ['tar', 'czf', bkpname, 'PAD']
        logging.debug('Shell command: '+' '.join(TheCommand))
        ok, out= ShellCommand.ExecuteWithLog(TheCommand,logname,self.main.archi_info.ma5dir,silent=False)
        if not ok:
            return False
        logging.info("     => Backup done")
        from madanalysis.IOinterface.folder_writer import FolderWriter
        return FolderWriter.RemoveDirectory(self.installdir,question)


    def GetNcores(self):
        self.ncores = InstallService.get_ncores(self.main.archi_info.ncores,\
                                                self.main.forced)


    def CreatePackageFolder(self):
        TheCommand = ['bin/ma5', '-E', '-f', 'PAD', 'cms_sus_13_011']
        logname = os.path.normpath(self.main.archi_info.ma5dir+'/PAD-workingdir.log')
        ok, out= ShellCommand.ExecuteWithLog(TheCommand,logname,self.main.archi_info.ma5dir,silent=False)
        if not ok:
            return False
        for analysis in self.analyses:
          TheCommand = ['./newAnalyzer.py', analysis, analysis]
          lname = os.path.normpath(self.installdir+'/PAD-'+analysis+'.log')
          ok, out= ShellCommand.ExecuteWithLog(TheCommand,lname,\
            self.installdir+'/Build/SampleAnalyzer',silent=False)
          if not ok:
              return False
        TheCommand = ['mv',logname,self.installdir]
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False
        return True


    def Download(self):
        # Checking connection with InSpire
        if not InstallService.check_inspire():
            return False
        # Launching wget
        logname = os.path.normpath(self.installdir+'/wget.log')
        if not InstallService.wget(self.files,logname,self.downloaddir):
            return False
        # Ok
        return True


    def Configure(self):
        # Updating the makefile
        TheCommand = ['mv',self.installdir+'/Build/Makefile', self.installdir+'/Build/Makefile.save']
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False
        inp = open(self.installdir+'/Build/Makefile.save', 'r')
        out = open(self.installdir+'/Build/Makefile', 'w')
        for line in inp:
          out.write(line)
          if 'LIBFLAGS += -lcommons_for_ma5' in line:
            out.write("LIBFLAGS += -lMinuit\n")
        inp.close()
        out.close()
        return ok


    def Build(self):
        # Input
        theCommands=['make','-j'+str(self.ncores)]
        logname=os.path.normpath(self.installdir+'/Build/compilation.log')
        # Execute
        logging.debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,logname,self.installdir+'/Build',silent=False)
        # return result
        if not ok:
            logging.error('impossible to build the project. For more details, see the log file:')
            logging.error(logname)
        return ok

    def NeedToRestart(self):
        return False


