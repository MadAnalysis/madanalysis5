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
import glob

class DelphesMigration():

    def __init__(self,main):
        self.main = main

    def Migrate(self):
        self.ChangeTreeNames()

    def ChangeTreeNames(self):
        myfiles = glob.glob(self.main.ma5dir+'/tools/delfes/readers/Delphes*.cpp')
        for myfile in myfiles:
            self.ChangeTreeName(myfile)

    def ChangeTreeName(self,filename):

        try:
            input = open(filename)
        except:
            return False

        try:
            output = open(filename+"~","w")
        except:
            return False

        for line in input:
            if line.find("treeWriter")!=-1 and line.find("ExRootTreeWriter")!=1:
                line = line.replace('"Delphes"','"Delfes"')
            output.write(line)
                
        output.close()
        input.close()
        os.system("mv "+filename+"~ "+filename)
        return True

        
        
        
        
        
