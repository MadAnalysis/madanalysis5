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
import logging
class TextFileWriter():

    def __init__(self,filename):
        self.filename = filename
        self.isopen = False

    def Open(self):
        if self.isopen:
            logging.getLogger('MA5').error("the file called '"+self.filename+"' cannot be opened. It is already opened")
            return False
        try:
            self.file = open ( self.filename, "w" )
            self.isopen = True
            return True
        except:
            logging.getLogger('MA5').error("Impossible to create the file called '" + self.filename + "'")
            return False

    def Close(self):
        if self.isopen:
            self.file.close()
            self.isopen = False

