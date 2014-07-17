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

class ArchitectureInfo:

    def __init__(self):

        self.ma5_version = ""
        self.ma5_date    = ""
        self.ma5dir      = ""

        self.root_priority      = False
        self.has_zlib           = False
        self.zlib_priority      = False
        self.has_delphes        = False
        self.delphes_priority   = False
        self.has_delphesMA5tune = False
        self.delphesMA5tune_priority = False
        self.has_fastjet        = False
        self.fastjet_priority   = False
        self.has_fortran        = False
        self.has_root           = False
        self.isMac              = False
        
        self.platform         = ""
        self.release          = ""
        self.python_version   = ""
        self.gcc_version      = ""
        self.make_version     = ""
        self.gfortran_version = ""
        self.root_version     = ""
        self.fastjet_version  = ""
        self.libraries        = {}
        self.headers          = {}
        self.ncores           = 0

        self.toPATH1   = []
        self.toLDPATH1 = []
        self.toPATH2   = []
        self.toLDPATH2 = []
    
        self.root_bin_path=""
        self.root_inc_path=""
        self.root_lib_path=""
        self.root_features=[]
        self.zlib_inc_path=""
        self.zlib_lib_path=""
        self.zlib_lib=""
        self.delphes_inc_paths=[]
        self.delphes_lib_paths=[]
        self.delphes_lib=""
        self.delphesMA5tune_inc_paths=[]
        self.delphesMA5tune_lib_paths=[]
        self.delphesMA5tune_lib=""
        self.fastjet_bin_path=""
        self.fastjet_lib_paths=[]


    def dump(self):
        for item in self.__dict__:
            logging.debug(item+'\t'+str(self.__dict__[item]))

    def __eq__(self,other):
        logging.debug("Compare 2 ArchitureInfo objects:")
        logging.debug("The current one:")
        logging.debug(str(self.__dict__))
        logging.debug("The other one:")
        logging.debug(str(other.__dict__))
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

    def Compare(self, other):
        return self==other

