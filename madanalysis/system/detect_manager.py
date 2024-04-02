################################################################################
#  
#  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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
import sys
from string_tools import StringTools
from madanalysis.enumeration.detect_status_type import DetectStatusType


class DetectManager():

    # same as checker.name
    hidden_packages = ["likelihood simplifier"]

    def __init__(self,archi_info,user_info,session_info,script,debug):
        self.archi_info      = archi_info
        self.user_info       = user_info
        self.session_info    = session_info
        self.script          = script
        self.debug           = debug
        self.logger          = logging.getLogger('MA5')
        self.hidden_packages = [self.PrintPackageName(x) for x in self.hidden_packages]


    def Execute(self, rawpackage):

        self.logger.debug('------------------------------------------------------')
        package=rawpackage.lower()
        self.logger.debug('Detect package '+str(package))

        # Selection of the package
        if package=='zlib':
            from madanalysis.system.detect_zlib import DetectZlib
            checker=DetectZlib(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='onnx':
            from madanalysis.system.detect_onnx import DetectONNX
            checker=DetectONNX(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='fastjet':
            from madanalysis.system.detect_fastjet import DetectFastjet
            checker=DetectFastjet(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='pad':
            from madanalysis.system.detect_pad import DetectPAD
            checker=DetectPAD(self.archi_info, self.user_info, self.session_info, self.debug, padtype='')
        elif package=='padma5':
            from madanalysis.system.detect_pad import DetectPAD
            checker=DetectPAD(self.archi_info, self.user_info, self.session_info, self.debug, padtype="ma5")
        elif package=='padsfs':
            from madanalysis.system.detect_pad import DetectPAD
            checker=DetectPAD(self.archi_info, self.user_info, self.session_info, self.debug, padtype="sfs")
        elif package=='fastjet-contrib':
            from madanalysis.system.detect_fastjetcontrib import DetectFastjetContrib
            checker=DetectFastjetContrib(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='delphes':
            from madanalysis.system.detect_delphes import DetectDelphes
            checker=DetectDelphes(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='delphesma5tune':
            from madanalysis.system.detect_delphesMA5tune import DetectDelphesMA5tune
            checker=DetectDelphesMA5tune(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='gnuplot':
            from madanalysis.system.detect_gnuplot import DetectGnuPlot
            checker=DetectGnuPlot(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='matplotlib':
            from madanalysis.system.detect_matplotlib import DetectMatplotlib
            checker=DetectMatplotlib(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='gnuplot':
            from madanalysis.system.detect_gnuplot import DetectGnuPlot
            checker=DetectMatplotlib(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='scipy':
            from madanalysis.system.detect_scipy import DetectScipy
            checker=DetectScipy(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='root_graphical':
            from madanalysis.system.detect_root_graphical import DetectRootGraphical
            checker=DetectRootGraphical(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='root':
            from madanalysis.system.detect_root import DetectRoot
            checker=DetectRoot(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='pyroot':
            from madanalysis.system.detect_pyroot import DetectPyRoot
            checker=DetectPyRoot(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='python':
            from madanalysis.system.detect_python import DetectPython
            checker=DetectPython(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='gpp':
            from madanalysis.system.detect_gpp import DetectGpp
            checker=DetectGpp(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='make':
            from madanalysis.system.detect_make import DetectMake
            checker=DetectMake(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='numpy':
            from madanalysis.system.detect_numpy import DetectNumpy
            checker=DetectNumpy(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='pyhf':
            from madanalysis.system.detect_pyhf import Detectpyhf
            checker=Detectpyhf(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='simplify':
            from madanalysis.system.detect_simplify import DetectSimplify
            checker=DetectSimplify(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='pdflatex':
            from madanalysis.system.detect_pdflatex import DetectPdflatex
            checker=DetectPdflatex(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='dvipdf':
            from madanalysis.system.detect_dvipdf import DetectDvipdf
            checker=DetectDvipdf(self.archi_info, self.user_info, self.session_info, self.debug)
        elif package=='latex':
            from madanalysis.system.detect_latex import DetectLatex
            checker=DetectLatex(self.archi_info, self.user_info, self.session_info, self.debug)
        else:
            self.logger.error('the package "'+rawpackage+'" is unknown')
            return False

        # 1. Displaying the name of the package
        package_name = self.PrintPackageName(checker.name)

        # 2. Initialization
        if hasattr(checker, 'Initialize'):
            self.logger.debug('Initialization ...')
            if not checker.Initialize():
                self.PrintFAILURE(package_name)
                if checker.mandatory:
                    self.logger.error('This package is a mandatory package: MadAnalysis 5 can not run without it.')
                    for line in checker.log:
                        self.logger.error(line)
                    return False
                else:
                    if hasattr(checker, 'PrintDisableMessage'):
                        checker.PrintDisableMessage()
                    if hasattr(checker, 'PrintInstallMessage'):
                        checker.PrintInstallMessage()
                    return True

        # 3. Veto
        if hasattr(checker, 'IsItVetoed'):
            self.logger.debug('Is there a veto? ...')
            if checker.IsItVetoed():
                # Should not happen because veto possible only on optional packages
                if checker.mandatory:
                    self.logger.error('This package is a mandatory package: MadAnalysis 5 can not run without it.')
                    return False
                # normal case
                else:
                    self.PrintUSERDISABLED(package_name)
                    if hasattr(checker, 'PrintDisableMessage'):
                        checker.PrintDisableMessage()
                    return True

        # 3bis. Dependencies
        if hasattr(checker, 'AreDependenciesInstalled'):
            self.logger.debug('Are dependencies installed on the machine? ...')
            if not checker.AreDependenciesInstalled():
                # Should not happen because veto possible only on optional packages
                if checker.mandatory:
                    self.logger.error('Dependencies are missing.')
                    self.logger.error('This package is a mandatory package: MadAnalysis 5 can not run without it.')
                    return False
                # normal case
                else:
                    self.PrintDISABLED(package_name)
                    if hasattr(checker, 'PrintDisableMessage'):
                        checker.PrintDisableMessage()
                    return True

        # 4. Does the user force something?
        search = True
        if hasattr(checker, 'ManualDetection'):
            self.logger.debug('Detection of the package in the location specified by the user ...')
            search = False
            status, msg = checker.ManualDetection()

            # If problem
            if status==DetectStatusType.ISSUE:
                self.PrintFAILURE(package_name)
                if checker.mandatory:
                    self.logger.error('This package is a mandatory package: MadAnalysis 5 can not run without it.')
                    for line in checker.log:
                        self.logger.error(line)
                    return False
                else:
                    if hasattr(checker, 'PrintDisableMessage'):
                        checker.PrintDisableMessage()
                    if hasattr(checker, 'PrintInstallMessage'):
                        checker.PrintInstallMessage()
                    return True

            # No found -> autodetection
            elif status==DetectStatusType.UNFOUND:
                search = True

        # 5. Is it installed in the tools folder?
        if search and hasattr(checker, 'ToolsDetection'):
            self.logger.debug('Detection of the package in the "tools" folder ...')
            search = False
            status, msg = checker.ToolsDetection()

            # If problem
            if status==DetectStatusType.ISSUE:
                self.PrintFAILURE(package_name)
                if checker.mandatory:
                    self.logger.error('This package is a mandatory package: MadAnalysis 5 can not run without it.')
                    for line in checker.log:
                        self.logger.error(line)
                    return False
                else:
                    if hasattr(checker, 'PrintDisableMessage'):
                        checker.PrintDisableMessage()
                    if hasattr(checker, 'PrintInstallMessage'):
                        checker.PrintInstallMessage()
                    return True

            # No found -> autodetection
            elif status==DetectStatusType.UNFOUND:
                search = True

            # OK -> autodetection
            elif status==DetectStatusType.FOUND:
                search = False

        # 6. Autodetection of the package
        if search and hasattr(checker, 'AutoDetection'):
            self.logger.debug('Try to detect automatically the package ...')
            search = False
            status,msg = checker.AutoDetection()

            if status in [DetectStatusType.UNFOUND,DetectStatusType.ISSUE]:
                if checker.mandatory:
                    self.PrintFAILURE(package_name)
                    self.logger.error('This package is a mandatory package: MadAnalysis 5 can not run without it.')
                    for line in checker.log:
                        self.logger.error(line)
                    return False
                else:
                    if status==DetectStatusType.UNFOUND:
                        self.PrintDISABLED(package_name)
                    else:
                        self.PrintFAILURE(package_name)
                    if hasattr(checker, 'PrintDisableMessage'):
                        checker.PrintDisableMessage()
                    if hasattr(checker, 'PrintInstallMessage'):
                        checker.PrintInstallMessage()
                    return True

        # Case of no autodetection of the package
        if search:
            if checker.mandatory:
                self.PrintFAILURE(package_name)
                self.logger.error('This package is a mandatory package: MadAnalysis 5 can not run without it.')
                for line in checker.log:
                    self.logger.error(line)
                return False
            else:
                self.PrintDISABLED(package_name)
                if hasattr(checker, 'PrintDisableMessage'):
                    checker.PrintDisableMessage()
                if hasattr(checker, 'PrintInstallMessage'):
                    checker.PrintInstallMessage()
                return True

        # 7. Getting more details about the package
        if hasattr(checker, 'ExtractInfo'):
            self.logger.debug('Extract more informations related to the package ...')
            if not checker.ExtractInfo():
                self.PrintFAILURE(package_name)
                if checker.mandatory:
                    self.logger.error('This package is a mandatory package: MadAnalysis 5 can not run without it.')
                    for line in checker.log:
                        self.logger.error(line)
                    return False
                else:
                    if hasattr(checker, 'PrintDisableMessage'):
                        checker.PrintDisableMessage()
                    if hasattr(checker, 'PrintInstallMessage'):
                        checker.PrintInstallMessage()
                    return True


        # 8. Saving package information
        if hasattr(checker, 'SaveInfo'):
            self.logger.debug('Saving informations ...')
            if not checker.SaveInfo():
                self.PrintFAILURE(package_name)
                if checker.mandatory:
                    self.logger.error('This package is a mandatory package: MadAnalysis 5 can not run without it.')
                    for line in checker.log:
                        self.logger.error(line)
                    return False
                else:
                    if hasattr(checker, 'PrintDisableMessage'):
                        checker.PrintDisableMessage()
                    if hasattr(checker, 'PrintInstallMessage'):
                        checker.PrintInstallMessage()
                    return True

        # 9. Finalize: displaying OK
        if hasattr(checker, 'Finalize'):
            self.logger.debug('Finalization ...')
            if not checker.Finalize():
                self.PrintFAILURE(package_name)
                if checker.mandatory:
                    self.logger.error('This package is a mandatory package: MadAnalysis 5 can not run without it.')
                    for line in checker.log:
                        self.logger.error(line)
                    return False
                else:
                    if hasattr(checker, 'PrintDisableMessage'):
                        checker.PrintDisableMessage()
                    if hasattr(checker, 'PrintInstallMessage'):
                        checker.PrintInstallMessage()
                    return True

        # Ok
        self.PrintOK(package_name)
        return True


    def Print(self,status):
        if status==DetectStatusType.FOUND:
            PrintOK('')
        elif status==DetectStatusType.UNFOUND:
            PrintDISABLED('')
        elif status==DetectStatusType.DEACTIVATED:
            PrintDEACTIVATED('')
        elif status==DetectStatusType.ISSUE:
            PrintFAILURE('')

    def PrintOK(self,text):
        if text not in self.hidden_packages:
            self.logger.info(text+'\x1b[32m'+'[OK]'+'\x1b[0m')
        else:
            self.logger.debug(text+'\x1b[32m'+'[OK]'+'\x1b[0m')


    def PrintFAILURE(self,text):
        if text not in self.hidden_packages:
            self.logger.info(text + '\x1b[31m'+'[FAILURE]'+'\x1b[0m')
        else:
            self.logger.debug(text + '\x1b[31m'+'[FAILURE]'+'\x1b[0m')


    def PrintDISABLED(self,text):
        if text not in self.hidden_packages:
            self.logger.info(text + '\x1b[35m'+'[DISABLED]'+'\x1b[0m')
        else:
            self.logger.debug(text + '\x1b[35m'+'[DISABLED]'+'\x1b[0m')


    def PrintUSERDISABLED(self,text):
        if text not in self.hidden_packages:
            self.logger.info(text+'\x1b[35m'+'[DISABLED BY THE USER]'+'\x1b[0m')
        else:
            self.logger.debug(text+'\x1b[35m'+'[DISABLED BY THE USER]'+'\x1b[0m')

    def PrintDEACTIVATED(self,text):
        if text not in self.hidden_packages:
            self.logger.info(text+'\x1b[33m'+'[DEACTIVATED]'+'\x1b[0m')
        else:
            self.logger.debug(text+'\x1b[33m'+'[DEACTIVATED]'+'\x1b[0m')

    def PrintWARNING(self,text):
        if text not in self.hidden_packages:
            self.logger.info(text+'\x1b[35m'+'[WARNING]'+'\x1b[0m')
        else:
            self.logger.debug(text+'\x1b[35m'+'[WARNING]'+'\x1b[0m')


    def PrintPackageName(self,text,tab=5,width=25):
        # Displaying the package name without "\n"
        mytab = '%'+str(tab)+'s'
        mytab = mytab % ' '
        mytab += '- '
        mywidth = '%-'+str(width)+'s'
        mywidth = mywidth % text
        return mytab+mywidth

