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

class UserOptions:

    def __init__(self):
        self.root_includes    = '0'
        self.root_libs        = '0'
        self.delphes_veto     = '0'
        self.delphes_includes = '0'
        self.delphes_libs     = '0'
        self.delfes_veto      = '0'
        self.delfes_includes  = '0'
        self.delfes_libs      = '0'
        self.zlib_veto        = '0'
        self.zlib_includes    = '0'
        self.zlib_libs        = '0'
        self.fastjet_veto     = '0'
        self.fastjet_bin_path = '0'
        self.pdflatex_veto    = '0'
        self.latex_veto       = '0'
        self.dvipdf_veto      = '0'
        
    def Dump(self):
        logging.info(" ROOT header path     = " + str(self.root_includes))
        logging.info(" ROOT library path    = " + str(self.root_libs))
        logging.info(" DELPHES veto         = " + str(self.delphes_veto))
        logging.info(" DELPHES header path  = " + str(self.delphes_includes))
        logging.info(" DELPHES library path = " + str(self.delphes_libs))
        logging.info(" DELFES veto          = " + str(self.delfes_veto))
        logging.info(" DELFES header path   = " + str(self.delfes_includes))
        logging.info(" DELFES library path  = " + str(self.delfes_libs))
        logging.info(" ZLIB veto            = " + str(self.zlib_veto))
        logging.info(" ZLIB header path     = " + str(self.zlib_includes))
        logging.info(" ZLIB library path    = " + str(self.zlib_libs))
        logging.info(" FASTJET veto         = " + str(self.fastjet_veto))
        logging.info(" FASTJET bin path     = " + str(self.fastjet_bin_path))
        logging.info(" PDFLATEX veto        = " + str(self.pdflatex_veto))
        logging.info(" LATEX veto           = " + str(self.latex_veto))
        logging.info(" DVIPDF veto          = " + str(self.dvipdf_veto))

    def Export(self,file):
        file.write(self.root_includes+"\n")
        file.write(self.root_libs+"\n")
        file.write(self.delphes_veto+"\n")
        file.write(self.delphes_includes+"\n")
        file.write(self.delphes_libs+"\n")
        file.write(self.delfes_veto+"\n")
        file.write(self.delfes_includes+"\n")
        file.write(self.delfes_libs+"\n")
        file.write(self.zlib_veto+"\n")
        file.write(self.zlib_includes+"\n")
        file.write(self.zlib_libs+"\n")
        file.write(self.fastjet_veto+"\n")
        file.write(self.fastjet_bin_path+"\n")
        file.write(self.pdflatex_veto+"\n")
        file.write(self.latex_veto+"\n")
        file.write(self.dvipdf_veto+"\n")

    def Import(self,file):
        self.root_includes    = file.readline().replace('\n','')
        self.root_libs        = file.readline().replace('\n','')
        self.delphes_veto     = file.readline().replace('\n','')
        self.delphes_includes = file.readline().replace('\n','')
        self.delphes_libs     = file.readline().replace('\n','')
        self.delfes_veto      = file.readline().replace('\n','')
        self.delfes_includes  = file.readline().replace('\n','')
        self.delfes_libs      = file.readline().replace('\n','')
        self.zlib_veto        = file.readline().replace('\n','')
        self.zlib_includes    = file.readline().replace('\n','')
        self.zlib_libs        = file.readline().replace('\n','')
        self.fastjet_veto     = file.readline().replace('\n','')
        self.fastjet_bin_path = file.readline().replace('\n','')
        self.pdflatex_veto    = file.readline().replace('\n','')
        self.latex_veto       = file.readline().replace('\n','')
        self.dvipdf_veto      = file.readline().replace('\n','')

    def Compare(self,other):
        if self.root_includes != other.root_includes:
            return False
        if self.root_libs != other.root_libs:
            return False
        if self.delphes_veto != other.delphes_veto:
            return False
        if self.delphes_includes != other.delphes_includes:
            return False
        if self.delphes_libs != other.delphes_libs:
            return False
        if self.delfes_veto != other.delfes_veto:
            return False
        if self.delfes_includes != other.delfes_includes:
            return False
        if self.delfes_libs != other.delfes_libs:
            return False
        if self.zlib_veto != other.zlib_veto:
            return False
        if self.zlib_includes != other.zlib_includes:
            return False
        if self.zlib_libs != other.zlib_libs:
            return False
        if self.fastjet_veto != other.fastjet_veto:
            return False
        if self.fastjet_bin_path != other.fastjet_bin_path:
            return False
        return True

