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

from madanalysis.core.main                    import Main
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.interpreter.cmd_install      import CmdInstall
from madanalysis.interpreter.interpreter      import Interpreter
from madanalysis.misc.freeze_environment      import freeze_environment


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

    def __init__(self, ma5dir, LoggerLevel=logging.INFO, LoggerStream=sys.stdout,
       no_compilation=False, forced=True, *args, **opts):

        # variables
        old_environ = dict(os.environ)

        # Checking if the correct release of Python is installed and tab completion
        if not sys.version_info[0] == 2 or sys.version_info[1] < 6:
            raise InvalidPython('Python release '+ sys.version + ' not supported.\n' + \
            'MadAnalysis 5 works only with python 2.6 or later (but not python 3.X).')
        try:
            import readline
        except ImportError:
            try:
                import pyreadline as readline
            except:
                print "For tab completion and history, install module readline."
        else:
            import rlcompleter
            if 'r261:67515' in sys.version and  'GCC 4.2.1 (Apple Inc. build 5646)' in sys.version:
                readline.parse_and_bind("bind ^I rl_complete")
                readline.__doc__ = 'libedit'  
            elif hasattr(readline, '__doc__'):
                if 'libedit' not in readline.__doc__:
                    readline.parse_and_bind("tab: complete")
                else:
                    readline.parse_and_bind("bind ^I rl_complete")
            else:
                readline.__doc__ = 'GNU'
                readline.parse_and_bind("tab: complete")

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
        self.main = Main()
        self.main.archi_info.ma5dir      = ma5dir
        self.main.archi_info.ma5_version = version
        self.main.archi_info.ma5_date    = date
        self.main.forced = forced
        Main.forced = forced

        # Displaying header
        self.logger.info('*************************************************************')
        self.logger.info('*        W E L C O M E  to  M A D A N A L Y S I S  5        *')
        self.logger.info('*                                                           *')
        self.logger.info('*   MA5 release : ' + \
                "%-24s" % self.main.archi_info.ma5_version + "%+15s" % self.main.archi_info.ma5_date  + '   *')
        self.logger.info('*                                                           *') 
        self.logger.info('*         Comput. Phys. Commun. 184 (2013) 222-256          *')
        self.logger.info('*             Eur. Phys. J. C74 (2014) 3103                 *')
        self.logger.info('*************************************************************')

        # Checking the configuration
        self.logger.setLevel(LoggerLevel)
        if not self.main.CheckConfig(debug=(LoggerLevel<=logging.DEBUG)):
            raise MA5Configuration('Issue with the configuration')
        self.ma5_environ = dict(os.environ)

        if not no_compilation:
            self.compile()

        # Initializing the interpreter
        Interpreter.__init__(self, self.main, *args, **opts)

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
