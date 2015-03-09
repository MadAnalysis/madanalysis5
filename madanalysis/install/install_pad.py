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
        self.delphesdir  = self.installdir + "/Input/Cards"
        self.untardir    = ""
        self.ncores      = 1
        self.analyses    = ["cms_sus_13_011", "cms_sus_13_012", "cms_sus_13_016", "atlas_sus_13_05", "atlas_susy_2013_11",
            "atlas_higg_2013_03"]
        self.files = {
    "cms_sus_13_011.cpp" : "http://inspirehep.net/record/1301484/files/cms_sus_13_011.cpp",
    "cms_sus_13_011.h"   : "http://inspirehep.net/record/1301484/files/cms_sus_13_011.h",
    "cms_sus_13_011.info": "http://inspirehep.net/record/1301484/files/cms_sus_13_011.info",
    "cms_sus_13_016.cpp" : "http://inspirehep.net/record/1305194/files/cms_sus_13_016.cpp",
    "cms_sus_13_016.h"   : "http://inspirehep.net/record/1305194/files/cms_sus_13_016.h",
    "cms_sus_13_016.info": "http://inspirehep.net/record/1305194/files/cms_sus_13_016.info",
    "cms_sus_13_012.cpp" : "http://inspirehep.net/record/1305458/files/cms_sus_13_012.cpp",
    "cms_sus_13_012.h"   : "http://inspirehep.net/record/1305458/files/cms_sus_13_012.h",
    "cms_sus_13_012.info": "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/PhysicsAnalysisDatabase/cms_sus_13_012.info",
    "atlas_sus_13_05.cpp"    : "http://inspirehep.net/record/1325001/files/atlas_sus_13_05.cpp",
    "atlas_sus_13_05.h"      : "http://inspirehep.net/record/1325001/files/atlas_sus_13_05.h",
    "atlas_sus_13_05.info"   : "http://inspirehep.net/record/1325001/files/atlas_sus_13_05.info",
    "atlas_susy_2013_11.cpp" : "http://inspirehep.net/record/1326686/files/atlas_susy_2013_11.cpp",
    "atlas_susy_2013_11.h"   : "http://inspirehep.net/record/1326686/files/atlas_susy_2013_11.h",
    "atlas_susy_2013_11.info": "http://inspirehep.net/record/1326686/files/atlas_susy_2013_11.info",
    "atlas_higg_2013_03.cpp" : "http://inspirehep.net/record/1347081/files/atlas_higg_2013_03.cpp",
    "atlas_higg_2013_03.h"   : "http://inspirehep.net/record/1347081/files/atlas_higg_2013_03.h",
    "atlas_higg_2013_03.info": "http://inspirehep.net/record/1347081/files/atlas_higg_2013_03.info"
}

        self.delphescards = {
    "delphes_card_cms_standard.tcl"   : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/PhysicsAnalysisDatabase/delphesMA5tune_card_CMS_SUSY.tcl",
    "delphes_card_atlas_standard.tcl" : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/PhysicsAnalysisDatabase/delphesMA5tune_card_ATLAS.tcl",
    "delphes_card_atlas_sus_2013_05.tcl" : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/PhysicsAnalysisDatabase/delphesMA5tune_card_ATLAS_05.tcl",
    "delphes_card_atlas_sus_2013_11.tcl" : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/PhysicsAnalysisDatabase/delphesMA5tune_card_ATLAS_dileptonSUSY.tcl"
    }

    def CreateBibtex(self):
      try:
        file = open(self.installdir+"/bibliography.bib",'w')
      except:
        logging.error('impossible to write the file '+file)
        return False

      # MA5
      file.write('@article{Dumont:2014tja,\n')
      file.write('      author         = \"Dumont, B. and Fuks, B. and Kraml, S. and Bein, S. and\n')
      file.write('                        Chalons, G. and others\",\n')
      file.write('      title          = \"{Toward a public analysis database for LHC new physics\n')
      file.write('                        searches using MADANALYSIS 5}\",\n')
      file.write('      journal        = \"Eur.Phys.J.\",\n')
      file.write('      number         = \"2\",\n')
      file.write('      volume         = \"C75\",\n')
      file.write('      pages          = \"56\",\n')
      file.write('      doi            = \"10.1140/epjc/s10052-014-3242-3\",\n')
      file.write('      year           = \"2015\",\n')
      file.write('      eprint         = \"1407.3278\",\n')
      file.write('      archivePrefix  = \"arXiv\",\n')
      file.write('      primaryClass   = \"hep-ph\",\n')
      file.write('      reportNumber   = \"CERN-PH-TH-2014-109, LAPTH-048-14, LPSC14143\",\n')
      file.write('      SLACcitation   = \"%%CITATION = ARXIV:1407.3278;%%\",\n')
      file.write('}\n')

      file.write('@article{Conte:2012fm,\n')
      file.write('      author         = \"Conte, Eric and Fuks, Benjamin and Serret, Guillaume\",\n')
      file.write('      title          = \"{MadAnalysis 5, A User-Friendly Framework for Collider\n')
      file.write('                        Phenomenology}\",\n')
      file.write('      journal        = \"Comput.Phys.Commun.\",\n')
      file.write('      volume         = \"184\",\n')
      file.write('      pages          = \"222-256\",\n')
      file.write('      doi            = \"10.1016/j.cpc.2012.09.009\",\n')
      file.write('      year           = \"2013\",\n')
      file.write('      eprint         = \"1206.1599\",\n')
      file.write('      archivePrefix  = \"arXiv\",\n')
      file.write('      primaryClass   = \"hep-ph\",\n')
      file.write('      reportNumber   = \"IPHC-PHENO-06\",\n')
      file.write('      SLACcitation   = \"%%CITATION = ARXIV:1206.1599;%%\",\n')
      file.write('}\n')

      # CMS-13-011
      file.write('@article{,\n')
      file.write('      author         = \"Dumont, Beranger and Fuks, Benjamin and Wymant, Chris\",\n')
      file.write('      title          = \"{MadAnalysis 5 implementation of CMS-SUS-13-011: search\n')
      file.write('                        for stops in the single lepton final state at 8 TeV}\",\n')
      file.write('      doi            = \"10.7484/INSPIREHEP.DATA.LR5T.2RR3\",\n')
      file.write('      SLACcitation   = \"%%CITATION = INSPIRE-1301484;%%\",\n')
      file.write('}\n')

      #CMS-13-012
      file.write('@article{,\n')
      file.write('      author         = \"Bein, Samuel and Sengupta, Dipan\",\n')
      file.write('      title          = \"{MadAnalysis 5 implementation of CMS-SUS-13-012}\",\n')
      file.write('      doi            = \"10.7484/INSPIREHEP.DATA.83GG.U5BW\",\n')
      file.write('      SLACcitation   = \"%%CITATION = INSPIRE-1305458;%%\",\n')
      file.write('}\n')

      # CMS-13-016
      file.write('@article{,\n')
      file.write('      author         = \"Sengupta, Dipan and Kulkarni, Suchita\",\n')
      file.write('      title          = \"{MadAnalysis 5 implementation of CMS-SUS-13-016}\",\n')
      file.write('      doi            = \"10.7484/INSPIREHEP.DATA.ZC3J.646F\",\n')
      file.write('      SLACcitation   = \"%%CITATION = INSPIRE-1305194;%%\",\n')
      file.write('}\n')

      #ATLAS-HIGG-2013-03
      file.write('@article{,\n')
      file.write('      author         = \"Dumont, Beranger\",\n')
      file.write('      title          = \"{MadAnalysis 5 implementation of ATLAS-HIGG-2013-03}\",\n')
      file.write('      doi            = \"10.7484/INSPIREHEP.DATA.RT3V.9PJK\",\n')
      file.write('      SLACcitation   = \"%%CITATION = INSPIRE-1347081;%%\",\n')
      file.write('}\n')

      #ATLAS-SUS-2013-11
      file.write('@article{,\n')
      file.write('      author         = \"Dumont, Beranger\",\n')
      file.write('      title          = \"{MadAnalysis 5 implementation of ATLAS-SUSY-2013-11:\n')
      file.write('                        di-leptons plus MET}\",\n')
      file.write('      doi            = \"10.7484/INSPIREHEP.DATA.HLMR.T56W.2\",\n')
      file.write('      SLACcitation   = \"%%CITATION = INSPIRE-1326686;%%\",\n')
      file.write('}\n')

      #ATLAS-SUS-2013-05
      file.write('@article{,\n')
      file.write('      author         = \"Chalons, Guillaume\",\n')
      file.write('      title          = \"{MadAnalysis 5 implementation of ATLAS-SUSY-2013-05}\",\n')
      file.write('      doi            = \"10.7484/INSPIREHEP.DATA.Z4ML.3W67.2\",\n')
      file.write('      SLACcitation   = \"%%CITATION = INSPIRE-1325001;%%\",\n')
      file.write('}\n')

      file.close()

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
        TheCommand = ['bin/ma5', '-R', '-E', '-f', 'PAD', 'cms_sus_13_011']
        logname = os.path.normpath(self.main.archi_info.ma5dir+'/PAD-workingdir.log')
        ok, out= ShellCommand.ExecuteWithLog(TheCommand,logname,self.main.archi_info.ma5dir,silent=False)
        if not ok:
            return False
        for analysis in self.analyses:
          if "cms_sus_13_011" not in analysis:
            TheCommand = ['./newAnalyzer.py', analysis, analysis]
            lname = os.path.normpath(self.installdir+'/PAD-'+analysis+'.log')
            ok, out= ShellCommand.ExecuteWithLog(TheCommand,lname,\
              self.installdir+'/Build/SampleAnalyzer',silent=False)
            if not ok:
                return False
          TheCommand = ['rm', '-f', self.installdir+'/Build/SampleAnalyzer/User/Analyzer/'+analysis+'.cpp',\
                self.installdir+'/Build/SampleAnalyzer/User/Analyzer/'+analysis+'.h']
          ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
          if not ok:
            return False
        # Logs
        TheCommand = ['mkdir', self.installdir+'/Logs']
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False
        TheCommand = ['mv',logname,self.installdir]
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False
        #bibtex
        self.CreateBibtex()
        # delphes card directory
        TheCommand = ['mkdir', self.installdir+'/Input/Cards']
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False
        self.CreateBibtex()
        return True


    def Download(self):
        # Checking connection with InSpire and the ma5 website
        if not InstallService.check_inspire():
            return False
        if not InstallService.check_ma5site():
            return False
        # Launching wget
        logname = os.path.normpath(self.installdir+'/wget_analyses.log')
        if not InstallService.wget(self.files,logname,self.downloaddir):
            return False
        # delphes cards
        logname = os.path.normpath(self.installdir+'/wget_delphescards.log')
        if not InstallService.wget(self.delphescards,logname,self.delphesdir):
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
        TheCommand = ['rm', '-f', self.installdir+'/Build/Makefile.save']
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False

        # Updating the main in order to get a correct file name for the template analysis
        TheCommand = ['mv',self.installdir+'/Build/Main/main.cpp', self.installdir+'/Build/Main/main.cpp.save']
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False
        inp = open(self.installdir+'/Build/Main/main.cpp.save', 'r')
        out = open(self.installdir+'/Build/Main/main.cpp', 'w')
        for line in inp:
          if 'user.saf' in line:
            out.write("      manager.InitializeAnalyzer(\"cms_sus_13_011\",\"cms_sus_13_011.saf\",parametersA1);\n")
          else:
            out.write(line)
        inp.close()
        out.close()
        TheCommand = ['rm', '-f', self.installdir+'/Build/Main/main.cpp.save']
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False
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

    def Check(self):
        for path in glob.glob(self.installdir+"/*.log"):
          shutil.move(path, self.installdir+'/Logs')
        return True

    def NeedToRestart(self):
        return False


