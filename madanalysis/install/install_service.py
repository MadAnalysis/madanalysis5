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
import os
import sys
import shutil

class InstallService():

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

        # Benj fix for small files
        if step ==0:
          step = 1

        if (numblocks+1)%step!=0:
            return
        try:
            percent = min(((numblocks+1)*blocksize*100)/filesize, 100)
        except:
            percent = 100
        theString="% 3.1f%%" % percent
        logging.info( "      "+theString + " of " + InstallService.convert_bytes(filesize) )

    @staticmethod
    def get_ncores(nmaxcores,forced):
        logging.info("   How many cores would you like " +\
                     "to use for the compilation ? default = max = " +\
                     str(nmaxcores)+"")
        
        if not forced:
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


    @staticmethod
    def untar(logname,installdir,tarball):
        # Unpacking the folder
        # Modified by Benj
#        downloaddir = os.path.join(os.path.join('/tmp', os.environ['USER']), 'MA5_downloads')
        downloaddir = os.path.join(installdir, '../MA5_downloads')
        # end of Benj fix
        theCommands=['tar','xzf',tarball, '-C', installdir]
        logging.debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             downloaddir,\
                                             silent=False)
        if not ok:
            return False, ''

#        # Removing the tarball
#        toRemove=installdir+'/'+tarball
#        logging.debug('removing the file: '+toRemove)
#        try:
#            os.remove(toRemove)
#        except:
#            logging.debug('impossible to remove the tarball: '+tarball)

        # Getting the good folder
        import glob
        folder_content = glob.glob(installdir+'/*')
        logging.debug('content of '+installdir+': '+str(folder_content))
        if len(folder_content)==0:
            logging.error('The content of the tarball is empty')
            return False, ''
        elif len(folder_content)==1:
            return True, folder_content[0]
        else:
            return True, installdir


    @staticmethod
    def prepare_tmp(untardir, downloaddir):
        # Removing previous temporary folder path
        if os.path.isdir(untardir):
            logging.debug("This temporary folder '"+untardir+"' is found. Try to remove it ...")
            try:
                shutil.rmtree(untardir)
            except:
                logging.error("impossible to remove the folder '"+untardir+"'")
                return False

        # Creating the temporary folder
        logging.debug("Creating a temporary folder '"+untardir+"' ...")
        try:
            os.mkdir(untardir)
        except:
            logging.error("impossible to create the folder '"+untardir+"'")
            return False


        # Creating the downloaddir folder
        logging.debug("Creating a temporary download folder '"+downloaddir+"' ...")
        if not os.path.isdir(downloaddir) :
            try:
                os.mkdir(downloaddir)
            except:
                logging.error("impossible to create the folder '"+downloaddir+"'")
                return False
        else:
            logging.debug("folder '"+downloaddir+"'" + " exists.")
        # Ok
        logging.debug('Name of the temporary untar    folder: '+untardir)
        logging.debug('Name of the temporary download folder: '+downloaddir)
        return True


    @staticmethod        
    def wget(filesToDownload,logFileName,installdir):
        
        import urllib
        ind=0
        error=False

        # Opening log file
        try:
            log=open(logFileName,'w')
        except:
            logging.error('impossible to create the file '+logFileName)
            return False

        # Loop over files to download
        for file,url in filesToDownload.items():
            ind+=1
            result="OK"
            logging.info('    - ' + str(ind)+"/"+str(len(filesToDownload.keys()))+" "+url+" ...")
            output = installdir+'/'+file
            if os.path.isfile(output) is True:
                logging.debug(output + " has been found.")
                try:
                    info = urllib.urlopen(url)
                    sizeURLFile = int(info.info().getheaders("Content-Length")[0])
                    sizeSYSFile = os.path.getsize(output)
                    if sizeURLFile != sizeSYSFile :
                        logging.info("   '" + file + "' is corrupted or is an old version." + os.linesep +\
                            "   Downloading a new package ...")
                        raise Exception(file + " is corrupted or is an old version.")
                    else:
                        logging.info("   '" + file + "' already exists. Package not downloaded.")
                    info.close()
                except:
                    try:
                        urllib.urlretrieve(url,output,InstallService.reporthook)
                    except:
                        logging.warning("Impossible to download the package from "+\
                                        url + " to "+output)
                        result="ERROR"
                        error=True
            else:
                try:
                    urllib.urlretrieve(url,output,InstallService.reporthook)
                except:
                    logging.warning("Impossible to download the package from "+\
                                    url + " to "+output)
                    result="ERROR"
                    error=True
            log.write(url+' : '+result+'\n')

        # Close the file
        try:
            log.close()
        except:
            logging.error('impossible to close the file '+logFileName)

        # Result
        if error:
            logging.warning("Error(s) occured during the installation.")
            return False
        else:
            return True


    @staticmethod
    def check_ma5site():
        logging.debug("Testing the access to MadAnalysis 5 website ...")
        import urllib
        try:
            urllib.urlopen('http://madanalysis.irmp.ucl.ac.be')
        except:
            logging.error("impossible to access MadAnalysis 5 website.")
            return False
        return True        


    @staticmethod
    def create_tools_folder(path):
        if os.path.isdir(path):
            logging.debug("   The installation folder 'tools' is already created.")
        else:
            logging.debug("   Creating the 'tools' folder ...")
            try:
                os.mkdir(path)
            except:
                logging.error("impossible to create the folder 'tools'.")
                return False
        return True    

        
    @staticmethod
    def create_package_folder(toolsdir,package):
        
        # Removing the folder package
        if os.path.isdir(os.path.join(toolsdir, package)):
            logging.error("impossible to remove the folder 'tools/"+package+"'")
            return False

        # Creating the folder package
        try:
            os.mkdir(os.path.join(toolsdir, package))
        except:
            logging.error("impossible to create the folder 'tools/" +\
                          package+"'")
            return False
        logging.debug("   Creation of the directory 'tools/" + package + "'") 
        return True
