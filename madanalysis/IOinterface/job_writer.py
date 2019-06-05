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


from madanalysis.selection.instance_name      import InstanceName
from madanalysis.IOinterface.folder_writer    import FolderWriter
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from string_tools                             import StringTools
from shell_command                            import ShellCommand
import logging
import shutil
import os
import commands

class JobWriter():

    def __init__(self,main,jobdir,resubmit=False):
        self.main       = main
        self.path       = jobdir
        self.resubmit   = resubmit
        self.output     = self.main.output
        self.fastsim    = self.main.fastsim
        self.merging    = self.main.merging

    @staticmethod
    def CheckJobStructureMute(path,recastflag):
        if not os.path.isdir(path):
            return False
        if not recastflag:
            if not os.path.isdir(path+"/Build"):
                return False
            elif not os.path.isdir(path+"/Build/Lib"):
                return False
            elif not os.path.isdir(path+"/Build/SampleAnalyzer"):
                return False
            elif not os.path.isdir(path+"/Build/SampleAnalyzer/User"):
                return False
            elif not os.path.isdir(path+"/Build/SampleAnalyzer/User/Analyzer"):
                return False
            elif not os.path.isdir(path+"/Build/Main"):
                return False
        if not os.path.isdir(path+"/Output"):
            return False
        if not os.path.isdir(path+"/Output/Histos"):
            return False
        if not os.path.isdir(path+"/Output/HTML"):
            return False
        if self.main.session_info.has_pdflatex:
            if not os.path.isdir(path+"/Output/PDF"):
                return False
        if self.main.session_info.has_latex:
            if not os.path.isdir(path+"/Output/DVI"):
                return False
        elif not os.path.isdir(path+"/Input"):
            return False
        elif not os.path.isfile(path+"/history.ma5"):
            return False
        return True


    @staticmethod
    def CreateJobStructure(path,recastflag):
        if not os.path.isdir(path):
            return False
        elif not recastflag:
            try:
                os.mkdir(path+"/Build")
            except:
                logging.getLogger('MA5').error("Impossible to create the folder 'Build'")
                return False
            try:
                os.mkdir(path+"/Build/Lib")
            except:
                logging.getLogger('MA5').error("Impossible to create the folder 'Build/Lib'")
                return False
            try:
                os.mkdir(path+"/Build/SampleAnalyzer")
            except:
                logging.getLogger('MA5').error("Impossible to create the folder 'Build/SampleAnalyzer'")
                return False
            try:
                os.mkdir(path+"/Build/SampleAnalyzer/User")
            except:
                logging.getLogger('MA5').error("Impossible to create the folder 'Build/SampleAnalyzer/User'")
                return False
            try:
                os.mkdir(path+"/Build/SampleAnalyzer/User/Analyzer")
            except:
                logging.getLogger('MA5').error("Impossible to create the folder 'Build/SampleAnalyzer/User/Analyzer'")
                return False
            try:
                os.mkdir(path+"/Build/Log")
            except:
                logging.getLogger('MA5').error("Impossible to create the folder 'Build/Log'")
                return False
            try:
                os.mkdir(path+"/Build/Main")
            except:
                logging.getLogger('MA5').error("Impossible to create the folder 'Build/Main'")
                return False
        try:
            os.mkdir(path+"/Output")
        except:
            logging.getLogger('MA5').error("Impossible to create the folder 'Output'")
            return False
        try:
            os.mkdir(path+"/Output/HTML")
        except:
            logging.getLogger('MA5').error("Impossible to create the folder 'Output/HTML'")
            return False
        try:
            os.mkdir(path+"/Output/Histos")
        except:
            logging.getLogger('MA5').error("Impossible to create the folder 'Output/Histos'")
            return False
        try:
            os.mkdir(path+"/Output/PDF")
        except:
            logging.getLogger('MA5').error("Impossible to create the folder 'Output/PDF'")
            return False
        try:
            os.mkdir(path+"/Output/DVI")
        except:
            logging.getLogger('MA5').error("Impossible to create the folder 'Output/DVI'")
            return False
        try:
            os.mkdir(path+"/Input")
        except:
            logging.getLogger('MA5').error("Impossible to create the folder 'Input'")
            return False

        return True


    def CheckJobStructure(self,recastflag):
        if not os.path.isdir(self.path):
            logging.getLogger('MA5').error("folder '"+self.path+"' is not found")
            return False
        if not recastflag:
            if not os.path.isdir(self.path+"/Build"):
                logging.getLogger('MA5').error("folder '"+self.path+"/Build' is not found")
                return False
            elif not os.path.isdir(self.path+"/Build/Lib"):
                logging.getLogger('MA5').error("folder '"+self.path+"/Build/Lib' is not found")
                return False
            elif not os.path.isdir(self.path+"/Build/SampleAnalyzer"):
                logging.getLogger('MA5').error("folder '"+self.path+"/Build/SampleAnalyzer' is not found")
                return False
            elif not os.path.isdir(self.path+"/Build/SampleAnalyzer/User"):
                logging.getLogger('MA5').error("folder '"+self.path+"/Build/SampleAnalyzer/User' is not found")
                return False
            elif not os.path.isdir(self.path+"/Build/SampleAnalyzer/User/Analyzer"):
                logging.getLogger('MA5').error("folder '"+self.path+"/Build/SampleAnalyzer/User/Analyzer' is not found")
                return False
            elif not os.path.isdir(self.path+"/Build/Main"):
                logging.getLogger('MA5').error("folder '"+self.path+"/Build/Main' is not found")
                return False
        if not os.path.isdir(self.path+"/Output"):
            logging.getLogger('MA5').error("folder '"+self.path+"/Output' is not found")
            return False
        elif not os.path.isdir(self.path+"/Input"):
            logging.getLogger('MA5').error("folder '"+self.path+"/Input' is not found")
            return False
        elif not os.path.isfile(self.path+"/history.ma5"):
            logging.getLogger('MA5').error("file '"+self.path+"/history.ma5' is not found")
            return False
        else:
            return True

    def Open(self):
        if not self.resubmit:
            InstanceName.Clear()
            return FolderWriter.CreateDirectory(self.path,question=True)
        else:
            recast = (self.main.recasting.status=="on")
            return self.CheckJobStructure(recast)

    def CopyDelphesCard(self,input,output,cfg,theFile):
        TagTreeWriter=False
        TagExecutionPath=False
        
        # READING THE FILE  
        for line in input:

            # Pileup
            if cfg.pileup!="":
                line=line.replace('MinBias.pileup',theFile)

            # Treatment
            myline = line.lstrip()
            myline = myline.rstrip()
            words  = myline.split()
            if myline.startswith('#'):
                output.write(line)
                continue
                
            if len(words)>=2:
                if words[0].lower()=='set' and \
                   words[1].lower()=='executionpath':
                    TagExecutionPath=True

            if len(words)>=1:
                if words[0].lower()=='treewriter' and TagExecutionPath:
                    TagExecutionPath=False
                    if cfg.skim_genparticles:
                        output.write('  MA5Filter\n')

            if len(words)>=3:
                if words[0].lower()=='module' and \
                   words[1].lower()=='treewriter' and \
                   words[2].lower()=='treewriter' :
                    TagTreeWriter=True
                    if cfg.skim_genparticles:
                        output.write('module MA5GenParticleFilter MA5Filter {\n')
                        output.write('\n')
                        output.write('  set InputArray Delphes/allParticles\n')
                        output.write('  set OutputArray filteredParticles\n')
                        output.write('  add PdgCode {18}\n')
                        output.write('\n')
                        output.write('}\n\n')


            if len(words)>=5 and TagTreeWriter:
                if words[0].lower()=='add' and\
                   words[1].lower()=='branch':
                    if words[3].lower()=='particle' and cfg.skim_genparticles:
                        output.write('  add Branch MA5Filter/filteredParticles      Particle    GenParticle\n')
                        continue
                    if words[3].lower()=='track' and cfg.skim_tracks:
                        output.write('#'+line)
                        continue
                    if words[3].lower()=='tower' and cfg.skim_towers:
                        output.write('#'+line)
                        continue
                    if words[3].lower()=='eflowtrack' and cfg.skim_eflow:
                        output.write('#'+line)
                        continue
                    if words[3].lower()=='eflowphoton' and cfg.skim_eflow:
                        output.write('#'+line)
                        continue
                    if words[3].lower()=='eflowneutralhadron' and cfg.skim_eflow:
                        output.write('#'+line)
                        continue
            
            # Enter TreeWriter
            output.write(line)
        

    def CopyDelphesMA5Card(self,input,output,cfg,theFile):
        TagTreeWriter=False
        TagExecutionPath=False
        
        # READING THE FILE  
        for line in input:

            # Pileup
            if cfg.pileup!="":
                line=line.replace('MinBias.pileup',theFile)

            # Enter TreeWriter
            output.write(line)


    def CreateDelphesCard(self):

        if self.main.fastsim.package=="delphes":
            cardname = self.main.fastsim.delphes.card
        elif self.main.fastsim.package=="delphesMA5tune":
            cardname = self.main.fastsim.delphesMA5tune.card
        if self.main.fastsim.package=="delphesMA5tune":
            cfg=self.main.fastsim.delphesMA5tune
        else:
            cfg=self.main.fastsim.delphes

        try:
            if self.main.fastsim.package=="delphes":
                input = open(self.main.archi_info.ma5dir+"/tools/SampleAnalyzer/Interfaces/delphes/"+cardname,'r')
            elif self.main.fastsim.package=="delphesMA5tune":
                input = open(self.main.archi_info.ma5dir+"/tools/SampleAnalyzer/Interfaces/delphesMA5tune/"+cardname,'r')
        except:
            logging.getLogger('MA5').error("impossible to find "+self.main.archi_info.ma5dir+"/tools/SampleAnalyzer/Interfaces/delphes/"+cardname)
        if "../../../.." in cardname:
            cardname=cardname.split('/')[-1]

        try:
            output = open(self.path+"/Input/"+cardname,'w')
        except:
            pass

        theFile = ""
        if cfg.pileup!="":
            # Getting current dir
            theDir = os.getcwd()

            # Adding file
            if cfg.pileup.startswith('/'):
                theFile = cfg.pileup
            else:    
                theFile = os.path.normpath(theDir+"/"+cfg.pileup)
 
        if self.main.fastsim.package=="delphes":
            self.CopyDelphesCard(input,output,cfg,theFile)
        elif self.main.fastsim.package=="delphesMA5tune": 
            self.CopyDelphesMA5Card(input,output,cfg,theFile)

        try:
            input.close()
        except:
            pass

        try:
            output.close()
        except:
            pass


    def CopyLHEAnalysis(self):
        recast = (self.main.recasting.status=="on")
        if not JobWriter.CreateJobStructure(self.path,recast):
            return False
        if not recast:
            try:
                shutil.copyfile\
                          (\
                          self.main.archi_info.ma5dir+"/tools/SampleAnalyzer/newAnalyzer.py",\
                          self.path+"/Build/SampleAnalyzer/newAnalyzer.py"\
                          )
            except:
                logging.getLogger('MA5').error('Impossible to copy the file "newAnalyzer"')
                return False
            try:    
                os.chmod(self.path+"/Build/SampleAnalyzer/newAnalyzer.py",0755)
            except:
                logging.getLogger('MA5').error('Impossible to make executable the file "newAnalyzer"')
                return False

        if self.main.fastsim.package in ["delphes","delphesMA5tune"]:
            self.CreateDelphesCard()

        if self.main.recasting.status=="on":
            if not self.main.recasting.CreateCard(self.path):
                return False

        return True

    def CreateHeader(self,file):
        file.write('////////////////////////////////////////////////////////////////////////////////\n')
        file.write('//  \n')
        file.write('//  This file has been generated by MadAnalysis 5.\n')
        file.write('//  The MadAnalysis development team: <ma5team@iphc.cnrs.fr>\n')
        file.write('//                Eric Conte and Benjamin Fuks\n')
        file.write('//  Official website: <https://launchpad.net/madanalysis5>\n')
        file.write('//  \n')
        file.write('////////////////////////////////////////////////////////////////////////////////\n\n')
        return

    def PrintIncludes(self,file):
        file.write('// SampleHeader header\n')
        file.write('#include "SampleAnalyzer/Process/Core/SampleAnalyzer.h"\n')
        file.write('#include "SampleAnalyzer/User/Analyzer/analysisList.h"\n') 
        file.write('using namespace MA5;\n\n')
        return

    def CreateMainFct(self,file,analysisName,outputName):

        # Info function
        file.write('// -----------------------------------------------------------------------\n')
        file.write('// Info function\n')
        file.write('// -----------------------------------------------------------------------\n')
        file.write('int Info(SampleAnalyzer& manager)\n')
        file.write('{\n')
        file.write('  INFO << "BEGIN " << __FILE__ << endmsg;\n')
        file.write('  manager.AnalyzerList().Print();\n')
        file.write('  INFO << "END " << __FILE__ << endmsg;\n')
        file.write('  return 0;\n')
        file.write('}\n')

        # Main program
        file.write('// -----------------------------------------------------------------------\n')
        file.write('// main program\n')
        file.write('// -----------------------------------------------------------------------\n')
        file.write('int main(int argc, char *argv[])\n')
        file.write('{\n')
        file.write('  // Creating a manager\n')
        file.write('  SampleAnalyzer manager;\n')
        file.write('  BuildUserTable(manager.AnalyzerList());\n')
        file.write('\n')

        # Info about the job
        file.write('  // Identifying --info argument\n')
        file.write('  if (argc==2)\n')
        file.write('  {\n')
        file.write('    std::string arg=argv[1];\n')
        file.write('    if (arg=="--info") return Info(manager);\n')
        file.write('  }\n')
        file.write('\n')

        # Initializing
        file.write('  // ---------------------------------------------------\n')
        file.write('  //                    INITIALIZATION\n')
        file.write('  // ---------------------------------------------------\n')
        file.write('  INFO << "    * Initializing all components" << endmsg;\n\n')
        file.write('  // Initializing the manager\n')
        file.write('  if (!manager.Initialize(argc,argv,"pdg.ma5")) '+\
            'return 1;\n\n')
        file.write('  // Creating data format for storing data\n')
        file.write('  EventFormat myEvent;\n')
        file.write('  std::vector<SampleFormat> mySamples;\n\n')
        file.write('  // Getting pointer to the analyzer\n')
        file.write('  std::map<std::string, std::string> parametersA1;\n')
        file.write('  AnalyzerBase* analyzer1 = \n')
        file.write('      manager.InitializeAnalyzer("'+analysisName+'","'+outputName+'",parametersA1);\n')
        file.write('  if (analyzer1==0) return 1;\n\n')
        if self.merging.enable:
            file.write('  // Getting pointer to the analyzer devoted to merging plots\n')
            file.write('  std::map<std::string, std::string> parametersA2;\n')
            file.write('  parametersA2["njets"]="'+str(self.main.merging.njets)+'";\n')
            file.write('  parametersA2["ma5_mode"]="'+str(int(self.main.merging.ma5_mode))+'";\n')
            file.write('  AnalyzerBase* analyzer2 = \n')
            file.write('      manager.InitializeAnalyzer("MergingPlots","MergingPlots.saf",parametersA2);\n')
            file.write('  if (analyzer2==0) return 1;\n\n')
        if self.output!="" and not self.output.lower().endswith('root'):
            file.write('  //Getting pointer to the writer\n')
            file.write('  WriterBase* writer1 = \n')
            if self.output.lower().endswith('lhe') or self.output.lower().endswith('lhe.gz'):
                file.write('      manager.InitializeWriter("lhe","'+self.output+'");\n')
            elif self.output.lower().endswith('lhco') or self.output.lower().endswith('lhco.gz'):
                file.write('      manager.InitializeWriter("lhco","'+self.output+'");\n')
            file.write('  if (writer1==0) return 1;\n\n')

        # Fast-Simulation detector
        # + Case Fastsim
        if self.main.fastsim.package=="fastjet":
            file.write('  //Getting pointer to the clusterer\n')
            file.write('  std::map<std::string, std::string> parametersC1;\n')
            parameters = self.main.fastsim.SampleAnalyzerConfigString()
            for k,v in sorted(parameters.iteritems(),\
                              key=lambda (k,v): (k,v)):
                file.write('  parametersC1["'+k+'"]="'+v+'";\n')
            file.write('  JetClusterer* cluster1 = \n')
            file.write('      manager.InitializeJetClusterer("'+self.main.fastsim.clustering.algorithm+'",parametersC1);\n')
            file.write('  if (cluster1==0) return 1;\n\n')
            if self.main.superfastsim.smearer.rules!={}  or self.main.superfastsim.reco.rules!={}:
                file.write('  // Declaration of the smearer\n')
                file.write('  NewSmearer* smearer = new NewSmearer();\n\n')
            if self.main.superfastsim.tagger.rules!={}:
                file.write('  // Declaration of a generic tagger\n')
                file.write('  NewTagger* tagger = new NewTagger();\n\n')

        # + Case Delphes
        if self.main.fastsim.package in ["delphes","delphesMA5tune"]:
            file.write('  //Getting pointer to fast-simulation package\n')
            file.write('  std::map<std::string, std::string> parametersD1;\n')
            if self.fastsim.package=="delphesMA5tune":
                cfg=self.main.fastsim.delphesMA5tune
            else:
                cfg=self.main.fastsim.delphes
            parameters = self.main.fastsim.SampleAnalyzerConfigString()
            for k,v in sorted(parameters.iteritems(),\
                              key=lambda (k,v): (k,v)):
                file.write('  parametersD1["'+k+'"]="'+v+'";\n')
            file.write('  DetectorBase* fastsim1 = \n')

            if self.main.fastsim.package=="delphes":
                cardname = self.main.fastsim.delphes.card
                if "../../../.." in cardname:
                    cardname=cardname.split('/')[-1]
            elif self.main.fastsim.package=="delphesMA5tune":
                cardname = self.main.fastsim.delphesMA5tune.card
                if "../../../.." in cardname:
                    cardname=cardname.split('/')[-1]

            if self.main.fastsim.package=="delphes":
                file.write('      manager.InitializeDetector("delphes","../Input/'+cardname+'",parametersD1);\n')
            else:
                file.write('      manager.InitializeDetector("delphesMA5tune","../Input/'+cardname+'",parametersD1);\n')

            file.write('  if (fastsim1==0) return 1;\n\n')

        # Post intialization (crating the output directory structure)
        file.write('  // Post initialization (creates the new output directory structure)\n')
        file.write('  if(!manager.PostInitialize()) return 1;\n\n')


        # Loop
        file.write('  // ---------------------------------------------------\n')
        file.write('  //                      EXECUTION\n')
        file.write('  // ---------------------------------------------------\n')
        file.write('  INFO << "    * Running over files ..." << endmsg;\n\n')
        file.write('  // Loop over files\n')
        file.write('  while(1)\n')
        file.write('  {\n')
        file.write('    // Opening input file\n')
        file.write('    mySamples.push_back(SampleFormat());\n')
        file.write('    SampleFormat& mySample=mySamples.back();\n')
        file.write('    StatusCode::Type result1 = manager.NextFile(mySample);\n')
        file.write('    if (result1!=StatusCode::KEEP)\n')
        file.write('    {\n')
        file.write('      if (result1==StatusCode::SKIP) continue;\n')
        file.write('      else if (result1==StatusCode::FAILURE) {mySamples.pop_back(); break;}\n')
        file.write('    }\n')
        file.write('    \n')
        file.write('    // Loop over events\n')
        file.write('    while(1)\n')
        file.write('    {\n')
        file.write('      StatusCode::Type result2 = manager.NextEvent(mySample,myEvent);\n')
        file.write('      if (result2!=StatusCode::KEEP)\n')
        file.write('      {\n')
        file.write('        if (result2==StatusCode::SKIP) continue;\n')
        file.write('        else if (result2==StatusCode::FAILURE) break;\n')
        file.write('      }\n')
        file.write('          manager.UpdateProgressBar();\n')
        if self.merging.enable:
            file.write('      if (!analyzer2->Execute(mySample,myEvent)) continue;\n')
        if self.main.fastsim.package=="fastjet":
            file.write('      cluster1->Execute(mySample,myEvent);\n')
            if self.main.superfastsim.smearer.rules!={} or self.main.superfastsim.reco.rules!={}:
                file.write('      smearer->Execute(mySample,myEvent);\n')
            if self.main.superfastsim.tagger.rules!={}:
                file.write('      tagger->Execute(mySample,myEvent);\n')
        elif self.main.fastsim.package=="delphes":
            file.write('      fastsim1->Execute(mySample,myEvent);\n')
        elif self.main.fastsim.package=="delphesMA5tune":
            file.write('      fastsim1->Execute(mySample,myEvent);\n')
        file.write('      if (!analyzer1->Execute(mySample,myEvent)) continue;\n')
        if self.output!="" and  not self.output.lower().endswith('root'):
            file.write('      writer1->WriteEvent(myEvent,mySample);\n')
        file.write('    }\n')
        file.write('  }\n\n')

        # Finalizing
        file.write('  // ---------------------------------------------------\n')
        file.write('  //                     FINALIZATION\n')
        file.write('  // ---------------------------------------------------\n')
        file.write('  INFO << "    * Finalizing all components ..." << endmsg;\n\n')
        file.write('  // Finalizing all components\n')
        file.write('  manager.Finalize(mySamples,myEvent);\n')
        file.write('  return 0;\n')
        file.write('}\n')
        return

    def CreateBldDir(self,analysisName="MadAnalysis5job",outputName="MadAnalysis5job.saf"):
        file = open(self.path+'/Build/Main/main.cpp','w')
        self.CreateHeader(file)
        self.PrintIncludes(file)
        self.CreateMainFct(file,analysisName,outputName)
        file.close()
        return True


    def WriteSelectionHeader(self,main):
        main.selection.RefreshStat();
        file = open(self.path+"/Build/SampleAnalyzer/User/Analyzer/user.h","w")
        import madanalysis.job.job_main as JobMain
        job = JobMain.JobMain(file,main)
        job.WriteHeader()
        file.close()

        ## Do we need a smearer?
        if main.superfastsim.smearer.rules!={} or main.superfastsim.reco.rules!={}:
            file = open(self.path+"/Build/SampleAnalyzer/User/Analyzer/new_smearer_reco.h","w")
            import madanalysis.job.job_smearer_reco_header as JobSmearerRecoHeader
            job = JobSmearerRecoHeader.JobSmearerRecoHeader(main.superfastsim)
            job.WriteNewSmearerRecoHeader(file)
            file.close()
            if main.superfastsim.smearer.rules!={}:
                file = open(self.path+"/Build/SampleAnalyzer/User/Analyzer/sigmas.h","w")
                job.WriteNewSmearerEfficiencies(file)
                file.close()
            if main.superfastsim.reco.rules!={}:
                file = open(self.path+"/Build/SampleAnalyzer/User/Analyzer/reco.h","w")
                job.WriteNewRecoEfficiencies(file)
                file.close()

        ## Do we need a tagger?
        if main.superfastsim.tagger.rules!={}:
            file = open(self.path+"/Build/SampleAnalyzer/User/Analyzer/new_tagger.h","w")
            import madanalysis.job.job_tagger_header as JobTaggerHeader
            job = JobTaggerHeader.JobTaggerHeader(main.superfastsim)
            job.WriteNewTaggerHeader(file)
            file.close()
            file = open(self.path+"/Build/SampleAnalyzer/User/Analyzer/efficiencies.h","w")
            job.WriteNewTaggerEfficiencies(file)
            file.close()
        return True

    def WriteSelectionSource(self,main):
        main.selection.RefreshStat();
        file = open(self.path+"/Build/SampleAnalyzer/User/Analyzer/user.cpp","w")
        import madanalysis.job.job_main as JobMain
        job = JobMain.JobMain(file,main)
        job.WriteSource()
        file.close()

        ## Do we need a smearer?
        if main.superfastsim.smearer.rules!={}  or self.main.superfastsim.reco.rules!={}:
            file = open(self.path+"/Build/SampleAnalyzer/User/Analyzer/new_smearer_reco.cpp","w")
            import madanalysis.job.job_smearer_reco_main as JobSmearerRecoMain
            job = JobSmearerRecoMain.JobSmearerRecoMain(main.superfastsim)
            job.WriteNewSmearerRecoSource(file)
            file.close()

        ## Do we need a tagger?
        if main.superfastsim.tagger.rules!={}:
            file = open(self.path+"/Build/SampleAnalyzer/User/Analyzer/new_tagger.cpp","w")
            import madanalysis.job.job_tagger_main as JobTaggerMain
            job = JobTaggerMain.JobTaggerMain(main.superfastsim)
            job.WriteNewTaggerSource(file)
            file.close()

        file = open(self.path+"/Build/SampleAnalyzer/User/Analyzer/analysisList.h","w")
        file.write('#include "SampleAnalyzer/Process/Analyzer/AnalyzerManager.h"\n')
        file.write('#include "SampleAnalyzer/User/Analyzer/user.h"\n')
        file.write('#include "SampleAnalyzer/Commons/Service/LogStream.h"\n')
        file.write('\n')
        file.write('// ------------------------------------------' +\
                   '-----------------------------------\n')
        file.write('// BuildUserTable\n')
        file.write('// ------------------------------------------' +\
                   '-----------------------------------\n')
        file.write('void BuildUserTable(MA5::AnalyzerManager& manager)\n')
        file.write('{\n')
        file.write('  using namespace MA5;\n')
        file.write('  manager.Add("MadAnalysis5job", new user);\n')
        file.write('}\n')
        file.close()
        return True

    def WriteSampleAnalyzerMakefile(self,option=""):

        from madanalysis.build.makefile_writer import MakefileWriter
        options=MakefileWriter.MakefileOptions()

        # Name of the Makefile
        filename = self.path+"/Build/SampleAnalyzer/Makefile"

        # Header
        title='User package'

        # Options
        option.has_commons   = True
        options.has_process  = True
        if self.main.archi_info.has_root:
            options.has_root_inc = True
            options.has_root_lib = True
        toRemove.extend(['compilation.log','linking.log','cleanup.log','mrproper.log'])

        # File to compile
        cppfiles = package+'/*.cpp'
        hfiles   = package+'/*.h'

        # Files to produce
        isLibrary=True
        ProductName='libUserPackage_for_ma5.so'
        ProductPath='../../Build/Lib/'

        # write makefile
        MakefileWriter.Makefile(filename,title,ProductName,ProductPath,isLibrary,cppfiles,hfiles,options,self.main.archi_info,toRemove)

        # Ok
        return True
    

    def WriteMakefiles(self,option=""):

        from madanalysis.build.makefile_writer import MakefileWriter
        options=MakefileWriter.MakefileOptions()

        # Name of the Makefile
        filename = self.path+"/Build/Makefile"

        # Header
        title='MadAnalysis Job'

        # Options
        options.has_commons  = True
        options.has_process  = True
        if self.main.archi_info.has_root:
            options.has_root_inc = True
            options.has_root_lib = True
        #options.has_userpackage = True
        toRemove=['Log/compilation.log','Log/linking.log','Log/cleanup.log','Log/mrproper.log']

        # File to compile
        cppfiles = ['Main/*.cpp','SampleAnalyzer/User/*/*.cpp']
        hfiles   = ['Main/*.h','SampleAnalyzer/User/*/*.h']

        # Files to produce
        isLibrary=False
        ProductName='MadAnalysis5job'
        ProductPath='./'

        # Write makefile
        MakefileWriter.Makefile(filename,title,ProductName,ProductPath,isLibrary,cppfiles,hfiles,options,self.main.archi_info,toRemove,moreIncludes=['./'])

        # Setup
        from madanalysis.build.setup_writer import SetupWriter
        SetupWriter.WriteSetupFile(True, self.path+'/Build/',self.main.archi_info)
        SetupWriter.WriteSetupFile(False,self.path+'/Build/',self.main.archi_info)
        
        # Ok
        return True

    @staticmethod
    def CleanPath(thestring):

        # Cleaning the string
        thestring = thestring.lstrip()
        thestring = thestring.rstrip()

        # Splitting the string into paths
        paths = thestring.split(':')

        # Declaring new container
        newpaths = []

        for path in paths:

            if path=="":
                continue

            # Normalizing the path
            # -> Settling A//B, A/B/, A/./B and A/foo/../B 
            path = os.path.normpath(path)

            # Adding the path if it is not already present
            if path not in newpaths:
                newpaths.append(path)

        # Return the string
        return ':'.join(newpaths)

    def CompileJob(self):

        # folder
        folder = self.path+'/Build'

        # log file name
        logfile = folder+'/Log/compilation.log'
        
        # shell command
        commands = ['make','compile']

        # call
        result, out = ShellCommand.ExecuteWithLog(commands,logfile,folder)

        # return result
        if not result:
            logging.getLogger('MA5').error('impossible to compile the project. For more details, see the log file:')
            logging.getLogger('MA5').error(logfile)
            
        return result


    def MrproperJob(self):

        # folder
        folder = self.path+'/Build'

        # log file name
        logfile = folder+'/Log/mrproper.log'
        
        # shell command
        commands = ['make','mrproper']

        # call
        result, out = ShellCommand.ExecuteWithLog(commands,logfile,folder)

        # return result
        if not result:
            logging.getLogger('MA5').error('impossible to clean the project. For more details, see the log file:')
            logging.getLogger('MA5').error(logfile)
            
        return result


    def MrproperJob(self):

        # folder
        folder = self.path+'/Build'

        # log file name
        logfile = folder+'/Log/mrproper.log'
        
        # shell command
        commands = ['make','mrproper']

        # call
        result, out = ShellCommand.ExecuteWithLog(commands,logfile,folder)

        # return result
        if not result:
            logging.getLogger('MA5').error('impossible to clean the project. For more details, see the log file:')
            logging.getLogger('MA5').error(logfile)
            
        return result


    def LinkJob(self):

        # folder
        folder = self.path+'/Build'

        # log file name
        logfile = folder+'/Log/linking.log'
        
        # shell command
        commands = ['make','link']

        # call
        result, out = ShellCommand.ExecuteWithLog(commands,logfile,folder)

        # return result
        if not result:
            logging.getLogger('MA5').error('impossible to link the project. For more details, see the log file:')
            logging.getLogger('MA5').error(logfile)

        return result


    def WriteHistory(self,history,firstdir):
        file = open(self.path+"/history.ma5","w")
        file.write('set main.currentdir = '+firstdir+'\n') 
        for line in history.history:
            items = line.split(';')
            for item in items :
                if item.startswith('help') or \
                   item.startswith('display') or \
                   item.startswith('history') or \
                   item.startswith('open') or \
                   item.startswith('resubmit') or \
                   item.startswith('shell') or \
                   item.startswith('!') or \
                   item.startswith('submit'):
                    pass
                else:
                    file.write(item)
                    file.write("\n")
        file.close()    

    def WriteDatasetList(self,dataset):
        name=InstanceName.Get(dataset.name)
        file = open(self.path+"/Input/"+name+".list","w")
        for item in dataset:
            file.write(item)
            file.write("\n")
        file.close()    


    def RunJob(self,dataset):

        # Getting the dataset name    
        name=InstanceName.Get(dataset.name)

        # Creating Output folder is not defined
        if not os.path.isdir(self.path+"/Output/"+name):
            os.mkdir(self.path+"/Output/"+name)

        # folder where the program is launched
        folder = self.path+'/Build/'

        # shell command
        commands = ['./MadAnalysis5job']

        # Weighted events
        if not dataset.weighted_events:
            commands.append('--no_event_weight')

        # Release
        commands.append('--ma5_version="'+\
                        self.main.archi_info.ma5_version+';'+\
                        self.main.archi_info.ma5_date+'"')

        # Inputs
        commands.append('../Input/'+name+'.list')

        # Running SampleAnalyzer
        if self.main.redirectSAlogger:
            result = ShellCommand.ExecuteWithMA5Logging(commands,folder)
        else:
            result = ShellCommand.Execute(commands,folder)

        return result


    def WriteTagger(self):
        # header file
        bla
