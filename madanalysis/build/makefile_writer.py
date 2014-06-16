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


import logging
from string_tools import StringTools


class MakefileWriter():

    class UserfriendlyMakefileOptions():
        def __init__(self):
            self.has_commons        = False
            self.has_process        = False
            self.has_zlib           = False
            self.has_fastjet        = False
            self.has_delphes        = False
            self.has_delphesMA5tune = False


    @staticmethod
    def DefineColours(file):
        file.write('# Defining colours\n')
        file.write('GREEN  = "\\\\033[1;32m"\n')
        file.write('RED    = "\\\\033[1;31m"\n')
        file.write('PINK   = "\\\\033[1;35m"\n')
        file.write('BLUE   = "\\\\033[1;34m"\n')
        file.write('YELLOW = "\\\\033[1;33m"\n')
        file.write('CYAN   = "\\\\033[1;36m"\n')
        file.write('NORMAL = "\\\\033[0;39m"\n')
        file.write('\n')

        
    @staticmethod
    def UserfriendlyMakefileForSampleAnalyzer(filename,options):

        # Open the Makefile
        try:
            file = open(filename,"w")
        except:
            logging.error('impossible to write the file '+filename)
            return False

        # Header
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('#'+StringTools.Center('GENERAL MAKEFILE DEVOTED TO SAMPLEANALYZER',78)+'#\n')
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('\n')

        # Defining colours for shell
        MakefileWriter.DefineColours(file)

        # All
        file.write('# All target\n')
        file.write('all:\n')
        if options.has_commons:
            file.write('\tcd Commons && $(MAKE)\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_commons\n')
        if options.has_zlib:
            file.write('\tcd Interfaces && $(MAKE) -f Makefile_zlib\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_zlib\n')
        if options.has_fastjet:
            file.write('\tcd Interfaces && $(MAKE) -f Makefile_fastjet\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_fastjet\n')
        if options.has_delphes:
            file.write('\tcd Interfaces && $(MAKE) -f Makefile_delphes\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_delphes\n')
        if options.has_delphesMA5tune:
            file.write('\tcd Interfaces && $(MAKE) -f Makefile_delphesMA5tune\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_delphesMA5tune\n')
        if options.has_process:
            file.write('\tcd Process && $(MAKE) -f Makefile\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_process\n')
        file.write('\n')

        # Clean
        file.write('# Clean target\n')
        file.write('clean:\n')
        if options.has_commons:
            file.write('\tcd Commons && $(MAKE) clean\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_commons clean\n')
        if options.has_zlib:
            file.write('\tcd Interfaces && $(MAKE) -f Makefile_zlib clean\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_zlib clean\n')
        if options.has_fastjet:
            file.write('\tcd Interfaces && $(MAKE) -f Makefile_fastjet clean\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_fastjet clean\n')
        if options.has_delphes:
            file.write('\tcd Interfaces && $(MAKE) -f Makefile_delphes clean\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_delphes clean\n')
        if options.has_delphesMA5tune:
            file.write('\tcd Interfaces && $(MAKE) -f Makefile_delphesMA5tune clean\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_delphesMA5tune clean\n')
        if options.has_process:
            file.write('\tcd Process && $(MAKE) -f Makefile clean\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_process clean\n')
        file.write('\n')

        # Mrproper
        file.write('# Mrproper target\n')
        file.write('mrproper:\n')
        file.write('\t@rm -f *~ */*~\n')
        if options.has_commons:
            file.write('\tcd Commons && $(MAKE) mrproper\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_commons mrproper\n')
        if options.has_zlib:
            file.write('\tcd Interfaces && $(MAKE) -f Makefile_zlib mrproper\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_zlib mrproper\n')
        if options.has_fastjet:
            file.write('\tcd Interfaces && $(MAKE) -f Makefile_fastjet mrproper\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_fastjet mrproper\n')
        if options.has_delphes:
            file.write('\tcd Interfaces && $(MAKE) -f Makefile_delphes mrproper\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_delphes mrproper\n')
        if options.has_delphesMA5tune:
            file.write('\tcd Interfaces && $(MAKE) -f Makefile_delphesMA5tune mrproper\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_delphesMA5tune mrproper\n')
        if options.has_process:
            file.write('\tcd Process && $(MAKE) -f Makefile mrproper\n')
            file.write('\tcd Test && $(MAKE) -f Makefile_process mrproper\n')
        file.write('\n')

        # Closing the file
        file.close()

        return True


    class MakefileOptions():
        def __init__(self):
            self.isMac                     = False
            self.has_commons               = False
            self.has_process               = False
            self.has_fastjet_tag           = False
            self.has_delphes_tag           = False
            self.has_delphesMA5tune_tag    = False
            self.has_zlib_tag              = False
            self.has_fastjet_inc           = False
            self.has_delphes_inc           = False
            self.has_delphesMA5tune_inc    = False
            self.has_zlib_inc              = False
            self.has_fastjet_lib           = False
            self.has_delphes_lib           = False
            self.has_delphesMA5tune_lib    = False
            self.has_zlib_lib              = False
            self.has_fastjet_ma5lib        = False
            self.has_delphes_ma5lib        = False
            self.has_delphesMA5tune_ma5lib = False
            self.has_zlib_ma5lib           = False


    @staticmethod
    def Makefile(MakefileName,title,ProductName,ProductPath,isLibrary,cppfiles,hfiles,options,archi_info,toRemove,moreIncludes=[]):

        import os
        # Open the Makefile
        try:
            file = open(MakefileName,"w")
        except:
            logging.error('impossible to write the file '+MakefileName)
            return False

        # Header
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('#'+StringTools.Center('MAKEFILE DEVOTED TO '+title.upper(),78)+'#\n')
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('\n')

        # Compilers
        file.write('# Compilers\n')
        file.write('CXX = g++\n')
        file.write('\n')

        # Options for C++ compilation
        file.write('# C++ Compilation options\n')

        # - general
        cxxflags=[]
        cxxflags.extend(['-Wall','-O3','-fPIC', '-I$(MA5_BASE)/tools/']) # general
        file.write('CXXFLAGS  = '+' '.join(cxxflags)+'\n')
        for item in moreIncludes:
            file.write('CXXFLAGS += '+' -I'+item+'\n')
            
        # - root
        cxxflags=[]
        cxxflags.extend(['$(shell root-config --cflags)'])
