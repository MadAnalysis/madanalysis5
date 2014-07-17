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


from madanalysis.selection.instance_name   import InstanceName
from madanalysis.IOinterface.folder_writer import FolderWriter
from madanalysis.IOinterface.job_writer    import JobWriter
from string_tools                          import StringTools
from shell_command                         import ShellCommand
import logging
import shutil
import os
import commands
import subprocess


class LibraryWriter():

    def __init__(self,jobdir,main):
        self.jobdir     = jobdir
        self.main       = main
        self.path       = os.path.normpath(self.main.archi_info.ma5dir+"/tools/")

    def get_ncores(self):
        # Number of cores
        import multiprocessing
        nmaxcores=multiprocessing.cpu_count()
        logging.info("     => How many cores for the compiling? default = max = " +\
                     str(nmaxcores)+"")
        
        if not self.main.forced:
            test=False
            while(not test):
                answer=raw_input("     Answer: ")
                if answer=="":
                    test=True
                    ncores=nmaxcores
                    break
                try:
                    ncores=int(answer)
                except:    
                    test=False
                    continue
                if ncores<=nmaxcores and ncores>0:
                    test=True
                    
        else:
            ncores=nmaxcores
        logging.info("     Number of cores used for the compilation = " +\
                     str(ncores))
        return ncores

    def get_ncores2(self):
        # Number of cores
        import multiprocessing
        nmaxcores=multiprocessing.cpu_count()
        logging.info("   How many cores for the compiling? default = max = " +\
                     str(nmaxcores)+"")
        
        if not self.main.forced:
            test=False
            while(not test):
                answer=raw_input("   Answer: ")
                if answer=="":
                    test=True
                    ncores=nmaxcores
                    break
                try:
                    ncores=int(answer)
                except:    
                    test=False
                    continue
                if ncores<=nmaxcores and ncores>0:
                    test=True
                    
        else:
            ncores=nmaxcores
        logging.info("   => Number of cores used for the compilation = " +\
                     str(ncores))
        return ncores


    def Open(self):
        return FolderWriter.CreateDirectory(self.path,overwrite=True)

    def WriteMakefileForInterfaces(self,package):

        from madanalysis.build.makefile_writer import MakefileWriter
        options=MakefileWriter.MakefileOptions()
        
        # Name of the Makefile
        filename = self.path+"/SampleAnalyzer/Interfaces/Makefile_"+package
        if package=='commons':
            filename = self.path+"/SampleAnalyzer/Commons/Makefile"
        elif package=='process':
            filename = self.path+"/SampleAnalyzer/Process/Makefile"
        elif package=='test_process':
            filename = self.path+"/SampleAnalyzer/Test/Makefile_process"
        elif package=='test_commons':
            filename = self.path+"/SampleAnalyzer/Test/Makefile_commons"
        elif package=='test_zlib':
            filename = self.path+"/SampleAnalyzer/Test/Makefile_zlib"
        elif package=='test_fastjet':
            filename = self.path+"/SampleAnalyzer/Test/Makefile_fastjet"
        elif package=='test_delphes':
            filename = self.path+"/SampleAnalyzer/Test/Makefile_delphes"
        elif package=='test_delphesMA5tune':
            filename = self.path+"/SampleAnalyzer/Test/Makefile_delphesMA5tune"

        # Header
        title=''
        if package=='commons':
            title='SampleAnalyzer commons'
        elif package=='process':
            title='SmapleAnalyzer process'
        elif package=='test_commons':
            title='*commons* test'
        elif package=='test_process':
            title='*process* test'
        elif package=='test_zlib':
            title='*zlib-interface* test'
        elif package=='test_fastjet':
            title='*fastjet-interface* test'
        elif package=='test_delphes':
            title='*delphes-interface* test'
        elif package=='test_delphesMA5tune':
            title='*delphesMA5tune-interface* test'
        else:
            title='interface to '+package

        toRemove=[]
        # Mode
        if package=='fastjet':
            options.has_commons=True
            options.has_fastjet_inc=True
            options.has_fastjet_lib=True
            toRemove.extend(['compilation_fastjet.log','linking_fastjet.log','cleanup_fastjet.log','mrproper_fastjet.log'])
        elif package=='test_fastjet':
            options.has_commons=True
            options.has_fastjet_ma5lib=True
          #  options.has_fastjet_lib=True
            toRemove.extend(['compilation_fastjet.log','linking_fastjet.log','cleanup_fastjet.log','mrproper_fastjet.log','../Bin/TestFastjet.log'])
        elif package=='commons':
            pass
            toRemove.extend(['compilation.log','linking.log','cleanup.log','mrproper.log'])
        elif package=='test_commons':
            options.has_commons  = True
            toRemove.extend(['compilation_commons.log','linking_commons.log','cleanup_commons.log','mrproper_commons.log','../Bin/TestCommons.log'])
        elif package=='zlib':
            options.has_commons  = True
            options.has_zlib_inc = True
            options.has_zlib_lib = True
            toRemove.extend(['compilation_zlib.log','linking_zlib.log','cleanup_zlib.log','mrproper_zlib.log'])
        elif package=='test_zlib':
            options.has_commons  = True
            options.has_zlib_ma5lib = True
          #  options.has_zlib_lib = True
            toRemove.extend(['compilation_zlib.log','linking_zlib.log','cleanup_zlib.log','mrproper_zlib.log','../Bin/TestZlib.log'])
        elif package=='delphes':
            options.has_commons     = True
            options.has_delphes_inc = True
            options.has_delphes_lib = True
            toRemove.extend(['compilation_delphes.log','linking_delphes.log','cleanup_delphes.log','mrproper_delphes.log'])
        elif package=='test_delphes':
            options.has_commons     = True
            options.has_delphes_ma5lib = True
          #  options.has_delphes_lib = True
            toRemove.extend(['compilation_delphes.log','linking_delphes.log','cleanup_delphes.log','mrproper_delphes.log','../Bin/TestDelphes.log'])
        elif package=='delphesMA5tune':
            options.has_commons            = True
            options.has_delphesMA5tune_lib = True
            options.has_delphesMA5tune_inc = True
            toRemove.extend(['compilation_delphesMA5tune.log','linking_delphesMA5tune.log','cleanup_delphesMA5tune.log','mrproper_delphesMA5tune.log'])
        elif package=='test_delphesMA5tune':
            options.has_commons            = True
            options.has_delphesMA5tune_ma5lib = True
         #   options.has_delphesMA5tune_lib = True
            toRemove.extend(['compilation_delphesMA5tune.log','linking_delphesMA5tune.log','cleanup_delphesMA5tune.log','mrproper_delphesMA5tune.log','../Bin/TestDelphesMA5tune.log'])
        elif package=='process':
            options.has_commons=True
            options.has_fastjet_ma5lib        = self.main.archi_info.has_fastjet
            options.has_delphes_ma5lib        = self.main.archi_info.has_delphes
            options.has_delphesMA5tune_ma5lib = self.main.archi_info.has_delphesMA5tune
            options.has_zlib_ma5lib           = self.main.archi_info.has_zlib
          #  options.has_fastjet_lib           = self.main.archi_info.has_fastjet
          #  options.has_delphes_lib           = self.main.archi_info.has_delphes
          #  options.has_delphesMA5tune_lib    = self.main.archi_info.has_delphesMA5tune
          #  options.has_zlib_lib              = self.main.archi_info.has_zlib
            options.has_fastjet_tag           = self.main.archi_info.has_fastjet
            options.has_delphes_tag           = self.main.archi_info.has_delphes
            options.has_delphesMA5tune_tag    = self.main.archi_info.has_delphesMA5tune
            options.has_zlib_tag              = self.main.archi_info.has_zlib
            toRemove.extend(['compilation.log','linking.log','cleanup.log','mrproper.log'])
        elif package=='test_process':
            options.has_commons               = True
            options.has_process               = True
          #  options.has_fastjet_ma5lib        = self.main.archi_info.has_fastjet
          #  options.has_delphes_ma5lib        = self.main.archi_info.has_delphes
          #  options.has_delphesMA5tune_ma5lib = self.main.archi_info.has_delphesMA5tune
          #  options.has_zlib_ma5lib           = self.main.archi_info.has_zlib
          #  options.has_fastjet_lib           = self.main.archi_info.has_fastjet
          #  options.has_delphes_lib           = self.main.archi_info.has_delphes
          #  options.has_delphesMA5tune_lib    = self.main.archi_info.has_delphesMA5tune
          #  options.has_zlib_lib              = self.main.archi_info.has_zlib
          #  options.has_fastjet_tag           = self.main.archi_info.has_fastjet
          #  options.has_delphes_tag           = self.main.archi_info.has_delphes
          #  options.has_delphesMA5tune_tag    = self.main.archi_info.has_delphesMA5tune
          #  options.has_zlib_tag              = self.main.archi_info.has_zlib
            toRemove.extend(['compilation_process.log','linking_process.log','cleanup_process.log','mrproper_process.log','../Bin/TestSampleAnalyzer.log'])

        # file pattern
        if package in ['commons','process']:
            cppfiles = ['*/*.cpp']
            hfiles   = ['*/*.h']
        elif package=='test_commons':
            cppfiles = ['Commons/*.cpp']
            hfiles   = ['Commons/*.h']
        elif package=='test_process':
            cppfiles = ['Process/*.cpp']
            hfiles   = ['Process/*.h']
        elif package=='test_zlib':
            cppfiles = ['Zlib/*.cpp']
            hfiles   = ['Zlib/*.h']
        elif package=='test_fastjet':
            cppfiles = ['Fastjet/*.cpp']
            hfiles   = ['Fastjet/*.h']
        elif package=='test_delphes':
            cppfiles = ['Delphes/*.cpp']
            hfiles   = ['Delphes/*.h']
        elif package=='test_delphesMA5tune':
            cppfiles = ['DelphesMA5tune/*.cpp']
            hfiles   = ['DelphesMA5tune/*.h']
        else:
            cppfiles = [package+'/*.cpp']
            hfiles   = [package+'/*.h']

        # product
        if package=='test_process':
            isLibrary=False
            ProductName='TestSampleAnalyzer'
            ProductPath='../Bin/'
        elif package=='test_commons':
            isLibrary=False
            ProductName='TestCommons'
            ProductPath='../Bin/'
        elif package=='test_zlib':
            isLibrary=False
            ProductName='TestZlib'
            ProductPath='../Bin/'
        elif package=='test_fastjet':
            isLibrary=False
            ProductName='TestFastjet'
            ProductPath='../Bin/'
        elif package=='test_delphes':
            isLibrary=False
            ProductName='TestDelphes'
            ProductPath='../Bin/'
        elif package=='test_delphesMA5tune':
            isLibrary=False
            ProductName='TestDelphesMA5tune'
            ProductPath='../Bin/'
        else:        
            isLibrary=True
            ProductName='lib'+package+'_for_ma5.so'
            ProductPath='../Lib/'

        # write makefile
        MakefileWriter.Makefile(filename,title,ProductName,ProductPath,isLibrary,cppfiles,hfiles,options,self.main.archi_info,toRemove)

        return True


    def Compile(self,ncores,package,folder):

        # number of cores
        strcores=''
        if ncores>1:
            strcores='-j'+str(ncores)

        # log file name
        if package in ['process','commons','test']:
            logfile = folder+'/compilation.log'
        elif package in ['test_process','test_commons','test_zlib','test_fastjet','test_delphes','test_delphesMA5tune']:
            logfile = folder+'/compilation_'+package[5:]+'.log'
        else:
            logfile = folder+'/compilation_'+package+'.log'

        # makefile
        if package in ['process','commons','test']:
            makefile = 'Makefile'
        elif package in ['test_process','test_commons','test_zlib','test_fastjet','test_delphes','test_delphesMA5tune']:
            makefile = 'Makefile_'+package[5:]
        else:
            makefile = 'Makefile_'+package

        # shell command
        commands = ['make','compile',strcores,'--file='+makefile]

        # call
        result, out = ShellCommand.ExecuteWithLog(commands,logfile,folder)

        # return result
        if not result:
            logging.error('impossible to compile the project. For more details, see the log file:')
            logging.error(logfile)
            
        return result


    def Link(self,package,folder):

        # log file name
        if package in ['process','commons','test']:
            logfile = folder+'/linking.log'
        elif package in ['test_process','test_commons','test_zlib','test_fastjet','test_delphes','test_delphesMA5tune']:
            logfile = folder+'/linking_'+package[5:]+'.log'
        else:
            logfile = folder+'/linking_'+package+'.log'

        # makefile
        if package in ['process','commons','test']:
            makefile = 'Makefile'
        elif package in ['test_process','test_commons','test_zlib','test_fastjet','test_delphes','test_delphesMA5tune']:
            makefile = 'Makefile_'+package[5:]
        else:
            makefile = 'Makefile_'+package

        # shell command
        commands = ['make','link','--file='+makefile]

        # call
        result, out = ShellCommand.ExecuteWithLog(commands,logfile,folder)

        # return result
        if not result:
            logging.error('impossible to link the project. For more details, see the log file:')
            logging.error(logfile)
            
        return result


    def Clean(self,package,folder):

        # log file name
        if package in ['process','commons','test']:
            logfile = folder+'/cleanup.log'
        elif package in ['test_process','test_commons','test_zlib','test_fastjet','test_delphes','test_delphesMA5tune']:
            logfile = folder+'/cleanup_'+package[5:]+'.log'
        else:
            logfile = folder+'/cleanup_'+package+'.log'

        # makefile
        if package in ['process','commons','test']:
            makefile = 'Makefile'
        elif package in ['test_process','test_commons','test_zlib','test_fastjet','test_delphes','test_delphesMA5tune']:
            makefile = 'Makefile_'+package[5:]
        else:
            makefile = 'Makefile_'+package

        # shell command
        commands = ['make','clean','--file='+makefile]

        # call
        result, out = ShellCommand.ExecuteWithLog(commands,logfile,folder)

        # return result
        if not result:
            logging.error('impossible to clean the project. For more details, see the log file:')
            logging.error(logfile)
            
        return result


    def MrProper(self,package,folder):

        # log file name
        if package in ['process','commons']:
            logfile = folder+'/mrproper.log'
        elif package in ['test_process','test_commons','test_zlib','test_fastjet','test_delphes','test_delphesMA5tune']:
            logfile = folder+'/mrproper_'+package[5:]+'.log'
        else:
            logfile = folder+'/mrproper_'+package+'.log'

        # makefile
        if package in ['process','commons','test']:
            makefile = 'Makefile'
        elif package in ['test_process','test_commons','test_zlib','test_fastjet','test_delphes','test_delphesMA5tune']:
            makefile = 'Makefile_'+package[5:]
        else:
            makefile = 'Makefile_'+package

        # shell command
        commands = ['make','mrproper','--file='+makefile]

        # call
        result, out = ShellCommand.ExecuteWithLog(commands,logfile,folder)

        # return result
        if not result:
            logging.error('impossible to clean the project. For more details, see the log file:')
            logging.error(logfile)
            
        return result


    def Run(self,program,args,folder,silent=False):

        # shell command
        commands = ['./'+program]
        commands.extend(args)

        # logfile
        logfile = os.path.normpath(folder+'/'+program+'.log')

        # call
        result, out = ShellCommand.ExecuteWithLog(commands,logfile,folder,silent)

        # return result
        if not result and not silent:
            logging.error('impossible to run the project. For more details, see the log file:')
            logging.error(logfile)
            
        return result


    def CheckRun(self,program,folder,silent=False):

        # log file name
        logfile = os.path.normpath(folder+'/'+program+'.log')

        # Open
        try:
            input = open(logfile)
        except:
            logging.error('impossible to open the file:'+logfile)
            return False

        end=False
        begin=False
        
        # Loop over the logfile
        for line in input:
            line=line.lstrip()
            line=line.rstrip()
            if line=='BEGIN-SAMPLEANALYZER-TEST':
                begin=True
            elif line=='END-SAMPLEANALYZER-TEST':
                end=True

        # Close
        try:
            input.close()
        except:
            logging.error('impossible to close the file:'+logfile)
            return False

        # CrossCheck
        if not (begin and end) and not silent:
            logging.error('expected program output is not found. More details, see the log file:')
            logging.error(logfile)
            return False

        return True
