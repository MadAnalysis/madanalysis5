################################################################################
#  
#  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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
from madanalysis.enumeration.ma5_running_type import MA5RunningType
import logging
from six.moves import range

class CmdRemove(CmdBase.CmdBase):
    """Command REMOVE"""

    def __init__(self,main):
        CmdBase.CmdBase.__init__(self,main,"remove")

    def remove_input(self,name):
        # Dataset removal
        if self.main.datasets.Find(name):
            self.main.datasets.Remove(name)
            return

       # Multiparticle removal
        if self.main.multiparticles.Find(name):
            theList = self.main.selection.GetItemsUsingMultiparticle(name) 
            if len(theList) == 0:
                self.main.multiparticles.Remove(name,self.main.mode)
            else:
                self.logger.error("The Particle/Multiparticle '" + name + "' cannot be removed, being used by: ")
                for item in theList:
                    self.logger.error(" - "+self.main.selection[item].GetStringDisplay())
                self.logger.error("Please remove these plots/cuts before removing the Particle/Multiparticle "+ name +".")
            return

        # Jet collection removal
        if name in self.main.jet_collection.GetNames():
            self.main.jet_collection.Delete(name)
            return

        # Region removal
        if self.main.regions.Find(name):
            self.remove_region(name)
            return

        # No object found 
        self.logger.error("No object called '"+name+"' found.")


    # Removal of a histogram or a cut
    def remove_selection(self,index):
        self.main.selection.Remove(index)
        return 

    # Removal of a signal region
    def remove_region(self, name):
        for index in range(len(self.main.selection) - 1, -1, -1):
            item = self.main.selection[index]
            if name not in item.regions:
                continue
            item.regions = [ region for region in item.regions if region != name]
            if not item.regions:
                self.logger.warning(f"   Removing selection #{index+1} solely attached to region {name}")
                self.main.selection.Remove(index + 1)
        self.main.regions.Remove(name)


    def do(self,args):

        if len(args)==1:
            self.remove_input(args[0])
        elif len(args)==4:
            if args[0]!='selection' or args[1]!='[' or not args[2].isdigit() or args[3]!="]":
                logging.getLogger('MA5').error("wrong syntax for the command 'remove'.")
                return
            self.remove_selection(int(args[2]))
        else:
            logging.getLogger('MA5').error("wrong number of arguments for the command 'remove'.")
            self.help()
            return


    def help(self):
        logging.getLogger('MA5').info("   Syntax: remove <object name>")
        logging.getLogger('MA5').info("   Removing an existing object or region from the memory.")
        logging.getLogger('MA5').info("   Removing a region also removes all cuts and histograms associated exclusively with it.")

    def complete(self,text,line,begidx,endidx):

        # remove selection[i]
        # 0      1
        args = line.split()
        nargs = len(args)
        if not text:
            nargs += 1

        if nargs>2:
            return []
        else:
            output = [ "selection["+str(ind+1)+"]" \
                       for ind in range(0,len(self.main.selection)) ]
            output.extend(self.main.datasets.GetNames())
            output.extend(self.main.jet_collection.GetNames())
            output.extend(self.main.multiparticles.GetNames())
            output.extend(self.main.regions.GetNames())

            # Cannot possible to remove invis
            if self.main.mode != MA5RunningType.RECO:
                output.remove("invisible")
                output.remove("hadronic")
            return self.finalize_complete(text,output)
    


