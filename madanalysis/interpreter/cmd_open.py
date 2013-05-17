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


from madanalysis.interpreter.cmd_base            import CmdBase
from madanalysis.IOinterface.html_report_writer  import HTMLReportWriter
from madanalysis.IOinterface.latex_report_writer import LATEXReportWriter
import logging
import os
import glob

class CmdOpen(CmdBase):
    """Command OPEN"""


    def __init__(self,main):
        CmdBase.__init__(self,main,"open")


    def do(self,args):

        # Are we in script mode ?
        if self.main.script:
            logging.error("command 'open' is not available in script mode")
            return

        # Checking argument number
        if len(args) == 0:
            if self.main.lastjob_name=='':
                logging.error("No analysis has been run -> no report to open.")
                logging.error("To open an existing report, please type the relevant path.")
                return
            else:
                args.append(self.main.lastjob_name+'/HTML')
        if len(args) != 1:
            logging.error("wrong number of arguments for the command 'open'.")
            self.help()
            return

        # Check directory presence
        if args[0][0] != '/':
           name = os.path.normpath(self.main.currentdir + "/" + args[0])
        else:
           name = args[0]
        if not os.path.isdir(name):
            logging.error("No directory called '"+args[0]+"' is not found")
            return False
            
        # Detect report structure
        filename=""
        if HTMLReportWriter.CheckStructure(name):
            filename="index.html"
        elif LATEXReportWriter.CheckStructure(name):
            if os.path.isfile(name + "/main.pdf"):
                filename="main.pdf"
            else:
                filename="main.tex"
        else:
            logging.error("Directory called '"+args[0]+"' has not the structure of a MadAnalysis report")
            return False
            
        
        # Computing the absolute name
        name = "file://"+os.path.normpath(name + "/" + filename)

        # Loading web browser module  
        import webbrowser

        # Opening a Web Browser window with the page
        webbrowser.open(name)
        
        return


    def help(self):
        logging.info("   Syntax: open <report_directory>")
        logging.info("   Opening a report with the default text editor or web browser")
        logging.info("   If no argument is provided, the latest generated HTML report is open")


    def complete(self,text,line,begidx,endidx):

        #Getting back arguments
        args = line.split()
        nargs = len(args)
        if not text:
            nargs += 1
        
        #Checking number of arguments
        if nargs==2:
            output=[]
            for file in glob.glob(text+"*"):

                # directory presence 
                if not os.path.isdir(file):
                    continue

                # check structure
#                if not HTMLReportWriter.CheckStructure(file) and \
#                   not LATEXReportWriter.CheckStructure(file):
#                    continue
                
                output.append(file)

            return self.finalize_complete(text,output)
        else:
            return []

