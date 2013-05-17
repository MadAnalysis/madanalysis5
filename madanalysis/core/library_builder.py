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


from madanalysis.core.linux_architecture import LinuxArchitecture
import logging
import glob
import os
import commands
import sys

class LibraryBuilder:

    def __init__(self,configLinux,ma5dir):

        self.configLinux=configLinux
        self.ma5dir=ma5dir
        self.configStore = LinuxArchitecture()

        
    def checkMA5(self):
        logging.info("Checking the MadAnalysis library:")
        FirstUse=False

        # Look for 'lib' directory
        name='/tools/SampleAnalyzer/Lib'
        if not os.path.isdir(self.ma5dir+name):
            try:
       	        FirstUse=True
                os.mkdir(self.ma5dir+name)
            except:
                logging.error("Impossible to create the directory :")
                logging.error(" "+name)
                return False

        # Look for shared library 'MadAnalysis' and 'config' file
        if not os.path.isfile(self.ma5dir+'/tools/SampleAnalyzer/Lib/libSampleAnalyzer.a') \
           or not os.path.isfile(self.ma5dir+'/tools/architecture.ma5'):
            FirstUse=True

        # Importing the configuration stored with the library
        if not FirstUse:
            if not self.configStore.Import(self.ma5dir+'/tools/architecture.ma5'):
                FirstUse=True

        if not FirstUse:
             logging.info('  => MadAnalysis library found.')
        return FirstUse
    
        
    def compare(self):
        return self.configLinux.Compare(self.configStore)
        
