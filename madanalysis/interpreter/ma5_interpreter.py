#!/usr/bin/env python

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
import os
import shutil
import sys

MA5_root_path = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath( __file__ )),os.pardir,os.pardir))
sys.path.insert(0, MA5_root_path)
sys.path.insert(0, os.path.abspath(os.path.join(MA5_root_path,'tools','ReportGenerator','Services')))

import colored_log

from madanalysis.core.main                        import Main
from madanalysis.enumeration.ma5_running_type     import MA5RunningType
from madanalysis.interpreter.interpreter          import Interpreter
from madanalysis.misc.freeze_environment          import freeze_environment
from madanalysis.enumeration.ma5_running_type     import MA5RunningType
from madanalysis.IOinterface.particle_reader      import ParticleReader
from madanalysis.IOinterface.multiparticle_reader import MultiparticleReader
from madanalysis.system.checkup                   import CheckUp
from madanalysis.install.install_manager          import InstallManager
from madanalysis.system.user_info                 import UserInfo
from madanalysis.system.session_info              import SessionInfo
from madanalysis.system.architecture_info         import ArchitectureInfo


class InvalidPython(Exception):
    pass
class InvalidMA5dir(Exception):
    pass
class InvalidServiceMA5dir(Exception):
    pass
class InvalidMA5version(Exception):
    pass
class MA5Configuration(Exception):
    pass
class MA5Dependence(Exception):
    pass
class SampleAnalyzer(Exception):
    pass
class UNK_OPT(Exception):
    pass



class MA5Interpreter(Interpreter):
    """This is a class allowing to call an MA5 interpreter from an external code"""

    # Make sure that this cmd.Cmd daughter class doesn't assume that its user
    # input is raw, i.e. provided by a user via the interactive interface.
    # In this case the input is supposed to be already processed by the
    # external code using this intepreter.
    use_rawinput = 0

    def __init__(self, ma5dir, LoggerLevel=logging.INFO, LoggerStream=sys.stdout,
       no_compilation=False, forced=True, *args, **opts):

        # variables
        old_environ = dict(os.environ)

        # Checking if the correct release of Python is installed and tab completion
        if not sys.version_info[0] == 2 or sys.version_info[1] < 6:
            raise InvalidPython('Python release '+ sys.version + ' not supported.\n' + \
            'MadAnalysis 5 works only with python 2.6 or later (but not python 3.X).')

        # Checking the MA5 path and adding it to sys.path
        if not os.path.isdir(ma5dir):
            raise InvalidMA5dir('Incorrect MadAnalysis5 folder: ' + ma5dir)
        os.environ['MA5_BASE']=ma5dir
        if not ma5dir in sys.path:
            sys.path.insert(0, ma5dir)

        # Python services
        ma5servicedir = os.path.normpath(ma5dir+'/tools/ReportGenerator/Services/')
        if not os.path.isdir(ma5servicedir):
            raise InvalidMA5Servicedir('Incorrect MadAnalysis5 service folder: ' + ma5servicedir)

        if not ma5servicedir in sys.path:
            sys.path.insert(0, ma5servicedir)

        # logger
        colored_log.init(LoggerStream=LoggerStream)
        self.logger = logging.getLogger('MA5')
        self.logger.setLevel(LoggerLevel)

        # version numbers
        try:
            inputfile = open(ma5dir+'/version.txt','r')
            for line in inputfile:
                if 'MA5 version' in line:
                    version = line.split()[-1]
                elif 'Date' in line:
                    date  = line.split()[-1]
            inputfile.close()
        except:
            raise InvalidMA5version('Cannot find the ma5 version info file')

        # Loading the MadAnalysis session
        main = Main()
        main.archi_info.ma5dir      = ma5dir
        main.archi_info.ma5_version = version
        main.archi_info.ma5_date    = date
        main.forced = forced
        Main.forced = forced

        main.redirectSAlogger = True

        # quiet please
        if (LoggerLevel>logging.DEBUG):
           self.logger.setLevel(10)

        # Checking the configuration
        if not main.CheckConfig(debug=(LoggerLevel<=logging.DEBUG)):
            raise MA5Configuration('Issue with the configuration')
        if not main.CheckConfig2(debug=(LoggerLevel<=logging.DEBUG)):
            raise MA5Configuration('Issue with the configuration')

        self.ma5_environ = dict(os.environ)
        main.madgraph.has_root           = main.archi_info.has_root
        main.madgraph.has_delphes        = main.archi_info.has_delphes
        main.madgraph.has_delphesMA5tune = main.archi_info.has_delphesMA5tune
        main.madgraph.has_matplotlib     = main.session_info.has_matplotlib

        # Initializing the interpreter and compiling if needed
        Interpreter.__init__(self, main, *args, **opts)

        if not no_compilation:
            self.compile()

        # stop being silent
        if (LoggerLevel>logging.DEBUG):
           self.logger.setLevel(LoggerLevel)

        # Backuping the environment
        os.environ.clear()
        os.environ.update(old_environ)

    @freeze_environment
    def compile(self):
        if not self.main.BuildLibrary():
            raise SampleAnalyzerBuilding('Issue with the configuration')
        return True

    @freeze_environment
    def print_banner(self):
        self.logger.info('*************************************************************')
        self.logger.info('*        W E L C O M E  to  M A D A N A L Y S I S  5        *')
        self.logger.info('*                                                           *')
        self.logger.info('*   MA5 release : ' + \
                "%-24s" % self.main.archi_info.ma5_version + "%+15s" % self.main.archi_info.ma5_date  + '   *')
        self.logger.info('*                                                           *')
        self.logger.info('*         Comput. Phys. Commun. 184 (2013) 222-256          *')
        self.logger.info('*             Eur. Phys. J. C74 (2014) 3103                 *')
        self.logger.info('*************************************************************')

    @freeze_environment
    def load(self, *args, **opts):
        from madanalysis.core.script_stack import ScriptStack
        ScriptStack.stack.append(['',args[0]])
        Interpreter.load(self)
