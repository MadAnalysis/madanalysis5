#!/usr/bin/env python

################################################################################
#
#  Copyright (C) 2012-2016 Eric Conte, Benjamin Fuks
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
import sys

MA5_root_path = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath( __file__ )),os.pardir,os.pardir))
sys.path.insert(0, MA5_root_path)
sys.path.insert(0, os.path.abspath(os.path.join(MA5_root_path,'tools','ReportGenerator','Services')))

import colored_log

from madanalysis.core.main                        import Main
from madanalysis.enumeration.ma5_running_type     import MA5RunningType
from madanalysis.interpreter.cmd_install          import CmdInstall
from madanalysis.interpreter.interpreter          import Interpreter
from madanalysis.misc.freeze_environment          import freeze_environment
from madanalysis.enumeration.ma5_running_type     import MA5RunningType
from madanalysis.IOinterface.particle_reader      import ParticleReader
from madanalysis.IOinterface.multiparticle_reader import MultiparticleReader


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

        # quiet please
        if (LoggerLevel>logging.DEBUG):
           self.logger.setLevel(100)

        # Checking the configuration
        if not main.CheckConfig(debug=(LoggerLevel<=logging.DEBUG)):
            raise MA5Configuration('Issue with the configuration')
        self.ma5_environ = dict(os.environ)

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
    def install(self, target):
        cmd_install  = CmdInstall(self.main)
        if not cmd_install.do([target]):
            raise MA5Dependence('Issue with the installation of ' + target)
        return True

    @freeze_environment
    def load(self, *args, **opts):
        Interpreter.load(self,*args,**opts)

    @freeze_environment
    def setLogLevel(self,level):
        self.logger.setLevel(level)

    @freeze_environment
    def init_reco(self):
        # banner
        banner_level=90
        self.logger.log(banner_level,'*************************************************************')
        self.logger.log(banner_level,'*        W E L C O M E  to  M A D A N A L Y S I S  5        *')
        self.logger.log(banner_level,'*                                                           *')
        self.logger.log(banner_level,'*   MA5 release : ' + \
                "%-24s" % self.main.archi_info.ma5_version + "%+15s" % self.main.archi_info.ma5_date  + '   *')
        self.logger.log(banner_level,'*                                                           *')
        self.logger.log(banner_level,'*         Comput. Phys. Commun. 184 (2013) 222-256          *')
        self.logger.log(banner_level,'*             Eur. Phys. J. C74 (2014) 3103                 *')
        self.logger.log(banner_level,'*************************************************************')

        # changing the running mode
        self.main.mode=MA5RunningType.RECO

        # resetting
        self.main.datasets.Reset()
        self.main.selection.Reset()
        self.main.ResetParameters()
        self.history=[]

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
        # banner
        banner_level=90
        self.logger.log(banner_level,'*************************************************************')
        self.logger.log(banner_level,'*        W E L C O M E  to  M A D A N A L Y S I S  5        *')
        self.logger.log(banner_level,'*                                                           *')
        self.logger.log(banner_level,'*   MA5 release : ' + \
                "%-24s" % self.main.archi_info.ma5_version + "%+15s" % self.main.archi_info.ma5_date  + '   *')
        self.logger.log(banner_level,'*                                                           *')
        self.logger.log(banner_level,'*         Comput. Phys. Commun. 184 (2013) 222-256          *')
        self.logger.log(banner_level,'*             Eur. Phys. J. C74 (2014) 3103                 *')
        self.logger.log(banner_level,'*************************************************************')

        # changing the running mode
        self.main.mode=MA5RunningType.PARTON

        # resetting
        self.main.datasets.Reset()
        self.main.selection.Reset()
        self.main.ResetParameters()
        self.history=[]


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

