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


from madanalysis.selection.instance_name      import InstanceName
from madanalysis.IOinterface.folder_writer    import FolderWriter
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.core.string_tools            import StringTools
import logging
import shutil
import os
import commands

class LayoutWriter():

    def __init__(self,main,jobdir):
        self.main   = main
        self.jobdir = jobdir

    
    def WriteLayoutConfig(self):
        
        # open the file in write-only mode 
        filename = os.path.normpath(self.jobdir+"/layout.ma5")
        try:
            file = open(filename,"w")
        except:
            logging.error("impossible to create the file '"+filename+"'")

        # Writing header
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('#'+StringTools.Center('MADANALYSIS5 CONFIGURATION FILE FOR PLOTS',78)+'#\n')
        file.write('#'+StringTools.Center('produced by MadAnalysis5 version '+self.main.version,78)+'#\n')
        file.write('#'+StringTools.Center(self.main.date,78)+'#\n')
        file.write(StringTools.Fill('#',80)+'\n')

        # Writing file block
        file.write('<File>\n')
        file.write('jodir = \n')
        file.write('html = 1\n')
        file.write('latex = 1\n')
        file.write('pdflatex = 1\n')
        file.write('</File>\n')

        # Writing main block
        file.write('<Main>\n')
        file.write('lumi = 10 # fb^{-1}\n')
        file.write('S_over_B = "S/B"\n')
        file.write('S_over_B_error = "S/B"\n')
        file.write('stack = stack\n')
        file.write('</Main>\n')

        # Writing dataset
        for dataset in self.main.datasets:
            file.write('<Dataset name="'+dataset.name+'">\n')
            file.write('<Physics>\n')
            file.write('background = '+str(main.background)+'\n')
            file.write('weight = '+str(main.weight)+'\n')
            file.write('xsection = '+str(main.xsection)+'\n')
            file.write('</Physics>\n')
            file.write('<Layout>\n')
            file.write('title = '+str(title)+'\n')
            file.write('linecolor = '+str(main.linecolor)+'\n')
            file.write('linestyle = '+str(main.linestyle)+'\n')
            file.write('lineshade = '+str(main.lineshade)+'\n')
            file.write('linewidth = '+str(main.linewidth)+'\n')
            file.write('backcolor = '+str(main.backcolor)+'\n')
            file.write('backstyle = '+str(main.backstyle)+'\n')
            file.write('backshade = '+str(main.backshade)+'\n')
            file.write('</Layout>\n')
            file.write('</Dataset>\n')

        # Writing selection
        counter=0
        for item in self.main.selection:
            if item.__class__.__name__=="Histogram":
               file.write('<Histogram name="selection'+str(counter)+'"\n')
               file.write('stack = '+str(stack)+'\n')
               file.write('titleX = '+str(titleX)+'\n')
               file.write('titleY = '+str(titleY)+'\n')
               file.write('</Histogram>\n')

            elif item.__class__.__name__=="Cut":
               file.write('<Cut name="selection'+str(counter)+'"\n')
               file.write('</Cut>\n')

            counter+=1

        # close the file
        try:
            file.close()
        except:
            logging.error("impossible to close the file '"+filename+"'")

