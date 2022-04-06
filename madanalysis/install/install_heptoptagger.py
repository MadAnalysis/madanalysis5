################################################################################
#
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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

from __future__ import absolute_import
from madanalysis.install.install_service import InstallService
from shell_command import ShellCommand
import os, json
import logging


class InstallHEPTopTagger:
    def __init__(self, main):
        self.main = main
        self.installdir = os.path.normpath(self.main.archi_info.ma5dir + "/tools/HEPTopTagger/")
        self.toolsdir = os.path.normpath(self.main.archi_info.ma5dir + "/tools")
        self.tmpdir = self.main.session_info.tmpdir
        self.downloaddir = self.main.session_info.downloaddir
        self.untardir = os.path.normpath(self.tmpdir + "/MA5_heptoptagger/")
        self.ncores = 1
        self.github_repo = "https://api.github.com/repos/MadAnalysis/HEPTopTagger/releases/latest"
        self.meta = {}

    def Detect(self):
        if not os.path.isdir(self.toolsdir):
            logging.getLogger("MA5").debug("The folder '" + self.toolsdir + "' is not found")
            return False
        if not os.path.isdir(self.installdir):
            logging.getLogger("MA5").debug("The folder " + self.installdir + "' is not found")
            return False
        return True

    def Remove(self, question=True):
        from madanalysis.IOinterface.folder_writer import FolderWriter
        return FolderWriter.RemoveDirectory(self.installdir, question)

    def CreatePackageFolder(self):
        if not InstallService.create_tools_folder(self.toolsdir):
            return False
        if not InstallService.create_package_folder(self.toolsdir, "HEPTopTagger"):
            return False
        return True

    def CreateTmpFolder(self):
        ok = InstallService.prepare_tmp(self.untardir, self.downloaddir)
        if ok:
            self.tmpdir = self.untardir
        return ok

    def Download(self):
        # Checking connection with MA5 web site
        theCommands=['curl', '-s', self.github_repo]
        logging.getLogger('MA5').debug('shell command: '+' '.join(theCommands))
        ok, out = ShellCommand.ExecuteWithLog(
            theCommands, os.path.normpath(self.installdir + "/github_meta.log"), ".", silent=False
        )
        if not ok:
            return False

        self.meta = json.loads(out)

        logging.getLogger('MA5').debug(f"  -> HEPTopTagger {self.meta['tag_name']}")
        logging.getLogger('MA5').debug(f"  -> Published at {self.meta['published_at']}")
        theCommands = [
            'curl', '-s', "-L", "--create-dirs", "-o",
            os.path.join(self.downloaddir, self.meta['tag_name'] + ".tar.gz"),
            self.meta["tarball_url"]
        ]
        ok, out = ShellCommand.ExecuteWithLog(
            theCommands, os.path.normpath(self.installdir + "/curl.log"), ".", silent=False
        )
        logging.getLogger('MA5').debug(f"is download ok? {ok} :: {out}")

        return ok

    def Unpack(self):
        # Logname
        logname = os.path.normpath(self.installdir + "/unpack.log")
        # Unpacking the tarball
        logging.getLogger("MA5").debug(f"Unpack : {self.meta['tag_name']+'.tar.gz'}")
        ok, packagedir = InstallService.untar(
            logname, self.downloaddir, self.installdir, self.meta['tag_name']+'.tar.gz'
        )
        logging.getLogger("MA5").debug(f"Unpack : {packagedir} is ok? {ok}")
        if not ok:
            return False

        from glob import glob
        content = glob(self.installdir + "/*")
        if len(content) == 0:
            return False

        found = False
        main_folder = None
        for file in content:
            if os.path.isdir(file) and "MadAnalysis-HEPTopTagger" in file:
                content = glob(os.path.join(file, "*"))
                found = True
                main_folder = file
                break

        if not found:
            return False

        import shutil
        for htt_file in content:
            logging.getLogger("MA5").debug(f"copy {htt_file} to {self.installdir}")
            shutil.copyfile(htt_file, os.path.join(self.installdir, os.path.basename(htt_file)))

        if main_folder is not None:
            try:
                shutil.rmtree(main_folder, ignore_errors=True)
            except Exception as err:
                logging.getLogger('MA5').debug(err)

        with open(os.path.join(self.installdir, "metadata.json"), "w") as meta:
            json.dump(self.meta, meta, indent=4)

        # Ok: returning the good folder
        self.tmpdir = packagedir
        return True

    def Check(self):
        # Check HTT files
        for htt_file in ["HEPTopTagger.hh", "HEPTopTagger.cc"]:
            if not os.path.isfile(os.path.join(self.installdir, htt_file)):
                logging.getLogger("MA5").error(f"{htt_file} is missing.")
                self.display_log()
                return False

        return True

    def display_log(self):
        logging.getLogger("MA5").error("More details can be found into the log files:")
        logging.getLogger("MA5").error(" - " + os.path.normpath(self.installdir + "/github_meta.log"))
        logging.getLogger("MA5").error(" - " + os.path.normpath(self.installdir + "/curl.log"))
        logging.getLogger("MA5").error(" - " + os.path.normpath(self.installdir + "/unpack.log"))

    def NeedToRestart(self):
        return True
