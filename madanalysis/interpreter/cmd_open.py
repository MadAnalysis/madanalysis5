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


"""Interpreter command ``open``: open a HTML or LaTeX report."""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from madanalysis.core.main import Main
from madanalysis.interpreter.cmd_base            import CmdBase
from madanalysis.IOinterface.html_report_writer  import HTMLReportWriter
from madanalysis.IOinterface.latex_report_writer import LATEXReportWriter
import logging
import os
import glob

class CmdOpen(CmdBase):
    """Command ``open [<report_directory>]``."""


    def __init__(self,main: Main) -> None:
        """Register the ``open`` command.

        Args:
            main (``Main``): session state.
        """
        CmdBase.__init__(self,main,"open")


    def do(self,args: list[str]) -> bool | None:
        """Open a report with the default web browser.

        Without argument, the latest HTML report of the last submitted job is opened. The
        command is not available in script mode.

        Args:
            args (``list[str]``): arguments of the command (split by
                :meth:`~madanalysis.interpreter.interpreter_base.InterpreterBase.split_arg`).

        Returns:
            ``bool | None``:
            ``False`` if the directory does not exist or is not a report, ``None`` otherwise.
        """

        # Are we in script mode ?
        if self.main.script:
            logging.getLogger('MA5').error("command 'open' is not available in script mode")
            return

        # Checking argument number
        if len(args) == 0:
            if self.main.lastjob_name=='':
                logging.getLogger('MA5').error("No analysis has been run -> no report to open.")
                logging.getLogger('MA5').error("To open an existing report, please type the relevant path.")
                return
            else:
                i=0
                while(os.path.isdir(self.main.lastjob_name+"/Output/HTML/MadAnalysis5job_"+str(i+1))):
                    i+=1
                # NOTE: if no MadAnalysis5job_<i> folder exists, MadAnalysis5job_0 (non-existing) is used.
                args.append(self.main.lastjob_name+'/Output/HTML/MadAnalysis5job_'+str(i))
        if len(args) != 1:
            logging.getLogger('MA5').error("wrong number of arguments for the command 'open'.")
            self.help()
            return

        # Check directory presence
        if args[0][0] != '/':
           name = os.path.normpath(self.main.currentdir + "/" + args[0])
        else:
           name = args[0]
        if not os.path.isdir(name):
            # FIXME: double negation in the error message ('No directory ... is not found').
            logging.getLogger('MA5').error("No directory called '"+args[0]+"' is not found")
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
            logging.getLogger('MA5').error("Directory called '"+args[0]+"' has not the structure of a MadAnalysis report")
            return False

        # Computing the absolute name
        name = "file://"+os.path.normpath(name + "/" + filename)

        # Loading web browser module  
        import webbrowser

        # Opening a Web Browser window with the page
        webbrowser.open(name)

        return


    def help(self) -> None:
        """Display the help of the ``open`` command."""
        logging.getLogger('MA5').info("   Syntax: open <report_directory>")
        logging.getLogger('MA5').info("   Opening a report with the default text editor or web browser")
        logging.getLogger('MA5').info("   If no argument is provided, the latest generated HTML report is open")


    def complete(self,text: str,line: str,begidx: int,endidx: int) -> list[str]:
        """Tab completion of the ``open`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str]``:
            Folders matching ``text``.
        """

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

