################################################################################
#  
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://github.com/MadAnalysis/madanalysis5>
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


from __future__ import absolute_import
import logging
import glob
import os
import sys
import re
import platform
from shell_command  import ShellCommand
from madanalysis.enumeration.detect_status_type import DetectStatusType


class DetectONNX:

    def __init__(self, archi_info, user_info, session_info, debug):
        # mandatory options
        self.archi_info   = archi_info
        self.user_info    = user_info
        self.session_info = session_info
        self.debug        = debug
        self.name         = 'ONNXRunTime'
        self.mandatory    = False
        self.force        = False 
        self.log          = []
        self.logger       = logging.getLogger('MA5')
        self.version = "1.17.1"
        if self.archi_info.isMac :
            if self.archi_info.isARM64: 
                self.ver_name = "onnxruntime-osx-arm64-"+self.version
            else : 
                self.ver_name = "onnxruntime-osx-x86_64-"+self.version
            self.lib_name = "libonnxruntime."+self.version+".dylib"
        else : #if not mac is linux 
            self.ver_name = "onnxruntime-linux-x64-"+self.version  
            self.lib_name = "libonnxruntime.so."+self.version       
        # NAme of the header
        self.headernames=['onnxruntime_cxx_api.h']
        
        # Name of the dynamic lib
        self.libnames=[self.lib_name] 

        # adding what you want here


    def PrintDisableMessage(self):
        self.logger.warning("ONNXRunTime is disabled. Cannot use .onnx files.")
        

    def IsItVetoed(self):
        if self.user_info.onnx_veto:
            self.logger.debug("user setting: veto on ONNX")
            return True
        else:
            self.logger.debug("no user veto")
            return False

        
    def ToolsDetection(self):

        # Check
        pathname = os.path.normpath(self.archi_info.ma5dir+'/tools/onnx/'+self.ver_name)
        self.logger.debug("Look for onnx folder in path "+pathname+" ...")
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

    def SaveInfo(self):
        self.session_info.has_onnx   = True
        self.archi_info.has_onnx     = True
        self.archi_info.onnx_priority      = self.force
        self.archi_info.onnx_lib           = self.library_files[0]
        self.archi_info.onnx_inc_path      = os.path.dirname(self.header_files[0])
        self.archi_info.onnx_lib_path      = os.path.dirname(self.library_files[0])
        self.archi_info.onnx_original_libs.extend([fl for fl in self.library_files if not fl in self.archi_info.onnx_original_libs])

        return True


