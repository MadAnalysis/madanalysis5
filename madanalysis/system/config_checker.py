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
from shell_command    import ShellCommand

class ConfigChecker:

    @staticmethod
    def AddIfValid(path,container):
        dirs=glob.glob(path)
        for item in dirs:
            if not (item in container):
                container.append(item)


    def __init__(self, archi_info, user_info, session_info, script=False, debug=False):

        # Getting parameter from the main program
        self.archi_info   = archi_info
        self.user_info    = user_info
        self.session_info = session_info
        self.script       = script

        self.paths = []
        self.fillPaths()

        self.libs = []
        self.fillLibraries()

        self.includes = []
        self.fillHeaders()
        self.debug=debug
        self.logger = logging.getLogger('MA5')

    def FillMA5Path(self):
        os.environ['MA5_BASE']=self.archi_info.ma5dir


    def fillPaths(self):
        # Filling container with paths included in $PATH
        try:
            self.paths = os.environ['PATH'].split(':')
        except:
            os.environ['PATH']=''

    def PrintOK(self,text):
        self.logger.info(text+'\x1b[32m'+'[OK]'+'\x1b[0m')

    def PrintFAIL(self,text,warning=False):
        if warning:
            self.logger.info(text + '\x1b[35m'+'[DISABLED]'+'\x1b[0m')
        else:
            self.logger.info(text + '\x1b[31m'+'[FAILURE]'+'\x1b[0m')

    def PrintDEACTIVATED(self,text):
        self.logger.info(text+'\x1b[33m'+'[DEACTIVATED]'+'\x1b[0m')

    def PrintLibrary(self,text,tab=5,width=25):
        mytab = '%'+str(tab)+'s'
        mytab = mytab % ' '
        mytab += '- '
        mywidth = '%-'+str(width)+'s'
        mywidth = mywidth % text
        return mytab+mywidth


    def fillHeaders(self):
        # Filling container with paths included in CPLUS_INCLUDE_PATH
        try:
            cplus_include_path = os.environ['CPLUS_INCLUDE_PATH'].split(':')
            for item in cplus_include_path:
                ConfigChecker.AddIfValid(item,self.includes)
        except:
            os.environ['CPLUS_INCLUDE_PATH']=''

        # Filling container with standard include paths
        ConfigChecker.AddIfValid('/usr/include',self.includes)
        ConfigChecker.AddIfValid('/usr/local/include',self.includes)
        ConfigChecker.AddIfValid('/local/include',self.includes)
        ConfigChecker.AddIfValid('/opt/local/include',self.includes)


    def fillLibraries(self):
        # Filling container with paths included in LD_LIBRARY_PATH
        try:
            ld_library_path = os.environ['LD_LIBRARY_PATH'].split(':')
            for item in ld_library_path:
                ConfigChecker.AddIfValid(item,self.libs)
        except:
            os.environ['LD_LIBRARY_PATH']=''

        # Filling container with paths included in DYLD_LIBRARY_PATH
        try:
            ld_library_path = os.environ['DYLD_LIBRARY_PATH'].split(':')
            for item in ld_library_path:
                ConfigChecker.AddIfValid(item,self.libs)
        except:
            os.environ['DYLD_LIBRARY_PATH']=''

        # Filling container with paths included in LIBRARY_PATH
        try:
            library_path = os.environ['LIBRARY_PATH'].split(':')
            for item in library_path:
                ConfigChecker.AddIfValid(item,self.libs)
        except:
            os.environ['LIBRARY_PATH']=''

        # Filling container with standard library paths
        ConfigChecker.AddIfValid('/usr/lib*',self.libs)
        ConfigChecker.AddIfValid('/usr/local/lib*',self.libs)
        ConfigChecker.AddIfValid('/local/lib*',self.libs)
        ConfigChecker.AddIfValid('/opt/local/lib*',self.libs)


    def FindLibraryWithPattern(self,pattern,files):
        return self.FindFilesWithPattern(self.libs,pattern,files)

    def FindLibraryWithPattern2(self,pattern,files):
        return self.FindFilesWithPattern2(self.libs,pattern,files)

    def FindHeader(self,file):
        return self.FindFilesWithPattern(self.includes,file,[file])

    def FindHeader2(self,file):
        return self.FindFilesWithPattern2(self.includes,file,[file])

    def FindFilesWithPattern(self,paths,pattern,targets):
        result_files=[]
        result_paths=[]
        for path in paths:
            path=os.path.normpath(path)
            if 'tools/SampleAnalyzer/ExternalSymLink' in path:
                continue
            rawfiles=glob.glob(path+"/"+pattern)

            filtered_files=[]
            for file in rawfiles:
                for target in targets:
                    if file.endswith('/'+target):
                        filtered_files.append(file)

            if len(filtered_files)!=0:
                result_files.extend(filtered_files)
            for item in filtered_files:
                result_paths.append(path)

        if len(result_files)==0:
            return "", ""
        else:
            return os.path.normpath(result_paths[0]), os.path.normpath(result_files[0])

    def FindFilesWithPattern2(self,paths,pattern,targets):
        result_files=[]
        result_paths=[]
        for path in paths:
            path=os.path.normpath(path)
            if 'tools/SampleAnalyzer/ExternalSymLink' in path:
                continue
            rawfiles=glob.glob(path+"/"+pattern)

            filtered_files=[]
            for file in rawfiles:
                for target in targets:
                    if file.endswith('/'+target):
                        filtered_files.append(file)

            if len(filtered_files)!=0:
                result_files.extend(filtered_files)
            for item in filtered_files:
                result_paths.append(path)

        if len(result_files)==0:
            return "", []
        else:
            files_to_send = []
            folder=os.path.normpath(result_paths[0])
            for item in result_files:
                item=os.path.normpath(item)
                if folder in item:
                    files_to_send.append(item)
            return folder, files_to_send





    def checkZLIB(self):

        # Checking if zlib is present
        package_name = self.PrintLibrary("Zlib")
        self.logger.debug("zlib")

        # Name of the dynamic lib
        libnames=['libz.so','libz.a']
        if self.archi_info.isMac:
            libnames.append('libz.dylib')

        # User veto
        if self.user_info.zlib_veto:
            self.logger.debug("User setting: veto on zlib module")
            self.PrintFAIL(package_name,warning=True)
            self.logger.warning("Library called 'zlib' disabled. Gzip format will be disabled.")
            return False

        # Does the user force the paths?
        force1=False
        force2=False
        if self.user_info.zlib_includes!=None:
            self.logger.debug("User setting: zlib include path is specified: "+ self.user_info.zlib_includes)
            self.archi_info.zlib_inc_path=os.path.normpath(self.user_info.zlib_includes)
            force1=True
        if self.user_info.zlib_libs!=None:
            self.logger.debug("User setting: zlib lib path is specified: "+ self.user_info.zlib_libs)
            self.archi_info.zlib_lib_path=os.path.normpath(self.user_info.zlib_libs)
            force2=True
        force=force1 and force2

        # Checking if zlib has been installed by MA5
        ma5installation = False
        if not force:
            pathname = os.path.normpath(self.archi_info.ma5dir+'/tools/zlib')
            self.logger.debug("Look for zlib in the folder "+pathname+" ...")
            if os.path.isdir(pathname):
                self.archi_info.zlib_inc_path=os.path.normpath(pathname+'/include/')
                self.archi_info.zlib_lib_path=os.path.normpath(pathname+'/lib/')
                self.logger.debug("-> found")
                ma5installation = True
            else:
                self.logger.debug("-> not found")

        # Check if the libraries and headers are available
        if force or ma5installation:

            # header
            filename=os.path.normpath(self.archi_info.zlib_inc_path+'/zlib.h')
            self.logger.debug("Look for the file "+filename+" ...")
            if not os.path.isfile(filename):
                self.logger.debug('-> not found')
                self.PrintFAIL(package_name,warning=True)
                self.logger.warning("Header file called '"+filename+"' not found.")
                self.logger.warning("Gzip format will be disabled.")
                self.logger.warning("To enable this format, please type 'install zlib'.")
                return False
            else:
                self.logger.debug('-> found')

            # lib
            self.logger.debug("Look for the libraries in folder "+self.archi_info.zlib_lib_path+" ...")
            mypath, myfiles = self.FindFilesWithPattern2([self.archi_info.zlib_lib_path],"libz.*",libnames)
            self.archi_info.zlib_lib=os.path.normpath(myfiles[0])
            self.logger.debug("-> result: "+str(self.archi_info.zlib_lib))
            if self.archi_info.zlib_lib=="":
                self.PrintFAIL(package_name,warning=True)
                self.logger.warning("Zlib library not found in "+self.archi_info.zlib_lib_path+" folder.")
                self.logger.warning("Gzip format will be disabled.")
                self.logger.warning("To enable this format, please type 'install zlib'.")
                return False
            self.archi_info.zlib_original_libs.extend([fl for fl in myfiles if not fl in self.archi_info.zlib_original_libs])


        # Checking zlib can be found in other folders
        if not force and not ma5installation:

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
            self.archi_info.zlib_original_libs.extend([fl for fl in myfiles if not fl in self.archi_info.zlib_original_libs])

        self.archi_info.libraries['ZLib']=self.archi_info.zlib_lib+":"+str(os.stat(self.archi_info.zlib_lib).st_mtime)
        self.archi_info.zlib_priority=(force or ma5installation)

        # Ok
        self.PrintOK(package_name)
        return True


    def checkDelphes(self, getpaths=False):
        # Checking if Delphes is present
        if not getpaths:
            package_name = self.PrintLibrary("Delphes")
            self.logger.debug("Delphes")

        # User veto
        if self.user_info.delphes_veto:
            if not getpaths:
                self.logger.debug("User setting: veto on Delphes")
                self.PrintFAIL(package_name,warning=True)
            return False

        if not self.archi_info.has_root:
            if not getpaths:
                self.logger.debug("This package needs ROOT")
                self.PrintFAIL(package_name,warning=True)
            return False

        # Name of the dynamic lib
        libnames=['libDelphes.so','libDelphes.a']
        if self.archi_info.isMac and 'libDelphes.dylib' not in libnames:
            libnames.append('libDelphes.dylib')

        # Does the user force the paths?
        force1=False
        force2=False
        if self.user_info.delphes_includes!=None:
            if not getpaths:
                self.logger.debug("User setting: Delphes include path is specified.")
                if not self.user_info.delphes_includes in self.archi_info.delphes_inc_paths:
                    self.archi_info.delphes_inc_paths.append(self.user_info.delphes_includes)
                if not self.user_info.delphes_includes+'/external' in self.archi_info.delphes_inc_paths:
                    self.archi_info.delphes_inc_paths.append(self.user_info.delphes_includes+'/external/')
            else:
                delpath=os.path.normpath(self.user_info.delphes_includes)
                deldeac = delpath.replace(delpath.split('/')[-1],"DEACT_"+delpath.split('/')[-1])
                if os.path.isdir(deldeac):
                    self.archi_info.delphes_inc_paths.insert(0,deldeac+'/external/')
                    self.archi_info.delphes_inc_paths.insert(0,deldeac)
                    self.includes.append(deldeac)
                    self.includes.append(deldeac+'/external/')
                else:
                    if not self.user_info.delphes_includes in self.archi_info.delphes_inc_paths:
                        self.archi_info.delphes_inc_paths.append(self.user_info.delphes_includes)
                if not self.user_info.delphes_includes+'/external' in self.archi_info.delphes_inc_paths:
                        self.archi_info.delphes_inc_paths.append(self.user_info.delphes_includes+'/external/')
            force1=True
        if self.user_info.delphes_libs!=None:
            if not getpaths:
                self.logger.debug("User setting: Delphes lib path is specified.")
                if not os.path.normpath(self.user_info.delphes_libs) in self.archi_info.delphes_lib_paths:
                    self.archi_info.delphes_lib_paths.append(os.path.normpath(self.user_info.delphes_libs))
            else:
                delpath=os.path.normpath(self.user_info.delphes_libs)
                deldeac = delpath.replace(delpath.split('/')[-1],"DEACT_"+delpath.split('/')[-1])
                if os.path.isdir(deldeac):
                    self.archi_info.delphes_lib_paths.insert(0,deldeac)
                    self.libs.append(deldeac)
                else:
                    if not os.path.normpath(self.user_info.delphes_libs) in self.archi_info.delphes_lib_paths:
                        self.archi_info.delphes_lib_paths.append(os.path.normpath(self.user_info.delphes_libs))
            force2=True
        force=force1 and force2

        # Checking if Delphes has been installed by MA5
        ma5installation = False
        if not force:
            if not getpaths:
                self.logger.debug("Look for Delphes in the folder "+self.archi_info.ma5dir+"/tools ...")
            if os.path.isdir(self.archi_info.ma5dir+'/tools/delphes') and \
               os.path.isdir(self.archi_info.ma5dir+'/tools/delphes/external'):
                dpath =  os.path.normpath(os.path.join(self.archi_info.ma5dir,'tools','delphes'))
                if not dpath in self.archi_info.delphes_inc_paths:
                    self.archi_info.delphes_inc_paths.append(dpath)
                if not dpath in self.archi_info.delphes_lib_paths:
                    self.archi_info.delphes_lib_paths.append(dpath)
                dpath=os.path.normpath(os.path.join(self.archi_info.ma5dir,'tools','delphes','external'))
                if not dpath in self.archi_info.delphes_inc_paths:
                    self.archi_info.delphes_inc_paths.append(dpath)
                if not getpaths:
                    self.logger.debug("-> found")
                ma5installation = True
            elif os.path.isdir(self.archi_info.ma5dir+'/tools/DEACT_delphes') and \
               os.path.isdir(self.archi_info.ma5dir+'/tools/DEACT_delphes/external') and getpaths:
                self.archi_info.delphes_inc_paths.insert(0,self.archi_info.ma5dir+'/tools/DEACT_delphes/external')
                self.archi_info.delphes_inc_paths.insert(0,self.archi_info.ma5dir+'/tools/DEACT_delphes')
                self.archi_info.delphes_lib_paths.insert(0,self.archi_info.ma5dir+'/tools/DEACT_delphes')
                self.includes.append(self.archi_info.ma5dir+'/tools/DEACT_delphes')
                self.includes.append(self.archi_info.ma5dir+'/tools/DEACT_delphes/external')
                self.libs.append(self.archi_info.ma5dir+'/tools/DEACT_delphes')
                ma5installation = True
            else:
                if not getpaths:
                    if os.path.isdir(self.archi_info.ma5dir+'/tools/DEACT_delphes') and \
                        os.path.isdir(self.archi_info.ma5dir+'/tools/DEACT_delphes/external'):
                        self.logger.debug("-> deactivated")
                        self.PrintDEACTIVATED(package_name)
                    else:
                        self.logger.debug("-> not found")
                        self.PrintFAIL(package_name,warning=True)
                return False

            if len(self.archi_info.delphes_inc_paths)>2:
              del self.archi_info.delphes_inc_paths[-1]
              del self.archi_info.delphes_inc_paths[-1]

        # Check if the libraries and headers are available
        if force or ma5installation:
            # header
            filename = os.path.normpath(self.archi_info.delphes_inc_paths[0]+'/modules/ParticlePropagator.h')
            if not getpaths:
                self.logger.debug("Look for the file "+filename+" ...")
            if not os.path.isfile(filename):
                if not getpaths:
                    self.logger.debug("-> not found")
                    self.PrintFAIL(package_name,warning=True)
                return False
            else:
                if not getpaths:
                    self.logger.debug("-> found")

            # lib
            if not getpaths:
                self.logger.debug("Look for the libraries in folder "+str(self.archi_info.delphes_lib_paths)+" ...")
            mypath, myfiles = self.FindFilesWithPattern2(self.archi_info.delphes_lib_paths,"libDelphes.*",libnames)
            self.archi_info.delphes_lib=os.path.normpath(myfiles[0])
            if not getpaths:
                self.logger.debug("-> result: "+str(self.archi_info.delphes_lib))
            if self.archi_info.delphes_lib=="":
                if not getpaths:
                    self.PrintFAIL(package_name,warning=True)
