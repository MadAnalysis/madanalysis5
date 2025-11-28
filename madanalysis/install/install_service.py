################################################################################
#
#  Copyright (C) 2012-2025 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#
#  This file is part of MadAnalysis 5.
#  Official website: <https://github.com/MadAnalysis/madanalysis5>
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


from __future__ import absolute_import

import glob
import logging
import os
import shutil
import sys

import six
from shell_command import ShellCommand
from six.moves import input, range

log = logging.getLogger("MA5")


class InstallService:
    @staticmethod
    def convert_bytes(bytes):
        bytes = float(bytes)
        if bytes >= 1099511627776:
            terabytes = bytes / 1099511627776
            size = "%.2fT" % terabytes
        elif bytes >= 1073741824:
            gigabytes = bytes / 1073741824
            size = "%.2fG" % gigabytes
        elif bytes >= 1048576:
            megabytes = bytes / 1048576
            size = "%.2fM" % megabytes
        elif bytes >= 1024:
            kilobytes = bytes / 1024
            size = "%.2fK" % kilobytes
        else:
            size = "%.2fb" % bytes
        return size

    @staticmethod
    def reporthook2(bytes_so_far, chunk_size, total_size):
        percent = float(bytes_so_far) / total_size
        percent = round(percent * 100, 2)
        sys.stdout.write(
            "             --> Download "
            + InstallService.convert_bytes(bytes_so_far)
            + " of "
            + InstallService.convert_bytes(total_size)
            + " (%0.1f%%)      \r" % (percent)
        )

    @staticmethod
    def reporthook(numblocks, blocksize, filesize):
        try:
            step = int(filesize / (blocksize * 10))
        except:
            step = 1

        # Benj fix for small files
        if step == 0:
            step = 1

        if (numblocks + 1) % step != 0:
            return
        try:
            percent = min(((numblocks + 1) * blocksize * 100) / filesize, 100)
        except Exception:
            percent = 100
        theString = "% 3.1f%%" % percent
        log.info("      " + theString + " of " + InstallService.convert_bytes(filesize))

    @staticmethod
    def get_ncores(nmaxcores, forced):
        log.info(
            "   How many cores would you like to use for the compilation ? default = max = %s",
            nmaxcores,
        )

        if not forced:
            test = False
            while not test:
                answer = input("   => Answer: ")
                if answer == "":
                    test = True
                    ncores = nmaxcores
                    break
                try:
                    ncores = int(answer)
                except Exception:
                    test = False
                    continue
                if ncores <= nmaxcores and ncores > 0:
                    test = True

        else:
            ncores = nmaxcores
        log.info("   => Number of cores used for the compilation = %s", str(ncores))
        return ncores

    @staticmethod
    def untar(logname, downloaddir, installdir, tarball):
        # Unpacking the folder
        theCommands = ["tar", "xzf", tarball, "-C", installdir]
        log.debug("shell command: " + " ".join(theCommands))
        log.debug("exected dir: %s", downloaddir)
        ok, out = ShellCommand.ExecuteWithLog(
            theCommands, logname, downloaddir, silent=False
        )
        if not ok:
            return False, ""

        folder_content = glob.glob(installdir + "/*")
        log.debug("content of " + installdir + ": " + str(folder_content))
        if len(folder_content) == 0:
            log.error("The content of the tarball is empty")
            return False, ""
        elif len(folder_content) == 1:
            return True, folder_content[0]
        else:
            return True, installdir

    @staticmethod
    def prepare_tmp(untardir, downloaddir):
        # Removing previous temporary folder path
        if os.path.isdir(untardir):
            log.debug(
                "This temporary folder '%s' is found. Try to remove it ...", untardir
            )
            try:
                shutil.rmtree(untardir)
            except Exception:
                log.error("impossible to remove the folder '%s'", untardir)
                return False

        # Creating the temporary folder
        log.debug("Creating a temporary folder '%s' ...", untardir)
        try:
            os.mkdir(untardir)
        except Exception:
            log.error("impossible to create the folder '%s' ...", untardir)
            return False

        # Creating the downloaddir folder
        log.debug("Creating a temporary download folder '%s' ...", downloaddir)
        if not os.path.isdir(downloaddir):
            try:
                os.mkdir(downloaddir)
            except Exception:
                log.error("impossible to create the folder '%s' ...", downloaddir)
                return False
        else:
            log.debug("folder '%s' exists.", downloaddir)
        # Ok
        log.debug("Name of the temporary untar    folder: %s", untardir)
        log.debug("Name of the temporary download folder: %s", downloaddir)
        return True

    @staticmethod
    def wget(filesToDownload, logFileName, installdir, **kwargs):

        # Opening log file
        try:
            logfile = open(logFileName, "w")
        except:
            log.error("impossible to create the file " + logFileName)
            return False

        # Parameters
        ind = 0  # interator on files to download
        error = False  # error flag ; True = there is at least one error

        # Loop over the files to download
        for file, url in filesToDownload.items():
            ind += 1
            result = "OK"
            log.info("    - %s/%s %s ...", ind, len(list(filesToDownload.keys())), url)
            output = installdir + "/" + file

            # Try to connect the file
            info = InstallService.UrlAccess(url, headers=kwargs.get("headers", None))
            ok = info != None

            # Check if the connection is OK
            if not ok:
                log.warning(
                    "Impossible to download the package from " + url + " to " + output
                )
                result = "ERROR"
                error = True

                # Write download status in the log file
                logfile.write(url + " : " + result + "\n")

                # skip the file
                continue

            # Decoding the size of the remote file
            log.debug("Decoding the size of the remote file...")
            sizeURLFile = -1
            try:
                if six.PY2:
                    sizeURLFile = int(info.info().getheaders("Content-Length")[0])
                else:
                    sizeURLFile = int(info.info().get("Content-Length"))
            except Exception as err:
                log.debug(err)
                log.debug("-> Problem to decode it")
                log.warning(
                    "Bad description for %s, can not read the size of the file.", url
                )
                # result = "ERROR"
                # error = True

                # Write download status in the log file
                logfile.write(url + " : " + result + "\n")

                # skip the file
                # pass
            log.debug("-> size=%s", str(sizeURLFile))

            # Does the file exist locally?
            ok = False
            if not os.path.isfile(output):
                log.debug("No file with the name '" + output + "' exists locally.")
            else:
                log.debug(
                    "A file with the same name '"
                    + output
                    + "' has been found on the machine."
                )

                ok = True

                # Decoding the size of the local file
                if ok:
                    log.debug("Decoding the size of the local file...")
                    sizeSYSFile = 0
                    try:
                        sizeSYSFile = os.path.getsize(output)
                    except:
                        log.debug("-> Problem to decode it")
                        ok = False

                # Comparing the sizes of two files
                if ok:
                    log.debug("-> size=" + str(sizeSYSFile))
                    log.debug("Comparing the sizes of two files...")
                    if sizeURLFile != sizeSYSFile:
                        log.debug("-> Difference detected!")
                        log.info(
                            "   '"
                            + file
                            + "' is corrupted or is an old version."
                            + os.linesep
                            + "         --> Downloading a new package ..."
                        )
                        ok = False

                # Case where the two files are identifical -> do nothing
                if ok:
                    log.debug("-> NO difference detected!")
                    log.info(
                        "        --> '"
                        + file
                        + "' already exists. Package not downloaded."
                    )

                # Other cases: download is necessary
                if not ok:
                    log.debug(
                        "Fail to get info about the local file. It will be overwritten."
                    )

            # Download of the package
            if not ok:
                log.debug("Downloading the file ...")

                # Open the output file [write mode]
                try:
                    outfile = open(output, "wb")
                except:
                    info.close()
                    log.warning("Impossible to write the file " + output)
                    result = "ERROR"
                    error = True

                # Copy the file
                if not error:
                    chunk_size = 8192
                    bytes_so_far = 0
                    while 1:
                        chunk = info.read(chunk_size)
                        bytes_so_far += len(chunk)
                        if not chunk:
                            break
                        outfile.write(chunk)
                        InstallService.reporthook2(bytes_so_far, chunk_size, sizeURLFile)

                    InstallService.reporthook2(sizeURLFile, chunk_size, sizeURLFile)
                    sys.stdout.write("\n")

                # Closing file
                if not error:
                    try:
                        outfile.close()
                        info.close()
                    except:
                        log.warning("Impossible to close the file " + output)
                        result = "ERROR"
                        error = True

            # Write download status in the log file
            logfile.write(url + " : " + result + "\n")

        # Close the log file
        try:
            logfile.close()
        except:
            log.error("impossible to close the file " + logFileName)

        # Result
        if error:
            log.warning("Error(s) occured during the installation.")
            return False
        else:
            return True

    @staticmethod
    def UrlAccess(url, headers: dict[str, str] = None):

        import ssl
        import time

        import six.moves.urllib.error
        import six.moves.urllib.parse
        import six.moves.urllib.request

        # max of attempts when impossible to access a file
        nMaxAttempts = 3

        # nb of seconds to wait between each attempt
        nSeconds = 3

        # ssl method for python v>2.7.9
        try:
            modeSSL = (
                sys.version_info[0] == 2
                and sys.version_info[1] >= 7
                and sys.version_info[2] >= 9
            ) or (sys.version_info[0] == 3)
        except:
            log.warning("Problem with Python version decoding!")
            modeSSL = False

        # Try to access
        ok = True
        for nAttempt in range(0, nMaxAttempts):
            if nAttempt > 0:
                log.warning("New attempt to access the url: " + url)
                log.debug("Waiting " + str(nSeconds) + " seconds ...")
                time.sleep(nSeconds)
            log.debug(
                "Attempt "
                + str(nAttempt + 1)
                + "/"
                + str(nMaxAttempts)
                + " to access the url"
            )
            try:
                if headers is not None:
                    url = six.moves.urllib.request.Request(url, headers=headers)
                if modeSSL:
                    info = six.moves.urllib.request.urlopen(
                        url, context=ssl._create_unverified_context()
                    )
                else:
                    info = six.moves.urllib.request.urlopen(url)
            except Exception as err:
                log.debug(err)
                log.warning("Impossible to access the url: " + url)
                ok = False
            if ok:
                break

        if not ok:
            return None

        # Display
        log.debug(
            "Info about the url: --------------------------------------------------------"
        )
        words = str(info.info()).split("\n")
        for word in words:
            word = word.lstrip()
            word = word.rstrip()
            if word != "":
                log.debug("Info about the url: " + word)
        log.debug(
            "Info about the url: --------------------------------------------------------"
        )

        return info

    @staticmethod
    def check_ma5site():
        url = "http://madanalysis.irmp.ucl.ac.be"
        log.debug("Testing the access to MadAnalysis 5 website: " + url + " ...")
        info = InstallService.UrlAccess(url)
        # Close the access
        if info != None:
            info.close()
        return True

    @staticmethod
    def check_dataverse():
        url = "http://dataverse.uclouvain.be"
        log.debug("Testing access to the MadAnalysis5 dataverse: " + url + " ...")
        info = InstallService.UrlAccess(url)
        # Close the access
        if info != None:
            info.close()
        return True

    @staticmethod
    def create_tools_folder(path):
        if os.path.isdir(path):
            log.debug("   The installation folder 'tools' is already created.")
        else:
            log.debug("   Creating the 'tools' folder ...")
            try:
                os.mkdir(path)
            except:
                log.error("impossible to create the folder 'tools'.")
                return False
        return True

    @staticmethod
    def create_package_folder(toolsdir, package):

        # Removing the folder package
        if os.path.isdir(os.path.join(toolsdir, package)):
            log.error("impossible to remove the folder 'tools/" + package + "'")
            return False

        # Creating the folder package
        try:
            os.mkdir(os.path.join(toolsdir, package))
        except:
            log.error("impossible to create the folder 'tools/" + package + "'")
            return False
        log.debug("   Creation of the directory 'tools/" + package + "'")
        return True
