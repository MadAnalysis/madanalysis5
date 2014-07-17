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


from madanalysis.system.architecture_info import ArchitectureInfo
import logging
import glob
import os
import commands
import sys

class LibraryBuilder:

    def __init__(self,archi_info):

        self.archi_info        = archi_info
        self.archi_info_stored = ArchitectureInfo()

    def checkMA5(self):
        logging.info("Checking the MadAnalysis library:")
        FirstUse=False

        # Look for 'lib' directory
        name='/tools/SampleAnalyzer/Lib'
        logging.debug('-> looking for folder: '+name)
        if not os.path.isdir(self.archi_info.ma5dir+name):
            try:
                FirstUse=True
                os.mkdir(self.archi_info.ma5dir+name)
            except:
                logging.error("Impossible to create the directory :")
                logging.error(" "+name)
                return False, False

        # Look for the shared library 'MadAnalysis' and 'config' file
        librairies = [self.archi_info.ma5dir+'/tools/SampleAnalyzer/Lib/libprocess_for_ma5.so',\
                        self.archi_info.ma5dir+'/tools/architecture.ma5',\
                        self.archi_info.ma5dir+'/tools/SampleAnalyzer/Lib/libcommons_for_ma5.so']
        for lib in librairies:
            logging.debug('-> looking for file: '+lib)
            if not os.path.isfile(lib):
                logging.debug('\t-> file '+ lib + " not found.")
                FirstUse = True
                return True, False

#        if not os.path.isfile(self.archi_info.ma5dir+'/tools/SampleAnalyzer/Lib/libprocess_for_ma5.so'): \
#           or not os.path.isfile(self.archi_info.ma5dir+'/tools/architecture.ma5') \
#           or not os.path.isfile(self.archi_info.ma5dir+'/tools/SampleAnalyzer/Lib/libcommons_for_ma5.so'):
#            FirstUse=True
#            return True, False

        # Look for optional library
        libraries = []
        if self.archi_info.has_fastjet:
            libraries.append(self.archi_info.ma5dir+'/tools/SampleAnalyzer/Lib/libfastjet_for_ma5.so')
        if self.archi_info.has_zlib:
            libraries.append(self.archi_info.ma5dir+'/tools/SampleAnalyzer/Lib/libzlib_for_ma5.so')
        if self.archi_info.has_delphes:
            libraries.append(self.archi_info.ma5dir+'/tools/SampleAnalyzer/Lib/libdelphes_for_ma5.so')
        if self.archi_info.has_delphesMA5tune:
            libraries.append(self.archi_info.ma5dir+'/tools/SampleAnalyzer/Lib/libdelphesMA5tune_for_ma5.so')
        for library in libraries:
            if not os.path.isfile(library):
                logging.debug('\t-> library '+ library + " not found.")
                return False, True

        # Importing the configuration stored with the library
        if not FirstUse:
            logging.debug('-> loading the architecture file.')
            if not self.archi_info_stored.load(self.archi_info.ma5dir+'/tools/architecture.ma5'):
                logging.debug('\t-> failed to load the architecture file.')
                FirstUse=True
                return True, False

        return FirstUse, False
    
        
    def compare(self):
        return self.archi_info.Compare(self.archi_info_stored)
        
