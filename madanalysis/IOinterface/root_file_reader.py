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


import logging
class RootFileReader():

    def __init__(self,filename):
        self.filename = filename
        self.isopen = False

    def Open(self):
        if self.isopen:
            logging.error("The file called '"+self.filename+"' cannot be opened. It is already opened")
            return False
        from ROOT import TFile
        self.file = TFile(self.filename)
        if self.file.IsZombie():
            logging.error("file called '"+self.filename+"' is not found or is not a ROOT file.")
            return False
        else:
            self.isopen = True
            return True

    def Close(self):
        if self.isopen:
            file.Close()

    def Get(self,name,object_type='',displayerror=True):

        # Getting pointer to object in file
        object = self.file.Get(name)

        # Checking if pointer is null
        if not bool(object):
            if displayerror:
                logging.error("branch called '"+name+"' is not found in the file " +\
                              self.filename)
            return

        # Checking type
        if type!='' and type(object).__name__!=object_type and displayerror:
            logging.error("branch called '"+name+"' is a '"+\
                          type(object).__name__+"' object instead of '"+\
                          object_type+"'")
        return object
        

