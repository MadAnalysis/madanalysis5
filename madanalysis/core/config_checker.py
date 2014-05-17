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

class ConfigChecker:

    @staticmethod
    def AddIfValid(path,container):
        dirs=glob.glob(path)
        for item in dirs:
            if not (item in container):
                container.append(item)
        

    def __init__(self,configLinux,ma5dir,script=False,isMAC=False):

        # Getting parameter from the main program
        self.isMAC=isMAC
        self.configLinux=configLinux
        self.ma5dir=ma5dir
        self.script=script

        self.paths = []
        self.fillPaths()

        self.libs = []
        self.fillLibraries()

        self.includes = []
        self.fillHeaders()


    def ReadUserOptions(self):

        # Open the user options
        filename = self.ma5dir+'/madanalysis/input/installation_options.dat'
        try:
            input = open(filename)
        except:
            logging.error('impossible to open the file: '+filename)
            return False

        # Loop over the file
        for rawline in input:

            if '#' in rawline:
                line = rawline.split('#')[0]
            if line=='':
                continue
            words=line.split('=')
            if len(words)!=2:
                logging.warning(filename+': the following line is incorrect and is skipped:')
                logging.warning(line)
            words[0]=words[0].lstrip()
            words[0]=words[0].rstrip()
            words[1]=words[1].lstrip()
            words[1]=words[1].rstrip()

            if words[0]=='root_includes':
                self.configLinux.useroptions.root_includes==words[1]
            elif words[0]=='root_libs':
                self.configLinux.useroptions.root_libs=words[1]
            elif words[0]=='delphes_veto':
                self.configLinux.useroptions.delphes_veto=words[1]
            elif words[0]=='delphes_includes':
                self.configLinux.useroptions.delphes_includes=words[1]
            elif words[0]=='delphes_libs':
                self.configLinux.useroptions.delphes_libs=words[1]
            elif words[0]=='delfes_veto':
                self.configLinux.useroptions.delfes_veto=words[1]
            elif words[0]=='delfes_includes':
                self.configLinux.useroptions.delfes_includes=words[1]
            elif words[0]=='delfes_libs':
                self.configLinux.useroptions.delfes_libs=words[1]
            elif words[0]=='zlib_veto':
                self.configLinux.useroptions.zlib_veto=words[1]
            elif words[0]=='zlib_includes':
                self.configLinux.useroptions.zlib_includes=words[1]
            elif words[0]=='zlib_libs':
                self.configLinux.useroptions.zlib_libs=words[1]
            elif words[0]=='fastjet_veto':
                self.configLinux.useroptions.fastjet_veto=words[1]
            elif words[0]=='fastjet_bin_path':
                self.configLinux.useroptions.fastjet_bin_path=words[1]
            elif words[0]=='pdflatex_veto':
                self.configLinux.useroptions.pdflatex_veto=words[1]
            elif words[0]=='latex_veto':
                self.configLinux.useroptions.latex_veto=words[1]
            elif words[0]=='dvipdf_veto':
                self.configLinux.useroptions.dvipdf_veto=words[1]
            else:
                logging.warning(filename+': the options called "'+words[0]+'" is not found')

        # Close the file
        input.close()
        
        # Ok
        return True
    

    def FillMA5Path(self):
        os.environ['MA5_BASE']=self.ma5dir


    def fillPaths(self):
        # Filling container with paths included in $PATH
        try:
            self.paths = os.environ['PATH'].split(':')
        except:
            os.environ['PATH']=''

    def PrintOK(self):
        sys.stdout.write('\x1b[32m'+'[OK]'+'\x1b[0m'+'\n')
        sys.stdout.flush()

    def PrintFAIL(self,warning=False):
        if warning:
            sys.stdout.write('\x1b[35m'+'[DISABLED]'+'\x1b[0m'+'\n')
        else:
            sys.stdout.write('\x1b[31m'+'[FAILURE]'+'\x1b[0m'+'\n')
        sys.stdout.flush()    

    def PrintLibrary(self,text,tab=5,width=25):
        mytab = '%'+str(tab)+'s'
        mytab = mytab % ' '
        mytab += '- '
        mywidth = '%-'+str(width)+'s'
        mywidth = mywidth % text
        sys.stdout.write(mytab+mywidth)
        sys.stdout.flush()
                
        
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


    def checkTextEditor(self):
        try:
            self.configLinux.editor = os.environ['EDITOR']
        except KeyError:
            self.configLinux.editor = 'vi'

    def checkROOT(self):
        # Checking if ROOT is present
        self.PrintLibrary('Root')

        # Does the user force the ROOT path
        force1=False
        force2=False
        if self.configLinux.useroptions.root_includes!='0':
            self.configLinux.root_inc_path=self.configLinux.useroptions.root_includes
            force1=True
        if self.configLinux.useroptions.root_libs!='0':
            self.configLinux.root_lib_path=self.configLinux.useroptions.root_libs
            force2=True
        force=force1 and force2

        # Trying to call root-config
        if not force:

            # Using root-config
            rootdirs = commands.getstatusoutput('root-config --libdir --incdir')
            if rootdirs[0]>0:
                self.PrintFAIL(warning=False)
                logging.error('ROOT module called "root-config" is not detected.\n'\
		              +'Two explanations :n'\
		              +' - ROOT is not installed. You can download it '\
		              +'from http://root.cern.ch\n'\
		              +' - ROOT binary folder must be placed in the '\
                              +'global environment variable $PATH')
                return False

            # Extracting ROOT library and header path
            root_tmp = rootdirs[1].split()
            self.configLinux.root_inc_path = root_tmp[1]
            self.configLinux.root_lib_path = root_tmp[0]

        # Adding ROOT library path to Python path
        sys.path.append(self.configLinux.root_lib_path)

        # Check: looking for files
        FilesToFind=[self.configLinux.root_lib_path+'/libPyROOT.so', \
                     self.configLinux.root_inc_path+'/TH1F.h']
        for file in FilesToFind:
            if os.path.isfile(file):
                self.configLinux.libraries[file.split('/')[-1]]=file+":"+str(os.stat(file).st_mtime)
            else:
                self.PrintFAIL(warning=False)
	        logging.error("ROOT file called '"+file+"' is not found")
                logging.error("Please check that ROOT is properly installed.")
                return False
            
        # Check: looking for files
        FilesToFind=[self.configLinux.root_lib_path+'/ROOT.py', \
                     self.configLinux.root_inc_path+'/ROOT.pyc']
        found=False
        for file in FilesToFind:
            if os.path.isfile(file):
                self.configLinux.libraries[file.split('/')[-1]]=file+":"+str(os.stat(file).st_mtime)
                found=True
                break
        if not found:
            self.PrintFAIL(warning=False)
            logging.error("ROOT file called 'ROOT.py' or 'ROOT.pyc' is not found")
            logging.error("Please check that ROOT is properly installed.")
            return False

        # Root Install
        self.PrintOK()
        return True


    def checkPyROOT(self):
    
        # Loading ROOT library
        self.PrintLibrary("PyRoot libraries")
        try :
	    from ROOT import gROOT
        except:
            self.PrintFAIL(warning=False)
            logging.error("'root-config --libdir' indicates a wrong path for ROOT"\
	                  +" libraries. Please specify the ROOT library path"\
		          +" into the environnement variable $PYTHONPATH")
            return False

        # Setting ROOT batch mode
        if not self.script:
            from ROOT import TApplication
            from ROOT import gApplication
            TApplication.NeedGraphicsLibs()
            gApplication.InitializeGraphics()
        gROOT.SetBatch(True)
        

        # Checking ROOT release
        RootVersion = str(gROOT.GetVersionInt())
        if len(RootVersion)<3:
            self.PrintFAIL(warning=False)
	    logging.error('Bad release of ROOT : '+gROOT.GetVersion()+\
                          '. MadAnalysis5 needs ROOT 5.27 or higher.\n Please upgrade your version of ROOT.')
            return False

        RootVersionA = int(RootVersion[0])
        RootVersionB = int(RootVersion[1]+RootVersion[2])
        if RootVersionA!=5 or RootVersionB<27:
            self.PrintFAIL(warning=False)
	    logging.error('Bad release of ROOT : '+gROOT.GetVersion()+\
                          '. MadAnalysis5 needs ROOT 5.27 or higher.\n Please upgrade your version of ROOT.')
            return False

        self.configLinux.root_version   = RootVersion
        self.PrintOK()
        return True


    def checkGPP(self):
        # Checking g++ release
        self.PrintLibrary('g++')
        gcc_version = commands.getstatusoutput('g++ -dumpversion')
        if gcc_version[0]>0:
            self.PrintFAIL(warning=False)
            logging.error('g++ compiler not found. Please install it before ' + \
	             'using MadAnalysis 5')
            return False
        else:
            self.PrintOK()
            self.configLinux.gcc_version = gcc_version[1]
            return True


    def checkMake(self):
        # Checking GNU Make
        self.PrintLibrary('GNU Make')
        make_version = commands.getstatusoutput('make --version')
        if make_version[0]>0:
            self.PrintFAIL(warning=False)
            logging.error('GNU Make not found. Please install it before ' + \
	             'using MadAnalysis 5')
            return False
        else:
            self.PrintOK()
            lines=make_version[1].split('\n')
            if len(lines)==0:
                logging.error('command "make --version" seems to not give the GNU Make version')
                return False
            line=lines[0]
            line=line.replace(' ','')
            line=line.lstrip()
            line=line.rstrip()
            self.configLinux.make_version = line
            return True


    def checkGF(self):
        self.PrintLibrary("gfortran")
        gfortran_version = commands.getstatusoutput('gfortran -dumpversion')
        gfortran_version = gfortran_version[1].split('\n')[0]
        gfor = (re.search(r'((\d.)(\d.)\d)',gfortran_version))
        if gfor:
            gfortran_version = gfor.group(1)
        else:
             gfor = (re.search(r'((\d.)\d)',gfortran_version))
             if gfor:
                 gfortran_version = gfor.group(1)
             else:
                 self.PrintFAIL(warning=True)
                 logging.warning('gfortran compiler not found.')
                 return True
        ver = gfortran_version.split('.')
        if (int(ver[0])<4) or (int(ver[0])==4 and int(ver[1])<4):
            self.PrintFAIL(warning=True)
            logging.warning('gfortran ' + gfortran_version + ' older than 4.4.0.')
            return True
            
        self.configLinux.gfortran_version = gfortran_version
        self.PrintOK()
        return True


    def FindLibraryWithPattern(self,pattern,files):
        return self.FindFilesWithPattern(self.libs,pattern,files)

    def FindHeader(self,file):
        return self.FindFilesWithPattern(self.includes,file,[file])

    def FindFilesWithPattern(self,paths,pattern,targets):
        result_files=[]
        result_paths=[]
        for path in paths:
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
    

    def checkZLIB(self):

        self.PrintLibrary("zlib library")

        # Name of the dynamic lib
        libnames=['libz.so','libz.a']
        if self.isMAC:
            libnames.append('libz.dylib')
        
        # User veto
        if self.configLinux.useroptions.zlib_veto=='1':
            self.PrintFAIL(warning=True)
	    logging.warning("Library called 'zlib' disabled. Gzip format will be disabled.")
            return False

        # Does the user force the paths?
        force1=False
        force2=False
        if self.configLinux.useroptions.zlib_includes!="0":
            self.configLinux.zlib_inc_path=self.configLinux.useroptions.zlib_includes
            force1=True
        if self.configLinux.useroptions.zlib_libs!="0":
            self.configLinux.zlib_lib_path=self.configLinux.useroptions.zlib_libraries
            force2=True
        force=force1 and force2
        
        # Checking if zlib has been installed by MA5
        ma5installation = False
        if not force:
            if os.path.isdir(self.ma5dir+'/tools/zlib'):
                self.configLinux.zlib_inc_path=self.ma5dir+'/tools/zlib/'
                self.configLinux.zlib_lib_path=self.ma5dir+'/tools/zlib/'
                ma5installation = True

        # Check if the libraries and headers are available
        if force or ma5installation:

            # header
            if not os.path.isfile(self.configLinux.zlib_inc_path+'/zlib.h'):
                self.PrintFAIL(warning=True)
	        logging.warning("Header file called '"+self.configLinux.zlib_inc_path+"'/zlib.h not found. Gzip format will be disabled.")
                logging.warning("To enable this format, please type 'install zlib' package.")
                return False

            # lib
            mypath, myfile = self.FindFilesWithPattern([self.configLinux.zlib_lib_path],"libz.*",libnames)
            self.configLinux.zlib_lib=myfile
            if self.configLinux.zlib_lib=="":
                self.PrintFAIL(warning=True)
	        logging.warning("Zlib library not found in "+self.configLinux.zlib_lib_path+" folder. Gzip format will be disabled.")
                logging.warning("To enable this format, please type 'install zlib' package.")
                return False

        # Checking zlib can be found in other folders
        if not force and not ma5installation:

            # header
            mypath, myfile = self.FindHeader('zlib.h')
            self.configLinux.zlib_inc_path = mypath
            if self.configLinux.zlib_inc_path=="":
                self.PrintFAIL(warning=True)
  	        logging.warning("Header file called 'zlib.h' not found. Gzip format will be disabled.")
                logging.warning("To enable this format, please type 'install zlib' package.")
                return False
            
            # lib
            mypath, myfile = self.FindLibraryWithPattern('libz.*',libnames)
            self.configLinux.zlib_lib_path = mypath
            self.configLinux.zlib_lib      = myfile
            if self.configLinux.zlib_lib_path=="":
                self.PrintFAIL(warning=True)
                logging.warning("Library called 'zlib' not found. Gzip format will be disabled.")
                logging.warning("To enable this format, please type 'install zlib' package.")
                return False

        self.configLinux.libraries['ZLib']=self.configLinux.zlib_lib+":"+str(os.stat(self.configLinux.zlib_lib).st_mtime)
        self.PrintOK()
 
        return True


    def checkDelphes(self):

        self.PrintLibrary("delphes library")

        # Name of the dynamic lib
        libnames=['libDelphes.so','libDelphes.a']
        if self.isMAC:
            libnames.append('libDelphes.dylib')

        # User veto
        if self.configLinux.useroptions.delphes_veto=='1':
            self.PrintFAIL(warning=True)
	    logging.warning("Library called 'delphes' disabled. Delphes ROOT format will be disabled.")
            return False

        # Does the user force the paths?
        force1=False
        force2=False
        if self.configLinux.useroptions.delphes_includes!="0":
            self.configLinux.delphes_inc_paths.append(self.configLinux.useroptions.delphes_includes)
            self.configLinux.delphes_inc_paths.append(self.configLinux.useroptions.delphes_includes+'/external/')
            force1=True
        if self.configLinux.useroptions.delphes_libs!="0":
            self.configLinux.delphes_lib_paths.append(self.configLinux.useroptions.delphes_libraries)
            force2=True
        force=force1 and force2

        # Checking if Delphes has been installed by MA5
        ma5installation = False
        if not force:
            if os.path.isdir(self.ma5dir+'/tools/delphes') and \
               os.path.isdir(self.ma5dir+'/tools/delphes/external'):
                self.configLinux.delphes_inc_paths.append(self.ma5dir+'/tools/delphes/')
                self.configLinux.delphes_inc_paths.append(self.ma5dir+'/tools/delphes/external/')
                self.configLinux.delphes_lib_paths.append(self.ma5dir+'/tools/delphes/')
                ma5installation = True

        # Check if the libraries and headers are available
        if force or ma5installation:

            # header
            if not os.path.isfile(self.configLinux.delphes_inc_paths[0]+'/modules/ParticlePropagator.h'):
                self.PrintFAIL(warning=True)
	        logging.warning("Header file called '"+self.configLinux.delphes_inc_paths[0]+"/modules/ParticlePropagator.h' not found." +\
                                "Delphes ROOT format will be disabled.")
                logging.warning("To enable this format, please type 'install delphes' package.")
                return False

            # lib
            mypath, myfile = self.FindFilesWithPattern(self.configLinux.delphes_lib_paths,"libDelphes.*",libnames)
            self.configLinux.delphes_lib=myfile
            if self.configLinux.delphes_lib=="":
                self.PrintFAIL(warning=True)
	        logging.warning("Delphes library not found in "+self.configLinux.delphes_lib_paths[0]+" folder. Delphes ROOT format will be disabled.")
                logging.warning("To enable this format, please type 'install delphes' package.")
                return False

        # Checking Delphes can be found in other folders
        if not force and not ma5installation:

            # header
            mypath, myfile = self.FindHeader('/modules/ParticlePropagator.h')
            self.delphes_inc_path = mypath
            if self.delphes_inc_path=="":
                self.PrintFAIL(warning=True)
  	        logging.warning("Header file called '/modules/ParticlePropagator.h' not found. Delphes ROOT format will be disabled.")
                logging.warning("To enable this format, please type 'install delphes' package.")
                return False
            
            # lib
            mypath, myfile = self.FindLibraryWithPattern('libDelphes.*',libnames)
            self.configLinux.delphes_lib_path = mypath
            self.configLinux.delphes_lib      = myfile
            if self.configLinux.delphes_lib_path=="":
                self.PrintFAIL(warning=True)
                logging.warning("Delphes library not found. Delphes format will be disabled.")
                logging.warning("To enable this format, please type 'install delphes' package.")
                return False

        self.configLinux.libraries['Delphes']=self.configLinux.delphes_lib+":"+str(os.stat(self.configLinux.delphes_lib).st_mtime)
        self.PrintOK()
 
        return True


    def checkDelfes(self):

