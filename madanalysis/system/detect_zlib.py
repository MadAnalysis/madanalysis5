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
import glob
import os
import commands
import sys
import re
import platform
from shell_command  import ShellCommand
from madanalysis.enumeration.detect_status_type import DetectStatusType


class DetectZlib:

    def __init__(self,archi_info, user_info, session_info, debug):
        # madatory options
        self.archi_info   = archi_info
        self.user_info    = user_info
        self.session_info = session_info
        self.debug        = debug
        self.name         = 'Zlib'
        self.mandatory    = False
        self.force        = False
        self.logger       = logging.getLogger('MA5')

        # Name of the dynamic lib
        if self.archi_info.isMac:
            self.libnames=['libz.dylib','libz.so','libz.a']
        else:
            self.libnames=['libz.so','libz.a']

        #specific options
        self.header_file  = ''
        self.library_file = ''


    def IsItVetoed(self):
        if self.user_info.zlib_veto:
            self.logger.debug("user setting: veto on Zlib module")
            return True
        else:
            self.logger.debug("no user veto")
            return False


    def ManualDetection(self):

        # User setting for header
        force1=False
        test1=False
        if self.user_info.zlib_includes!=None:

             self.logger.debug("User setting: zlib include path is specified.")
             force1 = True
             test1  = self.LookForHeader(self.user_info.zlib_includes)

        # User setting for header
        force2=False
        test2=False
        if self.user_info.zlib_libs!=None:

             self.logger.debug("User setting: zlib lib path is specified.")
             force2 = True
             test2  = self.LookForLibrary(self.user_info.zlib_libs)

        # Return
        if force1 and force2 and test1 and test2:
            return DetectStatusType.FOUND, ''
        else:
            return DetectStatusType.UNFOUND, ''
        

    def ToolsDetection(self):

        # Check
        pathname = os.path.normpath(self.archi_info.ma5dir+'/tools/zlib')
        self.logger.debug("Look for zlib folder in path "+pathname+" ...")
        if not os.path.isdir(pathname):
            self.logger.debug("-> not found")
            return DetectStatusType.UNFOUND, ''
        else:
            self.logger.debug("-> found")

        # headers
        test1 = self.LookForHeader(pathname+'/include/')

        # libs
        test2 = self.LookForHeader(pathname+'/lib/')
            
        # Return
        if test1 and test2:
            return DetectStatusType.FOUND, ''
        else:
            return DetectStatusType.UNFOUND, ''


    def LookForHeader(self,path):
        filename=os.path.normpath(path+'/zlib.h')
        self.logger.debug("Look for the header file "+filename+" ...")
        if not os.path.isfile(filename):
            self.logger.debug('-> not found')
            logging.warning("Header file called '"+filename+"' not found.")
            return False
        else:
            self.logger.debug('-> found')
            self.header_file=filename
            return True


    def LookForLibrary(self,path):
        self.logger.debug("Look for the libraries in folder "+path+" ...")
        found = False
        for libname in self.libnames:
            filename=os.path.normpath(path+'/'+libname)
            self.logger.debug("Look for the library file "+filename+" ...")
            if not os.path.isfile(filename):
                self.logger.debug('-> not found')
            else:
                self.logger.debug('-> found')
                found = True
                self.library_file=filename
                return True
        return False

                
    def AutoDetection(self):

        # header
        self.logger.debug("Look for the header file zlib.h ...")
        mypath, myfile = self.FindHeader('zlib.h')
        self.archi_info.zlib_inc_path = os.path.normpath(mypath)
        self.logger.debug("-> result for the path: "+str(self.archi_info.zlib_inc_path))
        self.logger.debug("-> result for the file: "+str(os.path.normpath(myfile)))
        if self.archi_info.zlib_inc_path=="":
            self.PrintFAIL(package_name,warning=True)
            self.logger.warning("Header file called 'zlib.h' not found.")
            self.logger.warning("Gzip format will be disabled.")
            self.logger.warning("To enable this format, please type 'install zlib'.")
            return False

        # lib
        self.logger.debug("Look for the zlib libraries ...")
        mypath, myfiles = self.FindLibraryWithPattern2('libz.*',libnames)
        if mypath!='':
            self.archi_info.zlib_lib_path = os.path.normpath(mypath)
            self.archi_info.zlib_lib      = os.path.normpath(myfiles[0])
            self.logger.debug("-> result for lib paths: "+str(self.archi_info.zlib_lib_path))
            self.logger.debug("-> result for lib files: "+str(self.archi_info.zlib_lib))
        if self.archi_info.zlib_lib_path=="":
            self.PrintFAIL(package_name,warning=True)
            self.logger.warning("Library called 'zlib' not found.")
            self.logger.warning("Gzip format will be disabled.")
            self.logger.warning("To enable this format, please type 'install zlib'.")
            return False
        self.archi_info.zlib_original_libs.extend(myfiles)

        return DetectStatusType.FOUND, msg


    def SaveInfo(self):
        # archi_info
        self.archi_info.has_root           = True
        self.archi_info.zlib_priority      = self.force
        self.archi_info.zlib_lib           = self.library_file
        self.archi_info.zlib_inc_path      = os.path.dirname[self.header_file]
        self.archi_info.zlib_lib_path      = os.path.dirname[self.library_file]
        self.archi_info.zlib_original_libs.extend([self.library_file])
        self.archi_info.libraries['ZLib']=self.archi_info.zlib_lib+":"+str(os.stat(self.archi_info.zlib_lib).st_mtime)
        # Ok
        return True


