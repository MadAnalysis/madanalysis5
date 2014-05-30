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

class UserInfo:

    def __init__(self):
        self.root_bin         = '0'
        self.delphes_veto     = '0'
        self.delphes_includes = '0'
        self.delphes_libs     = '0'
        self.delphesMA5tune_veto      = '0'
        self.delphesMA5tune_includes  = '0'
        self.delphesMA5tune_libs      = '0'
        self.zlib_veto        = '0'
        self.zlib_includes    = '0'
        self.zlib_libs        = '0'
        self.fastjet_veto     = '0'
        self.fastjet_bin_path = '0'
        self.pdflatex_veto    = '0'
        self.latex_veto       = '0'
        self.dvipdf_veto      = '0'
        
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

    def ReadUserOptions(self,filename):

        # Open the user options
        logging.debug("Opening the file: "+filename)
        try:
            input = open(filename)
        except:
            logging.error('impossible to open the file: '+filename)
            return False

        # Loop over the file
        logging.debug("Lines to interpret: ")

        for line in input:

            if '#' in line:
                line = line.split('#')[0]
            line=line.lstrip()
            line=line.rstrip()
            if line=='':
                continue
            logging.debug("  - "+line)
            words=line.split('=')
            if len(words)!=2:
                logging.warning(filename+': the following line is incorrect and is skipped:')
                logging.warning(line)
            words[0]=words[0].lstrip()
            words[0]=words[0].rstrip()
            words[1]=words[1].lstrip()
            words[1]=words[1].rstrip()
 
            if words[0]=='root_bin_path':
                self.root_bin=words[1]
            elif words[0]=='delphes_veto':
                self.delphes_veto=words[1]
            elif words[0]=='delphes_includes':
                self.delphes_includes=words[1]
            elif words[0]=='delphes_libs':
                self.delphes_libs=words[1]
            elif words[0]=='delphesMA5tune_veto':
                self.delphesMA5tune_veto=words[1]
            elif words[0]=='delphesMA5tune_includes':
                self.delphesMA5tune_includes=words[1]
            elif words[0]=='delphesMA5tune_libs':
                self.delphesMA5tune_libs=words[1]
            elif words[0]=='zlib_veto':
                self.zlib_veto=words[1]
            elif words[0]=='zlib_includes':
                self.zlib_includes=words[1]
            elif words[0]=='zlib_libs':
                self.zlib_libs=words[1]
            elif words[0]=='fastjet_veto':
                self.fastjet_veto=words[1]
            elif words[0]=='fastjet_bin_path':
                self.fastjet_bin_path=words[1]
            elif words[0]=='pdflatex_veto':
                self.pdflatex_veto=words[1]
            elif words[0]=='latex_veto':
                self.latex_veto=words[1]
            elif words[0]=='dvipdf_veto':
                self.dvipdf_veto=words[1]
            else:
                logging.warning(filename+': the options called "'+words[0]+'" is not found')

        # Close the file
        logging.debug("Closing the file: "+filename)
        input.close()
        
        # Ok
        return True
