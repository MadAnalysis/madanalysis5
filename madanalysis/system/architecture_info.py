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


import logging
class ArchitectureInfo:

    def __init__(self):

        self.ma5_version = ""
        self.ma5_date    = ""
        self.ma5dir      = ""

        self.platform = ""
        self.release  = ""

        # Main flags
        self.isMac = False

        # Is there optional package?
        self.has_root           = False
        self.has_fastjet        = False
        self.has_zlib           = False
        self.has_delphes        = False
        self.has_delphesMA5tune = False

        # Library to put before all the others?
        self.root_priority           = False
        self.zlib_priority           = False
        self.delphes_priority        = False
        self.delphesMA5tune_priority = False
        self.fastjet_priority        = False

        # Library files
        self.zlib_original_libs           = []
        self.fastjet_original_bins        = []
        self.root_original_bins           = []
        self.delphes_original_libs        = []
        self.delphesMA5tune_original_libs = []

        # Version
        self.python_version   = ""
        self.gcc_version      = ""
        self.make_version     = ""
        self.gfortran_version = ""
        self.root_version     = ""
        self.fastjet_version  = ""

        # Some library information to detect any change
        self.libraries        = {}
        self.headers          = {}
        self.ncores           = 0

        # Library to put before all the others
        self.toPATH1   = []
        self.toLDPATH1 = []

        # Library to put after all the others
        self.toPATH2   = []
        self.toLDPATH2 = []

        self.root_compiler=''
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
            logging.getLogger('MA5').debug(item+'\t'+str(self.__dict__[item]))

    def __eq__(self,other):
        logging.getLogger('MA5').debug("Compare 2 ArchitureInfo objects:")
        logging.getLogger('MA5').debug("The current one (number of items="+str(len(self.__dict__))+"):")
        logging.getLogger('MA5').debug(str(self.__dict__))
        logging.getLogger('MA5').debug("The other   one (number of items="+str(len(other.__dict__))+"):")
        logging.getLogger('MA5').debug(str(other.__dict__))
        items_ok = self.__dict__.keys() == other.__dict__.keys()
        if not items_ok:
            diff = list(set(self.__dict__.keys()) - set(other.__dict__.keys()))
            logging.getLogger('MA5').debug("The comparison of categories -> differences detected: "+str(diff))
            return False
        logging.getLogger('MA5').debug("The comparison of categorie names -> OK")
        logging.getLogger('MA5').debug("The comparison of categorie values:")
        diff=False
        for key in self.__dict__.keys():
            if self.__dict__[key] != other.__dict__[key]:
                logging.getLogger('MA5').debug("  -> difference here: "+str(key))
                diff=True
        if not diff:
            logging.getLogger('MA5').debug("  -> OK")

        return self.__dict__==other.__dict__

    def __neq__(self,other):
        return not self.__eq__(other)

    def save(self,filename):

        # Open the file
        try:
            file = open(filename,"w")
        except:
            logging.getLogger('MA5').error("impossible to write the configuration file '" + \
                          filename + "'")
            return False

        # Dump data
        import pickle
        try:
            pickle.dump(self,file)
            test=True
        except:
            logging.getLogger('MA5').error("error occured during saving data to "+filename)
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
            logging.getLogger('MA5').error("impossible to read the configuration file '" + \
                          filename + "'")
            return False

        # Import data
        import pickle
        try:
            newone = pickle.load(file)
            test=True
        except:
            logging.getLogger('MA5').error("error occured during reading data from "+filename)
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
            logging.getLogger('MA5').error("error occured during copying data from "+filename)
            test=False

        # Return the operation status
        return test

    def Compare(self, other):
        return self==other