#        self.PrintLibrary("delfes library")

        # Name of the dynamic lib
        libnames=['libDelphes.so','libDelphes.a']
        if self.isMAC:
            libnames.append('libDelphes.dylib')

        # User veto
        if self.configLinux.useroptions.delfes_veto=='1':
#            self.PrintFAIL(warning=True)
#	    logging.warning("Library called 'delfes' disabled. Delfes ROOT format will be disabled.")
            return False

        # Does the user force the paths?
        force1=False
        force2=False
        if self.configLinux.useroptions.delfes_includes!="0":
            self.configLinux.delfes_inc_paths.append(self.configLinux.useroptions.delfes_includes)
            self.configLinux.delfes_inc_paths.append(self.configLinux.useroptions.delfes_includes+'/external/')
            force1=True
        if self.configLinux.useroptions.delfes_libs!="0":
            self.configLinux.delfes_lib_paths.append(self.configLinux.useroptions.delfes_libraries)
            force2=True
        force=force1 and force2

        # Checking if delfes has been installed by MA5
        ma5installation = False
        if not force:
            if os.path.isdir(self.ma5dir+'/tools/delfes') and \
               os.path.isdir(self.ma5dir+'/tools/delfes/external'):
                self.configLinux.delfes_inc_paths.append(self.ma5dir+'/tools/delfes/')
                self.configLinux.delfes_inc_paths.append(self.ma5dir+'/tools/delfes/external/')
                self.configLinux.delfes_lib_paths.append(self.ma5dir+'/tools/delfes/')
                ma5installation = True

        # Check if the libraries and headers are available
        if force or ma5installation:

            # header
            if not os.path.isfile(self.configLinux.delfes_inc_paths[0]+'/modules/ParticlePropagator.h'):
