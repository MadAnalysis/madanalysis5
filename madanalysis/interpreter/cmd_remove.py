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
from madanalysis.enumeration.ma5_running_type import MA5RunningType
import logging

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
            if len(theList) is 0:
                self.main.multiparticles.Remove(name,self.main.mode)
            else:
                logging.error("The Particle/Multiparticle '" + name + \
                              "' cannot be removed, being used by: ")
                for item in theList:
                    logging.error(" - "+self.main.selection[item].GetStringDisplay())
                logging.error("Please remove these plots/cuts before removing the Particle/Multiparticle "+ name +".")
            return
                
        # No object found 
        logging.error("No object called '"+name+"' found.")


    def remove_selection(self,index):

        self.main.selection.Remove(index)
        return 

    def do(self,args):

        if len(args)==1:
            self.remove_input(args[0])
        elif len(args)==4:
            if args[0]!='selection' or args[1]!='[' or not args[2].isdigit() or args[3]!="]":
                logging.error("wrong syntax for the command 'remove'.")
                return
            self.remove_selection(int(args[2]))
        else:
            logging.error("wrong number of arguments for the command 'remove'.")
            self.help()
            return


    def help(self):
        logging.info("   Syntax: remove <object name>")
        logging.info("   Removing an existing object from the memory.")

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
            output.extend(self.main.multiparticles.GetNames())

            # Cannot possible to remove invis
            if self.main.mode != MA5RunningType.RECO:
                output.remove("invisible")
                output.remove("hadronic")
            return self.finalize_complete(text,output)
    


