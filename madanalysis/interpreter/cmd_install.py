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


from madanalysis.interpreter.cmd_base import CmdBase
import logging
import os
import sys
import shutil
import urllib
import pwd

class CmdInstall(CmdBase):
    """Command INSTALL"""

    def __init__(self,main):
        CmdBase.__init__(self,main,"install")

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
        logging.info( theString + " of " + CmdInstall.convert_bytes(filesize) )
                            
    def do(self,args):

        # Checking argument number
        if len(args) != 1:
            logging.error("wrong number of arguments for the command 'install'.")
            self.help()
            return
        
        # Calling selection method
        if args[0]=='samples':
            return self.install_samples()
        elif args[0]=='zlib':
            return self.install_zlib()
        elif args[0]=='fastjet':
            return self.install_fastjet()
        elif args[0]=='MCatNLO-for-ma5':
            return self.install_mcatnlo()
        else:
            logging.error("the syntax is not correct.")
            self.help()
            return


    def help(self):
        logging.info("   Syntax: install <component>")
        logging.info("   Download and install a MadAnalysis component from the official site.")
        logging.info("   List of available components : samples zlib fastjet MCatNLO-for-ma5")


    def get_ncores(self):
        # Number of cores
        import multiprocessing
        nmaxcores=multiprocessing.cpu_count()
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
        logging.info("   => Number of cores used for the compilation = " +\
                     str(ncores))
        return ncores
        
    def untar(self,installdir,tarball,package):

        # Detarring package
        logging.info(" * Extracting the package ...")
        os.system("cd "+installdir+" ; tar xzf "+tarball+\
                  " > "+self.main.ma5dir+"/tools/"+package+"/untar.log 2>&1")

        # Getting the good folder
        packagedir=""
        dirlist = os.listdir(installdir)
        for thedir in dirlist:
            if os.path.isdir(installdir+'/'+thedir):
                packagedir=installdir+'/'+thedir
                break
        if packagedir=="":
            logging.error("The package content is incorrect.")

        return packagedir    


    def install_zlib(self):

        # Asking for number of cores
        ncores = self.get_ncores()
        
        # Checking connection with MA5 web site
        if not self.check_ma5site():
            return False
    
        # Creating tools folder
        if not self.create_tools_folder():
            return False

        # Creating package folder
        if not self.create_package_folder('zlib'):
            return False

        # Creating temporary folder
        installdir = self.get_tmp()
        if installdir == "":
            return False

        # List of files
        files = { "zlib.tar.gz" : "http://zlib.net/zlib-1.2.8.tar.gz" }
        # Launching wget
        if not self.wget(files,'zlib',installdir):
            return False

        # Detarring package
        packagedir = self.untar(installdir,'zlib.tar.gz','zlib')
        if packagedir == "":
            return False
       
        # Configuring
        logging.info("Configuring the package ...")
        os.system("cd "+packagedir+" ; ./configure --prefix="+self.main.ma5dir+"/tools/zlib > "+self.main.ma5dir+"/tools/zlib/"+"configuration.log 2>&1")
        
        # Compiling
        logging.info("Compiling the package ...")
        os.system("cd "+packagedir+" ; make -j"+str(ncores)+" > "+self.main.ma5dir+"/tools/zlib/"+"compilation.log 2>&1")

        # Copying headers & libraries
        logging.info("Copying headers and libraries into 'tools/zlib' ...")
        os.system("cd "+packagedir+" ; make install > "+self.main.ma5dir+"/tools/zlib/"+"installation.log 2>&1")

        # Final check
        logging.info("Checking installation ...")
        if (not os.path.isdir(self.main.ma5dir+"/tools/zlib/include")) or \
           (not os.path.isdir(self.main.ma5dir+"/tools/zlib/lib")):
            logging.error('package headers and/or libraries are missing.')
            self.display_log('zlib')
            return False

        if not os.path.isfile(self.main.ma5dir+'/tools/zlib/include/zlib.h'):
            logging.error("header labeled 'zlib.h' is missing.")
            self.display_log('zlib')
            return False

        if (not os.path.isfile(self.main.ma5dir+'/tools/zlib/lib/libz.so')) and \
           (not os.path.isfile(self.main.ma5dir+'/tools/zlib/lib/libz.a')):
            logging.error("library labeled 'libz.so' or 'libz.a' is missing.")
            self.display_log('zlib')
            return False
        
        # End
        logging.info("Installation complete.")


        return True

    def install_mcatnlo(self):

