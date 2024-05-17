################################################################################
#
#  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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

import logging
import os
import sys

from shell_command import ShellCommand

from madanalysis.install.install_service import InstallService


class InstallOnnx:
    def __init__(self, main):
        self.main = main
        self.installdir = os.path.normpath(self.main.archi_info.ma5dir + "/tools/onnx/")
        self.toolsdir = os.path.normpath(self.main.archi_info.ma5dir + "/tools")
        self.tmpdir = self.main.session_info.tmpdir
        self.downloaddir = self.main.session_info.downloaddir
        self.untardir = os.path.normpath(self.tmpdir + "/MA5_onnx/")
        self.ncores = 1
        self.version = "1.17.1"
        if self.main.archi_info.isMac :
            self.ver_name = "onnxruntime-osx-x86_64-"+self.version
            self.lib_name = "libonnxruntime."+self.version+".dylib"
        else : #if not mac is linux 
            self.ver_name = "onnxruntime-linux-x64-"+self.version
            self.lib_name = "libonnxruntime.so."+self.version

        self.files = {self.ver_name+".tgz": "https://github.com/microsoft/onnxruntime/releases/download/v"+self.version+"/"+self.ver_name+".tgz"}

    def Detect(self):
        if not os.path.isdir(self.toolsdir):
            logging.getLogger("MA5").debug(
                "The folder '" + self.toolsdir + "' is not found"
            )
            return False
        if not os.path.isdir(self.installdir):
            logging.getLogger("MA5").debug(
                "The folder " + self.installdir + "' is not found"
            )
            return False
        return True

    def Remove(self, question=True):
        from madanalysis.IOinterface.folder_writer import FolderWriter
        return FolderWriter.RemoveDirectory(self.installdir, question)

    def GetNcores(self):
        self.ncores = InstallService.get_ncores(
            self.main.archi_info.ncores, self.main.forced
        )

    def CreatePackageFolder(self):
        if not InstallService.create_tools_folder(self.toolsdir):
            return False
        if not InstallService.create_package_folder(self.toolsdir, "onnx"):
            return False
        return True

    def CreateTmpFolder(self):
        ok = InstallService.prepare_tmp(self.untardir, self.downloaddir)
        if ok:
            self.tmpdir = self.untardir
        return ok

    def Download(self):
        # Checking connection with MA5 web site
        if not InstallService.check_ma5site():
            return False
        # Launching wget
        logname = os.path.normpath(self.installdir + "/wget.log")
        if not InstallService.wget(self.files, logname, self.downloaddir):
            return False
        # Ok
        return True

    def Unpack(self):
        # Logname
        logname = os.path.normpath(self.installdir + "/unpack.log")
        # Unpacking the tarball
        ok, packagedir = InstallService.untar(
            logname, self.downloaddir, self.installdir, self.ver_name+".tgz"
        )
        if not ok:
            return False
        # Ok: returning the good folder
        self.tmpdir = packagedir
        return True

    def Check(self):
        # Check folders
        dirs = [self.installdir +"/" +self.ver_name +"/include", self.installdir +"/" +self.ver_name + "/lib"]

        for dir in dirs:
            if not os.path.isdir(dir):
                logging.getLogger("MA5").error("folder " + dir + " is missing.")
                self.display_log()
                return False

        # Check one header file
        if not os.path.isfile(self.installdir +"/" +self.ver_name +"/include/onnxruntime_cxx_api.h"):
            logging.getLogger("MA5").error("header labeled 'include/onnx.h' is missing.")
            self.display_log()
            return False

        if (not os.path.isfile(self.installdir +"/" +self.ver_name +"/lib/"+self.lib_name)):
            logging.getLogger("MA5").error(
                "libonnxruntime is missing."
            )
            self.display_log()
            return False

        return True

    def display_log(self):
        logging.getLogger("MA5").error("More details can be found into the log files:")
        logging.getLogger("MA5").error(
            " - " + os.path.normpath(self.installdir + "/wget.log")
        )
        logging.getLogger("MA5").error(
            " - " + os.path.normpath(self.installdir + "/unpack.log")
        )

    def NeedToRestart(self):
        return True
