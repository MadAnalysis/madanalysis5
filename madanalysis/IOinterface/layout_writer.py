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
from string_tools                             import StringTools
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
        from madanalysis.core.main import Main
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('#'+StringTools.Center('MADANALYSIS5 CONFIGURATION FILE FOR PLOTS',78)+'#\n')
        file.write('#'+StringTools.Center('produced by MadAnalysis5 version '+Main.version,78)+'#\n')
        file.write('#'+StringTools.Center(Main.date,78)+'#\n')
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('\n')

        # Writing file block
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('#'+StringTools.Center('Files and Paths',78)+'#\n')
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('<Files>\n')
        file.write('  jodir = '+self.jobdir+'\n')
        file.write('  html = 1\n')
        file.write('  latex = 1\n')
        file.write('  pdflatex = 1\n')
        file.write('</Files>\n')
        file.write('\n')

        # Writing main block
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('#'+StringTools.Center('Global information related to the layout',78)+'#\n')
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('<Main>\n')
        file.write('  lumi = '+str(self.main.lumi)+' # fb^{-1}\n')
        file.write('  S_over_B = "'+ str(self.main.SBratio)+'"\n')
        file.write('  S_over_B_error = "'+str(self.main.SBerror)+ '"\n')
        file.write('  stack = '+str(self.main.stack)+'\n')
        file.write('</Main>\n')
        file.write('\n')

        # Writing dataset
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('#'+StringTools.Center('Definition of datasets',78)+'#\n')
        file.write(StringTools.Fill('#',80)+'\n')
        for dataset in self.main.datasets:
            file.write('<Dataset name="'+dataset.name+'">\n')
            file.write('  <Physics>\n')
            file.write('    background = '+str(dataset.background)+'\n')
            file.write('    weight = '+str(dataset.weight)+'\n')
            file.write('    xsection = '+str(dataset.xsection)+'\n')
            file.write('  </Physics>\n')
            file.write('  <Layout>\n')
            file.write('    title = '+str(dataset.title)+'\n')
            file.write('    linecolor = '+str(dataset.linecolor)+'\n')
            file.write('    linestyle = '+str(dataset.linestyle)+'\n')
            file.write('    lineshade = '+str(dataset.lineshade)+'\n')
            file.write('    linewidth = '+str(dataset.linewidth)+'\n')
            file.write('    backcolor = '+str(dataset.backcolor)+'\n')
            file.write('    backstyle = '+str(dataset.backstyle)+'\n')
            file.write('    backshade = '+str(dataset.backshade)+'\n')
            file.write('  </Layout>\n')
            file.write('</Dataset>\n')
            file.write('\n')

        # Writing selection
        file.write(StringTools.Fill('#',80)+'\n')
        file.write('#'+StringTools.Center('Definition of the selection : histograms and cuts',78)+'#\n')
        file.write(StringTools.Fill('#',80)+'\n')
        counter=0
        for item in self.main.selection:
            if item.__class__.__name__=="Histogram":
                file.write('<Histogram name="selection'+str(counter)+'">\n')
                file.write('  stack = '+str(item.stack)+'\n')
                file.write('  titleX = "'+str(item.GetXaxis())+'"\n')
                file.write('  titleY = "'+str(item.GetYaxis())+'"\n')
                file.write('  xmin = '+str(item.xmin)+'\n')
                file.write('  xmax = '+str(item.xmax)+'\n')
                file.write('</Histogram>\n')
                file.write('\n')

            if item.__class__.__name__=="HistogramFrequency":
                pass

            elif item.__class__.__name__=="Cut":
                file.write('<Cut name="selection'+str(counter)+'"/>\n')
                file.write('\n')

            counter+=1

        # Must we definite multiparticles ?
        MustBeDefined = False
        for item in self.main.selection:
            if item.__class__.__name__=="Histogram" and \
               item.observable.name in ["NPID","NAPID"]:
                MustBeDefined=True
                break

        # Definition of multiparticles
        if MustBeDefined:
            file.write(StringTools.Fill('#',80)+'\n')
            file.write('#'+StringTools.Center('Definition of the multiparticles used',78)+'#\n')
            file.write(StringTools.Fill('#',80)+'\n')
            sorted_keys = sorted(self.main.multiparticles.table.keys())
            for key in sorted_keys:
                file.write('<Multiparticle name="'+str(key)+'">\n')
                file.write('  ')
                for id in self.main.multiparticles.table[key]:
                    file.write(str(id)+'  ')
                file.write('\n')
                file.write('</Multiparticle>\n')
            
        
        # close the file
        try:
            file.close()
        except:
            logging.error("impossible to close the file '"+filename+"'")