#        cxxflags.extend(['-I'+archi_info.root_inc_path]) # root
        file.write('CXXFLAGS += '+' '.join(cxxflags)+'\n')

        # - fastjet
        if options.has_fastjet_inc:
            cxxflags=[]
            cxxflags.extend(['$(shell fastjet-config --cxxflags --plugins)'])
            file.write('CXXFLAGS += '+' '.join(cxxflags)+'\n')

        # - zlib
        if options.has_zlib_inc:
            cxxflags=[]
            cxxflags.extend(['-I'+archi_info.zlib_inc_path])
            file.write('CXXFLAGS += '+' '.join(cxxflags)+'\n')

        # - delphes
        if options.has_delphes_inc:
            cxxflags=[]
            for header in archi_info.delphes_inc_paths:
                cxxflags.extend(['-I'+header])
            file.write('CXXFLAGS += '+' '.join(cxxflags)+'\n')

        # - delphesMA5tune
        if options.has_delphesMA5tune_inc:
            cxxflags=[]
            for header in archi_info.delphesMA5tune_inc_paths:
                cxxflags.extend(['-I'+header])
            file.write('CXXFLAGS += '+' '.join(cxxflags)+'\n')

        # - tags
        cxxflags=[]
        cxxflags.extend(['-DROOT_USE'])
        if options.has_fastjet_tag:
            cxxflags.extend(['-DFASTJET_USE'])
        if options.has_zlib_tag:
            cxxflags.extend(['-DZIP_USE'])
        if options.has_delphes_tag:
             cxxflags.extend(['-DDELPHES_USE'])
        if options.has_delphesMA5tune_tag:
             cxxflags.extend(['-DDELPHESMA5TUNE_USE'])
        file.write('CXXFLAGS += '+' '.join(cxxflags)+'\n')
        file.write('\n')

        # Options for C++ linking
        file.write('# Linking options\n')

        # - general
        libs=[]
        file.write('LIBFLAGS  = \n')

        # - commons
        if options.has_commons:
            libs=[]
            libs.extend(['-L$(MA5_BASE)/tools/SampleAnalyzer/Lib'])
            file.write('LIBFLAGS += '+' '.join(libs)+'\n')
        
        # - process
        if options.has_process:
            libs=[]
            libs.extend(['-lprocess_for_ma5'])
            file.write('LIBFLAGS += '+' '.join(libs)+'\n')

        # - zlib
        if options.has_zlib_ma5lib or options.has_zlib_lib:
            libs=[]
            if options.has_zlib_ma5lib:
                libs.extend(['-lzlib_for_ma5'])
            if options.has_zlib_lib:
                libs.extend(['-L'+archi_info.zlib_lib_path,'-lz'])
            file.write('LIBFLAGS += '+' '.join(libs)+'\n')

        # - fastjet
        if options.has_fastjet_ma5lib or options.has_fastjet_lib:
            libs=[]
            if options.has_fastjet_ma5lib:
                libs.extend(['-lfastjet_for_ma5'])
            if options.has_fastjet_lib:
                libs.extend(['$(shell fastjet-config --libs --plugins)']) # --rpath=no)'])
            file.write('LIBFLAGS += '+' '.join(libs)+'\n')

        # - delphes
        if options.has_delphes_ma5lib or options.has_delphes_lib:
            libs=[]
            if options.has_delphes_ma5lib:
                libs.extend(['-ldelphes_for_ma5'])
            if options.has_delphes_lib:
                libs.extend(['-L'+archi_info.delphes_lib_paths[0],'-lDelphes'])
            file.write('LIBFLAGS += '+' '.join(libs)+'\n')

        # - delphesMA5tune
        if options.has_delphesMA5tune_ma5lib or options.has_delphesMA5tune_lib:
            libs=[]
            if options.has_delphesMA5tune_ma5lib:
                libs.extend(['-ldelphesMA5tune_for_ma5'])
            if options.has_delphesMA5tune_lib:
                libs.extend(['-L'+archi_info.delphesMA5tune_lib_paths[0],'-lDelphesMA5tune'])
            file.write('LIBFLAGS += '+' '.join(libs)+'\n')

        # - Commons
        if options.has_commons:
            libs=[]
            libs.extend(['-lcommons_for_ma5'])
            file.write('LIBFLAGS += '+' '.join(libs)+'\n')

        # - Root
        libs=[]
        #libs.extend(['-L'+archi_info.root_lib_path, \
        #            '-lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic -lEG'])
        # becareful: to not forget -lEG
        libs.extend(['$(shell root-config --libs)','-lEG'])
        file.write('LIBFLAGS += '+' '.join(libs)+'\n')
        file.write('\n')

        # Lib to check
        libs=[]
        if options.has_commons:
            libs.append('$(MA5_BASE)/tools/SampleAnalyzer/Lib/libcommons_for_ma5.so')
        if options.has_process:
            libs.append('$(MA5_BASE)/tools/SampleAnalyzer/Lib/libprocess_for_ma5.so')
        if options.has_zlib_ma5lib:
            libs.append('$(MA5_BASE)/tools/SampleAnalyzer/Lib/libzlib_for_ma5.so')
        if options.has_delphes_ma5lib:
            libs.append('$(MA5_BASE)/tools/SampleAnalyzer/Lib/libdelphes_for_ma5.so')
        if options.has_delphesMA5tune_ma5lib:
            libs.append('$(MA5_BASE)/tools/SampleAnalyzer/Lib/libdelphesMA5tune_for_ma5.so')
        if options.has_fastjet_ma5lib:
            libs.append('$(MA5_BASE)/tools/SampleAnalyzer/Lib/libfastjet_for_ma5.so')
        if len(libs)!=0:
            file.write('# Requirements to check before building\n')
            for ind in range(0,len(libs)):
                file.write('REQUIRED'+str(ind+1)+' = '+libs[ind]+'\n')
            file.write('\n')

        # Files for analyzers
        file.write('# Files\n')
        for ind in range(0,len(cppfiles)):
            if ind==0:
               file.write('SRCS  = $(wildcard '+cppfiles[ind]+')\n')
            else:
               file.write('SRCS += $(wildcard '+cppfiles[ind]+')\n')
        for ind in range(0,len(hfiles)):
            if ind==0:
               file.write('HDRS  = $(wildcard '+hfiles[ind]+')\n')
            else:
               file.write('HDRS += $(wildcard '+hfiles[ind]+')\n')
        file.write('OBJS  = $(SRCS:.cpp=.o)\n')
        file.write('\n')

        # Name of the library
        if isLibrary:
            file.write('# Name of the library\n')
            file.write('LIBRARY = '+ProductName+'\n')
        else:
            file.write('# Name of the executable\n')
            file.write('PROGRAM = '+ProductName+'\n')
        file.write('\n')

        # Defining colours for shell
        MakefileWriter.DefineColours(file)

        # All
        file.write('# All target\n')
        if len(libs)==0:
            file.write('all: header compile_header compile link_header link\n')
        else:
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
	file.write('\t@echo -e "'+StringTools.Center('Building '+title,50)+'"\n')
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
	file.write('\t@echo -e "'+StringTools.Center('Linking',50)+'"\n')
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

        # Compile
        file.write('# Compile target\n')
        file.write('compile: precompile $(OBJS)\n')
        file.write('\n')

        # Compile each file
        # TO NOT FORGET HDRS -> handling header dependencies
        file.write('# Compile each file\n')
        file.write('%.o: %.cpp $(HDRS)\n')
        file.write('\t$(CXX) $(CXXFLAGS) -o $@ -c $<\n')
        file.write('\n')

        # Link
        file.write('# Link target\n')
        file.write('link: $(OBJS)\n')
        if not ProductPath.endswith('/'):
            ProductPath=ProductPath+'/'
        if isLibrary:
            if options.isMac:
                file.write('\t$(CXX) -shared -flat_namespace -dynamiclib -undefined suppress -o '+ProductPath+'$(LIBRARY) $(OBJS) $(LIBFLAGS)\n')
            else:
                file.write('\t$(CXX) -shared -o '+ProductPath+'$(LIBRARY) $(OBJS) $(LIBFLAGS)\n')
        else:
            file.write('\t$(CXX) $(OBJS) $(LIBFLAGS) -o '+ProductPath+'$(PROGRAM)\n')
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
        if isLibrary:
            file.write('\t@rm -f '+ProductPath+'$(LIBRARY)\n')
        else:
            file.write('\t@rm -f '+ProductPath+'$(PROGRAM)\n')
        file.write('\t@rm -f *~ */*~\n')
        file.write('\t@rm -f '+' '.join(toRemove)+'\n')
        file.write('\n')

        # Closing the file
        file.close()

        return True
    
