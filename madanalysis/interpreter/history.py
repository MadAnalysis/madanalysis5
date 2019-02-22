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


# Python import
import os


#===============================================================================
#  History
#===============================================================================
class History():

    def __init__(self,autosave=".history"):
        self.history = []
        self.autosave = autosave


    def Add(self,line):
        # Cleaning the line (safety)
        # Not done in interpreter_base because space is required by tab completion
        line=line.rstrip()

        # Remove simple commands
        toBypass = ["history","exit","quit"]
        if line in toBypass:
            return False

        # Remove commands starting with
        toBypass = ['help','#*']
        for item in toBypass:
            if line.startswith(item):
                return False

        # Add
        self.history.append(line)


    def Reset(self):
        self.history = []


    def Print(self):
        return '\n'.join(self.history)


    def Save(self,filename,forced=False):

        # Failure if the file does not exist
        if (not forced) and os.path.exists(filename):
            return False

        # Save the file
        file = open(filename, 'w')
        file.write('\n'.join(self.history))
        file.close()

        return True
