################################################################################
#  
#  Copyright (C) 2012-2025 Jack Araz, Eric Conte & Benjamin Fuks
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


from __future__                          import absolute_import
from madanalysis.install.install_service import InstallService
from shell_command                       import ShellCommand
import glob, os, logging, shutil


class InstallPad:

    def __init__(self,main, padname):
        self.main        = main
        self.padname     = padname.replace('pad','PAD').replace('ma5','MA5');
        self.installdir  = os.path.join(self.main.archi_info.ma5dir,'tools', padname)
        self.tmpdir      = self.main.session_info.tmpdir
        self.downloaddir = self.main.session_info.downloaddir
        self.PADdir      = self.installdir + "/Build/SampleAnalyzer/User/Analyzer"
        self.delphesdir  = self.installdir + "/Input/Cards"
        self.pileupdir   = self.installdir + "/Input/Pileup"
        self.untardir    = ""
        self.ncores      = 1
        self.files= {
          "pad.dat"    : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/pad3.dat",
          "bib_pad.dat": "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/bib_pad3.dat",
          "json_pad.dat":"http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/json_pad3.dat"
        }
        if padname=='PADForMA5tune':
            self.files  = {
             "padma5tune.dat"   : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/padma5tune3.dat",
             "bib_padma5tune.dat": "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/bib_pad3.dat"
            }
        elif padname=='PADForSFS':
            self.files  = {
             "padsfs.dat"     : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/SFS/padsfs3.dat",
             "bib_padsfs.dat" : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/bib_pad3.dat",
             "json_padsfs.dat": "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/SFS/json_padsfs3.dat",
             "csv_padsfs.dat" : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/SFS/csv_padsfs.dat"
            }
        self.analyses       = []
        self.analysis_files = []
        self.pileup_files   = []
        self.delphes_cards  = []
        self.json_cards     = []
        self.csv_cards      = []


    def Detect(self):
        if not os.path.isdir(self.installdir):
            logging.getLogger('MA5').debug("The folder "+self.installdir+"' is not found")
            return False
        return True


    def Remove(self,question=True):
        import time
        bkpname = self.padname + "-v" + time.strftime("%Y%m%d-%Hh%M") + ".tgz"
        logging.getLogger('MA5').info("     => Backuping the previous installation: " + bkpname)
        logname = os.path.normpath(self.main.archi_info.ma5dir+'/tools/'+self.padname+'-backup.log')
        TheCommand = ['tar', 'czf', bkpname, self.padname]
        logging.getLogger('MA5').debug('Shell command: '+' '.join(TheCommand))
        ok, out= ShellCommand.ExecuteWithLog(TheCommand,logname,self.main.archi_info.ma5dir+'/tools',silent=False)
        if not ok:
            return False, False
        logging.getLogger('MA5').info("     => Backup done")
        from madanalysis.IOinterface.folder_writer import FolderWriter
        return FolderWriter.RemoveDirectory(self.installdir,question)


    def GetNcores(self):
        if self.padname != 'PADForSFS':
            self.ncores = InstallService.get_ncores(self.main.archi_info.ncores,\
                                                    self.main.forced)
        else:
            self.ncores = 1


    def CreatePackageFolder(self):
        logname = os.path.normpath(self.main.archi_info.ma5dir+'/tools/'+self.padname+'-workingdir.log')

        # Initialize the expert mode
        filename="cms_b2g_12_012"
        if self.padname == 'PADForMA5tune':
            filename="cms_sus_13_011"
        elif self.padname == 'PADForSFS':
            filename="atlas_susy_2016_07"
        logging.getLogger('MA5').debug('Calling the expert mode for file ' + filename)
        logging.getLogger('MA5').debug('BEGIN ExpertMode')
        from madanalysis.core.expert_mode import ExpertMode
        backup = self.main.expertmode
        self.main.expertmode = True
        expert = ExpertMode(self.main)
        dirname="tools/"+self.padname
        if not expert.CreateDirectory(dirname):
            return False
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

        # delphes card and pileip directory
        for mydir in [ self.delphesdir ] + (self.padname != 'PADForSFS') * [ self.pileupdir ]:
            TheCommand = ['mkdir', mydir]
            ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir+'/tools')
            if not ok:
                return False

        # CSV files
        if "csv_padsfs.dat" in self.files.keys():
            logging.debug('Creating folder '+self.installdir+'/Input/CSV')
            TheCommand = ['mkdir', self.installdir+'/Input/CSV']
            ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir+'/tools')

        # EXIT
        return True

    def Download(self):
        # Checking connection with InSpire and the ma5 website
        if not InstallService.check_ma5site():
            return False
        if not InstallService.check_dataverse():
            return False

        # Downloading the files with the information on the implemented analyses
        logname = os.path.normpath(self.installdir+'/wget_mainfiles.log')
        if not InstallService.wget(self.files,logname,self.downloaddir):
            return False

        # Json information for pyhf
        json_dictionary    = {}
        if self.padname in ['PAD', 'PADForSFS']:
            json_struct_name = [x for x in self.files.keys() if 'json' in x]
            if len(json_struct_name) == 1:
                logging.getLogger('MA5').debug(" ** Getting the list of pyhf-compatible analyses in " +\
                                               self.downloaddir+"/"+json_struct_name[0])
                json_input = open(os.path.join(self.downloaddir,json_struct_name[0]));
                for line in json_input:
                    if len(line.strip())==0 or line.strip().startswith('#'):
                        continue;
                    json_dictionary[line.strip().split('|')[0].strip()] = \
                        [ line.strip().split('|')[1].split(), line.strip().split('|')[2].split()];
                json_input.close();

        # CSV files
        csv_dictionary    = {}
        if self.padname in ['PADForSFS']:
            csv_struct_name = [x for x in self.files.keys() if 'csv' in x]
            if len(csv_struct_name) == 1:
                logging.getLogger('MA5').debug(" ** Getting the list of data CSV files in " + self.downloaddir+"/"+csv_struct_name[0])
                csv_input = open(os.path.join(self.downloaddir,csv_struct_name[0]))
                for line in csv_input:
                    if len(line.strip())==0 or line.strip().startswith('#'): continue
                    analysis_name = line.strip().split('|')[0].strip()
                    server_name = line.strip().split('|')[1].strip()
                    csv_name = line.strip().split('|')[2].strip()
                    tag = line.strip().split('|')[3].strip()
                    if not analysis_name in csv_dictionary.keys(): csv_dictionary[analysis_name] = {}
                    if not server_name in csv_dictionary[analysis_name].keys(): csv_dictionary[analysis_name][server_name] = {}
                    csv_dictionary[analysis_name][server_name][csv_name] = tag
                csv_input.close();

        # Getting the analysis one by one (and creating first skeleton analyses for each of them)
        logging.getLogger('MA5').debug('Reading the analysis list in ' + \
          os.path.join(self.downloaddir,self.padname.replace('For','').lower()+'.dat'))
        analysis_file = open(os.path.join(self.downloaddir,self.padname.replace('For','').lower()+'.dat'))
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
            # getting the dataverse URLs
            delphes_url='';
            if 'dataverse' in delphes:
                if len(delphes.split())==3:
                    delphes_url = 'https://dataverse.uclouvain.be/api/access/datafile/'+delphes.split()[-1][:-1];
                if len(delphes.split())==4:
                    delphes_url = 'https://dataverse.uclouvain.be/api/access/datafile/:persistentId?persistentId=doi:10.14428/DVN/' + \
                       delphes.split()[2].strip() + '/' + delphes.split()[-1][:-1].strip();
                if len(analysis)==0 and len(url)==0:
                   delphes = delphes.split()[1]
                elif self.padname!='PADForSFS':
                   delphes = 'delphes_card_'+delphes.split()[1]+'.tcl';
                else:
                   delphes = 'sfs_card_'+delphes.split()[1]+'.ma5';
            # filling the global vectors
            if len(analysis)==0 and len(url)==0:
                self.pileup_files.append(delphes)
            elif not delphes in self.delphes_cards:
                self.delphes_cards.append(delphes)
                delphes_dictionary[delphes] = [analysis]
            else:
                delphes_dictionary[delphes].append(analysis)
                download_delphes = False
            logname  = os.path.join(self.installdir,self.padname+'-'+analysis+'.log')
            files = {}
            # Making space in the PAD for the analysis and preparing to download the analysis files
            if len(analysis)!=0 and len(url)!=0:
                logging.getLogger('MA5').debug(" ** Getting the analysis " + analysis + ' located at ' + url)
                ## Creating a skeleton if necessary (+ inclusion in the analysis list and in the main)
                if not (analysis in ["cms_b2g_12_012", "cms_sus_13_011"] or \
                        (analysis=="atlas_susy_2016_07" and self.padname=='PADForSFS')):
                    logging.getLogger('MA5').debug('  --> Creating a skeleton analysis for ' + analysis)
                    import sys
                    TheCommand = [sys.executable,'newAnalyzer.py', analysis, analysis]
                    logging.getLogger('MA5').debug('  -->  ' + ' '.join(TheCommand))
                    ok, out= ShellCommand.ExecuteWithLog(TheCommand,logname,
                                                         self.installdir+'/Build/SampleAnalyzer',
                                                         silent=False)
                    if not ok:
                        return False
                ## Making space for the new files
                for onefile in ['Build/SampleAnalyzer/User/Analyzer/'+analysis+'.cpp', 
                                'Build/SampleAnalyzer/User/Analyzer/'+analysis+'.h', 
                                'Build/Main/main.bak']:
                    TheCommand = ['rm', '-f', os.path.join(self.installdir,onefile)]
                    logging.getLogger('MA5').debug('  -->  ' + ' '.join(TheCommand))
                    ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir+'/tools')
                    if not ok:
                        return False
                ## Preparing the download of the analysis files
                if 'dataverse' in url:
                    exts = ['cpp', 'h', 'info'];
                    anl_files = url.split(']')[0].split()[1:];
                    doi=''
                    if len(anl_files)==4:
                       doi=anl_files[0]
                       anl_files=anl_files[1:]
                    for i in range(len(anl_files)):
                        if doi=='':
                            files[analysis+'.'+exts[i]] = 'https://dataverse.uclouvain.be/api/access/datafile/'+anl_files[i];
                        else:
                            files[analysis+'.'+exts[i]] = 'https://dataverse.uclouvain.be/api/access/datafile/:persistentId?persistentId=doi:10.14428/DVN/'+\
                               doi + '/' + anl_files[i];
                    ## json files
                    if analysis in list(json_dictionary.keys()):
                        for i_json in range(len(json_dictionary[analysis][0])):
                            if doi=='':
                                files[analysis+'_'+json_dictionary[analysis][0][i_json]+'.json'] =\
                                   'https://dataverse.uclouvain.be/api/access/datafile/' + json_dictionary[analysis][1][i_json]
                            else:
                                files[analysis+'_'+json_dictionary[analysis][0][i_json]+'.json'] =\
                                   'https://dataverse.uclouvain.be/api/access/datafile/:persistentId?persistentId=doi:10.14428/DVN/' + doi + '/' + json_dictionary[analysis][1][i_json]
                            self.json_cards.append(analysis+'_'+json_dictionary[analysis][0][i_json]+'.json')
                            self.analysis_files.append(analysis+'_'+json_dictionary[analysis][0][i_json]+'.json')
                    ## CSV files
                    if analysis in list(csv_dictionary.keys()):
                        TheCommand = ['mkdir', self.installdir+'/Input/CSV/'+ analysis.upper()]
                        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir+'/tools')
                        for k, v in csv_dictionary[analysis].items():
                            if k != 'dataverse': continue
                            for file, tag in v.items():
                                if doi=='':
                                    files[file+'.csv'] = 'https://dataverse.uclouvain.be/api/access/datafile/' + tag
                                else:
                                    files[file+'.csv'] = 'https://dataverse.uclouvain.be/api/access/datafile/:persistentId?persistentId=doi:10.14428/DVN/' +\
                                       doi + '/' +  tag
                                self.csv_cards.append([analysis, file+'.csv'])
                else:
                    if url=='MA5-local':
                        url='http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/'
                        if self.padname == 'PADForSFS':
                            url += 'SFS'
                        else:
                            url += 'MA5SandBox'
                    else:
                        url=os.path.join(url,'files')
                    for extension in ['cpp', 'info', 'h']:
                        files[analysis+'.'+extension] = os.path.join(url,analysis+'.'+extension)
                        self.analysis_files.append(analysis+'.'+extension)
                analysis_info[analysis] = dscrptn
            # Preparing to download the delphes card
            detector  = "delphes"
            wiki_page = 'MA5SandBox'
            if self.padname == "PADForSFS":
                detector  = "MA5-SFS"
                wiki_page = 'SFS'
            if len(delphes)!=0 and download_delphes:
                logging.getLogger('MA5').debug(" ** Getting the " + detector + " card " + delphes)
                if not 'dataverse' in delphes_url:
                    files[delphes] = os.path.join('http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/'+wiki_page, delphes)
                else:
                    files[delphes] = delphes_url;
            # download in a temporary folder
            if len(analysis)!=0:
                logging.getLogger('MA5').info('    --> Downloading the files for ' + analysis)
            else:
                logging.getLogger('MA5').info('    --> Downloading the pileup file ' + delphes)
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
            dico_file.write(k.ljust(40,' ') + '| ' + ' '.join([x.ljust(25,' ') for x in v]) + '\n')
        dico_file.close()
        dico_file = open(os.path.join(self.installdir,'Input','analysis_description.dat'),'w')
        dico_file.write('#    Analysis       | Description\n')
        dico_file.write('#                   |\n')
        for k,v in sorted(analysis_info.items()):
            dico_file.write(k.ljust(25,' ') + '| ' + v + '\n')
        dico_file.close()

        # Bibliography file
        logging.getLogger('MA5').debug(" ** Getting the bibliography file " + self.installdir+"/bibliography.bib")
        TheCommand = ['cp', os.path.join(self.downloaddir,'bib_'+self.padname.replace('For','').lower()+'.dat'),\
                      self.installdir+"/bibliography.bib"]
        logging.getLogger('MA5').debug('  -->  ' + ' '.join(TheCommand))
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir+'/tools')
        if not ok:
            return False

        # Ok
        return True

    def Unpack(self):
        # Copying the analysis files
        for analysis in self.analyses:
            for extension in ['h','cpp', 'info']:
                newfile = os.path.join(self.PADdir,      analysis + '.'+extension)
                oldfile = os.path.join(self.downloaddir, analysis + '.'+extension)
                shutil.copy(oldfile,newfile)

        # json files for pyhf
        for json in self.json_cards:
            shutil.copy(os.path.join(self.downloaddir,json), self.PADdir)

        # data files
        for csv in self.csv_cards:
            shutil.copy(os.path.join(self.downloaddir,csv[1]), self.installdir+'/Input/CSV/'+ csv[0].upper())

        # the delphes cards
        for myfile in self.delphes_cards:
            shutil.copy(os.path.join(self.downloaddir,myfile), self.delphesdir)

        # the pileup files
        for myfile in self.pileup_files:
            shutil.copy(os.path.join(self.downloaddir,myfile), self.pileupdir)

        return True

    def Configure(self):
        ## not needed for the SFS
        if self.padname == 'PADForSFS':
            return True
        # Updating the makefile
        logging.getLogger('MA5').debug(" ** Preparing the Makefile to build the " + self.padname)
        TheCommand = ['mv',os.path.join(self.installdir,'Build', 'Makefile'), os.path.join(self.installdir,'Build','Makefile.save')]
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir+'/tools')
        if not ok:
            return False
        inp = open(os.path.join(self.installdir,'Build', 'Makefile.save'), 'r')
        out = open(os.path.join(self.installdir,'Build', 'Makefile'     ), 'w')
        for line in inp:
          out.write(line)
          if 'LIBFLAGS += -lcommons_for_ma5' in line:
            out.write("LIBFLAGS += -lMinuit\n")
        inp.close()
        out.close()
        TheCommand = ['rm', '-f', os.path.join(self.installdir,'Build','Makefile.save')]
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir+'/tools')
        if not ok:
            return False

        # Updating the main in order to get a correct file name for the template analysis
        logging.getLogger('MA5').debug(" ** Preparing the main program of the PAD")
        TheCommand = ['mv',os.path.join(self.installdir,'Build','Main','main.cpp'),\
                      os.path.join(self.installdir,'Build','Main','main.cpp.save')]
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir+'/tools')
        if not ok:
            return False
        inp = open(os.path.join(self.installdir,'Build','Main','main.cpp.save'), 'r')
        out = open(os.path.join(self.installdir,'Build','Main','main.cpp'     ), 'w')
        for line in inp:
          if 'user.saf' in line and self.padname=='PAD':
            out.write("      manager.InitializeAnalyzer(\"cms_b2g_12_012\",\"cms_b2g_12_012.saf\",parametersA1);\n")
          elif 'user.saf' in line and self.padname=='PADForMA5tune':
            out.write("      manager.InitializeAnalyzer(\"cms_sus_13_011\",\"cms_sus_13_011.saf\",parametersA1);\n")
          else:
            out.write(line)
        inp.close()
        out.close()
        TheCommand = ['rm', '-f', os.path.join(self.installdir,'Build','Main','main.cpp.save')]
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir+'/tools')
        if not ok:
            return False
        return ok

    def Build(self):
        ## not needed for the SFS
        if self.padname == 'PADForSFS':
            return True
        # Input
        theCommands=['make','-j'+str(self.ncores)]
        logname=os.path.normpath(os.path.join(self.installdir,'Build','compilation.log'))
        # Execute
        logging.getLogger('MA5').debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,logname,os.path.join(self.installdir,'Build'),silent=False)
        # return result
        if not ok:
            logging.getLogger('MA5').error('impossible to build the project. For more details, see the log file:')
            logging.getLogger('MA5').error(logname)
        return ok

    def Check(self):
        for path in glob.glob(self.installdir+"/*.log"):
            shutil.move(path, self.installdir+'/Logs')
        setattr(self.main.session_info,
                "has_"+self.padname.lower().replace("for","").replace("tune",""),
                True)
        return True

    def NeedToRestart(self):
        return False


