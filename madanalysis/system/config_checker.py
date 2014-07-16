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


    def FillMA5Path(self):
        os.environ['MA5_BASE']=self.archi_info.ma5dir


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

    def checkPython(self):
        # Checking if Python is present
        self.PrintLibrary('python')

        # Debug general
        if self.debug:
            logging.debug("")
            logging.debug("  Python release:         " + str(platform.python_version()))
            logging.debug("  Python build:           " + str(platform.python_build()))
            logging.debug("  Python compiler:        " + str(platform.python_compiler()))
            logging.debug("  Python prefix:          " + str(sys.prefix))
            logging.debug("  Python executable used: " + str(sys.executable))

        # Which python
        if self.debug:
            result = ShellCommand.Which('python',all=False,mute=True)
            if len(result)==0:
                self.PrintFAIL(warning=False)
                logging.error('python compiler not found. Please install it before ' + \
	             'using MadAnalysis 5')
                return False
            logging.debug("  which:                  " + str(result[0]))

        # Which all
        if self.debug:
            result = ShellCommand.Which('python',all=True,mute=True)
            if len(result)==0:
                self.PrintFAIL(warning=False)
                logging.error('g++ compiler not found. Please install it before ' + \
	                 'using MadAnalysis 5')
                return False
            logging.debug("  which-all:              ")
            for file in result:
                logging.debug("    - "+str(file))

        # Python paths
        if self.debug:
            logging.debug("  Python internal paths: ")
            tmp = sys.path
            for path in tmp:
                logging.debug("    - "+path)
            logging.debug("  $PYTHONPATH: ")
            try:
                tmp = os.environ['PYTHONPATH']
            except:
                tmp = []
            if len(tmp)==0:
                logging.debug("    EMPTY OR NOT FOUND")
            else:
                tmp = tmp.split(':')
                for path in tmp:
                    logging.debug("    - "+path)
            logging.debug("")

        # Ok
        self.PrintOK()
        return True


    def checkROOT(self):
        # Checking if ROOT is present
        self.PrintLibrary('Root')
        logging.debug("")

        # Does the user force the ROOT path
        force=False
        if self.user_info.root_bin!=None:
            logging.debug("User setting: root bin path is specified.")
            self.archi_info.root_bin_path=os.path.normpath(self.user_info.root_bin)
            force=True

        # Detection of root-config
        if force:
            if not os.path.isfile(self.archi_info.root_bin_path+'/root-config'):
                self.PrintFAIL(warning=False)
	        logging.error("root-config program is not found in folder: "+self.archi_info.root_bin_path)
                logging.error("Please check that ROOT is properly installed.")
                return False

            # Using root-config
            logging.debug("root-config program found in: "+self.archi_info.root_bin_path)
            logging.debug("Launch root-config ...")
            theCommands = [self.archi_info.root_bin_path+'/root-config','--libdir','--incdir']
            ok, out, err = ShellCommand.ExecuteWithCapture(theCommands,'./')
            if not ok:
                self.PrintFAIL(warning=False)
                logging.error('ROOT module called "root-config" is not detected.\n'\
		              +'Two explanations :n'\
		              +' - ROOT is not installed. You can download it '\
		              +'from http://root.cern.ch\n'\
		              +' - ROOT binary folder must be placed in the '\
                              +'global environment variable $PATH')
                return False

            # Extracting ROOT library and header path
            out=out.lstrip()
            out=out.rstrip()
            root_tmp = out.split()
            if len(root_tmp)<2:
                self.PrintFAIL(warning=False)
                logging.error('"root-config --libdir --incdir" does not provide good information.')
                return False
            self.archi_info.root_inc_path = os.path.normpath(root_tmp[1])
            self.archi_info.root_lib_path = os.path.normpath(root_tmp[0])
            logging.debug("-> root-config found")
            logging.debug("-> root header  folder: "+self.archi_info.root_inc_path)
            logging.debug("-> root library folder: "+self.archi_info.root_lib_path)

        # Trying to call root-config
        if not force:

            # Which
            result = ShellCommand.Which('root-config')
            if len(result)==0:
                self.PrintFAIL(warning=False)
                logging.error('ROOT module called "root-config" is not detected.\n'\
		              +'Two explanations :n'\
		              +' - ROOT is not installed. You can download it '\
		              +'from http://root.cern.ch\n'\
		              +' - ROOT binary folder must be placed in the '\
                              +'global environment variable $PATH')
                return False
            self.archi_info.root_bin_path=os.path.normpath(result[0][:-11])
            if self.debug:
                logging.debug("")
                logging.debug("  which:         " + str(self.archi_info.root_bin_path))

            # Which all
            if self.debug:
                result = ShellCommand.Which('root-config',all=True,mute=True)
                if len(result)==0:
                    self.PrintFAIL(warning=False)
                    logging.error('ROOT module called "root-config" is not detected.\n'\
		              +'Two explanations :n'\
		              +' - ROOT is not installed. You can download it '\
		              +'from http://root.cern.ch\n'\
		              +' - ROOT binary folder must be placed in the '\
                              +'global environment variable $PATH')
                    return False
                logging.debug("  which-all:     ")
                for file in result:
                    logging.debug("    - "+str(file))


            # Using root-config
            logging.debug("Try to detect root-config ...")
            theCommands = ['root-config','--libdir','--incdir']
            ok, out, err = ShellCommand.ExecuteWithCapture(theCommands,'./')
            if not ok:
                self.PrintFAIL(warning=False)
                logging.error('ROOT module called "root-config" is not detected.\n'\
		              +'Two explanations :n'\
		              +' - ROOT is not installed. You can download it '\
		              +'from http://root.cern.ch\n'\
		              +' - ROOT binary folder must be placed in the '\
                              +'global environment variable $PATH')
                return False

            # Extracting ROOT library and header path
            out=out.lstrip()
            out=out.rstrip()
            root_tmp = out.split()
            self.archi_info.root_inc_path = os.path.normpath(root_tmp[1])
            self.archi_info.root_lib_path = os.path.normpath(root_tmp[0])
            logging.debug("-> root-config found")
            logging.debug("-> root header  folder: "+self.archi_info.root_inc_path)
            logging.debug("-> root library folder: "+self.archi_info.root_lib_path)

        # Adding ROOT library path to Python path
        sys.path.append(self.archi_info.root_lib_path)

        # Check: looking for files
        FilesToFind=[os.path.normpath(self.archi_info.root_lib_path+'/libCore.so'), \
                     os.path.normpath(self.archi_info.root_inc_path+'/TH1F.h')]
        for file in FilesToFind:
            logging.debug("Try to find "+file+" ...")
            if os.path.isfile(file):
                self.archi_info.libraries[file.split('/')[-1]]=file+":"+str(os.stat(file).st_mtime)
            else:
                self.PrintFAIL(warning=False)
	        logging.error("ROOT file called '"+file+"' is not found")
                logging.error("Please check that ROOT is properly installed.")
                return False

        # Getting the features
        ok, out, err = ShellCommand.ExecuteWithCapture([self.archi_info.root_bin_path+'/root-config','--features'],'./')
        if not ok:
            self.PrintFAIL(warning=False)
            logging.error('problem with root-config')
            return False
        out=out.lstrip()
        out=out.rstrip()
        features = str(out).split()
        features.sort()
        for feature in features:
            self.archi_info.root_features.append(feature)
        if self.debug:
            logging.debug("  features:      " + str(self.archi_info.root_features))

        # Root Install
        self.archi_info.root_priority=force
        self.PrintOK()
        return True


    def checkPyROOT(self):

        # Loading ROOT library
        self.PrintLibrary("PyRoot libraries")
        logging.debug("")

        # Check if Python is install
        if 'python' not in self.archi_info.root_features:
            self.PrintFAIL(warning=False)
            logging.error("ROOT has not been built with 'python' options.")
            return False

        # Check: looking for files
        FilesToFind=[os.path.normpath(self.archi_info.root_lib_path+'/libPyROOT.so')]
        for file in FilesToFind:
            logging.debug("Try to find "+file+" ...")
            if os.path.isfile(file):
                self.archi_info.libraries[file.split('/')[-1]]=file+":"+str(os.stat(file).st_mtime)
            else:
                self.PrintFAIL(warning=False)
	        logging.error("ROOT file called '"+file+"' is not found")
                logging.error("Please check that ROOT is properly installed.")
                return False

        # Check: looking for files
        FilesToFind=[os.path.normpath(self.archi_info.root_lib_path+'/ROOT.py'), \
                     os.path.normpath(self.archi_info.root_lib_path+'/ROOT.pyc')]
        found=False
        for file in FilesToFind:
            logging.debug("Try to find "+file+" ...")
            if os.path.isfile(file):
                self.archi_info.libraries[file.split('/')[-1]]=file+":"+str(os.stat(file).st_mtime)
                found=True
                break

        # If check failed: looking for Python path
        if not found:
            libnames=['ROOT.py','ROOT.py']
            logging.debug("Look for the libraries in Python Library folder ...")
            mypath, myfile = self.FindFilesWithPattern(sys.path,"ROOT.py*",libnames)
            logging.debug("-> result: "+str(myfile))
            if myfile=='':
                self.PrintFAIL(warning=False)
                logging.error("ROOT file called 'ROOT.py' or 'ROOT.pyc' is not found")
                logging.error("Please check that ROOT is properly installed.")
                return False
            else:
                self.archi_info.libraries[myfile.split('/')[-1]]=myfile+":"+str(os.stat(myfile).st_mtime)

        # Import gROOT
        logging.debug("Try to import the gROOT module ...")
        try:
	    from ROOT import gROOT
        except:
            self.PrintFAIL(warning=False)
            logging.error("'root-config --libdir' indicates a wrong path for ROOT"\
	                  +" libraries. Please specify the ROOT library path"\
		          +" into the environnement variable $PYTHONPATH")
            return False

        # Setting ROOT batch mode
        if not self.script:
            logging.debug("Initialize the ROOT graphical modules ...")
            from ROOT import TApplication
            from ROOT import gApplication
            TApplication.NeedGraphicsLibs()
            gApplication.InitializeGraphics()
        gROOT.SetBatch(True)

        # Checking ROOT release
        logging.debug("Extract the ROOT version ...")
        RootVersion = gROOT.GetVersionInt()
        if RootVersion<52700:
            self.PrintFAIL(warning=False)
	    logging.error('Bad release of ROOT : '+gROOT.GetVersion()+\
                          '. MadAnalysis5 needs ROOT 5.27 or higher.\n Please upgrade your version of ROOT.')
            return False

        self.archi_info.root_version   = RootVersion
        logging.debug("-> Root version: "+str(self.archi_info.root_version))

        self.PrintOK()
        return True


    def checkGPP(self):
        # Checking g++ release
        self.PrintLibrary('g++')

        # Which
        result = ShellCommand.Which('g++')
        if len(result)==0:
            self.PrintFAIL(warning=False)
            logging.error('g++ compiler not found. Please install it before ' + \
	             'using MadAnalysis 5')
            return False
        if self.debug:
            logging.debug("")
            logging.debug("  which:         " + str(result[0]))

        # Which all
        if self.debug:
            result = ShellCommand.Which('g++',all=True,mute=True)
            if len(result)==0:
                self.PrintFAIL(warning=False)
                logging.error('g++ compiler not found. Please install it before ' + \
	                 'using MadAnalysis 5')
                return False
            logging.debug("  which-all:     ")
            for file in result:
                logging.debug("    - "+str(file))

        # Getting the version
        ok, out, err = ShellCommand.ExecuteWithCapture(['g++','-dumpversion'],'./')
        if not ok:
            self.PrintFAIL(warning=False)
            logging.error('g++ compiler not found. Please install it before ' + \
	             'using MadAnalysis 5')
            return False
        out=out.lstrip()
        out=out.rstrip()
        self.archi_info.gcc_version = str(out)
        if self.debug:
            logging.debug("  version:       " + self.archi_info.gcc_version)

        self.PrintOK()
        return True


    def checkMake(self):
        # Checking GNU Make
        self.PrintLibrary('GNU Make')

        # Which
        result = ShellCommand.Which('make')
        if len(result)==0:
            self.PrintFAIL(warning=False)
            logging.error('GNU Make not found. Please install it before ' + \
	             'using MadAnalysis 5')
            return False
        if self.debug:
            logging.debug("")
            logging.debug("  which:         " + str(result[0]))

        # Which all
        if self.debug:
            result = ShellCommand.Which('make',all=True,mute=True)
            if len(result)==0:
                self.PrintFAIL(warning=False)
                logging.error('GNU Make not found. Please install it before ' + \
	                 'using MadAnalysis 5')
                return False
            logging.debug("  which-all:     ")
            for file in result:
                logging.debug("    - "+str(file))

        # Getting the version
        ok, out, err = ShellCommand.ExecuteWithCapture(['make','--version'],'./')
        if not ok:
            self.PrintFAIL(warning=False)
            logging.error('GNU Make not found. Please install it before ' + \
	             'using MadAnalysis 5')
            return False
        lines=out.split('\n')
        if len(lines)==0:
             logging.error('command "make --version" seems to not give the GNU Make version')
             return False
        firstline=lines[0]
        firstline=firstline.lstrip()
        firstline=firstline.rstrip()
        self.archi_info.make_version = str(firstline)
        if self.debug:
            logging.debug("  version:       " + self.archi_info.make_version)

        # Ok
        self.PrintOK()
        return True


    def checkGF(self):
        # Checking if gfortran is present
        self.PrintLibrary("gfortran")
        logging.debug("")

        # Which gfortran
        if self.debug:
            result = ShellCommand.Which('gfortran',all=False,mute=True)
            if len(result)==0:
                self.PrintFAIL(warning=False)
                logging.warning('gfortran compiler not found.')
                return False
            logging.debug("  which:                  " + str(result[0]))

        # Which all
        if self.debug:
            result = ShellCommand.Which('gfortran',all=True,mute=True)
            if len(result)==0:
                self.PrintFAIL(warning=False)
                logging.warning('gfortran compiler not found.')
                return False
            logging.debug("  which-all:              ")
            for file in result:
                logging.debug("    - "+str(file))

        # gfortran version
        ok, out, err = ShellCommand.ExecuteWithCapture(['gfortran','-dumpversion'],'./')
        if not ok:
            self.PrintFAIL(warning=False)
            logging.warning('gfortran compiler not found.')
            return False

        # treating version
        gfortran_version = out.split('\n')[0]
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

        self.archi_info.gfortran_version = gfortran_version
        if self.debug:
            logging.debug("  version:       " + self.archi_info.gfortran_version)

        # Ok
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

        # Checking if zlib is present
        self.PrintLibrary("zlib")
        self.archi_info.fastjet_version = "none"
        logging.debug("")

        # Name of the dynamic lib
        libnames=['libz.so','libz.a']
        if self.archi_info.isMac:
            libnames.append('libz.dylib')

        # User veto
        if self.user_info.zlib_veto:
            logging.debug("User setting: veto on zlib module")
            self.PrintFAIL(warning=True)
	    logging.warning("Library called 'zlib' disabled. Gzip format will be disabled.")
            return False

        # Does the user force the paths?
        force1=False
        force2=False
        if self.user_info.zlib_includes!=None:
            logging.debug("User setting: zlib include path is specified")
            self.archi_info.zlib_inc_path=os.path.normpath(self.user_info.zlib_includes)
            force1=True
        if self.user_info.zlib_libs!=None:
            logging.debug("User setting: zlib lib path is specified")
            self.archi_info.zlib_lib_path=os.path.normpath(self.user_info.zlib_libraries)
            force2=True
        force=force1 and force2

        # Checking if zlib has been installed by MA5
        ma5installation = False
        if not force:
            pathname = os.path.normpath(self.archi_info.ma5dir+'/tools/zlib')
            logging.debug("Look for zlib in the folder "+pathname+" ...")
            if os.path.isdir(pathname):
                self.archi_info.zlib_inc_path=os.path.normpath(pathname+'/include/')
                self.archi_info.zlib_lib_path=os.path.normpath(pathname+'/lib/')
                logging.debug("-> found")
                ma5installation = True
            else:
                logging.debug("-> not found")

        # Check if the libraries and headers are available
        if force or ma5installation:

            # header
            filename=os.path.normpath(self.archi_info.zlib_inc_path+'/zlib.h')
            logging.debug("Look for the file "+filename+" ...")
            if not os.path.isfile(filename):
                logging.debug('-> not found')
                self.PrintFAIL(warning=True)
                logging.warning("Header file called '"+filename+"' not found.")
                logging.warning("Gzip format will be disabled.")
                logging.warning("To enable this format, please type 'install zlib'.")
                return False
            else:
                logging.debug('-> found')

            # lib
            logging.debug("Look for the libraries in folder "+self.archi_info.zlib_lib_path+" ...")
            mypath, myfile = self.FindFilesWithPattern([self.archi_info.zlib_lib_path],"libz.*",libnames)
            self.archi_info.zlib_lib=os.path.normpath(myfile)
            logging.debug("-> result: "+str(self.archi_info.zlib_lib))
            if self.archi_info.zlib_lib=="":
                self.PrintFAIL(warning=True)
                logging.warning("Zlib library not found in "+self.archi_info.zlib_lib_path+" folder.")
                logging.warning("Gzip format will be disabled.")
                logging.warning("To enable this format, please type 'install zlib'.")
                return False


        # Checking zlib can be found in other folders
        if not force and not ma5installation:

            # header
            logging.debug("Look for the header file zlib.h ...")
            mypath, myfile = self.FindHeader('zlib.h')
            self.archi_info.zlib_inc_path = os.path.normpath(mypath)
            logging.debug("-> result for the path: "+str(self.archi_info.zlib_inc_path))
            logging.debug("-> result for the file: "+str(os.path.normpath(myfile)))
            if self.archi_info.zlib_inc_path=="":
                self.PrintFAIL(warning=True)
                logging.warning("Header file called 'zlib.h' not found.")
                logging.warning("Gzip format will be disabled.")
                logging.warning("To enable this format, please type 'install zlib'.")
                return False

            # lib
            logging.debug("Look for the zlib libraries ...")
            mypath, myfile = self.FindLibraryWithPattern('libz.*',libnames)
            self.archi_info.zlib_lib_path = os.path.normpath(mypath)
            self.archi_info.zlib_lib      = os.path.normpath(myfile)
            logging.debug("-> result for lib paths: "+str(self.archi_info.zlib_lib_path))
            logging.debug("-> result for lib files: "+str(self.archi_info.zlib_lib))
            if self.archi_info.zlib_lib_path=="":
                self.PrintFAIL(warning=True)
                logging.warning("Library called 'zlib' not found.")
                logging.warning("Gzip format will be disabled.")
                logging.warning("To enable this format, please type 'install zlib'.")
                return False

        self.archi_info.libraries['ZLib']=self.archi_info.zlib_lib+":"+str(os.stat(self.archi_info.zlib_lib).st_mtime)
        self.archi_info.zlib_priority=(force or ma5installation)

        # Ok
        self.PrintOK()
        return True


    def checkDelphes(self):
        # Checking if Delphes is present
        self.PrintLibrary("Delphes")
        logging.debug("")

        # Name of the dynamic lib
        libnames=['libDelphes.so','libDelphes.a']
        if self.archi_info.isMac:
            libnames.append('libDelphes.dylib')

        # User veto
        if self.user_info.delphes_veto:
            logging.debug("User setting: veto on Delphes")
            self.PrintFAIL(warning=True)
            logging.warning("Library called 'delphes' disabled.")
            logging.warning("Delphes ROOT format will be disabled.")
            return False

        # Does the user force the paths?
        force1=False
        force2=False
        if self.user_info.delphes_includes!=None:
            logging.debug("User setting: Delphes include path is specified.")
            self.archi_info.delphes_inc_paths.append(self.user_info.delphes_includes)
            self.archi_info.delphes_inc_paths.append(self.user_info.delphes_includes+'/external/')
            force1=True
        if self.user_info.delphes_libs!=None:
            logging.debug("User setting: Delphes lib path is specified.")
            self.archi_info.delphes_lib_paths.append(self.user_info.delphes_libraries)
            force2=True
        force=force1 and force2

        # Checking if Delphes has been installed by MA5
        ma5installation = False
        if not force:
            logging.debug("Look for Delphes in the folder "+self.archi_info.ma5dir+"/tools ...")
            if os.path.isdir(self.archi_info.ma5dir+'/tools/delphes') and \
               os.path.isdir(self.archi_info.ma5dir+'/tools/delphes/external'):
                self.archi_info.delphes_inc_paths.append(self.archi_info.ma5dir+'/tools/delphes/')
                self.archi_info.delphes_inc_paths.append(self.archi_info.ma5dir+'/tools/delphes/external/')
                self.archi_info.delphes_lib_paths.append(self.archi_info.ma5dir+'/tools/delphes/')
                logging.debug("-> found")
                ma5installation = True
            else:
                logging.debug("-> not found")

        # Check if the libraries and headers are available
        if force or ma5installation:

            # header
            filename = os.path.normpath(self.archi_info.delphes_inc_paths[0]+'/modules/ParticlePropagator.h')
            logging.debug("Look for the file "+filename+" ...")
            if not os.path.isfile(filename):
                logging.debug("-> not found")
                self.PrintFAIL(warning=True)
                logging.warning("Header file called '"+filename+"' not found.")
                logging.warning("Delphes ROOT format will be disabled.")
                logging.warning("To enable this format, please type 'install delphes'.")
                return False
            else:
                logging.debug("-> found")

            # lib
            logging.debug("Look for the libraries in folder "+str(self.archi_info.delphes_lib_paths)+" ...")
            mypath, myfile = self.FindFilesWithPattern(self.archi_info.delphes_lib_paths,"libDelphes.*",libnames)
            self.archi_info.delphes_lib=myfile
            logging.debug("-> result: "+str(self.archi_info.delphes_lib))
            if self.archi_info.delphes_lib=="":
                self.PrintFAIL(warning=True)
                logging.warning("Delphes library not found in "+self.archi_info.delphes_lib_paths[0]+" folder.")
                logging.warning("Delphes ROOT format will be disabled.")
                logging.warning("To enable this format, please type 'install delphes'.")
                return False
            self.archi_info.delphes_lib=os.path.normpath(myfile)
            
        # Checking Delphes can be found in other folders
        if not force and not ma5installation:

            # header
            logging.debug("Look for the header file /modules/ParticlePropagator.h ...")
            mypath, myfile = self.FindHeader('/modules/ParticlePropagator.h')
            if mypath!='' and myfile!='':
                self.archi_info.delphes_inc_paths.append(os.path.normpath(mypath))
                self.archi_info.delphes_inc_paths.append(os.path.normpath(mypath+'/external'))
                logging.debug("-> result for the path: "+str(self.archi_info.delphes_inc_paths))
                logging.debug("-> result for the file: "+str(os.path.normpath(myfile)))
            if len(self.archi_info.delphes_inc_paths)==0:
                self.PrintFAIL(warning=True)
                logging.warning("Header file called '/modules/ParticlePropagator.h' not found.")
                logging.warning("Delphes ROOT format will be disabled.")
                logging.warning("To enable this format, please type 'install delphes'.")
                return False

            # lib
            logging.debug("Look for the Delphes libraries ...")
            mypath, myfile = self.FindLibraryWithPattern('libDelphes.*',libnames)
            if mypath!='' and myfile!='':
                self.archi_info.delphes_lib_paths.append(os.path.normpath(mypath))
                self.archi_info.delphes_lib      = os.path.normpath(myfile)
                logging.debug("-> result for lib paths: "+str(self.archi_info.delphes_lib_paths))
                logging.debug("-> result for lib files: "+str(self.archi_info.delphes_lib))
            if len(self.archi_info.delphes_lib_paths)==0:
                self.PrintFAIL(warning=True)
                logging.warning("Delphes library not found.")
                logging.warning("Delphes format will be disabled.")
                logging.warning("To enable this format, please type 'install delphes'.")
                return False

        self.archi_info.libraries['Delphes']=self.archi_info.delphes_lib+":"+str(os.stat(self.archi_info.delphes_lib).st_mtime)
        self.archi_info.delphes_priority=(force or ma5installation)

        # Ok
        self.PrintOK()
        return True


    def checkDelphesMA5tune(self):
        # Checking if Delphes-MA5tune is present
        self.PrintLibrary("Delphes-MA5tune")
        logging.debug("")

        # Name of the dynamic lib
        libnames=['libDelphesMA5tune.so','libDelphesMA5tune.a']
        if self.archi_info.isMac:
            libnames.append('libDelphesMA5tune.dylib')

        # User veto
        if self.user_info.delphesMA5tune_veto:
            logging.debug("User setting: veto on Delphes-MA5tune")
            self.PrintFAIL(warning=True)
            logging.warning("Delphes-MA5tune is disabled. Delphes-MA5tune ROOT format will be disabled.")
            return False

        # Does the user force the paths?
        force1=False
        force2=False
        if self.user_info.delphesMA5tune_includes!=None:
            logging.debug("User setting: Delphes-MA5tune include path is specified.")
            self.archi_info.delphesMA5tune_inc_paths.append(self.user_info.delphesMA5tune_includes)
            self.archi_info.delphesMA5tune_inc_paths.append(self.user_info.delphesMA5tune_includes+'/external/')
            force1=True
        if self.user_info.delphesMA5tune_libs!=None:
            logging.debug("User setting: Delphes-MA5tune lib path is specified.")
            self.archi_info.delphesMA5tune_lib_paths.append(self.user_info.delphesMA5tune_libraries)
            force2=True
        force=force1 and force2

        # Checking if Delphes-MA5tune has been installed by MA5
        ma5installation = False
        if not force:
            logging.debug("Look for Delphes-MA5tune in the folder "+self.archi_info.ma5dir+"/tools ...")
            if os.path.isdir(self.archi_info.ma5dir+'/tools/delphesMA5tune') and \
               os.path.isdir(self.archi_info.ma5dir+'/tools/delphesMA5tune/external'):
                self.archi_info.delphesMA5tune_inc_paths.append(self.archi_info.ma5dir+'/tools/delphesMA5tune/')
                self.archi_info.delphesMA5tune_inc_paths.append(self.archi_info.ma5dir+'/tools/delphesMA5tune/external/')
                self.archi_info.delphesMA5tune_lib_paths.append(self.archi_info.ma5dir+'/tools/delphesMA5tune/')
                logging.debug("-> found")
                ma5installation = True
            else:
                logging.debug("-> not found")
                self.PrintFAIL(warning=True)
                logging.warning("DelphesMA5tune folder not found.")
                logging.warning("Delphes-MA5tune ROOT format will be disabled.")
                logging.warning("To enable this format, please type 'install delphesMA5tune'.")
                return False

        # Check if the libraries and headers are available
        if force or ma5installation:

            # header
            filename = os.path.normpath(self.archi_info.delphesMA5tune_inc_paths[0]+'/modules/ParticlePropagator.h')
            logging.debug("Look for the file "+filename+" ...")
            if not os.path.isfile(filename):
                logging.debug("-> not found")
                self.PrintFAIL(warning=True)
                logging.warning("Header file called '"+filename+"' not found.")
                logging.warning("Delphes-MA5tune ROOT format will be disabled.")
                logging.warning("To enable this format, please type 'install delphesMA5tune'.")
                return False
            else:
                logging.debug("-> found")

            # lib
            logging.debug("Look for the libraries in folder "+str(self.archi_info.delphesMA5tune_lib_paths)+" ...")
            mypath, myfile = self.FindFilesWithPattern(self.archi_info.delphesMA5tune_lib_paths,"libDelphesMA5tune.*",libnames)
            self.archi_info.delphesMA5tune_lib_paths.append(os.path.normpath(mypath))
            self.archi_info.delphesMA5tune_lib      = myfile
            logging.debug("-> result for lib paths: "+str(self.archi_info.delphesMA5tune_lib_paths))
            logging.debug("-> result for lib files: "+str(self.archi_info.delphesMA5tune_lib))
            if self.archi_info.delphesMA5tune_lib=="":
                self.PrintFAIL(warning=True)
                logging.warning("Delphes-MA5tune library not found in "+\
                  self.archi_info.delphesMA5tune_lib_paths[0]+" folder.")
                logging.warning("Delphes-MA5tune ROOT format will be disabled.")
                logging.warning("To enable this format, please type 'install delphesMA5tune'.")
                return False
            self.archi_info.delphesMA5tune_lib      = os.path.normpath(myfile)
            
        self.archi_info.libraries['DelphesMA5tune']=self.archi_info.delphesMA5tune_lib+":"+str(os.stat(self.archi_info.delphesMA5tune_lib).st_mtime)
        self.archi_info.delphesMA5tune_priority=(force or ma5installation)

        # Ok
        self.PrintOK()
        return True


    def checkFastJet(self):

        # Checking if FastJet is present
        self.PrintLibrary("FastJet")
        self.archi_info.fastjet_version = "none"
        logging.debug("")

        # User veto
        if self.user_info.fastjet_veto:
            logging.debug("User setting: veto on fastjet module")
            self.PrintFAIL(warning=True)
	    logging.warning("The FastJet package is disabled. JetClustering algorithms are disabled.")
            return False

        # Does the user force the paths?
        force=False
        if self.user_info.fastjet_bin_path!=None:
            logging.debug("User setting: fastjet bin path is specified")
            self.archi_info.fastjet_bin_path.append(self.user_info.fastjet_bin_path)
            force=True

        # Checking if FastJet has been installed by MA5
        ma5installation = False
        if not force:
            pathname = os.path.normpath(self.archi_info.ma5dir+'/tools/fastjet/bin')
            logging.debug("Look for FastJet in the folder "+pathname+" ...")
            if os.path.isdir(pathname):
                self.archi_info.fastjet_bin_path=pathname
                logging.debug("-> found")
                ma5installation = True
            else:
                logging.debug("-> not found")

        # Check if the libraries and headers are available
        if force or ma5installation:

            filename = os.path.normpath(self.archi_info.fastjet_bin_path+'/fastjet-config')
            logging.debug("Look for the file "+filename+" ...")
            if not os.path.isfile(filename):
                logging.debug("-> not found")
                self.PrintFAIL(warning=True)
                logging.warning("The FastJet package is not found.")
                logging.warning("JetClustering algorithms will be disabled.")
                logging.warning("To enable this functionnality, please type 'install fastjet'.")
                return False
            else:
                logging.debug("-> found")

        # Checking if FastJet is set into the PATH
        whichdetected=False
        if not force and not ma5installation:

            logging.debug("Try to locate fastjet-config program with the 'which' command ...")
            result = ShellCommand.Which('fastjet-config',all=False,mute=True)
            if len(result)==0:
                logging.debug('-> not found')
            else:
                self.archi_info.fastjet_bin_path=os.path.normpath(result[0][:-14])
                logging.debug('-> found in the folder: '+self.archi_info.fastjet_bin_path)
                whichdetected=True

        # Checking if FastJet can be found in other folders
        if not force and not ma5installation and not whichdetected:

            for item in self.paths:
                logging.debug("Look for the fastjet-config in the path "+item+" ...")
                files=glob.glob(os.path.normpath(item+"/fastjet-config"))
                logging.debug("-> result: "+str(files))
                if(len(files))!=0:
                    self.archi_info.fastjet_bin_path=item
                    break
            if self.archi_info.fastjet_bin_path=='':
                self.PrintFAIL(warning=True)
                logging.warning("The FastJet is package not found.")
                logging.warning("JetClustering algorithms will be disabled.")
                logging.warning("To enable this functionnality, please type 'install fastjet'.")
                return False


        self.archi_info.fastjet_priority=(force or ma5installation)

        # Treating FastJet bin path
        self.archi_info.fastjet_bin_path=os.path.normpath(self.archi_info.fastjet_bin_path)
        logging.debug("fastjet bin path chosen: "+self.archi_info.fastjet_bin_path)

        # Getting FastJet version
        filename = os.path.normpath(self.archi_info.fastjet_bin_path+'/fastjet-config')
        ok, out, err = ShellCommand.ExecuteWithCapture([filename,'--version'],'./')
        if not ok:
            self.PrintFAIL(warning=False)
            logging.error('fastjet-config program does not work properly.')
            return False
        out=out.lstrip()
        out=out.rstrip()
        self.archi_info.fastjet_version = str(out)
        if self.debug:
            logging.debug("  version:       " + self.archi_info.fastjet_version)

        # Getting FastJet lib
        filename = os.path.normpath(self.archi_info.fastjet_bin_path+'/fastjet-config')
        ok, out, err = ShellCommand.ExecuteWithCapture([filename,'--libs','--plugins'],'./')
        if not ok:
            self.PrintFAIL(warning=False)
            logging.error('fastjet-config program does not work properly.')
            return False
        out=out.lstrip()
        out=out.rstrip()
        logging.debug("  Lib flags:     " + str(out))
        words = out.split()
        for word in words:
            if word.startswith('-L'):
                self.archi_info.fastjet_lib_paths.append(word[2:])
        if self.debug:
            logging.debug("  Lib path:      " + str(self.archi_info.fastjet_lib_paths))

        # Ok
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


    def checkGnuplot(self):

        self.PrintLibrary("Gnuplot")

        # Checking if gnuplot is installed on the system
        # Which
        result = ShellCommand.Which('gnuplot',all=False,mute=True)
        if len(result)==0:
            self.PrintFAIL(warning=False)
            logging.warning("gnuplot disabled. Plots using gnuplot library will not be done.")
            return False
        if self.debug:
            logging.debug("  which:         " + str(result[0]))

        # Which all
        if self.debug:
            result = ShellCommand.Which('gnuplot',all=True,mute=True)
            if len(result)==0:
                self.PrintFAIL(warning=False)
                logging.warning("gnuplot disabled. Plots using gnuplot library will not be done.")
                return False
            logging.debug("  which-all:     ")
            for file in result:
                logging.debug("    - "+str(file))

        # Ok
        self.PrintOK()
        return True

    def checkMatplotlib(self):

        self.PrintLibrary("Matplotlib")

        # Checking if matplotlib is installed on the system
        try:
            import matplotlib
        except:
            self.PrintFAIL(warning=False)
            logging.warning("The python library 'matplotlib' is not found. Please install it with the following command line:")
            logging.warning("install matplotlib")
            return False

        self.PrintOK()
        return True

    def checkRoot(self):

        self.PrintLibrary("Root")

        # Checking if gnuplot is installed on the system
        # Which
        result = ShellCommand.Which('root',all=False,mute=True)
        if len(result)==0:
            self.PrintFAIL(warning=False)
            logging.warning("Root disabled. Plots using ROOT library will not be done.")
            return False
        if self.debug:
            logging.debug("  which:         " + str(result[0]))

        # Which all
        if self.debug:
            result = ShellCommand.Which('root',all=True,mute=True)
            if len(result)==0:
                self.PrintFAIL(warning=False)
                logging.warning("Root disabled. Plots using ROOT library will not be done.")
                return False
            logging.debug("  which-all:     ")
            for file in result:
                logging.debug("    - "+str(file))

        # Ok
        self.PrintOK()
        return True
