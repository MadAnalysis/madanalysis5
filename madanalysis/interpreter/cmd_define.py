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
from madanalysis.enumeration.ma5_running_type       import MA5RunningType
import logging

class CmdDefine(CmdBase.CmdBase):
    """Command DEFINE"""

    def __init__(self,main):
        CmdBase.CmdBase.__init__(self,main,"define")

    def do(self,args):

        # tagger / smearer
        if args[0] in ['tagger', 'smearer', 'reco_efficiency']:
            if self.main.mode != MA5RunningType.RECO:
                logging.getLogger('MA5').error("Smearing/tagging/reconstruction are only available in the RECO mode")
                logging.getLogger('MA5').error("Please restart the program with './bin/ma5 -R '")
                return
            if self.main.fastsim.package != 'fastjet':
                logging.getLogger('MA5').error("Smearing/tagging/reconstruction require fastjet as a fastsim package. ")
                return
            self.main.superfastsim.define(args,self.main.multiparticles)
            return

        #Checking argument number
        if not len(args) > 2:
            logging.getLogger('MA5').error("wrong number of arguments for the command 'define'.")
            self.help()
            return

        #Looking for '='
        if not args[1] == '=':
            logging.getLogger('MA5').error("syntax error with the command 'define'.")
            self.help()
            return

        #Calling fill
        self.fill(args[0],args[2:],self.main.forced)


    def fill(self,name,args,forced=False):
        # Checking if the name is authorized
        if name in self.reserved_words:
            logging.getLogger('MA5').error("name '" +name+ "' is a reserved keyword. Please choose a different name.")
            return

        # Checking if the name is authorized
        if not self.IsAuthorizedLabel(name):
            logging.getLogger('MA5').error("syntax error with the name '" + name + "'.")
            logging.getLogger('MA5').error("A correct name contains only characters being letters, digits or the '+', '-', '~' and '_' symbols.")
            logging.getLogger('MA5').error("Moreover, a correct name  starts with a letter or the '_' symbol.")
            return

        # Checking if no dataset with the same name has been defined
        if self.main.datasets.Find(name):
            logging.getLogger('MA5').error("A dataset '"+name+"' already exists. Please choose a different name.")
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
                    logging.getLogger('MA5').error("The (multi)particle '"+item+"' is not defined.")
                    return

        self.main.multiparticles.Add(name,ids,forced)

    def help(help):
        logging.getLogger('MA5').info("   Syntax: define <particle name> = PDG-id")
        logging.getLogger('MA5').info("   Associates a symbol to a specific particle defined by its PDG-id.")
        logging.getLogger('MA5').info("   Syntax: define <(multi)particle name> = <PDG-id / existing (multi)particle> <PDG-id / (multi)particle> ...")
        logging.getLogger('MA5').info("   Associates a symbol to a multiparticle defined by several particles.")
        logging.getLogger('MA5').info("")
        logging.getLogger('MA5').info("   Syntax: define tagger  <p1> as <p2> <function> [<bounds>]")
        logging.getLogger('MA5').info("   Create a tagger of an object <p1> as an object <p2>.")
        logging.getLogger('MA5').info("   The tagging efficiency is given by the function <function>.")
        logging.getLogger('MA5').info("   The bounds correspond to the domain the tagger applies (pt > ..., eta < ..., etc.).")
        logging.getLogger('MA5').info("")
        logging.getLogger('MA5').info("   Syntax: define smearer <p1> with <variable> <function> [<bounds>]")
        logging.getLogger('MA5').info("   Create a smearer for the object <p1>.")
        logging.getLogger('MA5').info("   The smearing function is given by the function <function>.")
        logging.getLogger('MA5').info("   The variable to which the smearer applies is given by <variable>.")
        logging.getLogger('MA5').info("   The bounds correspond to the domain the smearer applies (pt > ..., eta < ..., etc.).")
        logging.getLogger('MA5').info("")
        logging.getLogger('MA5').info("   Syntax: define reco_efficiency <p1> <function> [<bounds>]")
        logging.getLogger('MA5').info("   Define the efficiency to reconstruct the object <p1>.")
        logging.getLogger('MA5').info("   The corresponding efficiency function is given by <function>.")
        logging.getLogger('MA5').info("   The bounds correspond to the domain the efficiency applies (pt > ..., eta < ..., etc.).")


    def complete(self,text,line,begidx,endidx):

        #Getting back arguments
        args = line.split()
        nargs = len(args)
        if not text:
            nargs += 1

        if nargs==2:
            output=['tagger', 'smearer', 'reco_efficiency']
            return self.finalize_complete(text,output)

        elif nargs==3 or (nargs==5 and args[1] == 'tagger'):
            output=['=']
            if args[1] in ['tagger', 'smearer', 'reco_efficiency']:
                output=self.main.multiparticles.GetNames()
            return self.finalize_complete(text,output)

        elif nargs==4 and args[1] == 'tagger':
            output = ['as']
            return self.finalize_complete(text,output)

        elif nargs==4 and args[1] == 'smearer':
            output = ['with']
            return self.finalize_complete(text,output)

        elif nargs==5 and args[1] == 'smearer':
            output = ['PT','ETA','PHI','E','D0','PX','PY','PZ']
            return self.finalize_complete(text,output)

        elif nargs>3 and args[1] not in ['tagger', 'smearer', 'reco_efficiency']:
            output=self.main.multiparticles.GetNames()
            return self.finalize_complete(text,output)
        else:
            return

