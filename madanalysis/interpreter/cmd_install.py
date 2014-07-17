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


from madanalysis.interpreter.cmd_base          import CmdBase
from madanalysis.install.install_manager       import InstallManager
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


    def do(self,args):

        # Checking argument number
        if len(args) != 1:
            logging.error("wrong number of arguments for the command 'install'.")
            self.help()
            return

        # Calling selection method
        if args[0]=='samples':
            installer=InstallManager(self.main)
            return installer.Execute('samples')
        elif args[0]=='zlib':
            installer=InstallManager(self.main)
            return installer.Execute('zlib')
        elif args[0]=='delphes':
            installer=InstallManager(self.main)
            return installer.Execute('delphes')
        elif args[0]=='delphesMA5tune':
            installer=InstallManager(self.main)
            return installer.Execute('delphesMA5tune')
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
        elif args[0]=='RecastingTools':
            installer=InstallManager(self.main)
            return installer.Execute('RecastingTools')
        else:
            logging.error("the syntax is not correct.")
            self.help()
            return


    def help(self):
        logging.info("   Syntax: install <component>")
        logging.info("   Download and install a MadAnalysis component from the official site.")
        logging.info("   List of available components : samples zlib fastjet delphes delphesMA5tune")


    def complete(self,text,args,begidx,endidx):

        nargs = len(args)
        if not text:
            nargs +=1

        if nargs>2:
            return []
        else:
            output = ["samples","zlib","fastjet", "delphes", "delphesMA5tune", "gnuplot", "matplotlib", "root" , "numpy", "RecastingTools"]
            return self.finalize_complete(text,output)



