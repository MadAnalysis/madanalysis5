################################################################################
#
#  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


"""Helpers shared by the installers: downloads, unpacking, folders and number of cores."""

from __future__ import absolute_import
from typing import Any

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
    """Static helpers used by the ``install`` command."""
    @staticmethod
    def convert_bytes(bytes: "float") -> "str":
        """Format a size in bytes with a unit suffix.

        Args:
            bytes (``float``): size in bytes.

        Returns:
            ``str``:
            E.g. ``"1.50M"``.
        """
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
    def reporthook2(bytes_so_far: "int", chunk_size: "int", total_size: "int") -> "None":
        """Print the download progress on the standard output (same line).

        Args:
            bytes_so_far (``int``): downloaded size.
            chunk_size (``int``): size of a chunk (unused).
            total_size (``int``): total size (``-1`` if unknown).
        """
        # FIXME: ZeroDivisionError if the size is 0; negative percentage if the size is unknown (-1).
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
    def reporthook(numblocks: "int", blocksize: "int", filesize: "int") -> "None":
        """Legacy ``urlretrieve`` progress hook logging the download progress every 10%.

        Args:
            numblocks (``int``): number of downloaded blocks.
            blocksize (``int``): size of a block.
            filesize (``int``): total size.
        """
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
    def get_ncores(nmaxcores: "int", forced: "bool") -> "int":
        """Ask the user for the number of cores used for a compilation.

        Args:
            nmaxcores (``int``): number of available cores (default answer).
            forced (``bool``): do not ask, use all cores.

        Returns:
            ``int``:
            Number of cores.
        """
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
    def untar(logname: "str", downloaddir: "str", installdir: "str", tarball: "str") -> "tuple[bool, str]":
        """Unpack a ``.tar.gz`` archive.

        Args:
            logname (``str``): log file.
            downloaddir (``str``): folder containing the archive.
            installdir (``str``): destination folder.
            tarball (``str``): name of the archive.

        Returns:
            ``tuple[bool, str]``:
            Success flag and the unpacked folder (the single sub-folder if the archive
            contains only one, ``installdir`` otherwise).
        """
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
    def prepare_tmp(untardir: "str", downloaddir: "str") -> "bool":
        """Create an empty temporary folder (removing a previous one) and the download folder.

        Args:
            untardir (``str``): temporary unpacking folder.
            downloaddir (``str``): download folder.

        Returns:
            ``bool``:
            ``True`` on success.
        """
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
    def wget(filesToDownload: "dict[str, str]", logFileName: "str", installdir: "str", **kwargs) -> "bool":
        """Download files (files with the expected size already present are not downloaded again).

        Args:
            filesToDownload (``dict[str, str]``): ``{local file name: URL}``.
            logFileName (``str``): log file (one line per URL with ``OK``/``ERROR``).
            installdir (``str``): destination folder.
            **kwargs: ``headers`` (``dict[str, str]``), extra HTTP headers.

        Returns:
            ``bool``:
            ``True`` if all files have been downloaded.
        """

        # Opening log file
        try:
            logfile = open(logFileName, "w")
        except Exception:
            log.error("impossible to create the file %s", logFileName)
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
            ok = info is not None

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
                log.debug(
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
                    # FIXME: 'error' is a global flag: after one failure, the next files are neither downloaded
                    # nor closed ('if not error' blocks below).
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
    # FIXME: 'dict[str, str]' is evaluated at definition time: the module cannot be imported
    # with Python 3.8 (TypeError), although Python >= 3.8 is supported.
    def UrlAccess(url, headers: dict[str, str] = None) -> "Any":
        """Open a URL (three attempts, 3 s apart).

        .. warning::
            The SSL certificates are not verified.

        Args:
            url (``str | urllib.request.Request``): URL or request.
            headers (``dict[str, str]``, default ``None``): extra HTTP headers.

        Returns:
            ``Any``:
            The response object, or ``None`` if the URL cannot be accessed.
        """

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



        # Keep the URL used in messages separate from the request object.
        if isinstance(url, six.moves.urllib.request.Request):
            display_url = url.get_full_url()
            request = url
            if headers is not None:
                for name, value in headers.items():
                    request.add_header(name, value)
        else:
            display_url = url
            request = (
                six.moves.urllib.request.Request(url, headers=headers)
                if headers is not None
                else url
            )

        # Try to access.
        ok = False
        for nAttempt in range(nMaxAttempts):
            if nAttempt > 0:
                log.warning("New attempt to access the url: %s", display_url)
                log.debug("Waiting %s seconds ...", nSeconds)
                time.sleep(nSeconds)
            log.debug("Attempt %s/%s to access the url", nAttempt + 1, nMaxAttempts)

            try:
                if modeSSL:
                    info = six.moves.urllib.request.urlopen(request, context=ssl._create_unverified_context())
                else:
                    info = six.moves.urllib.request.urlopen(request)
            except Exception as err:
                log.debug(err)
                log.warning("Impossible to access the url: %s", display_url)
                continue

            # A successful attempt ends the retry loop.
            ok = True
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
    def check_ma5site() -> "bool":
        """Try to access the MadAnalysis 5 website.

        Returns:
            ``bool``:
            Always ``True`` (the result of the access is ignored).
        """
        url = "http://madanalysis.irmp.ucl.ac.be"
        log.debug("Testing the access to MadAnalysis 5 website: " + url + " ...")
        info = InstallService.UrlAccess(url)
        # Close the access
        # NOTE: the function always returns True, even if the site is unreachable.
        if info != None:
            info.close()
        return True

    @staticmethod
    def check_dataverse() -> "bool":
        """Try to access the UCLouvain Dataverse.

        Returns:
            ``bool``:
            Always ``True`` (the result of the access is ignored).
        """
        url = "http://dataverse.uclouvain.be"
        log.debug("Testing access to the MadAnalysis5 dataverse: " + url + " ...")
        info = InstallService.UrlAccess(url)
        # Close the access
        if info != None:
            info.close()
        return True

    @staticmethod
    def create_tools_folder(path: "str") -> "bool":
        """Create the ``tools`` folder if needed.

        Args:
            path (``str``): path of the folder.

        Returns:
            ``bool``:
            ``True`` on success.
        """
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
    def create_package_folder(toolsdir: "str", package: "str") -> "bool":
        """Create the installation folder of a package (it must not exist yet).

        Args:
            toolsdir (``str``): parent folder.
            package (``str``): name of the package folder.

        Returns:
            ``bool``:
            ``False`` if the folder already exists or cannot be created.
        """

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
