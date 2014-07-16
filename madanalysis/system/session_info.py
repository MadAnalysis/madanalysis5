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

class SessionInfo():

    def __init__(self):
        self.editor             = ""
        self.username           = ""
        self.tmpdir             = ""
        self.downloaddir        = ""
        self.recasttools_path   = ""
        self.has_matplotlib     = False
        self.has_gnuplot        = False
        self.has_pdflatex       = False
        self.has_latex          = False
        self.has_recasttools    = False
        self.has_dvipdf         = False
        self.has_web            = True

    def dump(self):
        for item in self.__dict__:
            logging.debug(item+'\t'+str(self.__dict__[item]))

    def __eq__(self,other):
        return self.__dict__==other.__dict__

    def __neq__(self,other):
        return not self.__eq__(other)

    def save(self,filename):

        # Open the file
        try:
            file = open(filename,"w")
        except:
            logging.error("impossible to write the configuration file '" + \
                          filename + "'")
            return False

        # Dump data
        import pickle
        try:
            pickle.dump(self,file)
            test=True
        except:
            logging.error("error occured during saving data to "+filename)
            test=False

        # Close the file
        file.close()

        # Return the operation status
        return test
        
    def load(self,filename):

        # Open the file
        try:
            file = open(filename,"r")
        except:
            logging.error("impossible to read the configuration file '" + \
                          filename + "'")
            return False

        # Import data
        import pickle
        try:
            newone = pickle.load(file)
            test=True
        except:
            logging.error("error occured during reading data from "+filename)
            test=False

        # Close the file
        file.close()

        if not test:
            return False

        # Fill the class variables
        import copy
        try:
            for item in self.__dict__:
                self.__dict__[item]=copy.copy(newone.__dict__[item])
        except:
            logging.error("error occured during copying data from "+filename)
            test=False

        # Return the operation status
        return test
