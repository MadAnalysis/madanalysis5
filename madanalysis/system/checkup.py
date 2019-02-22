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


from madanalysis.system.user_info          import UserInfo
from madanalysis.system.config_checker     import ConfigChecker
from madanalysis.system.detect_manager     import DetectManager
from string_tools                          import StringTools
import logging
import os

class CheckUp():

    def __init__(self,archi_info,session_info,debug,script):
        self.user_info    = UserInfo()
        self.archi_info   = archi_info
        self.session_info = session_info
        self.debug        = debug
        self.script       = script
        self.checker      = DetectManager(self.archi_info, self.user_info, self.session_info, self.script, self.debug)
        self.logger       = logging.getLogger('MA5')


    def CheckArchitecture(self):

        # Fill with Python info
        import sys
        self.archi_info.python_version = sys.version.replace('\n','')

        # Fill with Platform info
        import platform
        self.archi_info.platform = platform.system()
        self.archi_info.release  = platform.release()

        # Fill with number of cores
        import multiprocessing
        self.archi_info.ncores = multiprocessing.cpu_count()

        # Is Mac
        platform_text= "Platform: "+self.archi_info.platform+" "+self.archi_info.release+" "
        if self.archi_info.platform.lower() in ['darwin','mac','macosx']:
            self.archi_info.isMac = True
            platform_text+='\x1b[32m'+'[MAC/OSX mode]'+'\x1b[0m'
        else:
            self.archi_info.isMac = False
            platform_text+='\x1b[32m'+'[Linux mode]'+'\x1b[0m'
        self.logger.info(platform_text)

        # Info for debug mode
        if self.debug:

            # Machine general
            import platform
            self.logger.debug("")
            self.logger.debug("Machine - Cross platform information")
            self.logger.debug(StringTools.Left("  Machine type:     ",28) + str(platform.machine()))
            self.logger.debug(StringTools.Left("  Processor name:   ",28) + str(platform.processor()))
            self.logger.debug(StringTools.Left("  Platform:         ",28) + str(platform.platform()))
            self.logger.debug(StringTools.Left("  Platform release: ",28) + str(platform.release()))
            self.logger.debug(StringTools.Left("  System:           ",28) + str(platform.system()))
            self.logger.debug(StringTools.Left("  Node:             ",28) + str(platform.node()))
            self.logger.debug(StringTools.Left("  Number of cores:  ",28) + str(self.archi_info.ncores))
            self.logger.debug("")

            # Machine OS
            self.logger.debug("Machine - OS-specific information")
            try:
                tmp=platform.java_ver()
            except:
                tmp=''
            self.logger.debug(StringTools.Left("  Java version:     ",28) + str(tmp))
            try:
                tmp=platform.win32_ver()
            except:
                tmp=''
            self.logger.debug(StringTools.Left("  Windows version:  ",28) + str(tmp))
            try:
                tmp=platform.mac_ver()
            except:
                tmp=''
            self.logger.debug(StringTools.Left("  Mac Os version:   ",28) + str(tmp))
            try:
                tmp=platform.dist()
            except:
                tmp=''
            self.logger.debug(StringTools.Left("  Unix distribution:",28) + str(platform.platform()))
            self.logger.debug("")

        return True


    def CheckSessionInfo(self):

        # Fill with user name
        try:
            import getpass
            self.session_info.username = getpass.getuser()
        except:
            self.session_info.username = 'anonymous'

        # Display user info
        if self.debug:
            self.logger.debug('')
            self.logger.debug("User")
            self.logger.debug(StringTools.Left("  User name:",28)+str(self.session_info.username))
            import os
            self.logger.debug(StringTools.Left("  User ID:",28)+str(os.getuid()))
            self.logger.debug(StringTools.Left("  Expanding folder ~/:",28)+str(os.path.expanduser("~/")))
            for name in ["USER","USERNAME","LNAME","LOGNAME","HOME","HOMEDRIVE","posix","HOMEPATH"]:
                if name in os.environ:
                    tmp=os.environ[name]
                else:
                    tmp=''
                self.logger.debug(StringTools.Left("  Variable $"+name+":",28)+ str(tmp))
            self.logger.debug('')

        # Web access
        self.logger.debug("Web access")
        if self.user_info.webaccess_veto:
            self.session_info.has_web=False
            self.logger.debug('  disable')
        else:
            self.session_info.has_web=True
            self.logger.debug('  enable')
        self.logger.debug('')
 
        # Fill with tmp folder
        import os
        self.logger.debug("Temporary folder")
        tmpdir=''

        # force by the user?
        if self.user_info.tmp_dir!=None:
            self.logger.debug('  Folder forced by the user: '+str(self.user_info.tmp_dir))
            tmpdir=os.path.normpath(self.user_info.tmp_dir)
            if os.path.isdir(tmpdir):
                self.logger.debug('-> found')
            else:
                self.logger.debug('-> not found')
                self.logger.debug('Try to create the folder '+tmpdir+' ...')
                try:
                    os.mkdir(tmpdir)
                except:
                    self.logger.debug('-> impossible to create it')
                    tmpdir=''

        # environment variable
        if tmpdir=='':
            for name in ["TMPDIR","TMP","TEMP"]:
                if name in os.environ:
                    tmp=os.environ[name]
                else:
                    tmp=''
                self.logger.debug(StringTools.Left("  Variable $"+name+":",28)+ str(tmp))
                if tmp!='' and tmpdir=='':
                    tmp=os.path.normpath(tmp)
                    self.logger.debug('Check if the folder '+tmp+' exists ...')
                    if os.path.isdir(tmp):
                        self.logger.debug('-> found')
                        tmpdir=tmp
                    else:
                        self.logger.debug('-> not found')
                        self.logger.debug('Try to create this folder ...')
                        try:
                            os.mkdir(tmp)
                            self.logger.debug('-> ok')
                            tmpdir=tmp
                        except:
                            self.logger.debug('-> impossible to create it')
        
        # /tmp/ + username
        if tmpdir=='':
            pathname = os.path.normpath('/tmp/'+self.session_info.username)
            self.logger.debug('Check if the folder '+pathname+' exists ...')
            if os.path.isdir(pathname):
                self.logger.debug('-> found')
                tmpdir=pathname
            else:
                self.logger.debug('-> not found')
                self.logger.debug('Try to create the folder '+pathname+' ...')
                try:
                    os.mkdir(pathname)
                    tmpdir=pathname
                except:
                    self.logger.debug('-> impossible to create it')

        if tmpdir!='':
            self.session_info.tmpdir = tmpdir
            self.logger.debug('temporary folder will be used for MA5: '+tmpdir)
        else:
            self.logger.error('Impossible to create a tmp folder')
            return False
        self.logger.debug('')

        # Download dir
        self.logger.debug("Download dir")
        tmpdir=''

        # -> forced by the user?
        if self.user_info.download_dir!=None:
            self.logger.debug('  Folder forced by the user: '+str(self.user_info.download_dir))
            tmpdir=os.path.normpath(self.user_info.download_dir)
            if os.path.isdir(tmpdir):
                self.logger.debug('-> found')
            else:
                self.logger.debug('-> not found')
                self.logger.debug('Try to create the folder '+tmpdir+' ...')
                try:
                    os.mkdir(tmpdir)
                except:
                    self.logger.debug('-> impossible to create it')
                    tmpdir=''
        
        # -> temporary folder + 'MA5_download'
        if tmpdir=='':
            pathname = os.path.normpath(self.session_info.tmpdir+'/MA5_downloads/')
            self.logger.debug('Check if the folder '+pathname+' exists ...')
            if os.path.isdir(pathname):
                self.logger.debug('-> found')
                tmpdir=pathname
            else:
                self.logger.debug('-> not found')
                self.logger.debug('Try to create the folder '+pathname+' ...')
                try:
                    os.mkdir(pathname)
                    tmpdir=pathname
                except:
                    self.logger.debug('-> impossible to create it')

        if tmpdir!='':
            self.session_info.downloaddir = tmpdir
            self.logger.debug('download folder will be used for MA5: '+tmpdir)
        else:
            self.logger.error('Impossible to create a download folder')
            return False
        self.logger.debug('')


        # Fill with editor program
        self.logger.debug("Text editor")
        self.logger.debug("Look for the global variable $EDITOR ...")
        if 'EDITOR' in os.environ:
            self.session_info.editor = os.environ['EDITOR']
            self.session_info.editor = self.session_info.editor.lstrip()
            self.session_info.editor = self.session_info.editor.rstrip()
            self.logger.debug("-> variable $EDITOR found : "+self.session_info.editor)
        else:
            self.session_info.editor = 'vi'
            self.logger.debug('-> variable not found. VI editor is set by default.')
        if self.session_info.editor=='':
            self.session_info.editor = 'vi'
            self.logger.debug('-> variable empty. VI editor is set by default.')
        self.logger.debug('')

        # Ok
        return True

    def ReadUserOptions(self):
        # Reading user options
        self.logger.info("Reading user settings ...")
        filename = self.archi_info.ma5dir+'/madanalysis/input/installation_options.dat'
        if not self.user_info.ReadUserOptions(filename):
            return False
        return True

    def CheckMandatoryPackages(self):
        # Mandatory packages
        self.logger.info("Checking mandatory packages:")

        if not self.checker.Execute('python'):
            return False
        if not self.checker.Execute('gpp'):
            return False
        if not self.checker.Execute('make'):
            return False
        return True

    def CheckOptionalGraphicalPackages(self):
        # Optional packages
        self.logger.info("Checking optional packages devoted to histogramming:")

        if not self.checker.Execute('root_graphical'):
            return False
        if not self.checker.Execute('matplotlib'):
            return False
        if not self.checker.Execute('pdflatex'):
            return False
        if not self.checker.Execute('latex'):
            return False
        return True

    def CheckOptionalProcessingPackages(self):
        # Optional packages
        self.logger.info("Checking optional packages devoted to data processing:")
        checker2 = ConfigChecker(self.archi_info, self.user_info, self.session_info, self.script, self.debug)

        if not self.checker.Execute('zlib'):
            return False
        if not self.checker.Execute('fastjet'):
            return False
        if not self.checker.Execute('root'):
            return False

        self.archi_info.has_delphes           = checker2.checkDelphes()
        self.archi_info.has_delphesMA5tune    = checker2.checkDelphesMA5tune()
        return True


    def CheckOptionalReinterpretationPackages(self):
        # Optional packages
        self.logger.info("Checking optional packages devoted to reinterpretation:")

        if not self.checker.Execute('scipy'):
            return False
        if not self.checker.Execute('pad'):
            return False
        if not self.checker.Execute('padma5'):
            return False
        return True


    def CreateSymLink(self,source,destination):

        # Is it a good source
        if source=='':
            self.logger.error('source empty for creating symbolic link: '+source)

        # Is there a previous link?
        if os.path.islink(destination):
            try:
                os.remove(destination)
            except:
                self.logger.error('impossible to remove the file '+destination)
                return False

        # Does the destination exist but it is a file or a folder
        elif os.path.isfile(destination):
            self.logger.error('creating symbolic link: destination already exist and it is file')
            return False
        elif os.path.isdir(destination):
            self.logger.error('creating symbolic link: destination already exist and it is folder')
            return False

        # Creating a link
        logging.getLogger('MA5').debug('Creating symbolic link from '+source)
        logging.getLogger('MA5').debug('                       to   '+destination+' ...')
        try:
            os.symlink(source,destination)
        except:
            self.logger.error('impossible to create the link '+destination)
            return False

        return True

        
    def SetFolder(self):

        # Reset the pieces of environment variables
        self.archi_info.toPATH1=[]   # First in PATH variable 
        self.archi_info.toPATH2=[]   # Last  in PATH variable
        self.archi_info.toLDPATH1=[] # First in (DY)LD_LIBRARY_PATH
        self.archi_info.toLDPATH2=[] # Last  in (DY)LD_LIBRARY_PATH

        # Creating folder Lib if not found
        folder=os.path.normpath(self.archi_info.ma5dir+'/tools/SampleAnalyzer/Lib/')
        if not os.path.isdir(folder):
            try:
                os.mkdir(folder)
            except:
                self.logger.error('impossible to create the folder '+folder)
        self.archi_info.toLDPATH1.append(folder)
                
        # Creating folder ExternalSymLink if not found
        folder=os.path.normpath(self.archi_info.ma5dir+'/tools/SampleAnalyzer/ExternalSymLink')
        if not os.path.isdir(folder):
            try:
                os.mkdir(folder)
            except:
                self.logger.error('impossible to create the folder '+folder)

        # Creating folder ExternalSymLink/Lib if not found
        folder=os.path.normpath(self.archi_info.ma5dir+'/tools/SampleAnalyzer/ExternalSymLink/Lib')
        if not os.path.isdir(folder):
            try:
                os.mkdir(folder)
            except:
                self.logger.error('impossible to create the folder '+folder)

        self.archi_info.toLDPATH1.append(folder)
        folderSymLinkLib=folder

        # Creating folder ExternalSymLink/Bin if not found
        folder=os.path.normpath(self.archi_info.ma5dir+'/tools/SampleAnalyzer/ExternalSymLink/Bin')
        if not os.path.isdir(folder):
            try:
                os.mkdir(folder)
            except:
                self.logger.error('impossible to create the folder '+folder)

        self.archi_info.toPATH1.append(folder)
        folderSymLinkBin=folder

        # ROOT
        if self.archi_info.has_root:
            for source in self.archi_info.root_original_bins:
                destination=os.path.normpath(folderSymLinkBin+'/'+source.split('/')[-1])
                self.CreateSymLink(source,destination)

            if 1: #self.archi_info.root_priority:
                self.archi_info.toLDPATH1.append(self.archi_info.root_lib_path)
                self.archi_info.toPATH1.append(self.archi_info.root_bin_path)
            else:
                self.archi_info.toLDPATH2.append(self.archi_info.root_lib_path)
                self.archi_info.toPATH2.append(self.archi_info.root_bin_path)

        # FASTJET
        if self.archi_info.has_fastjet:
            for source in self.archi_info.fastjet_original_bins:
                destination=os.path.normpath(folderSymLinkBin+'/'+source.split('/')[-1])
                self.CreateSymLink(source,destination)

            if 1: #self.archi_info.fastjet_priority:
                self.archi_info.toPATH1.append(self.archi_info.fastjet_bin_path)
                for path in self.archi_info.fastjet_lib_paths:
                    self.archi_info.toLDPATH1.append(path)
            else:
                self.archi_info.toPATH2.append(self.archi_info.fastjet_bin_path)
                for path in self.archi_info.fastjet_lib_paths:
                    self.archi_info.toLDPATH2.append(path)

        # ZLIB
        if self.archi_info.has_zlib:
            for source in self.archi_info.zlib_original_libs:
                destination=os.path.normpath(folderSymLinkLib+'/'+source.split('/')[-1])
                self.CreateSymLink(source,destination)

