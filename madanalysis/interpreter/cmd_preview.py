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


from madanalysis.interpreter.cmd_base import CmdBase
from madanalysis.layout.preview import Preview
import logging

class CmdPreview(CmdBase):
    """Command PREVIEW"""

    def __init__(self,main):
        CmdBase.__init__(self,main,"preview")

    def do(self,args):

        # Are we in script mode ?
        if self.main.script:
            logging.error("command 'preview' is not available in script mode")
            return

        # Checking argument number
        if len(args) != 4:
            logging.error("wrong number of arguments for the command 'preview'.")
            self.help()
            return

        # Checking 'selection' and ']' word
        if args[0]!='selection'  or args[1]!='[' or \
           not args[2].isdigit() or args[3]!=']' :
            logging.error("the syntax is not correct.")
            self.help()
            return
        
        # Calling selection method
        preview = Preview(self.main)

        logging.info("   Opening the root file...")
        if not preview.Open():
            return

        logging.info("   Layouting the histogram...")
        if not preview.DoThePlot(int(args[2])):
            return
        
        return
        
    def help(self):
        
        logging.info("   Syntax: preview selection[X]")
        logging.info("   Displaying the Xth plot/cut like it will be presented in a report.")

    def complete(self,text,line,begidx,endidx):
        
        # preview  selection[i]
        # 0        1           
        args = line.split()
        nargs = len(args)
        if not text:
            nargs +=1

        if nargs>2:
            return []
        else:
            output = [ "selection["+str(ind+1)+"]" \
                       for ind in range(0,len(self.main.selection)) ]
            return self.finalize_complete(text,output)
    