#                    self.logger.warning("Delphes library not found in "+self.archi_info.delphes_lib_paths[0]+" folder.")
#                    self.logger.warning("Delphes ROOT format will be disabled.")
#                    self.logger.warning("To enable this format, please type 'install delphes'.")
                return False
            self.archi_info.delphes_original_libs.extend([fl for fl in myfiles if not fl in self.archi_info.delphes_original_libs])
            delphes_dict=glob.glob(os.path.dirname(self.archi_info.delphes_lib)+'/*.pcm')
            self.archi_info.delphes_original_libs.extend([fl for fl in delphes_dict if not fl in self.archi_info.delphes_original_libs])

        # Checking Delphes can be found in other folders
        if not force and not ma5installation:
            # header
            if not getpaths:
                self.logger.debug("Look for the header file /modules/ParticlePropagator.h ...")
            mypath, myfile = self.FindHeader('/modules/ParticlePropagator.h')
            if mypath!='' and myfile!='':
                if not os.path.normpath(mypath) in self.archi_info.delphes_inc_paths:
                    self.archi_info.delphes_inc_paths.append(os.path.normpath(mypath))
                if not os.path.normpath(mypath+'/external') in self.archi_info.delphes_inc_paths:
                    self.archi_info.delphes_inc_paths.append(os.path.normpath(mypath+'/external'))
                if not getpaths:
                    self.logger.debug("-> result for the path: "+str(self.archi_info.delphes_inc_paths))
                    self.logger.debug("-> result for the file: "+str(os.path.normpath(myfile)))
            if len(self.archi_info.delphes_inc_paths)==0:
                if not getpaths:
                    self.PrintFAIL(package_name,warning=True)