#                self.PrintFAIL(warning=True)
#	        logging.warning("Header file called '"+self.configLinux.delfes_inc_paths[0]+"/modules/ParticlePropagator.h' not found." +\
#                                "Delfes ROOT format will be disabled.")
#                logging.warning("To enable this format, please type 'install delfes' package.")
                return False

            # lib
            mypath, myfile = self.FindFilesWithPattern(self.configLinux.delfes_lib_paths,"libDelphes.*",libnames)
            self.configLinux.delfes_lib=myfile
            if self.configLinux.delfes_lib=="":
#                self.PrintFAIL(warning=True)
#	         logging.warning("Delfes library not found in "+self.configLinux.delfes_lib_paths[0]+" folder. Delfes ROOT format will be disabled.")
#                logging.warning("To enable this format, please type 'install delfes' package.")
                return False

        # Checking Delfes can be found in other folders
        if not force and not ma5installation:

            # header
            mypath, myfile = self.FindHeader('/modules/ParticlePropagator.h')
            self.delfes_inc_path = mypath
            if self.delfes_inc_path=="":
#                self.PrintFAIL(warning=True)
#                logging.warning("Header file called '/modules/ParticlePropagator.h' not found. Delfes ROOT format will be disabled.")
#                logging.warning("To enable this format, please type 'install delfes' package.")
                return False
            
            # lib
            mypath, myfile = self.FindLibraryWithPattern('libDelphes.*',libnames)
            self.configLinux.delfes_lib_path = mypath
            self.configLinux.delfes_lib      = myfile
            if self.configLinux.delfes_lib_path=="":
