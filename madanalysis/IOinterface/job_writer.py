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
        self.shower     = self.main.shower
        self.shwrmode   = ''

    @staticmethod     
    def CheckJobStructureMute(path):
        if not os.path.isdir(path):
            return False
        elif not os.path.isdir(path+"/Build"):
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
        elif not os.path.isdir(path+"/Output"):
            return False
        elif not os.path.isdir(path+"/Input"):
            return False
        elif not os.path.isfile(path+"/history.ma5"):
            return False
        else:
            return True


    @staticmethod     
    def CreateJobStructure(path):
        if not os.path.isdir(path):
            return False
        try:
            os.mkdir(path+"/Build")
        except:
            logging.error("Impossible to create the folder 'Build'")
            return False
        try:
            os.mkdir(path+"/Build/Lib")
        except:
            logging.error("Impossible to create the folder 'Build/Lib'")
            return False
        try:
            os.mkdir(path+"/Build/SampleAnalyzer")
        except:
            logging.error("Impossible to create the folder 'Build/SampleAnalyzer'")
            return False
        try:
            os.mkdir(path+"/Build/SampleAnalyzer/User")
        except:
            logging.error("Impossible to create the folder 'Build/SampleAnalyzer/User'")
            return False
        try:
            os.mkdir(path+"/Build/SampleAnalyzer/User/Analyzer")
        except:
            logging.error("Impossible to create the folder 'Build/SampleAnalyzer/User/Analyzer'")
            return False
        try:
            os.mkdir(path+"/Build/Log")
        except:
            logging.error("Impossible to create the folder 'Build/Log'")
            return False
        try:
            os.mkdir(path+"/Build/Main")
        except:
            logging.error("Impossible to create the folder 'Build/Main'")
            return False
        try:
            os.mkdir(path+"/Output")
        except:
            logging.error("Impossible to create the folder 'Output'")
            return False
        try:
            os.mkdir(path+"/Input")
        except:
            logging.error("Impossible to create the folder 'Input'")
            return False

        return True


    def CheckJobStructure(self):
        if not os.path.isdir(self.path):
            logging.error("folder '"+self.path+"' is not found")
            return False
        elif not os.path.isdir(self.path+"/Build"):
            logging.error("folder '"+self.path+"/Build' is not found")
            return False
        elif not os.path.isdir(self.path+"/Build/Lib"):
            logging.error("folder '"+self.path+"/Build/Lib' is not found")
            return False
        elif not os.path.isdir(self.path+"/Build/SampleAnalyzer"):
            logging.error("folder '"+self.path+"/Build/SampleAnalyzer' is not found")
            return False
        elif not os.path.isdir(self.path+"/Build/SampleAnalyzer/User"):
            logging.error("folder '"+self.path+"/Build/SampleAnalyzer/User' is not found")
            return False
        elif not os.path.isdir(self.path+"/Build/SampleAnalyzer/User/Analyzer"):
            logging.error("folder '"+self.path+"/Build/SampleAnalyzer/User/Analyzer' is not found")
            return False
        elif not os.path.isdir(self.path+"/Build/Main"):
            logging.error("folder '"+self.path+"/Build/Main' is not found")
            return False
        elif not os.path.isdir(self.path+"/Output"):
            logging.error("folder '"+self.path+"/Output' is not found")
            return False
        elif not os.path.isdir(self.path+"/Input"):
            logging.error("folder '"+self.path+"/Input' is not found")
            return False
        elif not os.path.isfile(self.path+"/history.ma5"):
            logging.error("file '"+self.path+"/history.ma5' is not found")
            return False
        else:
            return True

    def Open(self):
        if not self.resubmit:
            InstanceName.Clear()
            return FolderWriter.CreateDirectory(self.path,question=True)
        else:
            return self.CheckJobStructure()

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
            input = open(self.main.archi_info.ma5dir+"/tools/SampleAnalyzer/Process/Detector/"+cardname,'r')
        except:
            pass

        try:
            output = open(self.path+"/Input/"+cardname,'w')
        except:
            pass

        if cfg.pileup!="":
            # Getting current dir
            theDir = os.getcwd()

            # Adding file
            if cfg.pileup.startswith('/'):
                theFile = cfg.pileup
            else:    
                theFile = os.path.normpath(theDir+"/"+cfg.pileup)

        for line in input:
            if cfg.pileup!="":
                line=line.replace('MinBias.pileup',theFile)
            output.write(line)

        try:
            input.close()
        except:
            pass

        try:
            output.close()
        except:
            pass


    def CopyLHEAnalysis(self):
        if not JobWriter.CreateJobStructure(self.path):
            return False
        try:
            shutil.copyfile\
                      (\
                      self.main.archi_info.ma5dir+"/tools/SampleAnalyzer/newAnalyzer.py",\
                      self.path+"/Build/SampleAnalyzer/newAnalyzer.py"\
                      )
        except:
            logging.error('Impossible to copy the file "newAnalyzer"')
            return False
        try:    
            os.chmod(self.path+"/Build/SampleAnalyzer/newAnalyzer.py",0755)
        except:
            logging.error('Impossible to make executable the file "newAnalyzer"')
            return False

        if self.main.fastsim.package in ["delphes","delphesMA5tune"]:
            self.CreateDelphesCard()

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
        file.write('// -----------------------------------------------------------------------\n')
        file.write('// main program\n')
        file.write('// -----------------------------------------------------------------------\n')
        file.write('int main(int argc, char *argv[])\n')
        file.write('{\n')
        file.write('  // Creating a manager\n')
        file.write('  SampleAnalyzer manager;\n')
        file.write('  BuildUserTable(manager.AnalyzerList());\n')
        file.write('\n')

        # Initializing
        file.write('  // ---------------------------------------------------\n')
        file.write('  //                    INITIALIZATION\n')
        file.write('  // ---------------------------------------------------\n')
        file.write('  INFO << "    * Initializing all components" << endmsg;\n\n')
        file.write('  // Initializing the manager\n')
        if self.main.expertmode:
          file.write('  if (!manager.Initialize(argc,argv,"pdg.ma5",true)) '+\
                   'return 1;\n\n')
        else:
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
            file.write('  AnalyzerBase* analyzer2 = \n')
            file.write('      manager.InitializeAnalyzer("MergingPlots","MergingPlots.saf",parametersA2);\n')
            file.write('  if (analyzer2==0) return 1;\n\n')
        if self.output!="":
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
            elif self.main.fastsim.package=="delphesMA5tune":
                cardname = self.main.fastsim.delphesMA5tune.card

            if self.main.fastsim.package=="delphes":
                file.write('      manager.InitializeDetector("delphes","../../Input/'+cardname+'",parametersD1);\n')
            else:
                file.write('      manager.InitializeDetector("delphesMA5tune","../../Input/'+cardname+'",parametersD1);\n')

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
        elif self.main.fastsim.package=="delphes":
            file.write('      fastsim1->Execute(mySample,myEvent);\n')
        elif self.main.fastsim.package=="delphesMA5tune":
            file.write('      fastsim1->Execute(mySample,myEvent);\n')
        file.write('      if (!analyzer1->Execute(mySample,myEvent)) continue;\n')
        if self.output!="":
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
        return True

    def WriteSelectionSource(self,main):
        main.selection.RefreshStat();
        file = open(self.path+"/Build/SampleAnalyzer/User/Analyzer/user.cpp","w")
        import madanalysis.job.job_main as JobMain
        job = JobMain.JobMain(file,main)
        job.WriteSource()
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

    def CreateShowerDir(self,mode):
        if self.shower.type=='auto' and mode=='HERWIG6':
            self.shwrmode = mode
            self.fortran = True
            try:
                os.mkdir(self.path+'/Build/Showering')
                shutil.copy(self.main.archi_info.ma5dir+'/../madfks_hwdriver.f',\
                    self.path+'/Build/Showering/shower.f')
                shutil.copy(self.main.archi_info.ma5dir+'/../shower_card.dat', \
                    self.path+'/Build/shower_card.dat')
                shutil.copy(self.main.archi_info.ma5dir+'/../MCATNLO_HERWIG6_input', \
                    self.path+'/Build/MCATNLO_HERWIG6_input')
                logging.warning('THE SHOWER CARD THING MUST BE IMPROVED')
                logging.warning('THE INPUT CARD THING MUST BE IMPROVED')
                logging.warning('THE main function must be generated')
            except:
                logging.error('Cannot create the directory ' + self.path + '/MCatNLO.')
                return False              
            logging.warning('MUFFFFF and the shower is ready')
        else:
            logging.error('showering not implemented')
        return True

    def WriteSampleAnalyzerMakefile(self,option=""):

        from madanalysis.build.makefile_writer import MakefileWriter
        options=MakefileWriter.MakefileOptions()
        
        # Name of the Makefile
        filename = self.path+"/Build/SampleAnalyzer/Makefile"

        # Header
        title='User package'

        # Options
        option.has_commons  = True
        options.has_process = True
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
        options.has_process = True
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

        
    @staticmethod
    def WriteSetupFile(bash,path,MA5BASE,archi_info):

        # Variable to check at the end
        toCheck=[]

        # Opening file in write-only mode
        if bash:
            filename = path+"/setup.sh"
        else:
            filename = path+"/setup.csh"
        try:
            file = open(filename,"w")
        except:
            logging.error('Impossible to create the file "' + filename +'"')
            return False

        # Calling the good shell
        if bash:
            file.write('#!/bin/sh\n')
        else:
            file.write('#!/bin/csh -f\n')
        file.write('\n')

        # Defining colours
        file.write('# Defining colours for shell\n')
        if bash:
            file.write('GREEN="\\\\033[1;32m"\n')
            file.write('RED="\\\\033[1;31m"\n')
            file.write('PINK="\\\\033[1;35m"\n')
            file.write('BLUE="\\\\033[1;34m"\n')
            file.write('YELLOW="\\\\033[1;33m"\n')
            file.write('CYAN="\\\\033[1;36m"\n')
            file.write('NORMAL="\\\\033[0;39m"\n')
            # using ' ' could be more convenient to code
            # but in this case, the colour code are interpreted
            # by the linux command 'more'
        else:
            file.write('set GREEN  = "\\033[1;32m"\n')
            file.write('set RED    = "\\033[1;31m"\n')
            file.write('set PINK   = "\\033[1;35m"\n')
            file.write('set BLUE   = "\\033[1;34m"\n')
            file.write('set YELLOW = "\\033[1;33m"\n')
            file.write('set CYAN   = "\\033[1;36m"\n')
            file.write('set NORMAL = "\\033[0;39m"\n')
        file.write('\n')

        # Configuring PATH environment variable
        if MA5BASE:
            file.write('# Configuring MA5 environment variable\n')
            if bash:
                file.write('export MA5_BASE=' + JobWriter.CleanPath(archi_info.ma5dir)+'\n')
            else:
                file.write('setenv MA5_BASE ' + JobWriter.CleanPath(archi_info.ma5dir)+'\n')
            toCheck.append('MA5_BASE')
            file.write('\n')

        # Configuring PATH environment variable
        print "BILOUTE"
        print archi_info.toPATH
        if len(archi_info.toPATH)!=0:
            file.write('# Configuring PATH environment variable\n')
            if bash:
                file.write('if [ $PATH ]; then\n')
                file.write('export PATH=$PATH:' + JobWriter.CleanPath(':'.join(archi_info.toPATH))+'\n')
                file.write('else\n')
                file.write('export PATH=' + JobWriter.CleanPath(':'.join(archi_info.toPATH))+'\n')
                file.write('fi\n')
            else:
                file.write('if ( $?PATH ) then\n')
                file.write('setenv PATH "$PATH":' + JobWriter.CleanPath(':'.join(archi_info.toPATH))+'\n')
                file.write('else\n')
                file.write('setenv PATH ' + JobWriter.CleanPath(':'.join(archi_info.toPATH))+'\n')
                file.write('endif\n')
            toCheck.append('PATH')
            file.write('\n')

        if len(archi_info.toLDPATH)!=0:
            
            # Configuring LD_LIBRARY_PATH environment variable
            file.write('# Configuring LD_LIBRARY_PATH environment variable\n')
            if bash:
                file.write('if [ $LD_LIBRARY_PATH ]; then\n')
                file.write('export LD_LIBRARY_PATH=' + JobWriter.CleanPath(':'.join(archi_info.toLDPATH))+':$LD_LIBRARY_PATH\n')
                file.write('else\n')
                file.write('export LD_LIBRARY_PATH=' + JobWriter.CleanPath(':'.join(archi_info.toLDPATH))+'\n')
                file.write('fi\n')
            else:
                file.write('if ( $?LD_LIBRARY_PATH ) then\n')
                file.write('setenv LD_LIBRARY_PATH ' + JobWriter.CleanPath(':'.join(archi_info.toLDPATH))+':"$LD_LIBRARY_PATH"\n')
                file.write('else\n')
                file.write('setenv LD_LIBRARY_PATH ' + JobWriter.CleanPath(':'.join(archi_info.toLDPATH))+'\n')
                file.write('endif\n')
            toCheck.append('LD_LIBRARY_PATH')
            file.write('\n')

            # Configuring LIBRARY_PATH environment variable
            #file.write('# Configuring LIBRARY_PATH environment variable\n')
            #if bash:
            #    file.write('export LIBRARY_PATH=' + JobWriter.CleanPath(os.environ['LD_LIBRARY_PATH'])+'\n')
            #else:
            #    file.write('setenv LIBRARY_PATH ' + JobWriter.CleanPath(os.environ['LD_LIBRARY_PATH'])+'\n')
            #file.write('\n')

            # Configuring DYLD_LIBRARY_PATH environment variable
            if archi_info.isMac:
                file.write('# Configuring DYLD_LIBRARY_PATH environment variable\n')
                if bash:
                    file.write('if [ $DYLD_LIBRARY_PATH ]; then\n')
                    file.write('export DYLD_LIBRARY_PATH=' + JobWriter.CleanPath(':'.join(archi_info.toLDPATH))+':$DYLD_LIBRARY_PATH\n')
                    file.write('else\n')
                    file.write('export DYLD_LIBRARY_PATH=' + JobWriter.CleanPath(':'.join(archi_info.toLDPATH))+'\n')
                    file.write('fi\n')
                else:
                    file.write('if ( $?DYLD_LIBRARY_PATH ) then\n')
                    file.write('setenv DYLD_LIBRARY_PATH ' + JobWriter.CleanPath(':'.join(archi_info.toLDPATH))+':"$DYLD_LIBRARY_PATH"\n')
                    file.write('else\n')
                    file.write('setenv DYLD_LIBRARY_PATH ' + JobWriter.CleanPath(':'.join(archi_info.toLDPATH))+'\n')
                    file.write('endif\n')
                toCheck.append('DYLD_LIBRARY_PATH')
                file.write('\n')

            # Configuring CPLUS_INCLUDE_PATH environment variable
            #file.write('# Configuring CPLUS_INCLUDE_PATH environment variable\n')
            #if bash:
            #    file.write('export CPLUS_INCLUDE_PATH=' + JobWriter.CleanPath(os.environ['CPLUS_INCLUDE_PATH'])+'\n')
            #else:
            #    file.write('setenv CPLUS_INCLUDE_PATH ' + JobWriter.CleanPath(os.environ['CPLUS_INCLUDE_PATH'])+'\n')
            #file.write('\n')

        # Checking that all environment variables are defined
        file.write('# Checking that all environment variables are defined\n')
        if bash:
            file.write('if [[ ')
            for ind in range(0,len(toCheck)):
                if ind!=0:
                    file.write(' && ')
                file.write('$'+toCheck[ind])
            file.write(' ]]; then\n')
            file.write('echo -e $YELLOW"'+StringTools.Fill('-',56)+'"\n')
	    file.write('echo -e "'+StringTools.Center('Your environment is properly configured for MA5',56)+'"\n')
	    file.write('echo -e "'+StringTools.Fill('-',56)+'"$NORMAL\n')
            file.write('fi\n')
        else:
            file.write('if ( \n')
            for ind in range(0,len(toCheck)):
                if ind!=0:
                    file.write(' && ')
                file.write('$?'+toCheck[ind])
            file.write(' ) then\n')
            file.write('echo $YELLOW"'+StringTools.Fill('-',56)+'"\n')
	    file.write('echo "'+StringTools.Center('Your environment is properly configured for MA5',56)+'"\n')
	    file.write('echo "'+StringTools.Fill('-',56)+'"$NORMAL\n')
            file.write('endif\n')


        # Closing the file
        try:
            file.close()
        except:
            logging.error('Impossible to close the file "'+filename+'"')
            return False

        return True


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
            logging.error('impossible to compile the project. For more details, see the log file:')
            logging.error(logfile)
            
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
            logging.error('impossible to clean the project. For more details, see the log file:')
            logging.error(logfile)
            
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
            logging.error('impossible to clean the project. For more details, see the log file:')
            logging.error(logfile)
            
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
            logging.error('impossible to link the project. For more details, see the log file:')
            logging.error(logfile)
            
        return result


    def WriteHistory(self,history,firstdir):
        file = open(self.path+"/history.ma5","w")
        file.write('set main.currentdir = '+firstdir+'\n') 
        for line in history:
            items = line.split(';')
            for item in items :
                if item.startswith('help') or \
                   item.startswith('display') or \
                   item.startswith('history') or \
                   item.startswith('open') or \
                   item.startswith('preview') or \
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
        folder = self.path+'/Output/'+name

        # shell command
        commands = ['../../Build/MadAnalysis5job']

        # Weighted events
        if not dataset.weighted_events:
            commands.append('--no_event_weight')

        # Release
        commands.append('--ma5_version="'+\
                        self.main.archi_info.ma5_version+';'+\
                        self.main.archi_info.ma5_date+'"')

        # Inputs
        commands.append('../../Input/'+name+'.list')

        # Running SampleAnalyzer
        result = ShellCommand.Execute(commands,folder)

        return result

        
        
        
        
        
