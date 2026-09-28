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


"""Writer of the environment setup scripts (``setup.sh`` and ``setup.csh``).

The scripts export ``MA5_BASE`` and prepend the paths of the MadAnalysis 5 and
external libraries/binaries to ``PATH``, ``LD_LIBRARY_PATH`` and (on macOS)
``DYLD_LIBRARY_PATH``. They are written in ``tools/SampleAnalyzer/`` when the libraries
are built, and in the ``Build/`` folder of every job; they must be sourced before
compiling or running a job by hand.
"""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any
import logging
from pathlib import Path
from string_tools import StringTools  # pylint: disable=import-error
from six.moves import range

log = logging.getLogger("MA5")


class SetupWriter:
    """Static helpers writing the environment setup scripts."""
    @staticmethod
    def OrderPath(paths1: list[str], middle: str, paths2: list[str], ma5dir: str) -> tuple[list[str], list[str], list[str]]:
        """Build the ordered list of paths of an environment variable.

        The MadAnalysis 5 folder is replaced by ``$MA5_BASE`` in all paths.

        Args:
            paths1 (``list[str]``): paths to prepend.
            middle (``str``): previous value of the variable (e.g. ``"$PATH"``).
            paths2 (``list[str]``): paths to append.
            ma5dir (``str``): MadAnalysis 5 installation folder.

        Returns:
            ``tuple[list[str], list[str], list[str]]``:
            The paths without the previous value, the paths for ``sh`` (previous value
            inserted) and the paths for ``csh`` (previous value quoted).
        """
        all = []
        allsh = []
        allcsh = []
        for item in paths1:
            path = item.replace(ma5dir, "$MA5_BASE")
            all.append(path)
            allsh.append(path)
            allcsh.append(path)
        allsh.append(middle)
        allcsh.append('"' + middle + '"')
        for item in paths2:
            path = item.replace(ma5dir, "$MA5_BASE")
            all.append(path)
            allsh.append(path)
            allcsh.append(path)
        return all, allsh, allcsh

    @staticmethod
    def WriteSetupFile(bash: bool, path: str, archi_info: Any) -> bool:
        """Write ``setup.sh`` or ``setup.csh`` in a folder.

        Besides the paths, the scripts export ``ROOT_INCLUDE_PATH`` (Delphes headers) and
        ``FASTJET_FLAG`` when these packages are available.

        Args:
            bash (``bool``): ``True`` for ``setup.sh``, ``False`` for ``setup.csh``.
            path (``str``): destination folder.
            archi_info (``ArchitectureInfo``): detected configuration (paths, packages).

        Returns:
            ``bool``:
            ``True`` on success, ``False`` if the file cannot be written or closed.
        """

        # Variable to check at the end
        toCheck = []

        # Opening file in write-only mode
        path = Path(path).absolute()

        if bash:
            filename = path.joinpath("setup.sh")
        else:
            filename = path.joinpath("setup.csh")
        try:
            file = filename.open("w", encoding="utf-8")
        except (IOError, OSError) as e:
            log.error("Impossible to create the file `%s`: %s", filename, str(e))
            return False

        # Calling the good shell
        if bash:
            # NOTE: the generated script uses bash-only syntax ('[[ ]]', 'echo -e') despite '#!/bin/sh'.
            file.write("#!/bin/sh\n")
        else:
            file.write("#!/bin/csh -f\n")
        file.write("\n")

        # Defining colours
        file.write("# Defining colours for shell\n")

        delphes_inc_pths = []
        if len(archi_info.delphes_inc_paths) != 0:
            delphes_inc_pths = list(archi_info.delphes_inc_paths)
            delphes_inc_pths.append(
                next((p for p in delphes_inc_pths if Path(p).stem == "delphes"), "")
                + "/modules"
            )

        if bash:
            file.write('GREEN="\\\\033[1;32m"\n')
            file.write('RED="\\\\033[1;31m"\n')
            file.write('PINK="\\\\033[1;35m"\n')
            file.write('BLUE="\\\\033[1;34m"\n')
            file.write('YELLOW="\\\\033[1;33m"\n')
            file.write('CYAN="\\\\033[1;36m"\n')
            file.write('NORMAL="\\\\033[0;39m"\n\n')
            if archi_info.has_delphes:
                file.write(
                    "export ROOT_INCLUDE_PATH=" + ":".join(delphes_inc_pths) + "\n"
                )
            if archi_info.has_fastjet:
                file.write('export FASTJET_FLAG="-DMA5_FASTJET_MODE"\n')
        else:
            file.write('set GREEN  = "\\033[1;32m"\n')
            file.write('set RED    = "\\033[1;31m"\n')
            file.write('set PINK   = "\\033[1;35m"\n')
            file.write('set BLUE   = "\\033[1;34m"\n')
            file.write('set YELLOW = "\\033[1;33m"\n')
            file.write('set CYAN   = "\\033[1;36m"\n')
            file.write('set NORMAL = "\\033[0;39m"\n')
            if archi_info.has_delphes:
                file.write(
                    "setenv ROOT_INCLUDE_PATH " + ":".join(delphes_inc_pths) + "\n"
                )
            if archi_info.has_fastjet:
                file.write('setenv FASTJET_FLAG "-DMA5_FASTJET_MODE"\n')
        file.write("\n")

        # Treating ma5dir
        ma5dir = archi_info.ma5dir
        if ma5dir.endswith("/"):
            ma5dir = ma5dir[:-1]

        # Configuring PATH environment variable
        file.write("# Configuring MA5 environment variable\n")
        if bash:
            file.write("export MA5_BASE=" + (ma5dir) + "\n")
        else:
            file.write("setenv MA5_BASE " + (ma5dir) + "\n")
        toCheck.append("MA5_BASE")
        file.write("\n")

        # Treating PATH
        toPATH, toPATHsh, toPATHcsh = SetupWriter.OrderPath(
            archi_info.toPATH1, "$PATH", archi_info.toPATH2, ma5dir
        )
        toLDPATH, toLDPATHsh, toLDPATHcsh = SetupWriter.OrderPath(
            archi_info.toLDPATH1, "$LD_LIBRARY_PATH", archi_info.toLDPATH2, ma5dir
        )
        toDYLDPATH, toDYLDPATHsh, toDYLDPATHcsh = SetupWriter.OrderPath(
            archi_info.toLDPATH1, "$DYLD_LIBRARY_PATH", archi_info.toLDPATH2, ma5dir
        )

        # Configuring PATH environment variable
        if len(toPATH) != 0:
            file.write("# Configuring PATH environment variable\n")
            if bash:
                # FIXME: '$PATH' is not quoted in the generated test: if PATH contains spaces the test fails
                # and the else-branch overwrites PATH (same for LD_LIBRARY_PATH and DYLD_LIBRARY_PATH below).
                file.write("if [ $PATH ]; then\n")
                file.write("    export PATH=" + (":".join(toPATHsh)) + "\n")
                file.write("else\n")
                file.write("    export PATH=" + (":".join(toPATH)) + "\n")
                file.write("fi\n")
            else:
                file.write("if ( $?PATH ) then\n")
                file.write("    setenv PATH " + (":".join(toPATHcsh)) + "\n")
                file.write("else\n")
                file.write("    setenv PATH " + (":".join(toPATH)) + "\n")
                file.write("endif\n")
            toCheck.append("PATH")
            file.write("\n")

        if len(toLDPATH) != 0:

            # Configuring LD_LIBRARY_PATH environment variable
            file.write("# Configuring LD_LIBRARY_PATH environment variable\n")
            if bash:
                file.write("if [ $LD_LIBRARY_PATH ]; then\n")
                file.write("    export LD_LIBRARY_PATH=" + (":".join(toLDPATHsh)) + "\n")
                file.write("else\n")
                file.write("    export LD_LIBRARY_PATH=" + (":".join(toLDPATH)) + "\n")
                file.write("fi\n")
            else:
                file.write("if ( $?LD_LIBRARY_PATH ) then\n")
                file.write("    setenv LD_LIBRARY_PATH " + (":".join(toLDPATHcsh)) + "\n")
                file.write("else\n")
                file.write("    setenv LD_LIBRARY_PATH " + (":".join(toLDPATH)) + "\n")
                file.write("endif\n")
            toCheck.append("LD_LIBRARY_PATH")
            file.write("\n")

            # Configuring LIBRARY_PATH environment variable
            # file.write('# Configuring LIBRARY_PATH environment variable\n')
            # if bash:
            #    file.write('export LIBRARY_PATH=' + (os.environ['LD_LIBRARY_PATH'])+'\n')
            # else:
            #    file.write('setenv LIBRARY_PATH ' + (os.environ['LD_LIBRARY_PATH'])+'\n')
            # file.write('\n')

            # Configuring DYLD_LIBRARY_PATH environment variable
            if archi_info.isMac:
                file.write("# Configuring DYLD_LIBRARY_PATH environment variable\n")
                if bash:
                    file.write("if [ $DYLD_LIBRARY_PATH ]; then\n")
                    file.write(
                        "    export DYLD_LIBRARY_PATH=" + (":".join(toDYLDPATHsh)) + "\n"
                    )
                    file.write("else\n")
                    file.write(
                        "    export DYLD_LIBRARY_PATH=" + (":".join(toLDPATH)) + "\n"
                    )
                    file.write("fi\n")
                else:
                    file.write("if ( $?DYLD_LIBRARY_PATH ) then\n")
                    file.write(
                        "    setenv DYLD_LIBRARY_PATH " + (":".join(toDYLDPATHcsh)) + "\n"
                    )
                    file.write("else\n")
                    file.write(
                        "    setenv DYLD_LIBRARY_PATH " + (":".join(toLDPATH)) + "\n"
                    )
                    file.write("endif\n")
                toCheck.append("DYLD_LIBRARY_PATH")
                file.write("\n")

            # Configuring CPLUS_INCLUDE_PATH environment variable
            # file.write('# Configuring CPLUS_INCLUDE_PATH environment variable\n')
            # if bash:
            #    file.write('export CPLUS_INCLUDE_PATH=' + (os.environ['CPLUS_INCLUDE_PATH'])+'\n')
            # else:
            #    file.write('setenv CPLUS_INCLUDE_PATH ' + (os.environ['CPLUS_INCLUDE_PATH'])+'\n')
            # file.write('\n')

        # Checking that all environment variables are defined
        file.write("# Checking that all environment variables are defined\n")
        if bash:
            file.write("if [[ ")
            for ind in range(0, len(toCheck)):
                if ind != 0:
                    file.write(" && ")
                file.write("$" + toCheck[ind])
            file.write(" ]]; then\n")
            file.write('    echo -e $YELLOW"' + StringTools.Fill("-", 56) + '"\n')
            file.write(
                '    echo -e "'
                + StringTools.Center(
                    "Your environment is properly configured for MA5", 56
                )
                + '"\n'
            )
            file.write('    echo -e "' + StringTools.Fill("-", 56) + '"$NORMAL\n')
            file.write("fi\n")
        else:
            file.write("if ( ")
            for ind in range(0, len(toCheck)):
                if ind != 0:
                    file.write(" && ")
                file.write("$?" + toCheck[ind])
            file.write(" ) then\n")
            file.write('    printf $YELLOW"' + StringTools.Fill("-", 56) + '"$NORMAL\n')
            file.write(
                '    printf $YELLOW"'
                + StringTools.Center(
                    "Your environment is properly configured for MA5", 56
                )
                + '"$NORMAL\n'
            )
            file.write('    printf $YELLOW"' + StringTools.Fill("-", 56) + '"$NORMAL\n')
            file.write("endif\n")

        # Closing the file
        try:
            file.close()
        except:
            # FIXME: 'filename' is a Path: concatenation with str raises a TypeError.
            log.error('Impossible to close the file "' + filename + '"')
            return False

        return True
