################################################################################
#  
#  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <http://madanalysis.irmp.ucl.ac.be>
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
from madanalysis.core.string_tools         import StringTools
import logging
import shutil
import os
import commands

class LibraryWriter():

    def __init__(self,ma5dir,jobdir,libZIP,FAC,libFASTJET,forced,fortran):
        self.ma5dir     = ma5dir
        self.jobdir     = jobdir
        self.path       = os.path.normpath(ma5dir+"/tools/")
        self.libZIP     = libZIP
        self.FAC        = FAC
        self.libFASTJET = libFASTJET
        self.forced     = forced
        self.fortran    = fortran


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

    def Open(self):
        return FolderWriter.CreateDirectory(self.path,overwrite=True)

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
        file.write('GCC = g++\n')
        if self.fortran:
            file.write('FC = gfortran\n')
        file.write('\n')

        # Options for compilation
        file.write('# Options for compilation\n')
        if self.libFASTJET:
            file.write('CXXFASTJET = $(shell fastjet-config --cxxflags --plugins)\n')
        file.write('CXXFLAGS = -Wall -O3 $(shell root-config --cflags) -I./../')
        if self.libZIP:
            file.write(' -DZIP_USE')
        if self.FAC:
            file.write(' -DFAC_USE')
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
        if self.FAC:
            file.write('\tcd ..; rootcint -f "SampleAnalyzer/Reader/FACdict.cpp" ' +\
                       '-c -p SampleAnalyzer/Reader/FACdataformat.h SampleAnalyzer/Reader/FACLinkDef.h\n')
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
            file.write('\tar -ruc Lib/lib$(PROGRAM).a $(OBJS) $(FORTRAN_OBJS)\n')
        else:
            file.write('link: $(OBJS)\n')
            file.write('\tar -ruc Lib/lib$(PROGRAM).a $(OBJS)\n')
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
        file.write('\t@rm -f $(OBJS) Reader/FACdict.*\n')
        file.write('\n')

        # Mr Proper
        file.write('# Mr Proper target \n')
        file.write('mrproper: mrproper_header do_mrproper\n')
        file.write('\n')
        file.write('# Do Mr Proper target \n')
        file.write('do_mrproper: do_clean\n')
        file.write('\t@rm -f Lib/lib$(PROGRAM).a\n')
        file.write('\t@rm -f compilation.log linking.log cleanup.log *~ */*~\n')
        file.write('\n')

        # Closing the file
        file.close()

        return True


    def Compile(self):
        strcores=''
        ncores = self.get_ncores()
        if ncores>1:
            strcores='-j'+str(ncores)
        res=commands.getstatusoutput("cd "\
                                     +self.path+"/SampleAnalyzer/;"\
                                     +" make compile "+strcores+" > compilation.log 2>&1")
        if res[0]==0:
            return True
        else:
            logging.error("errors occured during compilation. For more details, see the file :")
            logging.error(" "+self.path+"/SampleAnalyzer/compilation.log")
            return False

    def Link(self):
        res=commands.getstatusoutput("cd "\
                                     +self.path+"/SampleAnalyzer/;"\
                                     +" make link > linking.log 2>&1")
        if res[0]==0:
            return True
        else:
            logging.error("errors occured during compilation. For more details, see the file :")
            logging.error(" "+self.path+"/SampleAnalyzer/linking.log")
            return False


    def Clean(self):
        res=commands.getstatusoutput("cd "\
                                     +self.path+"/SampleAnalyzer/;"\
                                     +" make clean > cleanup.log 2>&1")
        return True

    def MrProper(self):
        res=commands.getstatusoutput("cd "\
                                     +self.path+"/SampleAnalyzer/;"\
                                     +" make mrproper > cleanup.log 2>&1")
        return True