#        Interpreter.load(self,*args,**opts)

    @freeze_environment
    def setLogLevel(self,level):
        self.logger.setLevel(level)

    @freeze_environment
    def init_reco(self):
        # changing the running mode
        self.main.mode=MA5RunningType.RECO

        # resetting
        self.main.datasets.Reset()
        self.main.selection.Reset()
        self.main.ResetParameters()
        self.InitializeHistory()

        # Graphical mode
        self.main.AutoSetGraphicalRenderer()

        # observables
        self.main.InitObservables(self.main.mode)

        # labels (but no logs!)
        lvl = self.logger.getEffectiveLevel()
        self.setLogLevel(100)
        self.main.multiparticles.Reset()
        input = ParticleReader(self.main.archi_info.ma5dir,self.cmd_define,self.main.mode,self.main.forced)
        input.Load()
        input = MultiparticleReader(self.main.archi_info.ma5dir,self.cmd_define,self.main.mode,self.main.forced)
        input.Load()
        self.setLogLevel(lvl)

    @freeze_environment
    def init_parton(self):
        # changing the running mode
        self.main.mode=MA5RunningType.PARTON

        # resetting
        self.main.datasets.Reset()
        self.main.selection.Reset()
        self.main.ResetParameters()
        self.InitializeHistory()

        # Graphical mode
        self.main.AutoSetGraphicalRenderer()

        # observables
        self.main.InitObservables(self.main.mode)

        # labels
        lvl = self.logger.getEffectiveLevel()
        self.setLogLevel(100)
        self.main.multiparticles.Reset()
        input = ParticleReader(self.main.archi_info.ma5dir,self.cmd_define,self.main.mode,self.main.forced)
        input.Load()
        input = MultiparticleReader(self.main.archi_info.ma5dir,self.cmd_define,self.main.mode,self.main.forced)
        input.Load()
        self.setLogLevel(lvl)


    @freeze_environment
    def further_install(self, opts):
        # initialization
        install_delphes         = False
        install_delphesMA5tune  = False
        user_info           = UserInfo()

        # A few useful methods
        def validate_bool_key(key):
            if not isinstance(opts[key],bool):
                self.logger.warning('Unknown value for the further_install key '+ key + '. Ignoring.')
                return False
            return opts[key]

        def update_options(usrkey,value):
            inname  = os.path.join(MA5_root_path,'madanalysis','input','installation_options.dat')
            outname = os.path.join(MA5_root_path,'madanalysis','input','installation_options.new')
            infile  = open(inname ,'r')
            outfile = open(outname,'w')
            for line in infile:
                if usrkey in line:
                    outfile.write(usrkey + ' = ' + value+'\n')
                else:
                    outfile.write(line)
            infile.close()
            outfile.close()
            shutil.move(outname,inname)

        def setinc(key,usrkey,value, archi_reset=''):
            if opts[key] not in [True,None] and os.path.isdir(opts[key]):
                user_info.SetValue(usrkey,value,'')
                update_options(usrkey,value)
                if archi_reset != '':
                    self.main.archi_info.__dict__[archi_reset.keys()[0]] = archi_reset.values()[0]
            elif opts[key] not in [True,None]:
                self.logger.warning('Non-existing ' + key.replace('with-','') + \
                   ' path. Automatic detection used.')
            elif usrkey=='root_veto':
                update_options(usrkey,value)

        # Configuration
        for key in opts.keys():
            value=opts[key]
            if key=='veto-delphes':
                user_info.delphes_veto = validate_bool_key(key)
            elif key=='veto-delphesMA5tune':
                user_info.delphesMA5tune_veto = validate_bool_key(key)
            elif key=='veto-root':
                user_info.root_veto = validate_bool_key(key)
                if user_info.root_veto==True:
                    self.main.session_info.has_root = False
                    killroot1=[ 'root_bin_path', 'root_version', 'root_inc_path', 'root_compiler', \
                       'root_lib_path' ]
                    killroot2=[ 'root_original_bins', 'root_features' ]
                    for x in killroot1:
                        self.main.archi_info.__dict__[x] = ''
                    for x in killroot2:
                        self.main.archi_info.__dict__[x] = []
                    self.main.archi_info.has_root = False
                    setinc(key,'root_veto','1')
                    del self.main.archi_info.libraries['TH1F.h']
                    del self.main.archi_info.libraries['libCore.so']
            elif key=='with-zlib':
                if not isinstance(opts[key],bool):
                    setinc(key,'zlib_libs',opts[key]+'/lib',archi_reset={'zlib_original_libs':[]})
                    setinc(key,'zlib_includes', opts[key]+'/include')
            elif key=='with-fastjet':
                setinc(key,'fastjet_bin_path',opts[key])
            elif key=='with-root':
                setinc(key,'root_bin_path',opts[key])
            elif key=='with-delphes':
                install_delphes = validate_bool_key(key)
            elif key=='with-delphesMA5tune':
                install_delphesMA5tune = validate_bool_key(key)
            else:
                raise UNK_OPT('Unknown options for further_install')

        # Muting the logger
        lvl = self.logger.getEffectiveLevel()
        self.setLogLevel(10)

        # updating the configuration internally
        def config_update(checkup):
            if not checkup.CheckOptionalProcessingPackages():
                self.logger.error('Impossible to internally update the paths of the dependences.')
                return False
            arch_to_update = [ 'has_zlib', 'zlib_lib', 'zlib_lib_path', 'zlib_inc_path', 'zlib_original_libs',\
              'fastjet_bin_path', 'fastjet_original_bins', 'toPATH1', 'fastjet_lib_paths', 'toLDPATH1', \
              'has_fastjet', 'root_bin_path', 'root_original_bins', 'root_version', 'libraries', \
              'root_inc_path', 'root_features', 'root_compiler', 'root_lib_path' ]
            for x in arch_to_update:
                self.main.archi_info.__dict__[x] = checkup.archi_info.__dict__[x]


        checkup = CheckUp(self.main.archi_info, self.main.session_info, True, self.main.script)
        checkup.user_info         = user_info
        checkup.checker.user_info = user_info
        config_update(checkup)

        # If not zlib -> install zlib
        if not self.main.archi_info.has_zlib:
            self.logger.info('The zlib package has not been found. Proceeding with its local installation.')
            installer=InstallManager(self.main)
            if not installer.Execute('zlib'):
                self.logger.error('Impossible to install zlib.')
                return False

        # If not fastjet -> install fastjet
        if not self.main.archi_info.has_fastjet:
            self.logger.info('The fastjet package has not been found. Proceeding with its local installation.')
            installer=InstallManager(self.main)
            if not installer.Execute('fastjet'):
                self.logger.error('Impossible to install fastjet.')
                return False

        # Delphes installation
        if self.main.archi_info.has_root and user_info.delphes_veto and install_delphes:
            self.logger.warning('Delphes has been both vetoed and non-vetoed. Ignoring veto.')
            user_info.delphes_veto = False
        elif self.main.archi_info.has_root and user_info.delphes_veto:
            self.logger.info('Delphes installation has been vetoed. Skipping it.')

        if not self.main.archi_info.has_root and install_delphes:
            self.logger.warning('The root package has not been found. Skipping the delphes installation.')

        if self.main.archi_info.has_root and install_delphes:
            self.logger.info('Proceeding with the delphes installation.')
            installer=InstallManager(self.main)
            if not installer.Execute('delphes'):
                self.logger.error('Impossible to install delphes.')
                return False
            self.logger.info('Proceeding with the PAD installation.')
            if not installer.Execute('pad'):
                self.logger.error('Impossible to install the PAD.')
                return False

        # DelphesMA5tune installation
        root_v = (len(self.main.archi_info.root_version)==3) and (int(self.main.archi_info.root_version[0])==5)
        if self.main.archi_info.has_root and user_info.delphesMA5tune_veto and install_delphesMA5tune:
            self.logger.warning('DelphesMA5tune has been both vetoed and non-vetoed. Ignoring veto.')
            user_info.delphesMA5tune_veto = False
        elif self.main.archi_info.has_root and user_info.delphesMA5tune_veto:
            self.logger.info('DelphesMA5tune installation has been vetoed. Skipping it.')

        if not self.main.archi_info.has_root and install_delphesMA5tune:
            self.logger.warning('The root package has not been found. Skipping the delphesMA5tune installation.')

        if not self.main.archi_info.has_root and install_delphesMA5tune and not root_v:
            self.logger.warning('DelphesMA5tune is not compatible with root 6. Skipping its installation.')

        if self.main.archi_info.has_root and install_delphesMA5tune and root_v:
            self.logger.info('Proceeding with the delphesMA5tune installation.')
            installer=InstallManager(self.main)
            if not installer.Execute('delphesMA5tune'):
                self.logger.error('Impossible to install delphesMA5tune.')
                return False
            self.logger.info('Proceeding with the PADForMA5tune installation.')
            if not installer.Execute('padforma5tune'):
                self.logger.error('Impossible to install the PADForMA5tune.')
                return False

        # Reinitialization of main
        # Remember of FR/MG2018: fastjet installation bug
        tmp1 = self.main.archi_info.ma5dir 
        tmp2 = self.main.archi_info.ma5_version
        tmp3 = self.main.archi_info.ma5_date
        #
        self.main.archi_info   = ArchitectureInfo()
        self.main.session_info = SessionInfo()
        #
        self.main.archi_info.ma5dir      = tmp1
        self.main.archi_info.ma5_version = tmp2
        self.main.archi_info.ma5_date    = tmp3

        # Detection
        if not self.main.CheckConfig(debug=False):
            raise MA5Configuration('Issue with the configuration')

        # Building sampleanalyzer
        self.compile()

        # restoring the log level
        self.setLogLevel(lvl)

        return True
