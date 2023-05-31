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
import madanalysis.interpreter.cmd_base as CmdBase
from madanalysis.enumeration.ma5_running_type       import MA5RunningType
import logging

class CmdDefine(CmdBase.CmdBase):
    """Command DEFINE"""

    def __init__(self,main):
        CmdBase.CmdBase.__init__(self,main,"define")

    def do(self,args):

        # tagger / smearer
        if args[0] in ['tagger', 'smearer', 'reco_efficiency', 'jes', 'scaling', 'energy_scaling']:
            if self.main.mode != MA5RunningType.RECO:
                logging.getLogger('MA5').error("Smearing/tagging/reconstruction/scaling are only available in the RECO mode")
                logging.getLogger('MA5').error("Please restart the program with './bin/ma5 -R '")
                return
            if self.main.fastsim.package != 'fastjet':
                logging.getLogger('MA5').error("Smearing/tagging/reconstruction/scaling requires FastJet as a fastsim package. ")
                return
            self.main.superfastsim.define(args,self.main.multiparticles)
            return

        # Jet Definition
        if args[0] == 'jet_algorithm':
            if self.main.mode != MA5RunningType.RECO:
                logging.getLogger('MA5').error("Jet algorithms are only available in the RECO mode")
                logging.getLogger('MA5').error("Please restart the program with './bin/ma5 -R '")
                return
            if self.main.fastsim.package != 'fastjet':
                logging.getLogger('MA5').error("Jet algorithms requires FastJet as a fastsim package. ")
                return
            ok = self.main.jet_collection.define(args,self.main.datasets.GetNames()+\
                                                     [self.main.fastsim.clustering.JetID])
            if ok and len(self.main.jet_collection)==1:
                # Multi-cluster protection
                logging.getLogger('MA5').warning("Constituent-based smearing will be applied.")
                self.main.superfastsim.jetrecomode = 'constituents'
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
            logging.getLogger('MA5').error("A correct name contains only characters being letters, "+\
                                           "digits or the '+', '-', '~' and '_' symbols.")
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
        logging.getLogger('MA5').info("   Syntax: define <(multi)particle name> = "+\
                                      "<PDG-id / existing (multi)particle> <PDG-id / (multi)particle> ...")
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
        logging.getLogger('MA5').info("")
        logging.getLogger('MA5').info("   Syntax: define jes <function> [<bounds>]")
        logging.getLogger('MA5').info("   Define the Jet Energy Scale (JES) corrections to apply to clustered jets.")
        logging.getLogger('MA5').info("   The corresponding JES function is given by <function>.")
        logging.getLogger('MA5').info("   The bounds correspond to the domain that JES applies (pt > ..., eta < ..., etc.).")
        logging.getLogger('MA5').info("")
        logging.getLogger('MA5').info("   Syntax: define energy_scaling <p1> <function> [<bounds>]")
        logging.getLogger('MA5').info("   Define the rescaling corrections to apply to the energy of a reconstructed object <p1>.")
        logging.getLogger('MA5').info("   The corresponding energy scaling function is given by <function>.")
        logging.getLogger('MA5').info("   The bounds correspond to the domain that scaling function applies (pt > ..., eta < ..., etc.).")
        # For the future:
        logging.getLogger('MA5').info("")
        logging.getLogger('MA5').info("   Syntax: define scaling <variable> for <p1> <function> [<bounds>]")
        logging.getLogger('MA5').info("   Define rescaling corrections to apply to a variable <variable> for a reconstructed object <p1>.")
        logging.getLogger('MA5').info("   The corresponding scaling function is given by <function>.")
        logging.getLogger('MA5').info("   The bounds correspond to the domain that scaling function applies (pt > ..., eta < ..., etc.).")
        logging.getLogger('MA5').info("")
        algorithms = ['antikt','cambridge', 'genkt','gridjet','kt','genkt', 'cdfjetclu','cdfmidpoint','siscone']
        logging.getLogger('MA5').info('   Syntax: define jet_algorithm <name> <algorithm> <keyword args>')
        logging.getLogger('MA5').info('           - <name>         : Name to be assigned to the jet.')
        logging.getLogger('MA5').info('           - <algorithm>    : Clustering algorithm of the jet. Available algorithms are: ')
        logging.getLogger('MA5').info('                              '+', '.join(algorithms))
        logging.getLogger('MA5').info('           - <keyword args> : (Optional) depending on the nature of the algorithm.')
        logging.getLogger('MA5').info("                              it can be radius=0.4, ptmin=20 etc.")

    def complete(self,text,line,begidx,endidx):

        #Getting back arguments
        args = line.split()
        nargs = len(args)
        if not text:
            nargs += 1

        if nargs==2:
            output=['tagger', 'smearer', 'reco_efficiency', "jes",
                    "energy_scaling", "scaling", "jet_algorithm"]
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