class LinuxArchitecture:

    def __init__(self):
        self.useroptions      = UserOptions()
        self.platform         = ""
        self.release          = ""
        self.ma5_version      = ""
        self.ma5_date         = ""
        self.python_version   = ""
        self.gcc_version      = ""
        self.make_version     = ""
        self.gfortran_version = ""
        self.root_version     = ""
        self.fastjet_version  = ""
        self.libraries        = {}
        self.headers          = {}
        self.editor           = ""

        self.root_inc_path=""
        self.root_lib_path=""
        self.zlib_inc_path=""
        self.zlib_lib_path=""
        self.zlib_lib=""
        self.delphes_inc_paths=[]
        self.delphes_lib_paths=[]
        self.delphes_lib=""
        self.delfes_inc_paths=[]
        self.delfes_lib_paths=[]
        self.delfes_lib=""
        self.fastjet_bin_path=""
        self.fastjet_lib_paths=[]

    def Dump(self):
        logging.info(" User options")
        logging.info(" ------------")
        self.useroptions.Dump()
        logging.info("")

        logging.info(" Architecture")
        logging.info(" ------------")
        logging.info(" Platform         = " + self.platform)
        logging.info(" Release platform = " + self.release)
        logging.info(" MA5 version      = " + self.ma5_version)
        logging.info(" MA5 date         = " + self.ma5_date)
        logging.info(" Python version   = " + self.python_version)
        logging.info(" gcc version      = " + self.gcc_version)
        logging.info(" GNU Make version = " + self.make_version)
        logging.info(" gfortran version = " + self.gfortran_version)
        logging.info(" ROOT version     = " + self.root_version)
        logging.info(" FastJet version  = " + self.fastjet_version)
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

        self.useroptions.Export(file)
        file.write(self.platform+"\n")
        file.write(self.release+"\n")
        file.write(self.ma5_version+"\n")
        file.write(self.ma5_date+"\n")
        file.write(self.python_version+"\n")
        file.write(self.gcc_version+"\n")
        file.write(self.make_version+"\n")
        file.write(self.gfortran_version+"\n")
        file.write(self.root_version+"\n")
        file.write(self.fastjet_version+"\n")
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
            self.useroptions.Import(file)
            self.platform         = file.readline().replace('\n','')
            self.release          = file.readline().replace('\n','')
            self.ma5_version      = file.readline().replace('\n','')
            self.ma5_date         = file.readline().replace('\n','')
            self.python_version   = file.readline().replace('\n','')
            self.gcc_version      = file.readline().replace('\n','')
            self.make_version     = file.readline().replace('\n','')
            self.gfortran_version = file.readline().replace('\n','')
            self.root_version     = file.readline().replace('\n','')
            self.fastjet_version  = file.readline().replace('\n','')
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
        if not self.useroptions.Compare(other.useroptions):
            return False
        
        if self.platform != other.platform:
            return False
        if self.release != other.release:
            return False
        if self.ma5_version != other.ma5_version:
            return False
        if self.ma5_date != other.ma5_date:
            return False
        if self.python_version != other.python_version:
            return False
        if self.gcc_version != other.gcc_version:
            return False
        if self.make_version != other.make_version:
            return False
        if self.gfortran_version != other.gfortran_version:
            return False
        if self.root_version != other.root_version:
            return False
        if self.fastjet_version != other.fastjet_version:
            return False
        if self.libraries != other.libraries:
            return False
        if self.headers != other.headers:
            return False
        return True


    
