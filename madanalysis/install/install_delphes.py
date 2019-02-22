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


from madanalysis.install.install_service    import InstallService
from madanalysis.system.user_info           import UserInfo
from madanalysis.system.config_checker      import ConfigChecker
from madanalysis.IOinterface.library_writer import LibraryWriter
from madanalysis.IOinterface.folder_writer  import FolderWriter
from madanalysis.system.checkup             import CheckUp
from shell_command import ShellCommand
import os
import sys
import logging
import glob
import shutil

class InstallDelphes:

    def __init__(self,main,package):
        self.main        = main
        self.package     = 'delphes'
        if package == 'delphesma5tune':
            self.package = 'delphesMA5tune'
        self.toolsdir    = os.path.join(self.main.archi_info.ma5dir,'tools')
        self.installdir  = os.path.join(self.toolsdir,self.package)
        self.tmpdir      = self.main.session_info.tmpdir
        self.downloaddir = self.main.session_info.downloaddir
        self.untardir    = os.path.join(self.tmpdir, 'MA5_'+self.package)
        self.ncores      = 1
#        self.files = {"delphes.tar.gz" : "http://cp3.irmp.ucl.ac.be/downloads/Delphes-3.1.1.tar.gz"}
#        self.files = {"delphes.tar.gz" : "http://cp3.irmp.ucl.ac.be/downloads/Delphes-3.3.0.tar.gz"}
#        self.files = {"delphes.tar.gz" : "http://cp3.irmp.ucl.ac.be/downloads/Delphes-3.3.1.tar.gz"}
#        self.files = {"delphes.tar.gz" : "http://cp3.irmp.ucl.ac.be/downloads/Delphes-3.3.3.tar.gz"}
        if package == 'delphesma5tune':
            self.files = {package+".tar.gz" : "http://cp3.irmp.ucl.ac.be/downloads/Delphes-3.4.1.tar.gz"}
        else:
            self.files = {package+".tar.gz" : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/WikiStart/delphes342pre.tar.gz"} # Delphes for LLP not release yet
        self.logger = logging.getLogger('MA5')


    def Detect(self):
        if not os.path.isdir(self.toolsdir):
            self.logger.debug("The folder '"+self.toolsdir+"' is not found")
            return False
        if not os.path.isdir(self.installdir):
            self.logger.debug("The folder "+self.installdir+"' is not found")
            return False
        return True


    def Remove(self,question=True):
        from madanalysis.IOinterface.folder_writer import FolderWriter
        return FolderWriter.RemoveDirectory(self.installdir,question)


    def GetNcores(self):
        self.ncores = InstallService.get_ncores(self.main.archi_info.ncores,\
                                                self.main.forced)


    def CreatePackageFolder(self):
        if not InstallService.create_tools_folder(self.toolsdir):
            return False
        if not InstallService.create_package_folder(self.toolsdir,self.package):
            return False
        return True


    def CreateTmpFolder(self):
        ok = InstallService.prepare_tmp(self.untardir, self.downloaddir)
        if ok:
            self.tmpdir=self.untardir
        return ok


    def Download(self):
        # Checking connection with MA5 web site
        if not InstallService.check_ma5site():
            return False
        # Launching wget
        logname = os.path.normpath(self.installdir+'/wget.log')
        if not InstallService.wget(self.files,logname,self.downloaddir):
            return False
        # Ok
        return True


    def Unpack(self):
        # Logname
        logname = os.path.normpath(self.installdir+'/unpack.log')

        # Unpacking the tarball
        ok, packagedir = InstallService.untar(logname, self.downloaddir,self.tmpdir,
          self.package.lower()+'.tar.gz')
        if not ok:
            return False

        # Patching delphesMA5tune
        if self.package == 'delphesMA5tune':
            # Copying the patch
            self.logger.debug('Copying the patch ...')
            input=self.toolsdir+'/SampleAnalyzer/Interfaces/delphesMA5tune/patch_delphesMA5tune.tgz'
            output=packagedir+'/patch_delphesMA5tune.tgz'
            try:
                shutil.copy(input,output)
            except:
                self.logger.error('impossible to copy the patch '+input+' to '+output)
                return False

            # Unpacking the folder
            logname = os.path.normpath(self.installdir+'/unpack_patch.log')
            theCommands=['tar','xzf','patch_delphesMA5tune.tgz']
            self.logger.debug('shell command: '+' '.join(theCommands))
            ok, out= ShellCommand.ExecuteWithLog(theCommands,logname,packagedir,silent=False)
            if not ok:
                self.logger.error('impossible to untar the patch '+output)
                return False

            # Applying the patch
            logname = os.path.normpath(self.installdir+'/patch.log')
            theCommands=[sys.executable,'patch.py']
            self.logger.debug('shell command: '+' '.join(theCommands))
            ok, out= ShellCommand.ExecuteWithLog(theCommands,logname,packagedir,silent=False)
            if not ok:
                self.logger.error('impossible to apply the patch '+output)
                return False

        # Getting the list of files
        self.logger.debug('Getting the list of files ...')
        myfiles=glob.glob(packagedir+'/*')
        self.logger.debug('=> '+str(myfiles))

        # Moving files from packagedir to installdir
        self.logger.debug('Moving files from '+packagedir+' to '+self.installdir+' ...')
        for myfile in myfiles:
            myfile2=myfile.split('/')[-1]
            if os.path.isdir(myfile):
                try:
                    shutil.copytree(myfile,self.installdir+'/'+myfile2)
                except:
                    self.logger.error('impossible to move the file/folder '+myfile+' from '+packagedir+' to '+self.installdir)
                    return False
            else:
                try:
                    shutil.copy(myfile,self.installdir+'/'+myfile2)
                except:
                    self.logger.error('impossible to move the file/folder '+myfile+' from '+packagedir+' to '+self.installdir)
                    return False


# No need with the last release of ROOT
#        if self.package=='delphes':
#            # Updating DelphesFormula
#            filename = self.installdir+'/classes/DelphesFormula.cc'
#            self.logger.debug('Updating files '+filename+ ': adding d0\n')
#            self.AddD0(filename)

        # Updating Makefile
        filename = self.installdir+'/doc/genMakefile.tcl'
        self.logger.debug('Updating files '+filename+ ': no CMSSW\n')
        self.SwitchOffCMSSW(filename)

        # Updating ExRootTask
        filename = self.installdir+'/external/ExRootAnalysis/ExRootTask.cc'
        self.logger.debug('Updating files: commenting out lines in: '+filename+' ...')
        self.CommentLines(filename,[64,65,66],'//')

        # Updating ExRootTask
        filename = self.installdir+'/external/ExRootAnalysis/ExRootConfReader.cc'
        self.logger.debug('Updating files: commenting out lines in: '+filename+' ...')
        self.CommentLines(filename,[177,178,179,180],'//')

        # Adding files
        if self.package=='delphes':
            filesToAdd = ["MA5GenParticleFilter"]
        elif self.package=='delphesMA5tune':
            filesToAdd = ["MA5GenParticleFilter"]
        if not self.CopyFiles(filesToAdd):
            return False
        if not self.UpdateDictionnary(filesToAdd):
            return False

        # Ok
        return True


    def Configure(self):
        # Known delphes issues: generate issues because it uses tcslsh command
        # Input
        theCommands=['./configure']
        logname=os.path.normpath(self.installdir+'/configuration.log')
        # Execute
        self.logger.debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,logname,self.installdir,silent=False)

        # Updating the Makefile
        self.logger.debug('Updating the Makefiles: no CMSSW\n')
        self.SwitchOffCMSSW(os.path.join(self.installdir, 'Makefile'))

        # return result
        if not ok:
            self.logger.error('impossible to configure the project. For more details, see the log file:')
            self.logger.error(logname)
        return ok


    def Build(self):

        # Input
        theCommands=['make', '-j'+str(self.ncores)]
        logname=os.path.normpath(self.installdir+'/compilation.log')
        # Execute
        self.logger.debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,logname,self.installdir,silent=False)
        # return result
        if not ok:
            self.logger.error('impossible to build the project. For more details, see the log file:')
            self.logger.error(logname)
        return ok


    def Clean(self):
        # Input
        theCommands=['make','clean']
        logname=os.path.normpath(self.installdir+'/clean.log')
        # Execute
        self.logger.debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,logname,self.installdir,silent=False)

        # return result
        if not ok:
            self.logger.error('impossible to clean the project. For more details, see the log file:')
            self.logger.error(logname)
        return ok



    def Check(self):
        # Check folders
        dirs = [self.installdir+"/modules", self.installdir+"/classes"]
        for dir in dirs:
            if not os.path.isdir(dir):
                self.logger.error('folder '+dir+' is missing.')
                self.display_log()
                return False

        # Check one header file
        if not os.path.isfile(self.installdir+'/modules/ParticlePropagator.h'):
            self.logger.error("header labeled 'modules/ParticlePropagator.h' is missing.")
            self.display_log()
            return False

        # Check the libraries
        if self.package=='delphes':
            libname = 'libDelphes'
        elif self.package=='delphesMA5tune':
            libname = 'libDelphesMA5tune'
        check = [os.path.isfile(os.path.join(self.installdir,libname+'.'+ext)) for ext in ['a','so','dylib']]

        if not any(check):
            self.logger.error("The " + self.package + ' library is missing.')
            self.display_log()
            return False

        return True

    def display_log(self):
        self.logger.error("More details can be found into the log files:")
        self.logger.error(" - "+os.path.normpath(self.installdir+"/wget.log"))
        self.logger.error(" - "+os.path.normpath(self.installdir+"/unpack.log"))
        self.logger.error(" - "+os.path.normpath(self.installdir+"/configuration.log"))
        self.logger.error(" - "+os.path.normpath(self.installdir+"/compilation.log"))
        self.logger.error(" - "+os.path.normpath(self.installdir+"/clean.log"))

    def NeedToRestart(self):
        return True


    def CommentLines(self,filename,thelines,charac='//'):
        # open input file
        try:
            input = open(filename)
        except:
            self.logger.error("impossible to read the file:" + filename)
            return False

        # open output file
        try:
            output = open(filename+'.savema5','w')
        except:
            self.logger.error("impossible to read the file:" + filename+'.savema5')
            return False

        # lines
        ind = 0
        for line in input:
            ind+=1
            if ind in thelines:
                output.write(charac+' '+line)
            else:
                output.write(line)

        #close
        input.close()
        output.close()

        try:
            shutil.copy(filename+'.savema5',filename)
        except:
            self.logger.error("impossible to copy "+filename+'.savema5 in '+filename)
            return False

        return True


    def SwitchOffCMSSW(self,filename):
        # open input file
        try:
            input = open(filename)
        except:
            self.logger.error("impossible to read the file:" + filename)
            return False

        # open output file
        try:
            output = open(filename+'.savema5','w')
        except:
            self.logger.error("impossible to read the file:" + filename+'.savema5')
            return False

        # lines
        for line in input:
            output.write(line.replace('HAS_CMSSW = true','HAS_CMSSW = false'))

        #close
        input.close()
        output.close()

        try:
            shutil.copy(filename+'.savema5',filename)
        except:
            self.logger.error("impossible to copy "+filename+'.savema5 in '+filename)
            return False

        return True


    def AddD0(self,filename):
        # open input file
        try:
            input = open(filename)
        except:
            self.logger.error("impossible to read the file:" + filename)
            return False

        # open output file
        try:
            output = open(filename+'.savema5','w')
        except:
            self.logger.error("impossible to read the file:" + filename+'.savema5')
            return False

        # lines
        for line in input:
            line2=line.lstrip()
            line2=line2.rstrip()
            line2=line2.replace(' ','')
            if line2.startswith('buffer.ReplaceAll("energy"'):
                output.write(line)
                output.write('  buffer.ReplaceAll("d0",     "t");\n')
            else:
                output.write(line)
 
        #close
        input.close()
        output.close()

        try:
            shutil.copy(filename+'.savema5',filename)
        except:
            self.logger.error("impossible to copy "+filename+'.savema5 in '+filename)
            return False

        return True


    def CopyFiles(self,filesToAdd):

        for file in filesToAdd:
            logging.debug("Add module *"+file+"* ...")


            inputname  = self.main.archi_info.ma5dir+'/tools/SampleAnalyzer/Interfaces/delphes/'+file+'.cc.install'
            outputname = self.installdir+'/modules/'+file+'.cc'
            self.logger.debug("Copying file from '"+inputname+"' to '"+outputname+'" ...')

            try:
                shutil.copy(inputname,outputname)
            except:
                self.logger.error("impossible to copy "+inputname+' in '+outputname)
                return False
             
            inputname  = self.main.archi_info.ma5dir+'/tools/SampleAnalyzer/Interfaces/delphes/'+file+'.h.install'
            outputname = self.installdir+'/modules/'+file+'.h'
            self.logger.debug("Copying file from '"+inputname+"' to '"+outputname+'" ...')

            try:
                shutil.copy(inputname,outputname)
            except:
                self.logger.error("impossible to copy "+inputname+' in '+outputname)
                return False

        return True


    def UpdateDictionnary(self,filesToAdd):

        inputname = self.installdir+'/modules/ModulesLinkDef.h'
        self.logger.debug("Updating the Delphes dictionnary '"+inputname+'" ...')
        try:
            input = open(inputname)
        except:
            self.logger.error('impossible to open '+inputname)
            return False

        outputname = self.installdir+'/modules/ModulesLinkDef.savema5'
        try:
            output = open(outputname,'w')
        except:
            self.logger.error('impossible to write '+outputname)
            return False

        for line in input:
            myline = line.lstrip()
            myline = myline.rstrip()
            words = myline.split()

            if len(words)>=2:
                if words[0]=='#include' and words[1]=='"modules/ExampleModule.h"':
                    for file in filesToAdd:
                        output.write('#include "modules/'+file+'.h"\n')
            if len(words)>=5:
                if words[0]=='#pragma' and words[1]=='link' and \
                   words[2]=='C++' and words[3]=='class' and \
                   words[4]=='ExampleModule+;':
                    for file in filesToAdd:
                        output.write('#pragma link C++ class '+file+'+;\n')

            output.write(line)

        input.close()
        output.close()

        try:
            shutil.copy(outputname,inputname)
        except:
            self.logger.error("impossible to copy "+outputname+' in '+inputname)
            return False

        return True

    def Deactivate(self):
        ## INIT
        if self.package=='delphes':
            libpaths  = self.main.archi_info.delphes_lib_paths
            originals = self.main.archi_info.delphes_original_libs
            key       = 'Delphes'
        elif self.package=='delphesMA5tune':
            libpaths  = self.main.archi_info.delphesMA5tune_lib_paths
            originals = self.main.archi_info.delphesMA5tune_original_libs
            key       = 'DelphesMA5tune'

        ## Checking whether anything has to be deactivated
        if libpaths == []:
            return True
        if any([('DEACT' in x) for x in libpaths]):
            return True
        if os.path.isdir(libpaths[0]):
            self.logger.warning(self.package + " is installed. Deactivating it.")

        # Removing the symbolic links
        for to_remove in [x for x in originals if (os.path.exists(x) and 'ExternalSymLink' in x)]:
            os.remove(x)

        # Updating the architecture
        deac_path = libpaths[0].replace(libpaths[0].split('/')[-1],"DEACT_"+libpaths[0].split('/')[-1])
        self.main.archi_info.toLDPATH1 = [x for x in self.main.archi_info.toLDPATH1 if not self.package in x]
        if key in self.main.archi_info.libraries.keys():
            del self.main.archi_info.libraries[key]

        # If the deactivated directory already exists -> suppression
        if os.path.isdir(deac_path):
            if not FolderWriter.RemoveDirectory(os.path.normpath(deac_path),True):
                    return False

        # cleaning delphes + the samplanalyzer interface to delphes
        shutil.move(libpaths[0],deac_path)
        files = [os.path.join(self.main.archi_info.ma5dir,'tools','SampleAnalyzer','Lib','lib'+self.package+\
            '.'+ext) for ext in  ['so', 'a', 'dylib'] ]
        files = files + [os.path.join(self.main.archi_info.ma5dir,'tools','SampleAnalyzer','Interfaces',
           'Makefile_'+self.package) ]
        files = files + [os.path.join(self.main.archi_info.ma5dir,'tools','SampleAnalyzer','Interfaces',
           fname+'_'+self.package+'.log') for fname in [ 'compilation','linking', 'cleanup']]
        for to_remove in files:
            if os.path.isfile(to_remove):
                os.remove(to_remove)

        ## updating the architecture
        if self.package=='delphes':
            self.main.archi_info.has_delphes           = False
            self.main.archi_info.delphes_priority      = False
            self.main.archi_info.delphes_lib_paths     = []
            self.main.archi_info.delphes_inc_paths     = []
            self.main.archi_info.delphes_lib           = ""
            self.main.archi_info.delphes_original_libs = []
        elif self.package=='delphesMA5tune':
            self.main.archi_info.has_delphesMA5tune           = False
            self.main.archi_info.delphesMA5tune_priority      = False
            self.main.archi_info.delphesMA5tune_lib_paths     = []
            self.main.archi_info.delphesMA5tune_inc_paths     = []
            self.main.archi_info.delphesMA5tune_lib           = ""
            self.main.archi_info.delphesMA5tune_original_libs = []

        return True


    ## Activtation of an uninstalled delphes
    # output =  1: activation successfull.
    #           0: nothing is done.
    #          -1: error
    def Activate(self):
        ## init
        self.logger.debug('Starting the activation of ' + self.package)
        user_info = UserInfo()
        user_info.ReadUserOptions(self.main.archi_info.ma5dir+'/madanalysis/input/installation_options.dat')

        ## Checking what is installed
        self.logger.debug('Checking if ' + self.package + 'was previously installed')
        checker = ConfigChecker(self.main.archi_info, user_info, self.main.session_info, self.main.script,
          False)
        if self.package=='delphes':
            has_delphes = checker.checkDelphes(True)
        elif self.package=='delphesMA5tune':
            has_delphes = checker.checkDelphesMA5tune(True)
        self.logger.debug("  " + self.package + ' available? -> ' + str(has_delphes))

        ## Nothing to activate
        if not has_delphes:
            return 1

        # Paths and architecture update
        def activate(onelib):
            return onelib.replace("DEACT_","")
        def libclean(onelib):
            if len(onelib)>2:
                del onelib[-1]
                del onelib[-1]
            return onelib
        if self.package=='delphes':
            # shortcuts
            delphes_path = self.main.archi_info.delphes_lib_paths[0]
            originals    = self.main.archi_info.delphes_original_libs
            # architecture
            self.main.archi_info.delphes_lib           = activate(self.main.archi_info.delphes_lib)
            self.main.archi_info.delphes_original_libs = [activate(x) for x in originals]
            self.main.archi_info.delphes_inc_paths     = libclean(
                [activate(x) for x in self.main.archi_info.delphes_inc_paths ])
            self.main.archi_info.delphes_lib_paths     = libclean(
                [activate(x) for x in self.main.archi_info.delphes_lib_paths ])
            # Updating shortcuts
            originals    = self.main.archi_info.delphes_original_libs
        elif self.package=='delphesMA5tune':
            # shortcuts
            delphes_path = self.main.archi_info.delphesMA5tune_lib_paths[0]
            originals    = self.main.archi_info.delphesMA5tune_original_libs
            # architecture
            self.main.archi_info.delphesMA5tune_lib           = activate(self.main.archi_info.delphesMA5tune_lib)
            self.main.archi_info.delphesMA5tune_original_libs = [activate(x) for x in originals]
            self.main.archi_info.delphesMA5tune_inc_paths     = libclean(
                [ activate(x) for x in self.main.archi_info.delphesMA5tune_inc_paths ])
            self.main.archi_info.delphesMA5tune_lib_paths     = libclean(
                [ activate(x) for x in self.main.archi_info.delphesMA5tune_lib_paths ])
            # Updating shortcuts
            originals    = self.main.archi_info.delphesMA5tune_original_libs
        activated_path = activate(delphes_path)

        # do we have to activate delphes?
        if not 'DEACT' in delphes_path:
            return 0
        self.logger.warning(self.package + " is deactivated. Activating it.")

        # renaming the directory
        shutil.move(delphes_path,activated_path)

        # creating the virtual links
        checkup = CheckUp(self.main.archi_info, self.main.session_info, False, self.main.script)
        for link in [x.split('/')[-1] for x in originals]:
            dest = os.path.join(self.main.archi_info.ma5dir,'tools','SampleAnalyzer', 'ExternalSymLink', link)
            if not checkup.CreateSymLink(x,dest):
                return -1

        # Compiler setup
        compiler = LibraryWriter('lib',self.main)
        ncores = compiler.get_ncores2()
        smpl_path = os.path.join(self.main.archi_info.ma5dir, 'tools', 'SampleAnalyzer')
        from madanalysis.build.setup_writer import SetupWriter
        SetupWriter.WriteSetupFile(True , smpl_path, self.main.archi_info)
        SetupWriter.WriteSetupFile(False, smpl_path, self.main.archi_info)

        if self.package=='delphes':
            self.main.archi_info.has_delphes      = True
            self.main.archi_info.delphes_priority = True
            key     = 'Delphes'
            antikey = 'delphesMA5tune'
        elif self.package=='delphesMA5tune':
            self.main.archi_info.has_delphesMA5tune      = True
            self.main.archi_info.delphesMA5tune_priority = True
            key     = 'DelphesMA5tune'
            antikey = 'delphes'

        install_path = os.path.join(self.main.archi_info.ma5dir,'tools',self.package)
        mylib = os.path.join(install_path,'lib' + key + '.so')
        self.main.archi_info.libraries[key] = mylib + ":" + str(os.stat(mylib).st_mtime)
        self.main.archi_info.toLDPATH1 = [x for x in self.main.archi_info.toLDPATH1 if not antikey in x]
        self.main.archi_info.toLDPATH1.append(install_path)

        # Makefile
        for pack in [self.package, 'root', 'process']:
            if not compiler.WriteMakefileForInterfaces(pack):
                self.logger.error("library building aborted.")
                return -1

        # Cleaning
        for pack in [self.package, 'root']:
            if not compiler.MrProper(pack,os.path.join(smpl_path,'Interfaces')):
                self.logger.error("Library '" + pack + "' precleaning aborted.")
                return -1
        if not compiler.MrProper('process', os.path.join(smpl_path,'Process')):
            self.logger.error("Library 'process' precleaning aborted.")
            return -1

        # Compiling
        for pack in [self.package, 'root']:
            if not compiler.Compile(ncores, pack, os.path.join(smpl_path, 'Interfaces')):
                self.logger.error("Library '" + pack + "' compilation aborted.")
                return -1
        if not compiler.Compile(ncores, 'process', os.path.join(smpl_path, 'Process')):
            self.logger.error("Library 'process' compilation aborted.")
            return -1

        # Linking
        for pack in [self.package, 'root']:
            if not compiler.Link(pack, os.path.join(smpl_path, 'Interfaces')):
                self.logger.error("Library '" + pack + "' linking aborted.")
                return -1
        if not compiler.Link('process', os.path.join(smpl_path, 'Process')):
            self.logger.error("Library 'process' linking aborted.")
            return -1

        # Checking
        for pack in [self.package, 'root', 'process']:
            if not os.path.isfile(os.path.join(smpl_path, 'Lib', 'lib'+pack+'_for_ma5.so')):
                self.logger.error("Library '" + pack + "' checking aborted.")
                return -1

        # Cleaning
        for pack in [self.package, 'root']:
            if not compiler.Clean(pack,os.path.join(smpl_path,'Interfaces')):
                self.logger.error("Library '" + pack + "' cleaning aborted.")
                return -1
        if not compiler.Clean('process', os.path.join(smpl_path,'Process')):
            self.logger.error("Library 'process' cleaning aborted.")
            return -1

        # Paths
        lev=self.logger.getEffectiveLevel()
        self.logger.setLevel(100)
        checkup = CheckUp(self.main.archi_info, self.main.session_info, False, self.main.script)
        if not checkup.SetFolder():
            self.logger.error("Problem with the path updates.")
            return -1

        if not self.main.archi_info.save(self.main.archi_info.ma5dir+'/tools/architecture.ma5'):
            return -1
        if not self.main.CheckConfig():
            return -1
        self.logger.setLevel(lev)

        return 1
