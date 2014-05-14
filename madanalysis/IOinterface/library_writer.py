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


from madanalysis.selection.instance_name   import InstanceName
from madanalysis.IOinterface.folder_writer import FolderWriter
from madanalysis.IOinterface.job_writer    import JobWriter
from madanalysis.core.string_tools         import StringTools
import logging
import shutil
import os
import commands
import subprocess


class LibraryWriter():

    def __init__(self,ma5dir,jobdir,libZIP,libFASTJET,forced,fortran,delphes,delfes):
        self.ma5dir     = ma5dir
        self.jobdir     = jobdir
        self.path       = os.path.normpath(ma5dir+"/tools/")
        self.libZIP     = libZIP
        self.libFASTJET = libFASTJET
        self.forced     = forced
        self.fortran    = fortran
        self.libDelphes = delphes
        self.libDelfes  = delfes

    def get_ncores(self):
        # Number of cores
        import multiprocessing
        nmaxcores=multiprocessing.cpu_count()
        logging.info("     => How many cores for the compiling? default = max = " +\
                     str(nmaxcores)+"")
        
        if not self.forced:
            test=False
            while(not test):
                answer=raw_input("     Answer: ")
                if answer=="":
                    test=True
                    ncores=nmaxcores
                    break
                try:
                    ncores=int(answer)
                except:    
                    test=False
                    continue
                if ncores<=nmaxcores and ncores>0:
                    test=True
                    
        else:
            ncores=nmaxcores
        logging.info("     Number of cores used for the compilation = " +\
                     str(ncores))
        return ncores

    def get_ncores2(self):
        # Number of cores
        import multiprocessing
        nmaxcores=multiprocessing.cpu_count()
        logging.info("   How many cores for the compiling? default = max = " +\
                     str(nmaxcores)+"")
        
        if not self.forced:
            test=False
            while(not test):
                answer=raw_input("   Answer: ")
                if answer=="":
                    test=True
                    ncores=nmaxcores
                    break
                try:
                    ncores=int(answer)
                except:    
                    test=False
                    continue
                if ncores<=nmaxcores and ncores>0:
                    test=True
                    
        else:
            ncores=nmaxcores
        logging.info("   => Number of cores used for the compilation = " +\
                     str(ncores))
        return ncores


    def Open(self):
        return FolderWriter.CreateDirectory(self.path,overwrite=True)

    def WriteMakefileForInterfaces(self,package):

        # Open the file
        try:
            file = open(self.path + "/SampleAnalyzer/Interfaces/Makefile_" + package,"w")
        except:
            logging.error('impossible to write the file '+ self.path + "/SampleAnalyzer/Makefile_" + package)
            return False

        # Header
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('#'+StringTools.Center('MAKEFILE DEVOTED TO THE INTERFACE TO '+package.upper(),78)+'#\n')
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('\n')

        # Options for C++ compilation
        file.write('# Options for C++ compilation\n')
        file.write('CXX = g++\n')
        if package=='fastjet':
            file.write('CXXFASTJET = $(shell fastjet-config --cxxflags --plugins)\n')
        file.write('CXXFLAGS = -Wall -O3 -DROOT_USE -fPIC $(shell root-config --cflags) -I./../../')
       # if package=='zlib':
       #     file.write(' -DZIP_USE')
       # elif package=='delphes':
       #     file.write(' -DDELPHES_USE')
       # elif package=='delfes':
       #     file.write(' -DDELFES_USE')
        if package=='fastjet':
       #     file.write(' -DFASTJET_USE')
            file.write(' $(CXXFASTJET)')
        file.write('\n')

        # Options for C++ compilation
        if self.fortran:
            file.write('# Options for Fortran compilation\n')
            file.write('FC = gfortran\n')
            file.write('FCFLAGS = -Wall -O3 -fPIC\n')
            file.write('\n')

        # Files for analyzers
        file.write('# Files\n')
        file.write('SRCS = $(wildcard '+package+'/*.cpp)\n')
        file.write('HDRS = $(wildcard '+package+'/*.h)\n')
        file.write('OBJS = $(SRCS:.cpp=.o)\n')
        if self.fortran:
            file.write('FORTRAN_SRCS = $(wildcard '+package+'/*.f)\n')
            file.write('FORTRAN_OBJS = $(FORTRAN_SRCS:.f=.o)\n')
        file.write('\n')

        # Name of the library
        file.write('# Name of the library\n')
        file.write('PROGRAM = '+package+'_for_ma5\n')
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

        # All
        file.write('# All target\n')
        file.write('all: header compile_header compile link_header link\n')
        file.write('\n')

        # Header target
        file.write('# Header target\n')
        file.write('header:\n')
        file.write('\t@echo -e $(YELLOW)"'+StringTools.Fill('-',50)+'"\n')
	file.write('\t@echo -e "'+StringTools.Center('Building SampleAnalyzer library',50)+'"\n')
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

        # Precompile
        file.write('# Precompile target\n')
        file.write('precompile:\n')
        file.write('\n')

        # Precompile
        file.write('# Compile target\n')
        if self.fortran:
            file.write('compile: precompile $(OBJS) $(FORTRAN_OBJS)\n')
        else:
            file.write('compile: precompile $(OBJS)\n')
        file.write('\n')

        # Precompile
        file.write('# Link target\n')
        if self.fortran:
            file.write('link: $(OBJS) $(FORTRAN_OBJS)\n')
            file.write('\t$(CXX) -shared -o ../Lib/lib$(PROGRAM).so $(OBJS) $(FORTRAN_OBJS)\n')
        else:
            file.write('link: $(OBJS)\n')
            file.write('\t$(CXX) -shared -o ../Lib/lib$(PROGRAM).so $(OBJS)\n')
        file.write('\n')

        # Phony target
        file.write('# Phony target\n')
        file.write('.PHONY: do_clean header compile_header link_header\n')
        file.write('\n')

        # Cleaning
        file.write('# Clean target\n')
        file.write('clean: clean_header do_clean\n')
        file.write('\n')
        file.write('# Do clean target\n')
        file.write('do_clean: \n')
        file.write('\t@rm -f $(OBJS)\n')
        if self.fortran:
            file.write('\t@rm -f $(FORTRAN_OBJS)\n')
        file.write('\n')

        # Mr Proper
        file.write('# Mr Proper target \n')
        file.write('mrproper: mrproper_header do_mrproper\n')
        file.write('\n')
        file.write('# Do Mr Proper target \n')
        file.write('do_mrproper: do_clean\n')
        file.write('\t@rm -f ../Lib/lib$(PROGRAM).so\n')
        file.write('\t@rm -f compilation.log linking.log cleanup.log mrproper.log *~ */*~\n')
        file.write('\n')

        # Closing the file
        file.close()

        return True


    def WriteMakefile(self,option=""):

        # Open the file
        file = open(self.path + "/SampleAnalyzer/Makefile","w")

        # Header
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('#'+StringTools.Center('MAKEFILE DEVOTED TO SAMPLE ANALYZER LIBRARY',78)+'#\n')
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('\n')

        # Compilators
        file.write('# Compilators\n')
        file.write('CXX = g++\n')
        if self.fortran:
            file.write('FC = gfortran\n')
        file.write('\n')

        # Options for compilation
        file.write('# Options for compilation\n')
        if self.libFASTJET:
            file.write('CXXFASTJET = $(shell fastjet-config --cxxflags --plugins)\n')
        # BENJ FIX file.write('CXXFLAGS = -Wall -O3 -fPIC $(shell root-config --cflags) -I./../')
        file.write('CXXFLAGS = -Wall -O3 -DROOT_USE -fPIC $(shell root-config --cflags) -I./../')
        if self.libZIP:
            file.write(' -DZIP_USE')
        if self.libDelphes:
            file.write(' -DDELPHES_USE')
        if self.libDelfes:
            file.write(' -DDELFES_USE')
        if self.libFASTJET:
            file.write(' -DFASTJET_USE')
            file.write(' $(CXXFASTJET)')
        file.write('\n')
        if self.fortran:
            file.write('FC = gfortran\n')
            file.write('FCFLAGS = -O2\n')
        file.write('\n')

        # Files for analyzers
        file.write('# Files\n')
        file.write('SRCS = $(wildcard */*.cpp)\n')
