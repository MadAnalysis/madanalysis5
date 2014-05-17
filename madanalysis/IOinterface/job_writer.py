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
from madanalysis.core.string_tools            import StringTools
from madanalysis.IOinterface.shell_command    import ShellCommand
import logging
import shutil
import os
import commands

class JobWriter():

    def __init__(self,main,jobdir,resubmit=False):
        self.main       = main
        self.ma5dir     = self.main.ma5dir
        self.path       = jobdir
        self.resubmit   = resubmit
        self.libZIP     = self.main.libZIP
        self.libDelphes = self.main.libDelphes
        self.libDelfes  = self.main.libDelfes
        self.output     = self.main.output
        self.libFastjet = self.main.libFastJet
        self.fastsim    = self.main.fastsim
        self.merging    = self.main.merging
        self.fortran    = self.main.fortran
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
        elif not os.path.isdir(path+"/Build/SampleAnalyzer/Analyzer"):
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
            os.mkdir(path+"/Build/SampleAnalyzer/Analyzer")
        except:
            logging.error("Impossible to create the folder 'Build/SampleAnalyzer/Analyzer'")
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
        elif not os.path.isdir(self.path+"/Build/SampleAnalyzer/Analyzer"):
            logging.error("folder '"+self.path+"/Build/SampleAnalyzer/Analyzer' is not found")
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
        elif self.main.fastsim.package=="delfes":
            cardname = self.main.fastsim.delfes.card

        if self.main.fastsim.package=="delfes":
            cfg=self.main.fastsim.delfes
        else:
            cfg=self.main.fastsim.delphes

        try:
            input = open(self.ma5dir+"/tools/SampleAnalyzer/Detector/"+cardname,'r')
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
                      self.ma5dir+"/tools/SampleAnalyzer/newAnalyzer.py",\
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

        if self.main.fastsim.package in ["delphes","delfes"]:
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
        file.write('#include "SampleAnalyzer/Core/SampleAnalyzer.h"\n')
        file.write('#include "SampleAnalyzer/Analyzer/analysisList.h"\n') 
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
            file.write('  JetClustererBase* cluster1 = \n')
            file.write('      manager.InitializeJetClusterer("'+self.main.fastsim.clustering.algorithm+'",parametersC1);\n')
            file.write('  if (cluster1==0) return 1;\n\n')
            
        # + Case Delphes
        if self.main.fastsim.package in ["delphes","delfes"]:
            file.write('  //Getting pointer to fast-simulation package\n')
            file.write('  std::map<std::string, std::string> parametersD1;\n')
            if self.fastsim.package=="delfes":
                cfg=self.main.fastsim.delfes
            else:
                cfg=self.main.fastsim.delphes
            parameters = self.main.fastsim.SampleAnalyzerConfigString()
            for k,v in sorted(parameters.iteritems(),\
                              key=lambda (k,v): (k,v)):
                file.write('  parametersD1["'+k+'"]="'+v+'";\n')
            file.write('  DetectorBase* fastsim1 = \n')

            if self.main.fastsim.package=="delphes":
                cardname = self.main.fastsim.delphes.card
            elif self.main.fastsim.package=="delfes":
                cardname = self.main.fastsim.delfes.card

            if self.main.fastsim.package=="delphes":
                file.write('      manager.InitializeDetector("delphes","../../Input/'+cardname+'",parametersD1);\n')
            else:
                file.write('      manager.InitializeDetector("delfes","../../Input/'+cardname+'",parametersD1);\n')

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
            file.write('      analyzer2->Execute(mySample,myEvent);\n')
        if self.main.fastsim.package=="fastjet":
            file.write('      cluster1->Execute(mySample,myEvent);\n')
        elif self.main.fastsim.package=="delphes":
            file.write('      fastsim1->Execute(mySample,myEvent);\n')
        elif self.main.fastsim.package=="delfes":
            file.write('      fastsim1->Execute(mySample,myEvent);\n')
        file.write('      analyzer1->Execute(mySample,myEvent);\n')
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
        file = open(self.path+"/Build/SampleAnalyzer/Analyzer/user.h","w")
        import madanalysis.job.job_main as JobMain
        job = JobMain.JobMain(file,main)
        job.WriteHeader()
        file.close()
        return True

    def WriteSelectionSource(self,main):
        main.selection.RefreshStat();
        file = open(self.path+"/Build/SampleAnalyzer/Analyzer/user.cpp","w")
        import madanalysis.job.job_main as JobMain
        job = JobMain.JobMain(file,main)
        job.WriteSource()
        file.close()

        file = open(self.path+"/Build/SampleAnalyzer/Analyzer/analysisList.h","w")
        file.write('#include "SampleAnalyzer/Analyzer/AnalyzerManager.h"\n')
        file.write('#include "SampleAnalyzer/Analyzer/user.h"\n')
        file.write('#include "SampleAnalyzer/Service/LogStream.h"\n')
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
                shutil.copy(self.ma5dir+'/../madfks_hwdriver.f',\
                    self.path+'/Build/Showering/shower.f')
                shutil.copy(self.ma5dir+'/../shower_card.dat', \
                    self.path+'/Build/shower_card.dat')
                shutil.copy(self.ma5dir+'/../MCATNLO_HERWIG6_input', \
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

        # Open the file
        file = open(self.path+"/Build/SampleAnalyzer/Makefile","w")

        # Header
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('#'+StringTools.Center('SUB MAKEFILE DEVOTED TO SAMPLE ANALYZER',78)+'#\n')
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('\n')

        # Compilators
        file.write('# Compilators\n')
        file.write('CXX = g++\n')
        file.write('\n')

        # Options for compilation
        file.write('# Options for compilation\n')
        if self.libFastjet:
            file.write('CXXFASTJET = $(shell fastjet-config --cxxflags --plugins)\n')
        file.write('CXXFLAGS = -Wall -O3 -fPIC -I./ -I./../ -I' + '$(MA5_BASE)/tools/')
        if self.libZIP:
            file.write(' -DZIP_USE')
        if self.libDelphes:
            file.write(' -DROOT_USE -DDELPHES_USE')
        if self.libDelfes:
            file.write(' -DROOT_USE -DDELFES_USE')
        if self.libFastjet:
            file.write(' -DFASTJET_USE')
            file.write(' $(CXXFASTJET)')
        file.write('\n')
        file.write('\n')

        # Files for analyzers
        file.write('# Files for analyzers\n')
        file.write('SRCS = $(wildcard Analyzer/*.cpp)\n')
        file.write('HDRS = $(wildcard Analyzer/*.h)\n')
        file.write('OBJS = $(SRCS:.cpp=.o)\n')
        file.write('\n')

        # Name of the library
        file.write('# Name of the library\n')
        file.write('PROGRAM = UserPackage_for_ma5\n')
        file.write('\n')

        # All
        file.write('# All target\n')
        file.write('all: compile link\n')
        file.write('\n')

        # Compilation
        file.write('# Compile target\n')
        file.write('compile: $(OBJS)\n')
        file.write('\n')
        file.write('# Object file target\n')
        file.write('$(OBJS): $(HDRS)\n')
        file.write('\n')

        # Linking
        file.write('# Link target\n')
        file.write('link: $(OBJS)\n')
        if self.main.isMAC:
            file.write('\t$(CXX) -shared -flat_namespace -dynamiclib -undefined suppress -o ../../Build/Lib/lib$(PROGRAM).so $(OBJS)\n')
        else:
            file.write('\t$(CXX) -shared -o ../../Build/Lib/lib$(PROGRAM).so $(OBJS)\n')
        file.write('\n')

        # Phony target
        file.write('# Phony target\n')
        file.write('.PHONY: do_clean\n')
        file.write('\n')

        # Cleaning
        file.write('# Clean target\n')
        file.write('clean: do_clean\n')
        file.write('\n')
        file.write('# Do clean target\n')
        file.write('do_clean:\n')
        file.write('\t@rm -f $(OBJS)\n')
        file.write('\n')

        # Mr Proper
        file.write('# Mr Proper target\n')
        file.write('mrproper: do_mrproper\n')
        file.write('\n')
        file.write('# Do clean target\n')
        file.write('do_mrproper: do_clean\n')
        file.write('\t@rm -f ../../Build/Lib/lib$(PROGRAM).so\n')
        file.write('\t@rm -f *~ */*~ \n')
        file.write('\n')
        
        # Closing the file
        file.close()

    def WriteShoweringMakefile(self,option=""):
        file = open(self.path+"/Build/Showering/Makefile","w")
        file.write('FC  = gfortran\n')
        file.write('FFLAGS = -I' + \
            '$(MA5_BASE)/tools/MCatNLO-utilities/MCatNLO/include\n\n')
        file.write('SRCS = $(wildcard ' + self.path + '/Build/Showering/*.f)\n')
        file.write('OBJS = $(SRCS:.f=.o)\n')
        file.write('PROGRAM = Showering\n\n')
        file.write('all:\t precompile compile link\n\n')
        file.write('precompile:\t\n\n')
        file.write('compile:\tprecompile $(OBJS)\n\n')
        file.write('link:\t$(OBJS)\n')
        file.write('\tar -ruc ../../Build/Lib/lib$(PROGRAM).a $(OBJS)\n')
        file.write('\tranlib ../../Build/Lib/lib$(PROGRAM).a\n')
        file.write('\t@rm -f $(OBJS)\n\n')
        file.write('clean:;\t@rm -f $(OBJS) ../../Build/Lib/lib$(PROGRAM).a' + \
            ' Log/compilation.log Log/linking.log *~ */*~ \n')
        file.close()

    def WriteMakefiles(self,option=""):

        # Writing sub-Makefiles
        #self.WriteSampleAnalyzerMakefile(option)
        #if self.shwrmode!='':
        #    self.WriteShoweringMakefile(option)

        # Opening the main Makefile    
        file = open(self.path+"/Build/Makefile","w")

        # Writing header
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('#'+StringTools.Center('GENERAL MAKEFILE FOR MAD ANALYSIS 5 JOB',78)+'#\n')
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('\n')

        # Compilators
        file.write('# Compilators\n')
        file.write('CXX = g++\n')
        file.write('\n')

        # Options for compilation : CXXFLAGS
        file.write('# Options for compilation\n')
        options = []
        options.extend(['-Wall','-O3','-I./','-I./SampleAnalyzer/','-I$(MA5_BASE)/tools/','-I'+self.main.configLinux.root_inc_path])
        if self.libZIP:
            options.extend(['-DZIP_USE'])
        if self.libDelphes:
            options.extend(['-DROOT_USE','-DDELPHES_USE'])
        if self.libDelfes:
            options.extend(['-DROOT_USE','-DDELFES_USE'])
        if self.libFastjet:
            options.extend(['-DROOT_USE','-DFASTJET_USE'])#,'$(shell fastjet-config --cxxflags --plugins)'])
        file.write('CXXFLAGS = '+' '.join(options))
        file.write('\n')

        # Options for compilation : LIBFLAGS

        # - Root
        libs=[]
        libs.extend(['-L'+self.main.configLinux.root_lib_path, \
                     '-lGpad','-lHist','-lGraf','-lGraf3d','-lTree', \
                     '-lRint','-lPostscript','-lMatrix','-lPhysics', \
                     '-lMathCore','-lEG', '-lRIO','-lNet','-lThread', \
                     '-lCore','-lCint','-pthread','-lm','-ldl','-rdynamic'])

        # - SampleAnalyzer
        libs.extend(['-L$(MA5_BASE)/tools/SampleAnalyzer/Lib/','-lSampleAnalyzer'])
        if self.libZIP:
            libs.extend(['-lz','-lzlib_for_ma5'])
        if self.libDelphes:
            libs.extend(['-L'+self.main.configLinux.delphes_lib_paths[0],'-lDelphes','-ldelphes_for_ma5'])
        if self.libDelfes:
            libs.extend(['-L'+self.main.configLinux.delfes_lib_paths[0],'-lDelphes','-ldelfes_for_ma5'])
        if self.fortran:
            libs.extend(['-lgfortran'])
        if self.libFastjet:
            libs.extend(['$(shell fastjet-config --libs --plugins --rpath=no)','-lfastjet_for_ma5'])
        file.write('LIBFLAGS = '+' '.join(libs)+'\n')
        file.write('\n')

        # Files to process
        file.write('# Files to process\n')
        file.write('SRCS  = $(wildcard Main/*.cpp)\n')
        file.write('SRCS += $(wildcard SampleAnalyzer/Analyzer/*.cpp)\n')
        file.write('HDRS  = $(wildcard Main/*.h)\n')
        file.write('HDRS += $(wildcard SampleAnalyzer/Analyzer/*.h)\n')
        file.write('\n')

        # Files to generate
        file.write('# Files to generate\n')
        file.write('OBJS    = $(SRCS:.cpp=.o)\n')
        file.write('PROGRAM = MadAnalysis5job\n')
        file.write('\n')

        # Lib to check
        file.write('# Requirements to check before building\n')
        libs=[]
        libs.append('$(MA5_BASE)/tools/SampleAnalyzer/Lib/libSampleAnalyzer.so')
        if self.libZIP:
            libs.append('$(MA5_BASE)/tools/SampleAnalyzer/Lib/libzlib_for_ma5.so')
        if self.libDelphes:
            libs.append('$(MA5_BASE)/tools/SampleAnalyzer/Lib/libdelphes_for_ma5.so')
        if self.libDelfes:
            libs.append('$(MA5_BASE)/tools/SampleAnalyzer/Lib/libdelfes_for_ma5.so')
        if self.libFastjet:
            libs.append('$(MA5_BASE)/tools/SampleAnalyzer/Lib/libfastjet_for_ma5.so')
        for ind in range(0,len(libs)):
            file.write('REQUIRED'+str(ind+1)+' = '+libs[ind]+'\n')
        file.write('\n')


        # Defining colours for shell
        file.write('# Defining colours\n')
        file.write('GREEN  = "\\\\033[1;32m"\n')
        file.write('RED    = "\\\\033[1;31m"\n')
        file.write('PINK   = "\\\\033[1;35m"\n')
        file.write('BLUE   = "\\\\033[1;34m"\n')
        file.write('YELLOW = "\\\\033[1;33m"\n')
        file.write('CYAN   = "\\\\033[1;36m"\n')
        file.write('NORMAL = "\\\\033[0;39m"\n')
        file.write('\n')

        # All target
        file.write('# All target\n')
        file.write('all: header library_check compile_header compile link_header link\n')
        file.write('\n')

        # Check library
        if len(libs)!=0:
            file.write('# Check library\n')
            file.write('library_check:\n')
            for ind in range(0,len(libs)):
                file.write('ifeq ($(wildcard $(REQUIRED'+str(ind+1)+')),)\n')
                file.write('\t@echo -e $(RED)"The shared library "$(REQUIRED'+str(ind+1)+')" is not found"\n')
	        file.write('\t@echo -e $(RED)" 1) Please check that MadAnalysis 5 is installed in the folder : "$(MA5_BASE)\n')
                file.write('\t@echo -e $(RED)" 2) Launch MadAnalysis 5 in normal mode in order to build this library."\n')
	        file.write('\t@echo -e $(NORMAL)\n')
	        file.write('\t@false\n')
                file.write('endif\n')
            file.write('\n')

        # Header target
        file.write('# Header target\n')
        file.write('header:\n')
        file.write('\t@echo -e $(YELLOW)"'+StringTools.Fill('-',50)+'"\n')
	file.write('\t@echo -e "'+StringTools.Center('Building MA5 job',50)+'"\n')
	file.write('\t@echo -e "'+StringTools.Fill('-',50)+'"$(NORMAL)\n')
        file.write('\n')

        # Compile_header target
        file.write('# Compile_header target\n')
        file.write('compile_header:\n')
        file.write('\t@echo -e $(YELLOW)"'+StringTools.Fill('-',50)+'"\n')
	file.write('\t@echo -e "'+StringTools.Center('Compilation',50)+'"\n')
	file.write('\t@echo -e "'+StringTools.Fill('-',50)+'"$(NORMAL)\n')
        file.write('\n')

        # Linking_header target
        file.write('# Link_header target\n')
        file.write('link_header:\n')
        file.write('\t@echo -e $(YELLOW)"'+StringTools.Fill('-',50)+'"\n')
	file.write('\t@echo -e "'+StringTools.Center('Final linking',50)+'"\n')
	file.write('\t@echo -e "'+StringTools.Fill('-',50)+'"$(NORMAL)\n')
        file.write('\n')

        # Clean_header target
        file.write('# clean_header target\n')
        file.write('clean_header:\n')
        file.write('\t@echo -e $(YELLOW)"'+StringTools.Fill('-',50)+'"\n')
	file.write('\t@echo -e "'+StringTools.Center('Removing intermediate files from building',50)+'"\n')
	file.write('\t@echo -e "'+StringTools.Fill('-',50)+'"$(NORMAL)\n')
        file.write('\n')

        # Mrproper_header target
        file.write('# mrproper_header target\n')
        file.write('mrproper_header:\n')
        file.write('\t@echo -e $(YELLOW)"'+StringTools.Fill('-',50)+'"\n')
	file.write('\t@echo -e "'+StringTools.Center('Cleaning all the project',50)+'"\n')
	file.write('\t@echo -e "'+StringTools.Fill('-',50)+'"$(NORMAL)\n')
        file.write('\n')

        # Compile target
        file.write('# Compile target\n')
        file.write('compile: $(OBJS)\n')
        file.write('\n')

        # Object file target
        file.write('# Object file target\n')
        file.write('$(OBJS): $(HDRS)\n')
        file.write('\n')

        # Link target
        file.write('# Link target\n')
        file.write('link: $(OBJS)\n')
        file.write('\t$(CXX) $(CXXFLAGS) $(OBJS) ')
        file.write('$(LIBFLAGS) -o $(PROGRAM)\n')
        file.write('\n')

        # Clean target
        file.write('# Clean target\n')
        file.write('clean: clean_header do_clean\n')
        file.write('\n')
        file.write('# Do clean target\n')
        file.write('do_clean: \n')
        file.write('\t@rm -f $(OBJS)\n')
        file.write('\n')

        # Mr Proper target
        file.write('# Mr Proper target \n')
        file.write('mrproper: mrproper_header do_mrproper\n')
        file.write('\n')
        file.write('# Do Mr Proper target \n')
        file.write('do_mrproper: do_clean\n')
        file.write('\t@rm -f $(PROGRAM) Log/compilation.log' + \
                   ' Log/linking.log Log/cleanup.log Log/mrproper.log *~ */*~ */*~ \n')
        file.write('\n')

        # Phony target
        file.write('# Phony target\n')
        file.write('.PHONY: do_clean header link_header compile_header\n')
        file.write('\n')

        # Closing the file
        file.close()

        if not JobWriter.WriteSetupFile(True,self.path+'/Build/',self.ma5dir,True,self.main.configLinux,self.main.isMAC):
            return False
        if not JobWriter.WriteSetupFile(False,self.path+'/Build/',self.ma5dir,True,self.main.configLinux,self.main.isMAC):
            return False
        
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
    def WriteSetupFile(bash,path,ma5dir,MA5BASE,configLinux,isMAC):

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
                file.write('export MA5_BASE=' + JobWriter.CleanPath(ma5dir)+'\n')
            else:
                file.write('setenv MA5_BASE=' + JobWriter.CleanPath(ma5dir)+'\n')
            file.write('\n')

        # Configuring PATH environment variable
        file.write('# Configuring PATH environment variable\n')
        if bash:
            file.write('export PATH=$PATH:' + JobWriter.CleanPath(':'.join(configLinux.toPATH))+'\n')
        else:
            file.write('setenv PATH $PATH:' + JobWriter.CleanPath(':'.join(configLinux.toPATH))+'\n')
        file.write('\n')

        # Configuring LD_LIBRARY_PATH environment variable
        file.write('# Configuring LD_LIBRARY_PATH environment variable\n')
        if bash:
            file.write('export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:' + JobWriter.CleanPath(':'.join(configLinux.toLDPATH))+'\n')
        else:
            file.write('setenv LD_LIBRARY_PATH $LD_LIBRARY_PATH:' + JobWriter.CleanPath(':'.join(configLinux.toLDPATH))+'\n')
        file.write('\n')

        # Configuring LIBRARY_PATH environment variable
        #file.write('# Configuring LIBRARY_PATH environment variable\n')
        #if bash:
        #    file.write('export LIBRARY_PATH=' + JobWriter.CleanPath(os.environ['LD_LIBRARY_PATH'])+'\n')
        #else:
        #    file.write('setenv LIBRARY_PATH ' + JobWriter.CleanPath(os.environ['LD_LIBRARY_PATH'])+'\n')
        #file.write('\n')

        # Configuring DYLD_LIBRARY_PATH environment variable
        if isMAC:
            file.write('# Configuring DYLD_LIBRARY_PATH environment variable\n')
            if bash:
                file.write('export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:' + JobWriter.CleanPath(':'.join(configLinux.toLDPATH))+'\n')
            else:
                file.write('setenv DYLD_LIBRARY_PATH $DYLD_LIBRARY_PATH:' + JobWriter.CleanPath(':'.join(configLinux.toLDPATH))+'\n')
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
            file.write('if [[ $PATH && $LD_LIBRARY_PATH ]]; then\n')
            file.write('echo -e $YELLOW"'+StringTools.Fill('-',56)+'"\n')
	    file.write('echo -e "'+StringTools.Center('Your environment is properly configured for MA5',56)+'"\n')
	    file.write('echo -e "'+StringTools.Fill('-',56)+'"$NORMAL\n')
            file.write('fi\n')
        else:
            file.write('if ( $?PATH && $?LD_LIBRARY_PATH ) then\n')
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
        commands.append('--ma5_version="'+self.main.version+';'+self.main.date+'"')

        # Inputs
        commands.append('../../Input/'+name+'.list')

        # Running SampleAnalyzer
        result = ShellCommand.Execute(commands,folder)

        return result

        
        
        
        
        