#                self.PrintFAIL(warning=True)
#                logging.warning("Delfes library not found. Delfes format will be disabled.")
#                logging.warning("To enable this format, please type 'install delfes' package.")
                return False

        self.configLinux.libraries['Delfes']=self.configLinux.delfes_lib+":"+str(os.stat(self.configLinux.delfes_lib).st_mtime)
#        self.PrintOK()
 
        return True


    def checkMCatNLOUtils(self):

        self.PrintLibrary("MCatNLO-utilities")
        find = os.path.isfile(self.ma5dir+'/tools/MCatNLO-utilities/MCatNLO/lib/libstdhep.a')
        if not find:
            self.PrintFAIL(warning=True)
            logging.warning('MCatNLO-utilities not found. Showering aMCatNLO events deactivated.')
            logging.warning('To install the utilities, please type \'install MCatNLO-utilities\'.')
            return False
        else:
            self.PrintOK()

        return True


    def checkFastJet(self):

        self.PrintLibrary("FastJet")
        self.configLinux.fastjet_version = "none"

        # User veto
        if self.configLinux.useroptions.fastjet_veto=='1':
            self.PrintFAIL(warning=True)
	    logging.warning("The FastJet package is disabled. JetClustering algorithms are disabled.")
            return False

        # Does the user force the paths?
        force=False
        if self.configLinux.useroptions.fastjet_bin_path!="0":
            self.configLinux.fastjet_bin_path.append(self.configLinux.useroptions.fastjet_bin_path)
            force=True

        # Checking if FastJet has been installed by MA5
        ma5installation = False
        if not force:
            if os.path.isdir(self.ma5dir+'/tools/fastjet/bin'):
                self.configLinux.fastjet_bin_path=self.ma5dir+'/tools/fastjet/bin/'
                ma5installation = True

        # Check if the libraries and headers are available
        if force or ma5installation:

            if not os.path.isfile(self.configLinux.fastjet_bin_path+'/fastjet-config'):
                self.PrintFAIL(warning=True)
	        logging.warning("The FastJet package not found. JetClustering algorithms are disabled.")
                logging.warning("To enable this functionnality, please type 'install fastjet'.")
                return False
                
        # Checking FastJet can be found in other folders
        if not force and not ma5installation:

            for item in self.paths:
                files=glob.glob(item+"/fastjet-config")
                files.extend(glob.glob(item+"/fastjet-config"))
                if(len(files))!=0:
                    self.configLinux.fastjet_bin_path=item
                    break
            if self.configLinux.fastjet_bin_path=='':
                self.PrintFAIL(warning=True)
	        logging.warning("The FastJet package not found. JetClustering algorithms are disabled.")
                logging.warning("To enable this functionnality, please type 'install fastjet'.")
                return False

        self.configLinux.fastjet_version = commands.getstatusoutput(self.ma5dir+'/tools/fastjet/bin/fastjet-config --version')[1]
        tmp = commands.getstatusoutput(self.ma5dir+'/tools/fastjet/bin/fastjet-config --libs --plugins')[1]
        words = tmp.split()
        for word in words:
            if word.startswith('-L'):
                self.configLinux.fastjet_lib_paths.append(word[2:])
        self.PrintOK()

        return True


    def checkNumPy(self):

        self.PrintLibrary("python library: numpy")

        # Checking if fastjet is installed on the system
        try:
            import numpy
        except:
            self.PrintFAIL(warning=False)
            logging.error("The python library 'numpy' is not found. Please install it with the following command line:")
            logging.error("apt-get install python-numpy")
            return False
        
        self.PrintOK()
        return True


    def checkPdfLatex(self):
        self.PrintLibrary('pdflatex')

        if self.configLinux.useroptions.pdflatex_veto=='1':
            self.PrintFAIL(warning=True)
	    logging.warning("pdflatex disabled. Reports under the pdf format will not be compiled.")
            return False

        pdflatex_version = commands.getstatusoutput('pdflatex -version')
        if 'not found' in str(pdflatex_version) or 'no such file' in str(pdflatex_version):
            self.PrintFAIL(warning=True)
	    logging.warning("pdflatex not found. Reports under the pdf format will not be compiled.")
            return False
        else:
            self.PrintOK()
            return True
         
    def checkLatex(self):
        self.PrintLibrary('latex')

        if self.configLinux.useroptions.latex_veto=='1':
            self.PrintFAIL(warning=True)
	    logging.warning("latex disabled. Reports under the dvi format will not be compiled.")
            return False

        latex_version = commands.getstatusoutput('latex -version')
        if 'not found' in str(latex_version) or 'no such file' in str(latex_version):
            self.PrintFAIL(warning=True)
	    logging.warning("latex not found. Reports under the dvi format will not be compiled.")
            return False
        else:
            self.PrintOK()
            return True
         
    def checkdvipdf(self):
        self.PrintLibrary('dvipdf')

        if self.configLinux.useroptions.dvipdf_veto=='1':
            self.PrintFAIL(warning=True)
	    logging.warning("dvipdf disabled. DVI reports will not be converted to pdf files.")
            return False

        dvipdf_version = commands.getstatusoutput('dvipdf')
        if 'not found' in str(dvipdf_version) or 'no such file' in str(dvipdf_version):
            self.PrintFAIL(warning=True)
	    logging.warning("dvipdf not found. DVI reports will not be converted to pdf files.")
            return False
        else:
            self.PrintOK()
            return True
