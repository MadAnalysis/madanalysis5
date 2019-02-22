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
    def reporthook2(bytes_so_far, chunk_size, total_size):
        percent = float(bytes_so_far) / total_size
        percent = round(percent*100, 2)
        sys.stdout.write("             --> Download "+\
                         InstallService.convert_bytes(bytes_so_far)+\
                         " of "+\
                         InstallService.convert_bytes(total_size)+\
                         " (%0.1f%%)      \r"%(percent))

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
        logging.getLogger('MA5').info( "      "+theString + " of " + InstallService.convert_bytes(filesize) )

    @staticmethod
    def get_ncores(nmaxcores,forced):
        logging.getLogger('MA5').info("   How many cores would you like " +\
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
        logging.getLogger('MA5').info("   => Number of cores used for the compilation = "+str(ncores))
        return ncores


    @staticmethod
    def untar(logname,downloaddir,installdir,tarball):
        # Unpacking the folder
        theCommands=['tar','xzf',tarball, '-C', installdir]
        logging.getLogger('MA5').debug('shell command: '+' '.join(theCommands))
        logging.getLogger('MA5').debug('exected dir: '+downloaddir)
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             downloaddir,\
                                             silent=False)
        if not ok:
            return False, ''

#        # Removing the tarball
#        toRemove=installdir+'/'+tarball
#        logging.getLogger('MA5').debug('removing the file: '+toRemove)
#        try:
#            os.remove(toRemove)
#        except:
#            logging.getLogger('MA5').debug('impossible to remove the tarball: '+tarball)

        # Getting the good folder
        import glob
        folder_content = glob.glob(installdir+'/*')
        logging.getLogger('MA5').debug('content of '+installdir+': '+str(folder_content))
        if len(folder_content)==0:
            logging.getLogger('MA5').error('The content of the tarball is empty')
            return False, ''
        elif len(folder_content)==1:
            return True, folder_content[0]
        else:
            return True, installdir


    @staticmethod
    def prepare_tmp(untardir, downloaddir):
        # Removing previous temporary folder path
        if os.path.isdir(untardir):
            logging.getLogger('MA5').debug("This temporary folder '"+untardir+"' is found. Try to remove it ...")
            try:
                shutil.rmtree(untardir)
            except:
                logging.getLogger('MA5').error("impossible to remove the folder '"+untardir+"'")
                return False

        # Creating the temporary folder
        logging.getLogger('MA5').debug("Creating a temporary folder '"+untardir+"' ...")
        try:
            os.mkdir(untardir)
        except:
            logging.getLogger('MA5').error("impossible to create the folder '"+untardir+"'")
            return False


        # Creating the downloaddir folder
        logging.getLogger('MA5').debug("Creating a temporary download folder '"+downloaddir+"' ...")
        if not os.path.isdir(downloaddir) :
            try:
                os.mkdir(downloaddir)
            except:
                logging.getLogger('MA5').error("impossible to create the folder '"+downloaddir+"'")
                return False
        else:
            logging.getLogger('MA5').debug("folder '"+downloaddir+"'" + " exists.")
        # Ok
        logging.getLogger('MA5').debug('Name of the temporary untar    folder: '+untardir)
        logging.getLogger('MA5').debug('Name of the temporary download folder: '+downloaddir)
        return True


    @staticmethod        
    def wget(filesToDownload,logFileName,installdir):

        # Opening log file
        try:
            log=open(logFileName,'w')
        except:
            logging.getLogger('MA5').error('impossible to create the file '+logFileName)
            return False

        # Parameters
        ind          = 0     # interator on files to download
        error        = False # error flag ; True = there is at least one error
        
        # Loop over the files to download
        for file,url in filesToDownload.items():
            ind+=1
            result="OK"
            logging.getLogger('MA5').info('    - ' + str(ind)+"/"+str(len(filesToDownload.keys()))+" "+url+" ...")
            output = installdir+'/'+file

            # Try to connect the file
            info = InstallService.UrlAccess(url)
            ok=(info!=None)

            # Check if the connection is OK
            if not ok:
                logging.getLogger('MA5').warning("Impossible to download the package from "+\
                                url + " to "+output)
                result="ERROR"
                error=True

                # Write download status in the log file
                log.write(url+' : '+result+'\n')

                # skip the file
                continue

            
            # Decoding the size of the remote file
            logging.getLogger('MA5').debug('Decoding the size of the remote file...')
            sizeURLFile = 0
            try:
                sizeURLFile = int(info.info().getheaders("Content-Length")[0])
            except:
                logging.getLogger('MA5').debug('-> Problem to decode it')
                logging.getLogger('MA5').warning("Bad description for "+url)
                result="ERROR"
                error=True

                # Write download status in the log file
                log.write(url+' : '+result+'\n')

                # skip the file
                continue
            logging.getLogger('MA5').debug('-> size='+str(sizeURLFile))

            # Does the file exist locally?
            ok=False
            if not os.path.isfile(output):
                logging.getLogger('MA5').debug("No file with the name '"+output+"' exists locally.")
            else:
                logging.getLogger('MA5').debug("A file with the same name '"+output+"' has been found on the machine.")

                ok=True
                        
                # Decoding the size of the local file
                if ok:
                    logging.getLogger('MA5').debug('Decoding the size of the local file...')
                    sizeSYSFile = 0
                    try:
                        sizeSYSFile = os.path.getsize(output)
                    except:
                        logging.getLogger('MA5').debug('-> Problem to decode it')
                        ok=False

                # Comparing the sizes of two files
                if ok:
                    logging.getLogger('MA5').debug('-> size='+str(sizeSYSFile))
                    logging.getLogger('MA5').debug('Comparing the sizes of two files...')
                    if sizeURLFile != sizeSYSFile :
                        logging.getLogger('MA5').debug('-> Difference detected!')
                        logging.getLogger('MA5').info("   '" + file + "' is corrupted or is an old version." + os.linesep +\
                                     "         --> Downloading a new package ...")
                        ok=False

                # Case where the two files are identifical -> do nothing
                if ok:
                    logging.getLogger('MA5').debug('-> NO difference detected!')
                    logging.getLogger('MA5').info("        --> '" + file + "' already exists. Package not downloaded.")

                # Other cases: download is necessary
                if not ok:
                    logging.getLogger('MA5').debug('Fail to get info about the local file. It will be overwritten.')

            # Download of the package
            if not ok:
                logging.getLogger('MA5').debug('Downloading the file ...')

                # Open the output file [write mode]
                try:
                    outfile = open(output, 'wb')
                except:
                    info.close()
                    logging.getLogger('MA5').warning("Impossible to write the file "+output)
                    result="ERROR"
                    error=True

                # Copy the file
                if not error:
                    chunk_size   = 8192
                    bytes_so_far = 0
                    while 1:
                        chunk = info.read(chunk_size)
                        bytes_so_far += len(chunk)
                        if not chunk:
                            break
                        outfile.write(chunk)
                        InstallService.reporthook2(bytes_so_far, chunk_size, sizeURLFile)

                    InstallService.reporthook2(sizeURLFile, chunk_size, sizeURLFile)
                    sys.stdout.write('\n')

                #Closing file
                if not error:
                    try:
                        outfile.close()
                        info.close()
                    except:
                        logging.getLogger('MA5').warning("Impossible to close the file "+output)
                        result="ERROR"
                        error=True

            # Write download status in the log file
            log.write(url+' : '+result+'\n')

        # Close the log file
        try:
            log.close()
        except:
            logging.getLogger('MA5').error('impossible to close the file '+logFileName)

        # Result
        if error:
            logging.getLogger('MA5').warning("Error(s) occured during the installation.")
            return False
        else:
            return True


    @staticmethod
    def UrlAccess(url):

        import urllib2
        import ssl
        import time

        # max of attempts when impossible to access a file
        nMaxAttempts = 3

        # nb of seconds to wait between each attempt
        nSeconds     = 3
        
        # ssl method for python v>2.7.9
        try:
            modeSSL = (sys.version_info[0]>=2 and \
                       sys.version_info[1]>=7 and \
                       sys.version_info[2]>=9 )
        except:
            logging.getLogger('MA5').warning("Problem with Python version decoding!")
            modeSSL = False

        # Try to access
        ok=True
        for nAttempt in range(0,nMaxAttempts):
            if nAttempt>0:
                logging.getLogger('MA5').warning("New attempt to access the url: "+url)
                logging.getLogger('MA5').debug("Waiting "+str(nSeconds)+" seconds ...")
                time.sleep(nSeconds)
            logging.getLogger('MA5').debug("Attempt "+str(nAttempt+1)+"/"+str(nMaxAttempts)+" to access the url")
            try:
                if modeSSL:
                    info = urllib2.urlopen(url, context=ssl._create_unverified_context())
                else:
                    info = urllib2.urlopen(url)
            except:
                logging.getLogger('MA5').warning("Impossible to access the url: "+url)
                ok=False
            if ok:
                break

        if not ok:
            return None
        
        # Display
        logging.getLogger('MA5').debug('Info about the url: --------------------------------------------------------')
        words = str(info.info()).split('\n')
        for word in words:
            word=word.lstrip()
            word=word.rstrip()
            if word!='':
                logging.getLogger('MA5').debug('Info about the url: '+word)
        logging.getLogger('MA5').debug('Info about the url: --------------------------------------------------------')

        return info


    @staticmethod
    def check_ma5site():
        url='http://madanalysis.irmp.ucl.ac.be'
        logging.getLogger('MA5').debug("Testing the access to MadAnalysis 5 website: "+url+" ...")

        info = InstallService.UrlAccess(url)

        # Close the access
        if info!=None:
            info.close()
        return True        

        
    @staticmethod
    def check_inspire():
        url='http://inspirehep.net/'
        logging.getLogger('MA5').debug("Testing the access to InSpire: "+url+" ...")

        info = InstallService.UrlAccess(url)

        # Close the access
        if info!=None:
            info.close()
        return True


    @staticmethod
    def create_tools_folder(path):
        if os.path.isdir(path):
            logging.getLogger('MA5').debug("   The installation folder 'tools' is already created.")
        else:
            logging.getLogger('MA5').debug("   Creating the 'tools' folder ...")
            try:
                os.mkdir(path)
            except:
                logging.getLogger('MA5').error("impossible to create the folder 'tools'.")
                return False
        return True    

        
    @staticmethod
    def create_package_folder(toolsdir,package):
        
        # Removing the folder package
        if os.path.isdir(os.path.join(toolsdir, package)):
            logging.getLogger('MA5').error("impossible to remove the folder 'tools/"+package+"'")
            return False

        # Creating the folder package
        try:
            os.mkdir(os.path.join(toolsdir, package))
        except:
            logging.getLogger('MA5').error("impossible to create the folder 'tools/" +\
                          package+"'")
            return False
        logging.getLogger('MA5').debug("   Creation of the directory 'tools/" + package + "'") 
        return True
