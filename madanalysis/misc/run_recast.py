################################################################################
#  
#  Copyright (C) 2012-2016 Eric Conte, Benjamin Fuks
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


from madanalysis.install.detector_manager                       import DetectorManager
from madanalysis.configuration.delphesMA5tune_configuration     import DelphesMA5tuneConfiguration
from madanalysis.configuration.delphes_configuration            import DelphesConfiguration
from madanalysis.IOinterface.folder_writer                      import FolderWriter
from madanalysis.IOinterface.job_writer                         import JobWriter
from madanalysis.IOinterface.library_writer                     import LibraryWriter
from shell_command                                              import ShellCommand
from string_tools                                               import StringTools
import logging
import os
import shutil
import time

class RunRecast():

    def __init__(self, main, dirname):
        self.dirname          = dirname
        self.main             = main
        self.delphes_runcard  = []
        self.analysis_runcard = []
        self.forced           = self.main.forced
        self.detector         = ""
        self.pad              = ""
        self.first11          = True
        self.first12          = True

    def init(self):
        ### First, the analyses to take care off
        self.edit_recasting_card()
        logging.getLogger('MA5').info("   Getting the list of delphes simulation to be performed...")

        ### Getting the list of analyses to recast
        self.get_runs()

        ### Check if we have abnything to do
        if len(self.delphes_runcard)==0:
            logging.getLogger('MA5').warning('No recasting to do... Please check the recasting card')
            return False

        ### Exit
        return True

    def fastsim(self):
        self.main.forced=True
        for runcard in sorted(self.delphes_runcard):
            ## Extracting run infos and checks
            version = runcard[:4]
            card    = runcard[5:]
            if not self.check_run(version):
                self.main.forced=self.forced
                return False

            ## Running the fastsim
            if not self.fastsim_single(version, card):
                self.main.forced=self.forced
                return False
        ## exit
        self.main.forced=self.forced
        return True

    def analysis(self):
        self.main.forced=True
        for del_card in list(set(sorted(self.delphes_runcard))):
            ## Extracting run infos and checks
            version = del_card[:4]
            card    = del_card[5:]
            if not self.check_run(version):
                self.main.forced=self.forced
                return False

            ## Running the analyses
            if not self.analysis_single(version, card):
                self.main.forced=self.forced
                return False
        ## exit
        self.main.forced=self.forced
        return True

    def fastsim_single(self,version,delphescard):
        # Init and header
        self.fastsim_header(version)

        # Activating the right delphes
        detector_handler = DetectorManager(self.main)
        if not detector_handler.manage(self.detector):
            logging.getLogger('MA5').error('Problem with the activation of delphesMA5tune')
            return False

        # Checking whether events have already been generated and if not, event generation
        for item in self.main.datasets:
            if self.detector=="delphesMA5tune":
                evtfile = self.dirname+'/Output/'+item.name+'/RecoEvents_v1x1_'+delphescard.replace('.tcl','')+'.root'
            elif self.detector=="delphes":
                evtfile = self.dirname+'/Output/'+item.name+'/RecoEvents_v1x2_'+delphescard.replace('.tcl','')+'.root'
            if not os.path.isfile(os.path.normpath(evtfile)):
                if not self.generate_events(item,delphescard):
                    return False

        # Exit
        return True


    def analysis_single(self, version, card):

        #ERIC
        PADdir=self.pad
        dirname=self.dirname
        forced_bkp=self.forced
        #ERIC

        ## Init and header
        self.analysis_header(version, card)

        # Activating the right delphes
        detector_handler = DetectorManager(self.main)
        if not detector_handler.manage(self.detector):
            logging.getLogger('MA5').error('Problem with the activation of delphesMA5tune')
            return False

        ## Getting the analyses associated with the given card
        analyses = [ x.replace(version+'_','') for x in self.analysis_runcard if version in x ]
        for del_card,ana_list in self.main.recasting.DelphesDic.items():
            if card == del_card:
                analyses = [ x for x in analyses if x in ana_list]
                break
        #ERIC
        myanalyses=analyses
        mycard=card
        #ERIC

        # Executing the PAD
        for myset in self.main.datasets:
            ## Preparing the PAD
            if not self.update_pad_main(analyses):
                self.main.forced=self.forced
                return False
            if not self.make_pad():
                self.main.forced=self.forced
                return False
            ## Getting the file name corresponding to the events
            eventfile = os.path.normpath(self.dirname + '/Output/' + myset.name + '/RecoEvents/RecoEvents_' +\
                   version.replace('.','x')+'_' + card.replace('.tcl','')+'.root')
            if not os.path.isfile(eventfile):
                logging.getLogger('MA5').error('The file called '+eventfile+' is not found...')
                return False
            ## Running the PAD
            if not self.run_pad(eventfile):
                self.main.forced=self.forced
                return False
            ## Restoring the PAD as it was before
            #ERIC : restore_pad_main() does not exist!!!!!!
            #if not self.restore_pad_main():
            #   self.main.forced=self.forced
            #    return False
            #ERIC
            time.sleep(1.);



            ## event file
            for myset in self.main.datasets:
                ## saving the output
                if not self.main.recasting.SavePADOutput(PADdir,dirname,myanalyses,myset.name):
                    self.main.forced=forced_bkp
                    return False
                if not self.main.recasting.store_root:
                    eventfile = os.path.normpath(self.dirname + '/Output/' + myset.name +\
                                 '/RecoEvents/RecoEvents_' + version.replace('.','x') +\
                                 '_' + mycard.replace('.tcl','')+'.root') 
                    if not os.path.isfile(eventfile):
                        logging.getLogger('MA5').warning('The file called '+eventfile+\
                                                         ' is not found and cannot be removed.')
                    else:
                        try:
                            os.remove(eventfile)
                        except:
                            logging.getLogger('MA5').warning('Impossible to remove the file called '+eventfile)

                time.sleep(1.);
                ## Running the CLs exclusion script (if available)
                if not self.main.recasting.GetCLs(PADdir,dirname,myanalyses,myset.name,myset.xsection,myset.name):
                    self.main.forced=forced_bkp
                    return False
                ## Saving the results
            self.main.forced=forced_bkp
        return True



    def fastsim_header(self, version):
        ## Gettign the version dependent stuff
        to_print = False
        if version=="1.1" and self.first11:
            to_print = True
            tag = version
            self.first11 = False
        elif version!="1.1" and self.first12:
            to_print = True
            tag = "v1.2+"
            self.first12 = False
        ## Printing
        if to_print:
            logging.getLogger('MA5').info("   **********************************************************")
            logging.getLogger('MA5').info("   "+StringTools.Center(tag+' detector simulations',57))
            logging.getLogger('MA5').info("   **********************************************************")

    def analysis_header(self, version, card):
        ## Printing
        logging.getLogger('MA5').info("   **********************************************************")
        logging.getLogger('MA5').info("   "+StringTools.Center(version+' running of the PAD'+\
               ' on events generated with',57))
        logging.getLogger('MA5').info("   "+StringTools.Center(card,57))
        logging.getLogger('MA5').info("   **********************************************************")

    def generate_events(self,dataset,card):
        # Preparing the run
        self.main.recasting.status="off"
        self.main.fastsim.package=self.detector
        self.main.fastsim.clustering=0
        if self.detector=="delphesMA5tune":
            self.main.fastsim.delphes=0
            self.main.fastsim.delphesMA5tune = DelphesMA5tuneConfiguration()
            self.main.fastsim.delphesMA5tune.card = os.path.normpath("../../../../PADForMA5tune/Input/Cards/"+card)
        elif self.detector=="delphes":
            self.main.fastsim.delphesMA5tune = 0
            self.main.fastsim.delphes        = DelphesConfiguration()
            self.main.fastsim.delphes.card   = os.path.normpath("../../../../PAD/Input/Cards/"+card)
        # Execution
        if not self.RunDelphes(dataset):
            logging.getLogger('MA5').error('The ' + detector + ' problem with the running of the fastsim')
            return False
        # Restoring the run
        self.main.recasting.status="on"
        self.main.fastsim.package="none"
        ## Saving the output
        if not os.path.isdir(self.dirname+'/Output/'+dataset.name):
            os.mkdir(self.dirname+'/Output/'+dataset.name)
        if not os.path.isdir(self.dirname+'/Output/'+dataset.name+'/RecoEvents'):
            os.mkdir(self.dirname+'/Output/'+dataset.name+'/RecoEvents')
        if self.detector=="delphesMA5tune":
            shutil.move(self.dirname+'_FastSimRun/Output/_defaultset/RecoEvents0_0/DelphesMA5tuneEvents.root',\
                self.dirname+'/Output/'+dataset.name+'/RecoEvents/RecoEvents_v1x1_'+card.replace('.tcl','')+'.root')
        elif self.detector=="delphes":
            shutil.move(self.dirname+'_FastSimRun/Output/_defaultset/RecoEvents0_0/DelphesEvents.root',\
                self.dirname+'/Output/'+dataset.name+'/RecoEvents/RecoEvents_v1x2_'+card.replace('.tcl','')+'.root')
        ## Cleaning the temporary directory
        if not FolderWriter.RemoveDirectory(os.path.normpath(self.dirname+'_FastSimRun')):
            return False
        ## Exit
        return True

    def RunDelphes(self,dataset):
        # Initializing the JobWriter
        jobber = JobWriter(self.main,self.dirname+'_FastSimRun')

        # Writing process
        logging.getLogger('MA5').info("   Creating folder '"+self.dirname.split('/')[-1]  + "'...")
        if not jobber.Open():
            return False
        logging.getLogger('MA5').info("   Copying 'SampleAnalyzer' source files...")
        if not jobber.CopyLHEAnalysis():
            return False
        if not jobber.CreateBldDir():
            return False
        logging.getLogger('MA5').info("   Inserting your selection into 'SampleAnalyzer'...")
        if not jobber.WriteSelectionHeader(self.main):
            return False
        if not jobber.WriteSelectionSource(self.main):
            return False
        logging.getLogger('MA5').info("   Writing the list of datasets...")
        jobber.WriteDatasetList(dataset)
        logging.getLogger('MA5').info("   Creating Makefiles...")
        if not jobber.WriteMakefiles():
            return False

        # Creating executable
        logging.getLogger('MA5').info("   Compiling 'SampleAnalyzer'...")
        if not jobber.CompileJob():
            return False
        logging.getLogger('MA5').info("   Linking 'SampleAnalyzer'...")
        if not jobber.LinkJob():
            return False

        # Running
        logging.getLogger('MA5').info("   Running 'SampleAnalyzer' over dataset '" +dataset.name+"'...")
        logging.getLogger('MA5').info("    *******************************************************")
        if not jobber.RunJob(dataset):
            logging.getLogger('MA5').error("run over '"+dataset.name+"' aborted.")
        logging.getLogger('MA5').info("    *******************************************************")

        # Exit
        return True

    def update_pad_main(self,analysislist):
        ## backuping the main files and init
        logging.getLogger('MA5').info("   Updating the PAD main executable")
        if os.path.isfile(self.pad+'/Build/Main/main.bak'):
            shutil.move(self.pad+'/Build/Main/main.bak',self.pad+'/Build/Main/main.cpp')
        shutil.move(self.pad+'/Build/Main/main.cpp',self.pad+'/Build/Main/main.bak')
        mainfile = open(self.pad+"/Build/Main/main.bak",'r')
        newfile  = open(self.pad+"/Build/Main/main.cpp",'w')
        ignore = False

        ## creating the main file with the desired analyses inside
        for line in mainfile:
            if '// Getting pointer to the analyzer' in line:
                ignore = True
                newfile.write(line)
                for analysis in analysislist:
                    newfile.write('  std::map<std::string, std::string> prm'+analysis+';\n')
                    newfile.write('  AnalyzerBase* analyzer_'+analysis+'=\n')
                    newfile.write('    manager.InitializeAnalyzer(\"'+analysis+'\",\"'+analysis+'.saf\",'+\
                       'prm'+analysis+');\n')
                    newfile.write(  '  if (analyzer_'+analysis+'==0) return 1;\n\n')
            elif '// Post initialization (creates the new output directory structure)' in line:
                ignore=False
                newfile.write(line)
            elif '!analyzer_' in line and not ignore:
                ignore=True
                for analysis in analysislist:
                    newfile.write('      if (!analyzer_'+analysis+'->Execute(mySample,myEvent)) continue;\n')
            elif '!analyzer1' in line:
                ignore=False
            elif not ignore:
                newfile.write(line)

        ## exit
        mainfile.close()
        newfile.close()
        time.sleep(1.);
        return True

    def make_pad(self):
        # Initializing the compiler
        logging.getLogger('MA5').info('   Compiling the PAD located in '  +self.pad)
        #ERIC
        PADdir=self.pad
        #ERIC
        compiler = LibraryWriter('lib',self.main)
        ncores = compiler.get_ncores2()
        # compiling
        if ncores>1:
            strcores='-j'+str(ncores)
        command = ['make',strcores]
        logfile = self.pad+'/Build/PADcompilation.log'
        result, out = ShellCommand.ExecuteWithLog(command,logfile,PADdir+'/Build')
        time.sleep(1.);
        # Checks and exit
        if not result:
            logging.getLogger('MA5').error('Impossible to compile the PAD. For more details, see the log file:')
            logging.getLogger('MA5').error(logfile)
            return False
        return True

    def run_pad(self,eventfile):
        #ERIC
        PADdir=self.pad
        #ERIC
        ## input file
        if os.path.isfile(self.pad+'/Input/PADevents.list'):
            os.remove(self.pad+'/Input/PADevents.list')
        infile = open(self.pad+'/Input/PADevents.list','w')
        infile.write(eventfile)
        infile.close()
        ## cleaning the output directory
        if not FolderWriter.RemoveDirectory(os.path.normpath(self.pad+'/Output/PADevents')):
            return False
        ## running
        command = ['./MadAnalysis5job', '../Input/PADevents.list']
        ok = ShellCommand.Execute(command,self.pad+'/Build')
        ## checks
        if not ok:
            logging.getLogger('MA5').error('Problem with the run of the PAD on the file: '+ eventfile)
            return False
        os.remove(PADdir+'/Input/PADevents.list')
        ## exit
        time.sleep(1.);
        return True

    ## Prompt to edit the recasting card
    def edit_recasting_card(self):
        if self.forced or self.main.script:
            return
        logging.getLogger('MA5').info("Would you like to edit the recasting Card ? (Y/N)")
        allowed_answers=['n','no','y','yes']
        answer=""
        while answer not in  allowed_answers:
            answer=raw_input("Answer: ")
            answer=answer.lower()
        if answer=="no" or answer=="n":
            return
        else:
            os.system(self.main.session_info.editor+" "+self.dirname+"/Input/recasting_card.dat")
        return

    ## Checking the recasting card to get the analysis to run
    def get_runs(self):
        del_runs = []
        ana_runs = []
        ## decoding the card
        runcard = open(self.dirname+"/Input/recasting_card.dat",'r')
        for line in runcard:
            if len(line.strip())==0 or line.strip().startswith('#'):
                continue
            myline=line.split()
            if myline[2].lower() =='on' and myline[3] not in del_runs:
                del_runs.append(myline[1]+'_'+myline[3])
            if myline[2].lower() =='on':
                ana_runs.append(myline[1]+'_'+myline[0])
        ## saving the information and exti
        self.delphes_runcard = del_runs
        self.analysis_runcard = ana_runs
        return


    def check_run(self,version):
        ## setup
        if version == "v1.1":
            self.detector = "delphesMA5tune"
            self.pad      = self.main.archi_info.ma5dir+'/PADForMA5tune'
            check         = self.main.recasting.ma5tune
        else:
            self.detector = "delphes"
            self.pad      = self.main.archi_info.ma5dir+'/PAD'
            check         = self.main.recasting.delphes
        ## Check and exit
        if not check:
           logging.getLogger('MA5').error('The ' + detector + ' library is not present -> the associated analyses cannot be used')
           return False
        return True



