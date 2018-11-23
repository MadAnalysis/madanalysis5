################################################################################
#  
#  Copyright (C) 2012-2018 Eric Conte, Benjamin Fuks
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

class InstallPadForMA5tune:

    def __init__(self,main):
        self.main        = main
        self.installdir  = os.path.normpath(self.main.archi_info.ma5dir+'/PADForMA5tune')
        self.tmpdir      = self.main.session_info.tmpdir
        self.downloaddir = self.main.session_info.downloaddir
        self.PADdir      = self.installdir + "/Build/SampleAnalyzer/User/Analyzer"
        self.delphesdir  = self.installdir + "/Input/Cards"
        self.untardir    = ""
        self.ncores      = 1
        self.files       = {
          "padma5tune.dat"   : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/padma5tune.dat",
          "bib_padma5tune.dat": "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/bib_padma5tune.dat"
        }
        self.analyses    = []
        self.analysis_files = []
        self.delphes_cards  = []


    def Detect(self):
        if not os.path.isdir(self.installdir):
            logging.getLogger('MA5').debug("The folder "+self.installdir+"' is not found")
            return False
        return True


    def Remove(self,question=True):
        import time
        bkpname = "pad_forma5tune-v" + time.strftime("%Y%m%d-%Hh%M") + ".tgz"
        logging.getLogger('MA5').info("     => Backuping the previous installation: " + bkpname)
        logname = os.path.normpath(self.main.archi_info.ma5dir+'/pad-backup.log')
        TheCommand = ['tar', 'czf', bkpname, 'PADForMA5tune']
        logging.getLogger('MA5').debug('Shell command: '+' '.join(TheCommand))
        ok, out= ShellCommand.ExecuteWithLog(TheCommand,logname,self.main.archi_info.ma5dir,silent=False)
        if not ok:
            return False
        logging.getLogger('MA5').info("     => Backup done")
        from madanalysis.IOinterface.folder_writer import FolderWriter
        return FolderWriter.RemoveDirectory(self.installdir,question)


    def GetNcores(self):
        self.ncores = InstallService.get_ncores(self.main.archi_info.ncores,\
                                                self.main.forced)


    def CreatePackageFolder(self):
        logname = os.path.normpath(self.main.archi_info.ma5dir+'/PAD-workingdir.log')

        # Initialize the expert mode
        logging.getLogger('MA5').debug('Calling the expert mode for file cms_sus_13_011')
        logging.getLogger('MA5').debug('BEGIN ExpertMode')
        from madanalysis.core.expert_mode import ExpertMode
        backup = self.main.expertmode
        self.main.expertmode = True
        expert = ExpertMode(self.main)
        dirname="PADForMA5tune"
        if not expert.CreateDirectory(dirname):
            return False
        filename="cms_sus_13_011"
        if not expert.Copy(filename):
            return False
        self.main.expertmode=backup
        logging.getLogger('MA5').debug('END ExpertMode')

        # Logs
        logging.debug('Creating folder '+self.installdir+'/Logs')
        try:
            os.mkdir(self.installdir+'/Logs')
        except:
            return False

        logging.debug('Move '+logname+' in '+self.installdir)
        import shutil
        try:
            shutil.move(logname,self.installdir+'/'+os.path.basename(logname))
        except:
            pass

        # delphes card directory
        TheCommand = ['mkdir', self.delphesdir]
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False

        # EXIT
        return True

    # Formatting
    def formatting(self,analysis):
        new_analysis =     analysis.replace('-','_')
        new_analysis = new_analysis.replace('atlas_1405_7875', 'atlas_susy_2013_02')
        new_analysis = new_analysis.replace('atlas_sus_13_05' ,'atlas_susy_2013_05')
        return new_analysis.lower()

    def Download(self):
        # Checking connection with InSpire and the ma5 website
        if not InstallService.check_inspire():
            return False
        if not InstallService.check_ma5site():
            return False

        # Downloading the files with the information on the implemented analyses
        logname = os.path.normpath(self.installdir+'/wget_mainfiles.log')
        if not InstallService.wget(self.files,logname,self.downloaddir):
            return False

        # Getting the analysis one by one (and creating first skeleton analyses for each of them)
        logging.getLogger('MA5').debug('Reading the analysis list in ' + os.path.join(self.downloaddir,'pad.dat'))
        analysis_file = open(os.path.join(self.downloaddir,'padma5tune.dat'))
        delphes_dictionary = {}
        analysis_info      = {}
        # Looping over all analyses
        for line in analysis_file:
            download_delphes = True
            if line.startswith('#') or len(line.strip())==0:
                continue
            # decoding the line
            analysis = line.split('|')[0].strip()
            if len(analysis)!=0:
                self.analyses.append(analysis)
            url      = line.split('|')[1].strip()
            delphes  = line.split('|')[2].strip()
            dscrptn  = line.split('|')[3].strip()
            new_delphes  = self.formatting(delphes)
            new_analysis = self.formatting(analysis)
            # filling the global vectors
            if not new_delphes in self.delphes_cards:
                self.delphes_cards.append(new_delphes)
                delphes_dictionary[new_delphes] = [new_analysis]
            else:
                delphes_dictionary[new_delphes].append(new_analysis)
                download_delphes = False
            logname  = os.path.join(self.installdir,'PAD-'+analysis+'.log')
            files = {}
            # Making space in the PAD for the analysis and preparing to download the analysis files
            if len(analysis)!=0 and len(url)!=0:
                logging.getLogger('MA5').debug(" ** Getting the analysis " + new_analysis + ' located at ' + url)
                ## Creating a skeleton if necessary (+ inclusion in the analysis list and in the main)
                if new_analysis != "cms_sus_13_011":
                    logging.getLogger('MA5').debug('  --> Creating a skeleton analysis for ' + new_analysis)
                    TheCommand = ['./newAnalyzer.py', new_analysis, new_analysis]
                    logging.getLogger('MA5').debug('  -->  ' + ' '.join(TheCommand))
                    ok, out= ShellCommand.ExecuteWithLog(TheCommand,logname,self.installdir+'/Build/SampleAnalyzer',silent=False)
                    if not ok:
                        return False
                ## Making space for the new files
                for onefile in ['Build/SampleAnalyzer/User/Analyzer/'+new_analysis+'.cpp', 'Build/SampleAnalyzer/User/Analyzer/'+new_analysis+'.h', 'Build/Main/main.bak']:
                    TheCommand = ['rm', '-f', os.path.join(self.installdir,onefile)]
                    logging.getLogger('MA5').debug('  -->  ' + ' '.join(TheCommand))
                    ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
                    if not ok:
                        return False
                ## Preparing the download of the analysis files
                if url=='MA5-local':
                    url='http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox'
                else:
                    url=os.path.join(url,'files')
                for extension in ['cpp', 'info', 'h']:
                    files[new_analysis+'.'+extension] = os.path.join(url,analysis+'.'+extension)
                    self.analysis_files.append(new_analysis+'.'+extension)
                analysis_info[new_analysis] = dscrptn
            # Preparing to dnwload the delphes card
            if len(delphes)!=0 and download_delphes:
                logging.getLogger('MA5').debug(" ** Getting the delphes card " + new_delphes)
                files[new_delphes] = os.path.join('http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox', delphes)
            # download in a temporary folder
            logging.getLogger('MA5').info('    --> Downloading the files for ' + new_analysis)
            for onefile in files.values():
                logging.getLogger('MA5').debug('     - ' + onefile)
            if not InstallService.wget(files,logname,self.downloaddir):
                return False
        analysis_file.close()

        # Saving the configuration (delphes_Card / analysis connections)
        dico_file = open(os.path.join(self.installdir,'Input','recast_config.dat'),'w')
        dico_file.write('#              delphes card             | Analyses\n')
        dico_file.write('#                                       |\n')
        for k,v in sorted(delphes_dictionary.items()):
            dico_file.write(k.ljust(40,' ') + '| ' + ' '.join([x.ljust(20,' ') for x in v]) + '\n')
        dico_file.close()
        dico_file = open(os.path.join(self.installdir,'Input','analysis_description.dat'),'w')
        dico_file.write('#    Analysis       | Description\n')
        dico_file.write('#                   |\n')
        for k,v in sorted(analysis_info.items()):
            dico_file.write(k.ljust(20,' ') + '| ' + v + '\n')
        dico_file.close()

        # Bibliography file
        logging.getLogger('MA5').debug(" ** Getting the bibliography file " + self.installdir+"/bibliography.bib")
        TheCommand = ['cp', os.path.join(self.downloaddir,'bib_padma5tune.dat'), self.installdir+"/bibliography.bib"]
        logging.getLogger('MA5').debug('  -->  ' + ' '.join(TheCommand))
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False

        # Ok
        return True


    def Unpack(self):
        # Restoring the compatibility with MA5 versions posterior to v1.4
        logging.getLogger('MA5').debug(" ** Restoring the compatibility with MA5 versions posterior to v1.4")
        for analysis in self.analyses:
            new_analysis = self.formatting(analysis)
            for extension in ['h','cpp', 'info']:
                newfile=open(os.path.join(self.PADdir,      new_analysis + '.'+extension),'w')
                oldfile=open(os.path.join(self.downloaddir, new_analysis + '.'+extension),'r')
                rootheaders=False
                for line in oldfile:
                    if 'RootMainHeaders.h' in line:
                        rootheaders=True
                    if 'TLorentzVector' in line:
                        if new_analysis=='atlas_susy_2013_05' and extension=='h':
                            newfile.write(line.replace('TLorentzVector','MA5::MALorentzVector'))
                        else:
                            newfile.write(line.replace('TLorentzVector','MALorentzVector'))
                    elif 'WARNING' in line:
                        newfile.write(line.replace('WARNING','//WARNING'))
                    elif new_analysis=='atlas_susy_2013_02' and 'pt() > 130' in line and '6jm' in line and not 'AddCut' in line:
                        newfile.write('}}}}\n')
                        newfile.write(line)
                    elif analysis in line:
                        newfile.write(line.replace(analysis, new_analysis))
                    else:
                        newfile.write(line)
                newfile.close()
                oldfile.close()
                if (extension == 'h') and not rootheaders:
                    with open(os.path.join(self.PADdir, new_analysis+'.'+extension), 'r+') as f:
                        content = f.read()
                        f.seek(0, 0)
                        f.truncate()
                        f.write(content.replace('#include','#include \"SampleAnalyzer/Interfaces/root/RootMainHeaders.h\"\n#include'))

        # the delphes cards
        for myfile in self.delphes_cards:
            shutil.copy(os.path.join(self.downloaddir,myfile), self.delphesdir)

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
        logging.getLogger('MA5').debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,logname,self.installdir+'/Build',silent=False)
        # return result
        if not ok:
            logging.getLogger('MA5').error('impossible to build the project. For more details, see the log file:')
            logging.getLogger('MA5').error(logname)
        return ok

    def Check(self):
        for path in glob.glob(self.installdir+"/*.log"):
          shutil.move(path, self.installdir+'/Logs')
        return True

    def NeedToRestart(self):
        return False


