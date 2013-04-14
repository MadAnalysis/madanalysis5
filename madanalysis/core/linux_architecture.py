################################################################################
#  
#  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <http://madanalysis.irmp.ucl.ac.be>
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
class LinuxArchitecture:

    def __init__(self):
        self.platform         = ""
        self.ma5_version      = ""
        self.ma5_date         = ""
        self.python_version   = ""
        self.gcc_version      = ""
        self.gfortran_version = ""
        self.root_version     = ""
        self.fastjet_version  = ""
        self.libraries        = {}
        self.headers          = {}
        self.FAC              = ""


    def Dump(self):
        logging.info(" Linux distrib    = " + self.platform)
        logging.info(" MA5 version      = " + self.ma5_version)
        logging.info(" MA5 date         = " + self.ma5_date)
        logging.info(" Python version   = " + self.python_version)
        logging.info(" gcc version      = " + self.gcc_version)
        logging.info(" gfortran version = " + self.gfortran_version)
        logging.info(" ROOT version     = " + self.root_version)
        logging.info(" FastJet version  = " + self.fastjet_version)
        logging.info(" FAC mode         = " + self.FAC)
        if len(self.libraries)!=0:
            for key, value in self.libraries.items():
                logging.info( " Library '" + key + "' = " + \
                              value )
        if len(self.headers)!=0:
            for key, value in self.headers.items():
                logging.info( " Header  '" + key + "' = " + \
                              value )


    def Export(self,filename):
        try:
            file = open(filename,"w")
        except:
            logging.error("impossible to write the configuration file '" + \
                          filename + "'")
            return False
        file.write(self.platform+"\n")
        file.write(self.ma5_version+"\n")
        file.write(self.ma5_date+"\n")
        file.write(self.python_version+"\n")
        file.write(self.gcc_version+"\n")
        file.write(self.gfortran_version+"\n")
        file.write(self.root_version+"\n")
        file.write(self.fastjet_version+"\n")
        file.write(self.FAC+"\n")
        file.write(str(len(self.libraries.keys()))+"\n")
        for key, value in self.libraries.items():
            file.write(key+"\n")
            file.write(value+"\n")
        file.write(str(len(self.headers.keys()))+"\n")
        for key, value in self.headers.items():
            file.write(key+"\n")
            file.write(value+"\n")
        file.close()
        return True
        

    def Import(self,filename):
        try:
            file = open(filename,"r")
        except:
            logging.error("impossible to read the configuration file '" + \
                          filename + "'")
            return False
        try:
            self.platform         = file.readline().replace('\n','')
            self.ma5_version      = file.readline().replace('\n','')
            self.ma5_date         = file.readline().replace('\n','')
            self.python_version   = file.readline().replace('\n','')
            self.gcc_version      = file.readline().replace('\n','')
            self.gfortran_version = file.readline().replace('\n','')
            self.root_version     = file.readline().replace('\n','')
            self.fastjet_version  = file.readline().replace('\n','')
            self.FAC              = file.readline().replace('\n','')
            Nlibraries = int(file.readline().replace('\n',''))
        except:
            logging.error("missing lines in the configuration file '" + \
                          filename + "'")
            return False
        for i in range(0,Nlibraries):
            try:
                key = file.readline().replace('\n','')
                value = file.readline().replace('\n','')
                self.libraries[key]=value
            except:
                logging.error("missing lines in the configuration file '" + \
                              filename + "'")
                return False
        try:
            Nheaders = int(file.readline().replace('\n',''))
        except:
            logging.error("missing lines in the configuration file '" + \
                          filename + "'")
            return False
        for i in range(0,Nheaders):
            try:
                key = file.readline().replace('\n','')
                value = file.readline().replace('\n','')
                self.headers[key]=value
            except:
                logging.error("missing lines in the configuration file '" + \
                              filename + "'")
                return False
            
        return True


    def Compare(self, other):
        if self.platform != other.platform:
            return False
        if self.ma5_version != other.ma5_version:
            return False
        if self.ma5_date != other.ma5_date:
            return False
        if self.python_version != other.python_version:
            return False
        if self.gcc_version != other.gcc_version:
            return False
        if self.gfortran_version != other.gfortran_version:
            return False
        if self.root_version != other.root_version:
            return False
        if self.fastjet_version != other.fastjet_version:
            return False
        if self.FAC != other.FAC:
            return False
        if self.libraries != other.libraries:
            return False
        if self.headers != other.headers:
            return False
        return True


    
