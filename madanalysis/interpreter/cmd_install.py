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


from madanalysis.interpreter.cmd_base       import CmdBase
from madanalysis.install.install_manager    import InstallManager
from madanalysis.system.user_info           import UserInfo
from madanalysis.system.config_checker      import ConfigChecker
import logging
import os
import sys
import shutil
import urllib
import pwd

class CmdInstall(CmdBase):
    """Command INSTALL"""

    def __init__(self,main):
        CmdBase.__init__(self,main,"install")
        self.logger = logging.getLogger('MA5')


    def do(self,args):

        # Checking argument number
        if len(args)!=1:
            self.logger.error("wrong number of arguments for the command 'install'.")
            self.help()
            return True

        # delphes preinstallation
        def inst_delphes(main,installer,pad=False):
            def UpdatePaths():
                main.archi_info.has_delphes=True
                main.archi_info.delphes_priority=True
                dpath =  os.path.normpath(os.path.join(main.archi_info.ma5dir,'tools','delphes'))
                mylib = os.path.normpath(os.path.join(dpath,'libDelphes.so'))
                main.archi_info.libraries['Delphes']= mylib+":"+str(os.stat(mylib).st_mtime)
                main.archi_info.delphes_lib = mylib
                main.archi_info.toLDPATH1 = [x for x in main.archi_info.toLDPATH1 if not 'MA5tune' in x]
                main.archi_info.toLDPATH1.append(dpath)
                main.archi_info.delphes_lib_paths = [dpath]
                main.archi_info.delphes_inc_paths =[dpath, os.path.normpath(os.path.join(dpath,'external'))]
            if not installer.Deactivate('delphesMA5tune'):
                return False
            ResuActi = installer.Activate('delphes')
            if ResuActi == -1:
                return False
            elif ResuActi == 1 and not self.main.archi_info.has_delphes:
                self.logger.warning("Delphes not installed: installing it...")
                resu = installer.Execute('delphes')
                if resu:
                    UpdatePaths()
                    if not main.CheckConfig():
                        return False
                    return resu
            elif ResuActi == 0 and self.main.archi_info.has_delphes and not pad:
                self.logger.warning("A previous installation of Delphes has been found. Skipping the installation.")
                self.logger.warning("To update Delphes, please remove the tools/delphes directory")
            return True

        # ma5tune preinstallation
        def inst_ma5tune(main,installer,pad=False):
            def UpdatePaths():
                main.archi_info.has_delphesMA5tune=True
                main.archi_info.delphesMA5tune_priority=True
                dpath =  os.path.normpath(os.path.join(main.archi_info.ma5dir,'tools','delphesMA5tune'))
                mylib = os.path.normpath(os.path.join(dpath,'libDelphesMA5tune.so'))
                main.archi_info.libraries['DelphesMA5tune']= mylib+":"+str(os.stat(mylib).st_mtime)
                main.archi_info.delphesMA5tune_lib=mylib
                main.archi_info.toLDPATH1 = [x for x in main.archi_info.toLDPATH1 if not 'delphes' in x]
                main.archi_info.toLDPATH1.append(dpath)
                main.archi_info.delphesMA5tune_lib_paths = [dpath]
                main.archi_info.delphesMA5tune_inc_paths=[dpath,os.path.normpath(os.path.join(dpath,'external'))]
            if not installer.Deactivate('delphes'):
                return False
            ResuActi = installer.Activate('delphesMA5tune')
            if ResuActi == -1:
                return False
            elif ResuActi == 1 and not self.main.archi_info.has_delphesMA5tune:
                self.logger.warning("DelphesMA5tune not installed: installing it...")
                resu = installer.Execute('delphesMA5tune')
                if resu:
                    UpdatePaths()
                    if not main.CheckConfig():
                        return False
                return resu
            elif ResuActi == 0 and self.main.archi_info.has_delphesMA5tune and not pad:
                self.logger.warning("A previous installation of DelphesMA5tune has been found. Skipping the installation.")
                self.logger.warning("To update DelphesMA5tune, please remove the tools/delphesMA5tune directory")
            return True

        # Calling selection method
        if args[0]=='samples':
            installer=InstallManager(self.main)
            return installer.Execute('samples')
        elif args[0]=='zlib':
            installer=InstallManager(self.main)
            return installer.Execute('zlib')
        elif args[0]=='delphes':
            installer=InstallManager(self.main)
            return inst_delphes(self.main,installer)
        elif args[0]=='delphesMA5tune':
            self.logger.warning("The package 'delphesMA5tune' is now obsolete. It is replaced by Delphes with special MA5-tuned cards.")
            if not self.main.forced:
              self.logger.warning("Are you sure to install this package? (Y/N)")
              allowed_answers=['n','no','y','yes']
              answer=""
              while answer not in  allowed_answers:
                 answer=raw_input("Answer: ")
                 answer=answer.lower()
              if answer=="no" or answer=="n":
                  return True
            installer=InstallManager(self.main)
            return inst_ma5tune(self.main,installer)
        elif args[0]=='fastjet':
            installer=InstallManager(self.main)
            if installer.Execute('fastjet')==False:
                return False
            return installer.Execute('fastjet-contrib')
        elif args[0]=='gnuplot':
            installer=InstallManager(self.main)
            return installer.Execute('gnuplot')
        elif args[0]=='matplotlib':
            installer=InstallManager(self.main)
            return installer.Execute('matplotlib')
        elif args[0]=='root':
            installer=InstallManager(self.main)
            return installer.Execute('root')
        elif args[0]=='numpy':
            installer=InstallManager(self.main)
            return installer.Execute('numpy')
        elif args[0]=='PADForMA5tune':
            installer=InstallManager(self.main)
            if inst_ma5tune(self.main,installer,True):
                return installer.Execute('PADForMA5tune')
            else:
                self.logger.warning('DelphesMA5tune is now installed... please exit the program and install the pad')
                return True
        elif args[0]=='PAD':
            installer=InstallManager(self.main)
            if inst_delphes(self.main,installer,True):
                return installer.Execute('PAD')
            else:
                self.logger.warning('Delphes is now installed... please exit the program and install the pad')
                return True
        else:
            self.logger.error("the syntax is not correct.")
            self.help()
            return True

    def help(self):
        self.logger.info("   Syntax: install <component>")
        self.logger.info("   Download and install a MadAnalysis component from the official site.")
        self.logger.info("   List of available components: samples zlib fastjet delphes delphesMA5tune PAD PADForMA5tune")


    def complete(self,text,args,begidx,endidx):

        nargs = len(args)
        if not text:
            nargs +=1

        if nargs>2:
            return []
        else:
            output = ["samples","zlib","fastjet", "delphes", "delphesMA5tune",\
                "gnuplot", "matplotlib", "root" , "numpy", "PAD", "PADForMA5tune"]
            return self.finalize_complete(text,output)



