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

        self.search_libs = []
        self.search_incs = []
        
        self.logger       = logging.getLogger('MA5')

        # NAme of the header
        self.headernames=['zlib.h']
        
        # Name of the dynamic lib
        self.libnames=['libz.so','libz.a','libz.so.*'] #taking into account: libz.so.1, ...
        if self.archi_info.isMac:
            self.libnames.extend(['libz.dylib'])

        #specific options
        self.header_files  = []
        self.library_files = []


    @staticmethod
    def AddIfValid(path,container):
        path=os.path.normpath(path)
        dirs=glob.glob(path)
        for item in dirs:
            if 'tools/SampleAnalyzer/ExternalSymLink/Lib' in item:
                continue
            if item in container:
                continue
            container.append(item)


    def IsItVetoed(self):
        if self.user_info.zlib_veto:
            self.logger.debug("user setting: veto on Zlib module")
            return True
        else:
            self.logger.debug("no user veto")
            return False


    def FillHeaders(self):
        # Filling container with paths included in CPLUS_INCLUDE_PATH
        try:
            cplus_include_path = os.environ['CPLUS_INCLUDE_PATH'].split(':')
            for item in cplus_include_path:
                DetectZlib.AddIfValid(item,self.search_incs)
        except:
            os.environ['CPLUS_INCLUDE_PATH']=''

        # Filling container with standard include paths
        DetectZlib.AddIfValid('/usr/include',self.search_incs)
        DetectZlib.AddIfValid('/usr/local/include',self.search_incs)
        DetectZlib.AddIfValid('/local/include',self.search_incs)
        DetectZlib.AddIfValid('/opt/local/include',self.search_incs)


    def FillLibraries(self):
        # Filling container with paths included in LD_LIBRARY_PATH
        try:
            ld_library_path = os.environ['LD_LIBRARY_PATH'].split(':')
            for item in ld_library_path:
                DetectZlib.AddIfValid(item,self.search_libs)
        except:
            os.environ['LD_LIBRARY_PATH']=''

        # Filling container with paths included in DYLD_LIBRARY_PATH
        try:
            ld_library_path = os.environ['DYLD_LIBRARY_PATH'].split(':')
            for item in ld_library_path:
                DetectZlib.AddIfValid(item,self.search_libs)
        except:
            os.environ['DYLD_LIBRARY_PATH']=''

        # Filling container with paths included in LIBRARY_PATH
        try:
            library_path = os.environ['LIBRARY_PATH'].split(':')
            for item in library_path:
                DetectZlib.AddIfValid(item,self.search_libs)
        except:
            os.environ['LIBRARY_PATH']=''

        # Filling container with standard library paths
        DetectZlib.AddIfValid('/usr/lib*',self.search_libs)
        DetectZlib.AddIfValid('/usr/local/lib*',self.search_libs)
        DetectZlib.AddIfValid('/local/lib*',self.search_libs)
        DetectZlib.AddIfValid('/opt/local/lib*',self.search_libs)
        

    def ManualDetection(self):

        # User setting for header
        force1=False
        test1=False
        result1=[]
        if self.user_info.zlib_includes!=None:

             self.logger.debug("User setting: zlib include path is specified.")
             force1  = True
             result1 = self.LookForPattern(self.user_info.zlib_includes,self.headernames)
             test1   = (len(result1)!=0)

        # User setting for header
        force2=False
        test2=False
        result2=[]
        if self.user_info.zlib_libs!=None:

             self.logger.debug("User setting: zlib lib path is specified.")
             force2  = True
             result2 = self.LookForPattern(self.user_info.zlib_libs,self.libnames)
             test2   = (len(result2)!=0)

        # Return
        if force1 and force2 and test1 and test2:
            self.force = True
            self.header_files  = result1
            self.library_files = result2
            
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
        result1 = self.LookForPattern(pathname+'/include/',self.headernames)
        test1   = (len(result1)!=0)

        # libs
        result2 = self.LookForPattern(pathname+'/lib/',self.libnames)
        test2   = (len(result2)!=0)
            
        # Return
        if test1 and test2:
            self.header_files  = result1
            self.library_files = result2
            return DetectStatusType.FOUND, ''
        else:
            return DetectStatusType.UNFOUND, ''


    def LookForPattern(self,path,patterns):
        result=[]
        for pattern in patterns:
            filename=os.path.normpath(path+'/'+pattern)
            self.logger.debug('look for pattern '+filename+' ...')
            thefiles = glob.glob(filename)
            for thefile in thefiles:
                if thefile not in result:
                    self.logger.debug('-> found: '+thefile)
                    result.append(thefile)
        if len(result)==0:
            self.logger.debug('-> no file found')
        return result

    
    def AutoDetection(self):

        self.logger.debug("Search for header & libraries possible paths...")
        self.FillHeaders()
        self.FillLibraries()
        self.logger.debug("->header  paths="+str(self.search_incs))
        self.logger.debug("->library paths="+str(self.search_libs))
         
        # header
        self.logger.debug("Look for the header file zlib.h ...")
        result1=[]
        for path in self.search_incs:
            result1 = self.LookForPattern(path,self.headernames)
            if len(result1)!=0:
                break
        test1   = (len(result1)!=0)
        if not test1:
#            self.logger.warning("Header file called 'zlib.h' not found.")
#            self.logger.warning("Gzip format will be disabled.")
#            self.logger.warning("To enable this format, please type 'install zlib'.")
            return DetectStatusType.UNFOUND, ""

        # lib
        self.logger.debug("Look for the zlib libraries ...")
        result2=[]
        for path in self.search_libs:
            result2 = self.LookForPattern(path,self.libnames)
            if len(result2)!=0:
                break
        test2   = (len(result2)!=0)
        if not test2:
#            self.logger.warning("Zlib libraries are not found.")
#            self.logger.warning("Gzip format will be disabled.")
#            self.logger.warning("To enable this format, please type 'install zlib'.")
            return DetectStatusType.UNFOUND, ""

        self.header_files  = result1
        self.library_files = result2

        return DetectStatusType.FOUND, ""


    def SaveInfo(self):

        # remove symlink
        library_files2=[]
        for item in self.library_files:
            if os.path.islink(item):
                item=os.readlink(item)
            library_files2.append(item)

        # archi_info
        self.archi_info.has_zlib           = True
        self.archi_info.zlib_priority      = self.force
        self.archi_info.zlib_lib           = self.library_files[0]
        self.archi_info.zlib_inc_path      = os.path.dirname(self.header_files[0])
        self.archi_info.zlib_lib_path      = os.path.dirname(self.library_files[0])
        self.archi_info.zlib_original_libs.extend([fl for fl in self.library_files if not fl in self.archi_info.zlib_original_libs])
        self.archi_info.libraries['ZLib']=self.archi_info.zlib_lib+":"+str(os.stat(self.archi_info.zlib_lib).st_mtime)

        # Ok
        return True