##        # Checking connection with MA5 web site
##        if not self.check_ma5site():
##            return False
        logging.warning("clear comments in install.py")

        # Creating tools folder
        if not self.create_tools_folder():
            return False

        # Creating package folder
        if not self.create_package_folder('MCatNLO-utilities'):
            return False

        # Launching wget
        name = 'mcatnlo-utilities.tar.gz' 
##         files = { name : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/utils/mcatnlo.tar.gz" }
##         if not self.wget(files,'MCatNLO-utilities', self.main.ma5dir + '/tools'):
##             return False
        shutil.copy(self.main.ma5dir+'/../mcatnlo.tar.gz', self.main.ma5dir + '/tools/mcatnlo-utilities.tar.gz')
        logging.warning("clear comments  + shutil copy in install.py")

        # Unpacking package
        packagedir = self.untar(self.main.ma5dir + '/tools/MCatNLO-utilities','../'+name, 'MCatNLO-utilities')
        if packagedir == "":
            return False
        os.system('rm -f ' + self.main.ma5dir + '/tools/' + name)

        # Configuring the Makefile
        logging.info(" * Configuring the package ...")
        path = self.main.ma5dir + '/tools/MCatNLO-utilities/StdHEP/src/make_opts'
        text = open(path).read()
        if sys.maxsize > 2**32:
            text = text.replace('MBITS=32','MBITS=64')
        text = text.replace('FC=g77','FC=gfortran')
        open(path, 'w').writelines(text)    

        # Compiling
        logging.info(" * Compiling the package ...")
        os.system('cd ' +self.main.ma5dir+'/tools/MCatNLO-utilities; make > '\
           + self.main.ma5dir + '/tools/MCatNLO-utilities/compilation.log 2>&1')

        self.main.mcatnloutils = True

        return True

 
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
        if os.path.isdir(self.main.ma5dir + '/tools'):
            logging.info(" * The installation folder 'tools' is already created")
        else:
            logging.info(" * Creating the 'tools' folder ...")
            try:
                os.mkdir(self.main.ma5dir + '/tools')
            except:
                logging.error("impossible to create the folder 'tools'")
                return False
        return True    
        
    def create_package_folder(self,package):
        
        # Removing the folder package
        if os.path.isdir(self.main.ma5dir + '/tools/'+package):
            logging.info(" * Removing the current folder 'tools/"+package+"' ...")
            try:
                shutil.rmtree(self.main.ma5dir + '/tools/'+package)
            except:
                logging.error("impossible to remove the folder 'tools/"+package+"'")
                return False

        # Creating the folder package
        try:
            os.mkdir(self.main.ma5dir + "/tools/" + package)
        except:
            logging.error("impossible to create the folder 'tools/" +\
                          package+"'")
            return False
        logging.info(" * Creation of the directory 'tools/" + package + "'") 
        return True
        
    def get_tmp(self):

        # Getting temporary folder path
        try:
            user = pwd.getpwuid(os.getuid())[0]
        except:
            user = 'unknown' 
        try:
            tmpdir = os.environ['TMPDIR']
        except:
            tmpdir = '/tmp'
        installdir = tmpdir+'/ma5install_'+user
        logging.info(" * Creating temporary folder '"+installdir+"' ...")

        # Removing previous temporary folder path
        if os.path.isdir(installdir):
            try:
                shutil.rmtree(installdir)
            except:
                logging.error("impossible to remove the folder '"+installdir+"'")
                return ""

        # Creating the temporary folder
        try:
            os.mkdir(installdir)
        except:
            logging.error("impossible to create the folder '"+installdir+"'")
            return ""

        return installdir

        
    def wget(self,files,package,installdir):
        
        import urllib
        ind=0
        error=False
        log=open(self.main.ma5dir+'/tools/'+package+'/url.log','w')
        for file,url in files.items():
            ind+=1
            result="OK"
            logging.info(' * ' + str(ind)+"/"+str(len(files.keys()))+" Downloading the file '"+file+"' ...")

            try:
                urllib.urlretrieve(url,installdir+'/'+file,CmdInstall.reporthook)
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

    def display_log(self,package):
        logging.error("More details can be found into the log files:")
        logdir=self.main.ma5dir+"/tools/"+package+"/"
        logging.error(" - "+logdir+"untar.log")
        logging.error(" - "+logdir+"configuration.log")
        logging.error(" - "+logdir+"compilation.log")
        logging.error(" - "+logdir+"installation.log")
        

    def install_fastjet(self):
        
        # Asking for number of cores
        ncores = self.get_ncores()

        # Checking connection with MA5 web site
        if not self.check_ma5site():
            return False
    
        # Creating tools folder
        if not self.create_tools_folder():
            return False

        # Creating package folder
        if not self.create_package_folder('fastjet'):
            return False
       
        # Creating temporary folder
        installdir = self.get_tmp()
        if installdir == "":
            return False

        # List of files
        files = { "fastjet.tar.gz" : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/WikiStart/fastjet-3.0.3.tar.gz" }

        # Launching wget
        if not self.wget(files,'fastjet',installdir):
            return False

        # Unpacking package
        packagedir = self.untar(installdir,'fastjet.tar.gz','fastjet')
        if packagedir == "":
            return False
        
        # Configuring
        logging.info("Configuring the package ...")
        os.system("cd "+packagedir+" ; ./configure --prefix="+self.main.ma5dir+"/tools/fastjet > "+self.main.ma5dir+"/tools/fastjet/"+"configuration.log 2>&1")
        
        # Compiling
        logging.info("Compiling the package ...")
        os.system("cd "+packagedir+" ; make -j"+str(ncores)+" > "+self.main.ma5dir+"/tools/fastjet/"+"compilation.log 2>&1")

        # Copying headers & libraries
        logging.info("Copying headers and libraries into 'tools/fastjet' ...")
        os.system("cd "+packagedir+" ; make install > "+self.main.ma5dir+"/tools/fastjet/"+"installation.log 2>&1")

        # Final check
        logging.info("Checking installation ...")
        if (not os.path.isdir(self.main.ma5dir+"/tools/fastjet/include")) or \
           (not os.path.isdir(self.main.ma5dir+"/tools/fastjet/lib")) or \
           (not os.path.isdir(self.main.ma5dir+"/tools/fastjet/bin")):
            logging.error('package headers and/or libraries are missing.')
            self.display_log('fastjet')
            return False

        if not os.path.isfile(self.main.ma5dir+'/tools/fastjet/bin/fastjet-config'):
            logging.error("binary labeled 'fastjet-config' is missing.")
            self.display_log('fastjet')
            return False

        if not os.path.isfile(self.main.ma5dir+'/tools/fastjet/include/fastjet/PseudoJet.hh'):
            logging.error("header labeled 'include/fastjet/PseudoJet.hh' is missing.")
            self.display_log('fastjet')
            return False

        if (not os.path.isfile(self.main.ma5dir+'/tools/fastjet/lib/libfastjet.so')) and \
           (not os.path.isfile(self.main.ma5dir+'/tools/fastjet/lib/libfastjet.a')):
            logging.error("library labeled 'libfastjet.so' or 'libfastjet.a' is missing.")
            self.display_log('fastjet')
            return False
        
        
        # End
        logging.info("Installation complete.")

        return True


    def install_samples(self):

        # List of files
        files = { "ttbar_fh.lhe.gz" :   "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/samples/ttbar_fh.lhe.gz",\
                  "ttbar_sl_1.lhe.gz" : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/samples/ttbar_sl_1.lhe.gz",\
                  "ttbar_sl_2.lhe.gz" : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/samples/ttbar_sl_2.lhe.gz",\
                  "zz.lhe.gz" :         "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/samples/zz.lhe.gz" }

        # Checking connection with MA5 web site
        if not self.check_ma5site():
            return False
    
        # Checking if the directory exits
        if os.path.isdir(self.main.ma5dir + '/samples'):
            logging.info("'samples' folder is already created")
        else:
            logging.info("Creating the 'samples' folder ...")
            try:
                os.mkdir(self.main.ma5dir + '/samples')
            except:
                logging.error("impossible to create the folder 'samples'")
                return False

        # Removing files
        for file in files.keys():

            if os.path.isfile(self.main.ma5dir + '/samples/' + file):
                logging.info("Removing file '" + file + "' ...")
                try:
                    os.remove(self.main.ma5dir + '/samples/' + file)
                except:
                    logging.error("impossible to remove the file 'samples/'"+file)
                    return False

        # Launching wget
        ind=0
        error=False
        
        for file,url in files.items():
            ind+=1
            logging.info(str(ind)+"/"+str(len(files.keys()))+" Downloading the file '"+file+"' ...")
            
#            try:
            urllib.urlretrieve(url,self.main.ma5dir+'/samples/'+file,CmdInstall.reporthook)
#            except:
#                logging.error("impossible to download '"+file+"'")
#                error=True

        # Result
        if error:
            logging.warning("Error(s) occured during the installation.")
        else:
            logging.info("Installation complete.")

        return True

    def complete(self,text,args,begidx,endidx):

        nargs = len(args)
        if not text:
            nargs +=1

        if nargs>2:
            return []
        else:
            output = ["samples","zlib","fastjet", "MCatNLO-for-ma5" ]
            return self.finalize_complete(text,output)
    


