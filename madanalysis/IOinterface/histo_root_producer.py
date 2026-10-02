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


"""Compilation and execution of the ROOT macros producing the histograms of the reports.
"""

from __future__ import absolute_import
from __future__ import annotations
from string_tools                             import StringTools
from shell_command                            import ShellCommand
import logging
import shutil
import os

class HistoRootProducer():
    """Compile and run the ROOT macros written by the layout (one ``.C`` file per histogram).

    Attributes:
        filenames (``list[str]``): ROOT macros (``<name>.C``).
        histo_path (``str``): folder of the macros.
    """

    def __init__(self,histo_path: str,filenames: list[str]) -> None:
        """Create the producer.

        Args:
            histo_path (``str``): folder of the macros.
            filenames (``list[str]``): macro names without the ``.C`` extension.
        """
        self.filenames  = []
        for filename in filenames:
            self.filenames.append((filename)+'.C')
        self.histo_path = histo_path


    def Execute(self) -> bool:
        """Write ``all.C``, compile it and run it.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        if not self.WriteMainFile():
            return False
#       if not self.LaunchInteractiveRoot():
        if not self.LaunchCompileRoot():
            return False
        return True
        

    def LaunchInteractiveRoot(self) -> bool:
        """Legacy: run the macros with the ROOT interpreter (``root -l -q -b``).

        Returns:
            ``bool``:
            ``True`` on success.
        """
        # Commands
        theCommands=['root','-l','-q','-b']
        theCommands.extend(self.filenames)

        # Log file name
        logname=os.path.normpath(self.histo_path+'/root.log')
        
        # Execute
        logging.getLogger('MA5').debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             self.histo_path,\
                                             silent=False)
        # return result
        if not ok:
            logging.getLogger('MA5').error('impossible to execute ROOT. For more details, see the log file:')
            logging.getLogger('MA5').error(logname)
        return ok


    def LaunchCompileRoot(self) -> bool:
        """Compile ``all.C`` into the ``goROOT`` executable and run it.

        The logs are ``compile_root.log`` and ``launch_root.log``; the run is successful if both
        stamps printed by ``all.C`` are found.

        Returns:
            ``bool``:
            ``True`` on success.
        """
        import os
        import stat

        # Commands
        output = open(self.histo_path+'/Makefile','w')
        output.write('CXX     = `$(MA5_BASE)/tools/SampleAnalyzer/ExternalSymLink/Bin/root-config --cxx`\n')
        output.write('CFLAGS  = `$(MA5_BASE)/tools/SampleAnalyzer/ExternalSymLink/Bin/root-config --cflags`\n')
        output.write('LIBS    = `$(MA5_BASE)/tools/SampleAnalyzer/ExternalSymLink/Bin/root-config --libs`\n')
        output.write('SOURCE  = all.C\n')
        output.write('PROGRAM = goROOT\n')
        output.write('\n')
        output.write('$(PROGRAM):\n')
        output.write('\t$(CXX) $(CFLAGS) $(SOURCE) $(LIBS) -o $(PROGRAM)\n')
        output.close()

        # Commands
        theCommands=['make']

        # Log file name
        logname=os.path.normpath(self.histo_path+'/compile_root.log')
        
        # Execute
        logging.getLogger('MA5').debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             self.histo_path,\
                                             silent=False)

        # return result
        if not ok:
            logging.getLogger('MA5').error('impossible to execute ROOT. For more details, see the log file:')
            logging.getLogger('MA5').error(logname)
            return ok

        # check
        if not os.path.isfile(self.histo_path+'/goROOT'):
            logging.getLogger('MA5').error('the file '+self.histo_path+'/goROOT is not found')
            logging.getLogger('MA5').error(logname)
            return False
            
        # Commands
        theCommands=[self.histo_path+'/goROOT']

        # Log file name
        logname=os.path.normpath(self.histo_path+'/launch_root.log')
        
        # Execute
        logging.getLogger('MA5').debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             self.histo_path,\
                                             silent=False)

        # return result
        if not ok:
            logging.getLogger('MA5').error('impossible to execute ROOT. For more details, see the log file:')
            logging.getLogger('MA5').error(logname)
            return ok

        # Reading log file
        input = open(self.histo_path+'/launch_root.log')
        ok1 = False
        ok2 = False
        for line in input:
            if line.startswith('BEGIN-STAMP'):
                ok1 = True
            if line.startswith('END-STAMP'):
                ok2 = True
        input.close()
        if not (ok1 and ok2):
            logging.getLogger('MA5').error('wrong behaviour of the ROOT execution. For more details, see the log file:')
            logging.getLogger('MA5').error(logname)
            # FIXME: 'ok' is True here: the missing stamps are logged as an error but success is returned.
            return ok
        
        return ok


    def WriteMainFile(self) -> bool:
        """Write ``all.C`` including every macro and calling every histogram function.

        Returns:
            ``bool``:
            Always ``True``.
        """
        output = open(self.histo_path+'/all.C','w')
        output.write('// STL headers\n')
        output.write('#include <iostream>\n')
        output.write('\n')
        output.write('// ROOT headers\n')
        output.write('#include <TAxis.h>\n')
        output.write('#include <TH1F.h>\n')
        output.write('#include <TLegend.h>\n')
        output.write('#include <TCanvas.h>\n')
        output.write('#include <TFile.h>\n')
        output.write('#include <THStack.h>\n')
        output.write('#include <TStyle.h>\n')
        output.write('#include <TSystem.h>\n')
        output.write('#include <TROOT.h>\n')
        output.write('\n')
        output.write('// Including histograms\n')
        for item in self.filenames:
            output.write('#include "'+item.split('/')[-1]+'"\n')
        output.write('\n')
        output.write('// Main program\n')
        output.write('int main()\n')
        output.write('{\n')
        output.write('  std::cout << "BEGIN-STAMP" << std::endl;\n')
        for item in self.filenames:
            output.write('  '+item.split('/')[-1][:-2]+'();\n')
        output.write('  std::cout << "END-STAMP" << std::endl;\n')
        output.write('  return 0;\n')
        output.write('}\n')
        
        output.close()
        return True


