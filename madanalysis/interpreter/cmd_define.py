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


import madanalysis.interpreter.cmd_base as CmdBase
import logging

class CmdDefine(CmdBase.CmdBase):
    """Command DEFINE"""

    def __init__(self,main):
        CmdBase.CmdBase.__init__(self,main,"define")

    def do(self,args):

        #Checking argument number
        if not len(args) > 2:
            logging.error("wrong number of arguments for the command 'define'.")
            self.help()
            return

        #Looking for '='
        if not args[1] == '=':
            logging.error("syntax error with the command 'define'.")
            self.help()
            return
        
        #Calling fill
        self.fill(args[0],args[2:],self.main.forced)

        
    def fill(self,name,args,forced=False):
        
        # Checking if the name is authorized
        if name in self.reserved_words:
            logging.error("name '" +name+ "' is a reserved keyword. Please choose a different name.")
            return

        # Checking if the name is authorized
        if not self.IsAuthorizedLabel(name):
            logging.error("syntax error with the name '" + name + "'.")
            logging.error("A correct name contains only characters being letters, digits or the '+', '-', '~' and '_' symbols.")
            logging.error("Moreover, a correct name  starts with a letter or the '_' symbol.")
            return

        # Checking if no dataset with the same name has been defined
        if self.main.datasets.Find(name):
            logging.error("A dataset '"+name+"' already exists. Please choose a different name.")
            return
                
        # Adding ids to the multiparticle
        ids = []
        for item in args:
            isPDGid = True
            
            try:
                id = int(item)
            except ValueError:
                isPDGid = False

            if isPDGid:
                ids.append(int(item))
            else:
                if self.main.multiparticles.Find(item):
                    ids.extend(self.main.multiparticles.Get(item).GetIds())
                else:
                    logging.error("The (multi)particle '"+item+"' is not defined.")
                    return

        self.main.multiparticles.Add(name,ids,forced)

    def help(help):
        logging.info("   Syntax: define <particle name> = PDG-id")
        logging.info("   Associates a symbol to a specific particle defined by its PDG-id.")
        logging.info("   Syntax: define <(multi)particle name> = <PDG-id / existing (multi)particle> <PDG-id / (multi)particle> ...")
        logging.info("   Associates a symbol to a multiparticle defined by several particles.")

    def complete(self,text,line,begidx,endidx):

        #Getting back arguments
        args = line.split()
        nargs = len(args)
        if not text:
            nargs += 1

        if nargs==3:
            output=['=']
            return self.finalize_complete(text,output)
        elif nargs>3:
            output=self.main.multiparticles.GetNames()
            return self.finalize_complete(text,output)
        else:
            return