#                    self.logger.warning("Header file called '/modules/ParticlePropagator.h' not found.")
#                    self.logger.warning("Delphes ROOT format will be disabled.")
#                    self.logger.warning("To enable this format, please type 'install delphes'.")
                return False

            # lib
            if not getpaths:
                self.logger.debug("Look for the Delphes libraries ...")
            mypath, myfiles = self.FindLibraryWithPattern2('libDelphes.*',libnames)
            if mypath!='':
                if not os.path.normpath(mypath) in self.archi_info.delphes_lib_paths:
                    self.archi_info.delphes_lib_paths.append(os.path.normpath(mypath))
                self.archi_info.delphes_lib      = os.path.normpath(myfiles[0])
                if not getpaths:
                    self.logger.debug("-> result for lib paths: "+str(self.archi_info.delphes_lib_paths))
                    self.logger.debug("-> result for lib files: "+str(self.archi_info.delphes_lib))
            if len(self.archi_info.delphes_lib_paths)==0:
                if not getpaths:
                    self.PrintFAIL(package_name,warning=True)
                    self.logger.warning("Delphes library not found.")
                    self.logger.warning("Delphes format will be disabled.")
                    self.logger.warning("To enable this format, please type 'install delphes'.")
                return False
            self.archi_info.delphes_original_libs.extend([fl for fl in myfiles if not fl in self.archi_info.delphes_original_libs])
            self.archi_info.delphes_original_libs.extend([fl for fl in myfiles if not fl in self.archi_info.delphes_original_libs])
            delphes_dict=glob.glob(os.path.dirname(self.archi_info.delphes_lib)+'/*.pcm')
            self.archi_info.delphes_original_libs.extend([fl for fl in delphes_dict if not fl in self.archi_info.delphes_original_libs])
            if getpaths:
               self.libs=self.libs[:-1]

        self.archi_info.libraries['Delphes']=self.archi_info.delphes_lib+":"+str(os.stat(self.archi_info.delphes_lib).st_mtime)
        self.archi_info.delphes_priority=(force or ma5installation)

        # Lib

        # Ok
        if not getpaths:
            self.PrintOK(package_name)
        self.includes=[x for x in self.includes if not x.startswith('DEACT')]
        self.libs=[x for x in self.libs if not x.startswith('DEACT')]
        return True


    def checkDelphesMA5tune(self,getpaths=False):
        # Checking if Delphes-MA5tune is present
        package_name = self.PrintLibrary("Delphes-MA5tune")
        if not getpaths:
            self.logger.debug("Delphes-MA5tune")

        # User veto
        if self.user_info.delphesMA5tune_veto:
            if not getpaths:
                self.logger.debug("User setting: veto on Delphes-MA5tune")
                self.PrintFAIL(package_name,warning=True)
            return False

        if not self.archi_info.has_root:
            if not getpaths:
                self.logger.debug("This package needs ROOT")
                self.PrintFAIL(package_name,warning=True)
            return False

        # Name of the dynamic lib
        libnames=['libDelphesMA5tune.so','libDelphesMA5tune.a']
        if self.archi_info.isMac and 'libDelphesMA5tune.dylib' not in libnames:
            libnames.append('libDelphesMA5tune.dylib')

        # Does the user force the paths?
        force1=False
        force2=False
        if self.user_info.delphesMA5tune_includes!=None:
            if not getpaths:
                self.logger.debug("User setting: Delphes-MA5tune include path is specified.")
                if not self.user_info.delphesMA5tune_includes in self.archi_info.delphesMA5tune_inc_paths:
                    self.archi_info.delphesMA5tune_inc_paths.append(self.user_info.delphesMA5tune_includes)
                if not self.user_info.delphesMA5tune_includes+'/external' in self.archi_info.delphesMA5tune_inc_paths:
                    self.archi_info.delphesMA5tune_inc_paths.append(self.user_info.delphesMA5tune_includes+'/external/')
            else:
                delpath=os.path.normpath(self.user_info.delphesMA5tune_includes)
                deldeac = delpath.replace(delpath.split('/')[-1],"DEACT_"+delpath.split('/')[-1])
                if os.path.isdir(deldeac):
                    self.archi_info.delphesMA5tune_inc_paths.insert(0,deldeac+'/external/')
                    self.archi_info.delphesMA5tune_inc_paths.insert(0,deldeac)
                    self.includes.append(deldeac)
                    self.includes.append(deldeac+'/external/')
                else:
                    if not self.user_info.delphesMA5tune_includes in self.archi_info.delphesMA5tune_inc_paths:
                        self.archi_info.delphesMA5tune_inc_paths.append(self.user_info.delphesMA5tune_includes)
                    if not self.user_info.delphesMA5tune_includes+'/external' in self.archi_info.delphesMA5tune_inc_paths:
                        self.archi_info.delphesMA5tune_inc_paths.append(self.user_info.delphesMA5tune_includes+'/external/')
            force1=True
        if self.user_info.delphesMA5tune_libs!=None:
            if not getpaths:
                self.logger.debug("User setting: Delphes-MA5tune lib path is specified.")
                if not os.path.normpath(self.user_info.delphesMA5tune_libs) in self.archi_info.delphesMA5tune_lib_paths:
                    self.archi_info.delphesMA5tune_lib_paths.append(os.path.normpath(self.user_info.delphesMA5tune_libs))
            else:
                delpath=os.path.normpath(self.user_info.delphesMA5tune_libs)
                deldeac = delpath.replace(delpath.split('/')[-1],"DEACT_"+delpath.split('/')[-1])
                if os.path.isdir(deldeac):
                    self.archi_info.delphesMA5tune_lib_paths.insert(0,deldeac)
                    self.libs.append(deldeac)
                else:
                    if not os.path.normpath(self.user_info.delphesMA5tune_libs) in self.archi_info.delphesMA5tune_lib_paths:
                        self.archi_info.delphesMA5tune_lib_paths.append(os.path.normpath(self.user_info.delphesMA5tune_libs))
            force2=True
        force=force1 and force2

        # Checking if Delphes-MA5tune has been installed by MA5
        ma5installation = False
        if not force:
            if not getpaths:
                self.logger.debug("Look for DelphesMA5tune in the folder "+self.archi_info.ma5dir+"/tools...")
            if os.path.isdir(self.archi_info.ma5dir+'/tools/delphesMA5tune') and \
               os.path.isdir(self.archi_info.ma5dir+'/tools/delphesMA5tune/external'):
                dpath =  os.path.normpath(os.path.join(self.archi_info.ma5dir,'tools','delphesMA5tune'))
                if not dpath in self.archi_info.delphesMA5tune_inc_paths:
                    self.archi_info.delphesMA5tune_inc_paths.append(dpath)
                if not dpath in self.archi_info.delphesMA5tune_lib_paths:
                    self.archi_info.delphesMA5tune_lib_paths.append(dpath)
                dpath=os.path.normpath(os.path.join(self.archi_info.ma5dir,'tools','delphesMA5tune','external'))
                if not dpath in self.archi_info.delphesMA5tune_inc_paths:
                    self.archi_info.delphesMA5tune_inc_paths.append(dpath)
                if not getpaths:
                    self.logger.debug("-> found")
                ma5installation = True
            elif os.path.isdir(self.archi_info.ma5dir+'/tools/DEACT_delphesMA5tune') and \
               os.path.isdir(self.archi_info.ma5dir+'/tools/DEACT_delphesMA5tune/external') and getpaths:
                self.archi_info.delphesMA5tune_inc_paths.insert(0,self.archi_info.ma5dir+'/tools/DEACT_delphesMA5tune/external')
                self.archi_info.delphesMA5tune_inc_paths.insert(0,self.archi_info.ma5dir+'/tools/DEACT_delphesMA5tune')
                self.archi_info.delphesMA5tune_lib_paths.insert(0,self.archi_info.ma5dir+'/tools/DEACT_delphesMA5tune')
                self.includes.append(self.archi_info.ma5dir+'/tools/DEACT_delphesMA5tune')
                self.includes.append(self.archi_info.ma5dir+'/tools/DEACT_delphesMA5tune/external')
                self.libs.append(self.archi_info.ma5dir+'/tools/DEACT_delphesMA5tune')
                ma5installation = True
            else:
                if not getpaths:
                    if os.path.isdir(self.archi_info.ma5dir+'/tools/DEACT_delphesMA5tune') and \
                        os.path.isdir(self.archi_info.ma5dir+'/tools/DEACT_delphesMA5tune/external'):
                        self.logger.debug("-> deactivated")
                        self.PrintDEACTIVATED(package_name)
                    else:
                        self.logger.debug("-> not found")
                        self.PrintFAIL(package_name,warning=True)
                return False

            if len(self.archi_info.delphesMA5tune_inc_paths)>2:
              del self.archi_info.delphesMA5tune_inc_paths[-1]
              del self.archi_info.delphesMA5tune_inc_paths[-1]

        # Check if the libraries and headers are available
        if force or ma5installation:
            # header
            filename = os.path.normpath(self.archi_info.delphesMA5tune_inc_paths[0]+'/modules/ParticlePropagator.h')
            if not getpaths:
                self.logger.debug("Look for the file "+filename+" ...")
            if not os.path.isfile(filename):
                if not getpaths:
                    self.logger.debug("-> not found")
                    self.PrintFAIL(package_name,warning=True)
                return False
            else:
                if not getpaths:
                    self.logger.debug("-> found")

            # lib
            if not getpaths:
                self.logger.debug("Look for the libraries in folder "+str(self.archi_info.delphesMA5tune_lib_paths)+" ...")
            mypath, myfiles = self.FindFilesWithPattern2(self.archi_info.delphesMA5tune_lib_paths,"libDelphesMA5tune.*",libnames)
            if not os.path.normpath(mypath) in self.archi_info.delphesMA5tune_lib_paths:
                self.archi_info.delphesMA5tune_lib_paths.append(os.path.normpath(mypath))
            self.archi_info.delphesMA5tune_lib      = os.path.normpath(myfiles[0])
            self.archi_info.delphesMA5tune_original_libs.extend([fl for fl in myfiles if not fl in self.archi_info.delphesMA5tune_original_libs])
            delphesMA5tune_dict=glob.glob(os.path.dirname(self.archi_info.delphesMA5tune_lib)+'/*.pcm')
            self.archi_info.delphesMA5tune_original_libs.extend([fl for fl in delphesMA5tune_dict if not fl in self.archi_info.delphesMA5tune_original_libs])
            if not getpaths:
                self.logger.debug("-> result for lib paths: "+str(self.archi_info.delphesMA5tune_lib_paths))
                self.logger.debug("-> result for lib files: "+str(self.archi_info.delphesMA5tune_lib))
            if self.archi_info.delphesMA5tune_lib=="":
                if not getpaths:
                    self.PrintFAIL(package_name,warning=True)
#                    self.logger.warning("Delphes-MA5tune library not found in "+\
#                      self.archi_info.delphesMA5tune_lib_paths[0]+" folder.")
#                    self.logger.warning("Delphes-MA5tune ROOT format will be disabled.")
#                    self.logger.warning("To enable this format, please type 'install delphesMA5tune'.")
                return False
            
        self.archi_info.libraries['DelphesMA5tune']=self.archi_info.delphesMA5tune_lib+":"+str(os.stat(self.archi_info.delphesMA5tune_lib).st_mtime)
        self.archi_info.delphesMA5tune_priority=(force or ma5installation)

        # Lib

        # Ok
        if not getpaths:
            self.PrintOK(package_name)
        self.includes=[x for x in self.includes if not x.startswith('DEACT')]
        self.libs=[x for x in self.libs if not x.startswith('DEACT')]
        return True

    def checkPAD(self):
        return os.path.isdir(os.path.join(self.archi_info.ma5dir,'tools','PAD'))

    def checkPADForMA5tune(self):
        return os.path.isdir(os.path.join(self.archi_info.ma5dir,'tools','PADForMA5tune'))