#            if self.archi_info.zlib_priority:
#                self.archi_info.toLDPATH1.append(self.archi_info.zlib_lib_path)
#            else:
#                self.archi_info.toLDPATH2.append(self.archi_info.zlib_lib_path)

        # DELPHES
        if self.archi_info.has_delphes:
            for source in self.archi_info.delphes_original_libs:
                destination=os.path.normpath(folderSymLinkLib+'/'+source.split('/')[-1])
                self.CreateSymLink(source,destination)

#            if self.archi_info.delphes_priority:
#                for path in self.archi_info.delphes_lib_paths:
#                    self.archi_info.toLDPATH1.append(path)
#            else:
#                for path in self.archi_info.delphes_lib_paths:
#                    self.archi_info.toLDPATH2.append(path)

        # DELPHES MA5tune
        if self.archi_info.has_delphesMA5tune:
            for source in self.archi_info.delphesMA5tune_original_libs:
                destination=os.path.normpath(folderSymLinkLib+'/'+source.split('/')[-1])
                self.CreateSymLink(source,destination)
#            if self.archi_info.delphesMA5tune_priority:
#                for path in self.archi_info.delphesMA5tune_lib_paths:
#                    self.archi_info.toLDPATH1.append(path)
#            else:
#                for path in self.archi_info.delphesMA5tune_lib_paths:
#                    self.archi_info.toLDPATH2.append(path)


        # Setting environment variables
        self.logger.debug('-------- BEGIN: set environment variables --------')

        # - PATH
        if 'PATH' not in os.environ:
            os.environ['PATH'] = ''
        self.logger.debug('before PATH='+str(os.environ['PATH']))
        self.logger.debug('--------')
        os.environ['PATH'] = ':'.join(self.archi_info.toPATH1) + ":" + \
                             os.environ['PATH'] + ":" + \
                             ':'.join(self.archi_info.toPATH2)
        self.logger.debug('after PATH='+str(os.environ['PATH']))
        self.logger.debug('--------')

        # - LD_LIBRARY_PATH     
        if 'LD_LIBRARY_PATH' not in os.environ:
            os.environ['LD_LIBRARY_PATH'] = ''
        self.logger.debug('before LD_LIBRARY_PATH='+str(os.environ['LD_LIBRARY_PATH']))
        self.logger.debug('--------')
        os.environ['LD_LIBRARY_PATH'] = ':'.join(self.archi_info.toLDPATH1) + ":" + \
                                        os.environ['LD_LIBRARY_PATH'] + ":" + \
                                        ':'.join(self.archi_info.toLDPATH2)
        self.logger.debug('after LD_LIBRARY_PATH='+str(os.environ['LD_LIBRARY_PATH']))
        self.logger.debug('--------')

        # - DYLD_LIBRARY_PATH     
        if 'DYLD_LIBRARY_PATH' not in os.environ:
            os.environ['DYLD_LIBRARY_PATH'] = ''
        self.logger.debug('before DYLD_LIBRARY_PATH='+str(os.environ['DYLD_LIBRARY_PATH']))
        self.logger.debug('--------')
        if self.archi_info.isMac:
            os.environ['DYLD_LIBRARY_PATH'] = ':'.join(self.archi_info.toLDPATH1) + ":" + \
                                              os.environ['DYLD_LIBRARY_PATH'] + ":" + \
                                              ':'.join(self.archi_info.toLDPATH2)
        self.logger.debug('after DYLD_LIBRARY_PATH='+str(os.environ['DYLD_LIBRARY_PATH']))
        self.logger.debug('--------')

        self.logger.debug('-------- END: set environment variables --------')

        return True
