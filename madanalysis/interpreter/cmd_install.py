################################################################################
#  
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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


from __future__                             import absolute_import
from madanalysis.interpreter.cmd_base       import CmdBase
from madanalysis.install.install_manager    import InstallManager
from six.moves import input
import logging, os, sys


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
        def inst_delphes(main,installer,release,pad=False):
            ## INIT
            if release=='delphes':
                to_activate   = 'Delphes'
                to_deactivate = 'DelphesMA5tune'
            elif release == 'delphesMA5tune':
                to_activate   = 'DelphesMA5tune'
                to_deactivate = 'Delphes'

            ## to update all the paths
            def UpdatePaths():
                if release=='delphes':
                    main.archi_info.has_delphes             = True
                    main.archi_info.delphes_priority        = True
                elif release == 'delphesMA5tune':
                    main.archi_info.has_delphesMA5tune      = True
                    main.archi_info.delphesMA5tune_priority = True
                libname = 'lib'+to_activate
                dpath   =  os.path.join(main.archi_info.ma5dir,'tools',release)
                main.archi_info.toLDPATH1 = [x for x in main.archi_info.toLDPATH1 if not release in x]
                main.archi_info.toLDPATH1.append(dpath)

                if os.path.isfile(os.path.join(dpath,libname+'.so')):
                    mylib = os.path.join(dpath,libname+'.so')
                elif os.path.isfile(os.path.join(dpath,libname+'.dylib')):
                    mylib = os.path.join(dpath,libname+'.dylib')
                else:
                    return True

                main.archi_info.libraries[to_activate]= mylib+":"+str(os.stat(mylib).st_mtime)

                if release=='delphes':
                    main.archi_info.delphes_lib       = mylib
                    main.archi_info.delphes_lib_paths = [dpath]
                    main.archi_info.delphes_inc_paths = [dpath, os.path.join(dpath,'external')]
                elif release == 'delphesMA5tune':
                    main.archi_info.delphesMA5tune_lib       = mylib
                    main.archi_info.delphesMA5tune_lib_paths = [dpath]
                    main.archi_info.delphesMA5tune_inc_paths = [dpath,os.path.join(dpath,'external')]

            ## Deactivating the other delphes, if relevant
            if not installer.Deactivate(to_deactivate):
                return False

            ## Activating the good delphes, if relevant
            ResuActi = installer.Activate(to_activate)
            if release=='delphes':
                has_release = self.main.archi_info.has_delphes
            elif release=='delphesMA5tune':
                has_release = self.main.archi_info.has_delphesMA5tune

            if ResuActi == -1:
                return False
            elif ResuActi == 1 and not has_release:
                self.logger.warning(to_activate + " is not installed: installing it...")
                resu = installer.Execute(release)
                if resu:
                    UpdatePaths()
                    if not main.CheckConfig():
                        return False
                return resu
            elif ResuActi == 0 and has_release and not pad:
                self.logger.warning("A previous " + release +' installation has been found. Skipping...')
                self.logger.warning('To update ;' + release + ', please remove first the tools/' + release + 'delphes directory')
            return True

        # Calling selection method
        installer=InstallManager(self.main)
        if args[0]=='samples':
            return installer.Execute('samples')
        elif args[0]=='zlib':
            return installer.Execute('zlib')
        elif args[0] in ['delphes', 'delphesMA5tune']:
            return inst_delphes(self.main,installer,args[0])
        elif args[0]=='fastjet':
            if installer.Execute('fastjet')==False:
                return False
            return installer.Execute('fastjet-contrib')
        elif args[0]=='gnuplot':
            return installer.Execute('gnuplot')
        elif args[0]=='matplotlib':
            self.logger.warning("This command has been deprecated.")
            self.logger.warning(f"Please use '{sys.executable} -m pip install -r requirements.txt' instead.")
            return True
        elif args[0]=='root':
            return installer.Execute('root')
        elif args[0]=='numpy':
            self.logger.warning("This command has been deprecated.")
            self.logger.warning(f"Please use '{sys.executable} -m pip install -r requirements.txt' instead.")
            return True
        elif args[0]=='PADForMA5tune':
            if inst_delphes(self.main,installer,'delphesMA5tune',True):
                return installer.Execute('PADForMA5tune')
            else:
                self.logger.warning('DelphesMA5tune is not installed... please exit the program and install the pad')
                return True
        elif args[0]=='PAD':
            pad_install_check, padsfs_install_check = False, False
            # First check if PAD4SFS is installed
            if not self.main.session_info.has_padsfs:
                # check if FastJet is installed
                if not self.main.archi_info.has_fastjet:
                    answer = "y"
                    if not self.main.forced:
                        self.logger.warning("PADForSFS requires FastJet to be installed.")
                        self.logger.info("Would you like to install FastJet? [Y/N]")
                        while True:
                            answer = input("Answer : ")
                            if answer.lower() in ['y','n','yes','no', "yeap", "nope"]:
                                break
                    if answer.lower() in ['y','yes',"yeap"]:
                        for package in ["fastjet", "fastjet-contrib", "PADForSFS"]:
                            if not installer.Execute(package):
                                return False
                        padsfs_install_check = 'restart'
                else:
                    padsfs_install_check = installer.Execute('PADForSFS')

            if inst_delphes(self.main,installer,'delphes',True):
                pad_install_check = installer.Execute('PAD')
            else:
                self.logger.warning('Delphes is not installed (and will be installed). '+
                                    'Then please exit MA5 and re-install the PAD')
            if 'restart' in [pad_install_check, padsfs_install_check]:
                return 'restart'
            return any([pad_install_check, padsfs_install_check])
        elif args[0]=='PADForSFS':
            padsfs_install_check = False
            if self.main.archi_info.has_fastjet:
                padsfs_install_check = installer.Execute('PADForSFS')
            else:
                answer = "y"
                if not self.main.forced:
                    self.logger.warning("PADForSFS requires FastJet to be installed.")
                    self.logger.info("Would you like to install FastJet? [Y/N]")
                    while True:
                        answer = input("Answer : ")
                        if answer.lower() in ['y','n','yes','no', "yeap", "nope"]:
                            break
                if answer.lower() in ['y','yes',"yeap"]:
                    for package in ["fastjet", "fastjet-contrib", "PADForSFS"]:
                        if not installer.Execute(package):
                            return False
                    return 'restart'
            return padsfs_install_check
        elif args[0]=='pyhf':
            self.logger.warning("This command has been deprecated.")
            self.logger.warning(f"Please use '{sys.executable} -m pip install -r requirements.txt' instead.")
            return True
        elif args[0]=='likelihood_simplifier':
            if self.main.session_info.has_scipy and self.main.session_info.has_pyhf:
                return installer.Execute('simplify')
            else:
                self.logger.error("The `simplify` module requires pyhf, please retry"+ \
                                  "after installing pyhf.")
                return True
        else:
            self.logger.error("the syntax is not correct.")
            self.help()
            return True

    def help(self):
        self.logger.info("   Syntax: install <component>")
        self.logger.info("   Download and install a MadAnalysis component from the official site.")
        self.logger.info("   List of available components: samples zlib fastjet delphes delphesMA5tune PAD PADForMA5tune PADForSFS")


    def complete(self,text,args,begidx,endidx):

        nargs = len(args)
        if not text:
            nargs +=1

        if nargs>2:
            return []
        else:
            output = ["samples","zlib","fastjet", "delphes", "delphesMA5tune",\
                "gnuplot", "matplotlib", "root" , "numpy", "PAD", "PADForMA5tune",\
                "PADForSFS", "pyhf"]
            return self.finalize_complete(text,output)



