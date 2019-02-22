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
class UserInfo:

    def __init__(self):
        # General
        self.tmp_dir        = None
        self.download_dir   = None
        self.webaccess_veto = None

        # Root
        self.root_veto = None
        self.root_bin  = None

        # MatPlotLib
        self.matplotlib_veto = None

        # SciPy
        self.scipy_veto = None

        # Delphes
        self.delphes_veto     = None
        self.delphes_includes = None
        self.delphes_libs     = None

        # DelphesMA5tune
        self.delphesMA5tune_veto     = None
        self.delphesMA5tune_includes = None
        self.delphesMA5tune_libs     = None

        # Zlib
        self.zlib_veto     = None
        self.zlib_includes = None
        self.zlib_libs     = None

        # Fastjet
        self.fastjet_veto     = None
        self.fastjet_bin_path = None

        # PAD
        self.pad_veto       = None
        self.pad_build_path = None

        # PADForMA5Tune
        self.padma5_veto       = None
        self.padma5_build_path = None

        # Pdflatex
        self.pdflatex_veto = None

        # latex
        self.latex_veto = None

        # dvipdf
        self.dvipdf_veto = None

        # logger
        self.logger = logging.getLogger('MA5')

    def dump(self):
        for item in self.__dict__:
            self.logger.debug(item+'\t'+str(self.__dict__[item]))

    def __eq__(self,other):
        return self.__dict__==other.__dict__

    def __neq__(self,other):
        return not self.__eq__(other)

    def save(self,filename):

        # Open the file
        try:
            file = open(filename,"w")
        except:
            self.logger.error("impossible to write the configuration file '" + \
                          filename + "'")
            return False

        # Dump data
        import pickle
        try:
            pickle.dump(self,file)
            test=True
        except:
            self.logger.error("error occured during saving data to "+filename)
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
            self.logger.error("impossible to read the configuration file '" + \
                          filename + "'")
            return False

        # Import data
        import pickle
        try:
            newone = pickle.load(file)
            test=True
        except:
            self.logger.warning("error occured during reading data from "+filename)
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
            self.logger.error("error occured during copying data from "+filename)
            test=False

        # Return the operation status
        return test


    def ConvertToBool(self,option,value,filename):
        if value=='0':
            return False
        elif value=='1':
            return True
        else:
            self.logger.warning(filename+': the option called "'+option+'" allows only the values "1" or "0"')
            return None
        

    def SetValue(self,option,value,filename):

        # General
        if   option=='tmp_dir':
            self.tmp_dir=value
        elif option=='download_dir':
            self.download_dir=value
        elif option=='webaccess_veto':
            self.webaccess_veto=self.ConvertToBool(option,value,filename)

        # Root
        elif   option=='root_bin_path':
            self.root_bin=value
        elif   option=='root_veto':
            self.root_veto=self.ConvertToBool(option,value,filename)

        # Matplotlib
        elif   option=='matplotlib_veto':
            self.matplotlib_veto=self.ConvertToBool(option,value,filename)

        # Scipy
        elif   option=='scipy_veto':
            self.scipy_veto=self.ConvertToBool(option,value,filename)

        # Delphes
        elif option=='delphes_veto':
            self.delphes_veto=self.ConvertToBool(option,value,filename)
        elif option=='delphes_includes':
            self.delphes_includes=value
        elif option=='delphes_libs':
            self.delphes_libs=value

        # DelphesMA5tune
        elif option=='delphesMA5tune_veto':
            self.delphesMA5tune_veto=self.ConvertToBool(option,value,filename)
        elif option=='delphesMA5tune_includes':
            self.delphesMA5tune_includes=value
        elif option=='delphesMA5tune_libs':
            self.delphesMA5tune_libs=value

        # Zlib
        elif option=='zlib_veto':
            self.zlib_veto=self.ConvertToBool(option,value,filename)
        elif option=='zlib_includes':
            self.zlib_includes=value
        elif option=='zlib_libs':
            self.zlib_libs=value

        # Fastjet
        elif option=='fastjet_veto':
            self.fastjet_veto=self.ConvertToBool(option,value,filename)
        elif option=='fastjet_bin_path':
            self.fastjet_bin_path=value

        # PAD
        elif option=='pad_veto':
            self.pad_veto=self.ConvertToBool(option,value,filename)
        elif option=='pad_build_path':
            self.pad_build_path=value

        # PADForMA5Tune
        elif option=='padma5_veto':
            self.padma5_veto=self.ConvertToBool(option,value,filename)
        elif option=='padma5_build_path':
            self.padma5_build_path=value

        # Pdflatex
        elif option=='pdflatex_veto':
            self.pdflatex_veto=self.ConvertToBool(option,value,filename)

        # latex
        elif option=='latex_veto':
            self.latex_veto=self.ConvertToBool(option,value,filename)
            
        # dvipdf
        elif option=='dvipdf_veto':
            self.dvipdf_veto=self.ConvertToBool(option,value,filename)

        # other
        else:
            self.logger.warning(filename+': the option called "'+option+'" is not found')
        

    def ReadUserOptions(self,filename):

        # Open the user options
        self.logger.debug("Opening the file: "+filename)
        try:
            input = open(filename)
        except:
            self.logger.error('impossible to open the file: '+filename)
            return False

        # Loop over the file
        self.logger.debug("Lines to interpret: ")

        for line in input:

            if '#' in line:
                line = line.split('#')[0]
            line=line.lstrip()
            line=line.rstrip()
            if line=='':
                continue
            self.logger.debug("  - "+line)
            words=line.split('=')
            if len(words)!=2:
                self.logger.warning(filename+': the following line is incorrect and is skipped:')
                self.logger.warning(line)
            words[0]=words[0].lstrip()
            words[0]=words[0].rstrip()
            words[1]=words[1].lstrip()
            words[1]=words[1].rstrip()

            self.SetValue(words[0], words[1], filename)
    
        # Close the file
        self.logger.debug("Closing the file: "+filename)
        input.close()
        
        # Ok
        return True
