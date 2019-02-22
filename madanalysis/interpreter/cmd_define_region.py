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


import madanalysis.interpreter.cmd_base as CmdBase
import logging

class CmdDefineRegion(CmdBase.CmdBase):
    """Command DEFINE_REGION"""

    def __init__(self,main):
        self.logger       = logging.getLogger('MA5')
        CmdBase.CmdBase.__init__(self,main,"define_region")

    def do(self,args):
        #Checking argument number
        if len(args) == 0:
            logging.getLogger('MA5').error("wrong number of arguments for the command 'define_region'.")
            self.help()
            return True

        # Calling fill
        self.fill(args,self.main.forced)

    def fill(self,args,forced=False):
        # Checking if the name is authorized
        for x in args:
            if x in self.reserved_words:
                self.logger.error("the name '" + x + "' is a reserved keyword. Please choose a different name.")
                return False

        # Checking if the name is authorized
        for x in args:
            if not self.IsAuthorizedLabel(x):
                self.logger.error("syntax error with the name '" + x + "'.")
                self.logger.error("A correct name contains only characters being letters, digits or the '+', '-', '~' and '_' symbols.")
                self.logger.error("Moreover, a correct name starts with a letter or the '_' symbol.")
                return False

        # Checking if no dataset with the same name has been defined
        for x in args:
            if self.main.datasets.Find(x):
                logging.getLogger('MA5').error("A dataset '"+x+"' already exists. Please choose a different name.")
                return False

        # Checking if no (multi)particle with the same name has been defined
        for x in args:
            if self.main.multiparticles.Find(x):
                logging.getLogger('MA5').error("A (multi)particle '"+x+"' already exists. Please choose a different name.")
                return False

        # Checking if no observable with the same name has been defined
        for x in args:
            if x in self.main.observables.full_list:
                logging.getLogger('MA5').error("An observable '"+x+"' already exists. Please choose a different name.")
                return False

        # Checking if the region has already been defined
        for x in args:
            if self.main.regions.Find(x):
                logging.getLogger('MA5').error("A region '"+x+"' already exists. Please choose a different name.")
                return False
            self.main.regions.Add(x)

        return True

    def help(help):
        logging.getLogger('MA5').info("   Syntax: define_region <list of regions>")
        logging.getLogger('MA5').info("   Creates one or more analysis regions.")

    def complete(self,text,line,begidx,endidx):
        return True

