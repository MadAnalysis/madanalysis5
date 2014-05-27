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


from shell_command import ShellCommand
import logging

class InstallBase():

    def __init__(self,main):
        self.main = main


    @staticmethod
    def convert_bytes(bytes):
        bytes = float(bytes)
        if bytes >= 1099511627776:
            terabytes = bytes / 1099511627776
            size = '%.2fT' % terabytes
        elif bytes >= 1073741824:
            gigabytes = bytes / 1073741824
            size = '%.2fG' % gigabytes
        elif bytes >= 1048576:
            megabytes = bytes / 1048576
            size = '%.2fM' % megabytes
        elif bytes >= 1024:
            kilobytes = bytes / 1024
            size = '%.2fK' % kilobytes
        else:
            size = '%.2fb' % bytes
        return size


    @staticmethod
    def reporthook(numblocks,blocksize,filesize):
        try:
            step = int(filesize/(blocksize*10))
        except:
            step = 1

        if (numblocks+1)%step!=0:
            return
        try:
            percent = min(((numblocks+1)*blocksize*100)/filesize, 100)
        except:
            percent = 100
        theString="% 3.1f%%" % percent
        logging.info( theString + " of " + InstallBase.convert_bytes(filesize) )


    def get_ncores(self):
        # Getting number max of cores
        nmaxcores=self.main.archi_info.ncores
        logging.info(" * How many cores would you like " +\
                     "to use for the compilation ? default = max=" +\
                     str(nmaxcores)+"")
        
        if not self.main.forced:
            test=False
            while(not test):
                answer=raw_input("   => Answer: ")
                if answer=="":
                    test=True
                    ncores=nmaxcores
                    break
                try:
                    ncores=int(answer)
                except:    
                    test=False
                    continue
                if ncores<=nmaxcores and ncores>0:
                    test=True
                    
        else:
            ncores=nmaxcores
        logging.info("   => Number of cores used for the compilation = "+str(ncores))
        return ncores


    def untar(self,logdir,installdir,tarball,package):
        # Unpacking the folder
        theCommands=['tar','xzf',tarball]
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logdir+'/unpack.log',\
                                             installdir,\
                                             silent=False)
        if not ok:
            return False, ''

        # Removing the tarball
        try:
            os.remove(installdir+'/'+tarball)
        except:
            logging.debug('impossible to remove the tarball: '+tarball)

        # Getting the good folder
        import glob
        folder_content = glob.glob(installdir+'/*')
        if len(folder_content)==0:
            logging.error('The content of the tarball is empty')
            return False, ''
        elif len(folder_content)==1:
            return True, folder_content[0]
        else:
            return True, installdir


    def get_tmp(self):
        # Getting temporary folder path
        installdir=self.main.archi_info.tmpdir+'/ma5install'
        logging.info(" * Creating a temporary folder '"+installdir+"' ...")

        # Removing previous temporary folder path
        if os.path.isdir(installdir):
            try:
                shutil.rmtree(installdir)
            except:
                logging.error("impossible to remove the folder '"+installdir+"'")
                return False, ""

        # Creating the temporary folder
        try:
            os.mkdir(installdir)
        except:
            logging.error("impossible to create the folder '"+installdir+"'")
            return False, ""

        # Ok
        logging.debug('Name of the temporary folder: '+installdir)
        return True, installdir

        
    def wget(self,files,package,installdir):
        
        import urllib
        ind=0
        error=False
        log=open(self.main.archi_info.ma5dir+'/tools/'+package+'/url.log','w')
        for file,url in files.items():
            ind+=1
            result="OK"
            logging.info(' * ' + str(ind)+"/"+str(len(files.keys()))+" Downloading the file "+url+" ...")

            try:
                urllib.urlretrieve(url,installdir+'/'+file,InstallBase.reporthook)
            except:
                logging.warning("Impossible to download.")
                result="ERROR"
                error=True
            log.write(url+' : '+result+'\n')
        log.close()    

        # Result
        if error:
            logging.warning("Error(s) occured during the installation.")
            return False
        else:
            return True

    def display_log(self,self.installdir):
        logging.error("More details can be found into the log files:")
        logging.error(" - "+installdir+"/url.log")
        logging.error(" - "+installdir+"/unpack.log")
        logging.error(" - "+installdir+"/configuration.log")
        logging.error(" - "+installdir+"/compilation.log")
        logging.error(" - "+installdir+"/installation.log")


    def check_ma5site(self):
        logging.info(" * Testing the access to MadAnalysis 5 website ...")
        import urllib
        try:
            urllib.urlopen('http://madanalysis.irmp.ucl.ac.be')
        except:
            logging.error("impossible to access MadAnalysis 5 website.")
            return False
        return True        


    def create_tools_folder(self):
        if os.path.isdir(self.main.archi_info.ma5dir + '/tools'):
            logging.info(" * The installation folder 'tools' is already created")
        else:
            logging.info(" * Creating the 'tools' folder ...")
            try:
                os.mkdir(self.main.archi_info.ma5dir + '/tools')
            except:
                logging.error("impossible to create the folder 'tools'")
                return False
        return True    

        
    def create_package_folder(self,package):
        
        # Removing the folder package
        if os.path.isdir(self.main.archi_info.ma5dir + '/tools/'+package):
            logging.info(" * Removing the current folder 'tools/"+package+"' ...")
            try:
                shutil.rmtree(self.main.archi_info.ma5dir + '/tools/'+package)
            except:
                logging.error("impossible to remove the folder 'tools/"+package+"'")
                return False

        # Creating the folder package
        try:
            os.mkdir(self.main.archi_info.ma5dir + "/tools/" + package)
        except:
            logging.error("impossible to create the folder 'tools/" +\
                          package+"'")
            return False
        logging.info(" * Creation of the directory 'tools/" + package + "'") 
        return True
