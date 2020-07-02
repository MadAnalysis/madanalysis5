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


from string_tools                             import StringTools
from shell_command                            import ShellCommand
import logging
import shutil
import os
import sys
import commands

class HistoMatplotlibProducer():

    def __init__(self,histo_path,filenames):
        self.filenames  = []
        for filename in filenames:
            self.filenames.append(filename+'.py')
        self.histo_path = histo_path


    def Execute(self):
        if not self.WriteMainFile():
            return False
        if not self.LaunchInteractiveMatplotlib():
            return False
        return True
        

    def WriteMainFile(self):
        output = open(self.histo_path+'/all.py','w')
        output.write('# Import all histograms\n')
        for item in self.filenames:
            output.write('import '+item.split('/')[-1][:-3]+'\n')
        output.write('\n')
        output.write('# Producing each histograms\n')
        output.write('print "BEGIN-STAMP"\n')
        for item in self.filenames:
            myname=item.split('/')[-1][:-3]
            output.write('print "- Producing histo '+myname+'..."\n')
            output.write(myname+'.'+myname+'()\n')
        output.write('print "END-STAMP"\n')
        output.close()
        return True


    def LaunchInteractiveMatplotlib(self):
        # Commands
        theCommands=[sys.executable,'all.py']

        # Log file name
        logname=os.path.normpath(self.histo_path+'/matplotlib.log')

        # Execute
        logging.getLogger('MA5').debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             self.histo_path,\
                                             silent=False)
        # return result
        if not ok:
            logging.getLogger('MA5').error('impossible to execute MatPlotLib. For more details, see the log file:')
            logging.getLogger('MA5').error(logname)
        return ok


