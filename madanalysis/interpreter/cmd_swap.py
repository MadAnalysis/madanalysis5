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
from madanalysis.interpreter.cmd_base import CmdBase
import logging
from six.moves import range

class CmdSwap(CmdBase):
    """Command SWAP"""

    def __init__(self,main):
        CmdBase.__init__(self,main,"swap")

    def do(self,args):

        # Checking argument number
        if len(args) != 8:
            logging.getLogger('MA5').error("wrong number of arguments for the command 'swap'.")
            self.help()
            return

        # Checking 'selection' and ']' word
        if args[0]!='selection'  or args[1]!='[' or \
           not args[2].isdigit() or args[3]!=']' or \
           args[4]!='selection' or args[5]!='[' or \
           not args[6].isdigit() or args[7]!=']' :
            logging.getLogger('MA5').error("the syntax is not correct.")
            self.help()
            return
        
        # Calling selection method
        self.main.selection.Swap(int(args[2]),int(args[6]))
        
        return
        



    def help(self):
        logging.getLogger('MA5').info("   Syntax: swap selection[X] selection[Y]")
        logging.getLogger('MA5').info("   Swaping the Xth plot/cut with the Yth plot/cut.")

    def complete(self,text,line,begidx,endidx):
        # swap  selection[i] selection[j]
        # 0     1            2  
        args = line.split()
        nargs = len(args)
        if not text:
            nargs +=1

        if nargs>3:
            return []
        else:
            output = [ "selection["+str(ind+1)+"]" \
                       for ind in range(0,len(self.main.selection)) ]
            return self.finalize_complete(text,output)
    