#        file.write('SRCS += $(wildcard */*/*.cpp)\n')
        file.write('HDRS = $(wildcard */*.h)\n')
        file.write('OBJS = $(SRCS:.cpp=.o)\n')
        if self.fortran:
            file.write('FORTRAN_SRCS = $(wildcard */*.f)\n')
            file.write('FORTRAN_OBJS = $(FORTRAN_SRCS:.f=.o)\n')
        file.write('\n')

        # Name of the library
        file.write('# Name of the library\n')
        file.write('PROGRAM = SampleAnalyzer\n')
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

        # All
        file.write('# All target\n')
        file.write('all: header compile_header compile link_header link\n')
        file.write('\n')

        # Header target
        file.write('# Header target\n')
        file.write('header:\n')
        file.write('\t@echo -e $(YELLOW)"'+StringTools.Fill('-',50)+'"\n')
	file.write('\t@echo -e "'+StringTools.Center('Building SampleAnalyzer library',50)+'"\n')
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

        # Precompile
        file.write('# Precompile target\n')
        file.write('precompile:\n')
        file.write('\n')

        # Precompile
        file.write('# Compile target\n')
        if self.fortran:
            file.write('compile: precompile $(OBJS) $(FORTRAN_OBJS)\n')
        else:
            file.write('compile: precompile $(OBJS)\n')
        file.write('\n')

        # Precompile
        file.write('# Link target\n')
        if self.fortran:
            file.write('link: $(OBJS) $(FORTRAN_OBJS)\n')
            file.write('\t$(CXX) -shared -o Lib/lib$(PROGRAM).so $(OBJS) $(FORTRAN_OBJS)\n')
        else:
            file.write('link: $(OBJS)\n')
            file.write('\t$(CXX) -shared -o Lib/lib$(PROGRAM).so $(OBJS)\n')
#            file.write('\tar -ruc Lib/lib$(PROGRAM).a $(OBJS)\n')
        file.write('\n')

        # Phony target
        file.write('# Phony target\n')
        file.write('.PHONY: do_clean header compile_header link_header\n')
        file.write('\n')

        # Cleaning
        file.write('# Clean target\n')
        file.write('clean: clean_header do_clean\n')
        file.write('\n')
        file.write('# Do clean target\n')
        file.write('do_clean: \n')
        file.write('\t@rm -f $(OBJS)\n')
        file.write('\n')

        # Mr Proper
        file.write('# Mr Proper target \n')
        file.write('mrproper: mrproper_header do_mrproper\n')
        file.write('\n')
        file.write('# Do Mr Proper target \n')
        file.write('do_mrproper: do_clean\n')
        file.write('\t@rm -f Lib/lib$(PROGRAM).so\n')
        file.write('\t@rm -f compilation.log linking.log cleanup.log mrproper.log *~ */*~\n')
        file.write('\n')

        # Closing the file
        file.close()

        JobWriter.WriteSetupFile(True,self.path+'/SampleAnalyzer',self.ma5dir,False)
        JobWriter.WriteSetupFile(False,self.path+'/SampleAnalyzer',self.ma5dir,False)

        return True


    def Compile(self,ncores,package,folder):

        # number of cores
        strcores=''
        if ncores>1:
            strcores='-j'+str(ncores)

        # log file name
        if package == 'SampleAnalyzer':
            logfile = folder+'/compilation.log'
        else:
            logfile = folder+'/compilation_'+package+'.log'

        # makefile
        if package == 'SampleAnalyzer':
            makefile = 'Makefile'
        else:
            makefile = 'Makefile_'+package

        # shell command
        commands = ['make','compile',strcores,'--file='+makefile]

        # call
        result, out, err = LibraryWriter.Launch(commands,logfile,folder)

        # return result
        if not result:
            logging.error('impossible to compile the project. For more details, see the log file:')
            logging.error(logfile)
            
        return result


    def Link(self,package,folder):

        # log file name
        if package == 'SampleAnalyzer':
            logfile = folder+'/linking.log'
        else:
            logfile = folder+'/linking_'+package+'.log'

        # makefile
        if package == 'SampleAnalyzer':
            makefile = 'Makefile'
        else:
            makefile = 'Makefile_'+package

        # shell command
        commands = ['make','link','--file='+makefile]

        # call
        result, out, err = LibraryWriter.Launch(commands,logfile,folder)

        # return result
        if not result:
            logging.error('impossible to link the project. For more details, see the log file:')
            logging.error(logfile)
            
        return result


    @staticmethod
    def Launch(theCommands,logfile,path):

        # Launching the commands
        try:
            result=subprocess.Popen(theCommands, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=path)
        except:
            logging.error('impossible to execute the commands: '+' '.join(theCommands))
            return False, None, None

        # Testing if errors
        out, err = result.communicate()

        # Open the log file
        opened=True
        try:
            output = open(logfile,'w')
        except:
            logging.error('impossible to write the file '+logfile)
            opened=False

        if opened:

            output.write('MadAnalysis redirection of stdout:\n')
            output.write('----------------------------------\n')
            output.write('\n')
            if out!=None:
                for line in out:
                    output.write(line)
            output.write('\n')
            output.write('MadAnalysis redirection of stderr:\n')
            output.write('----------------------------------\n')
            output.write('\n')
            if err!=None:
                for line in err:
                    output.write(line)
            output.write('\n')

            output.close()
            
        # Return results
        return (result.returncode==0), out, err
    

    def Clean(self,package,folder):

        # log file name
        if package == 'SampleAnalyzer':
            logfile = folder+'/cleanup.log'
        else:
            logfile = folder+'/cleanup_'+package+'.log'

        # makefile
        if package == 'SampleAnalyzer':
            makefile = 'Makefile'
        else:
            makefile = 'Makefile_'+package

        # shell command
        commands = ['make','clean','--file='+makefile]

        # call
        result, out, err = LibraryWriter.Launch(commands,logfile,folder)

        # return result
        if not result:
            logging.error('impossible to clean the project. For more details, see the log file:')
            logging.error(logfile)
            
        return result


    def MrProper(self,package,folder):

        # log file name
        if package == 'SampleAnalyzer':
            logfile = folder+'/mrproper.log'
        else:
            logfile = folder+'/mrproper_'+package+'.log'

        # makefile
        if package == 'SampleAnalyzer':
            makefile = 'Makefile'
        else:
            makefile = 'Makefile_'+package

        # shell command
        commands = ['make','mrproper','--file='+makefile]

        # call
        result, out, err = LibraryWriter.Launch(commands,logfile,folder)

        # return result
        if not result:
            logging.error('impossible to clean the project. For more details, see the log file:')
            logging.error(logfile)
            
        return result
