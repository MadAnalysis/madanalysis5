################################################################################
#
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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

import warnings

Red = "\x1b[31m"
End = "\x1b[0m"


class InvalidScript(Exception):
    """Invalid Script Exception"""

    def __init__(self, message="Invalid Script!"):
        super(InvalidScript, self).__init__(Red + message + End)


class InvalidMode(Exception):
    """Invalid Mode Exception"""

    def __init__(self, message="Invalid mode!"):
        super(InvalidMode, self).__init__(Red + message + End)


class InvalidMadAnalysisPath(Exception):
    """Invalid path for MadAnalysis 5 Package"""

    def __init__(self, message="Invalid MadAnalysis 5 path!"):
        super(InvalidMadAnalysisPath, self).__init__(Red + message + End)


class MadAnalysis5Error(Exception):
    """MadAnalysis 5 Error"""

    def __init__(self, message="MadAnalysis 5 raised an error!"):
        super(MadAnalysis5Error, self).__init__(Red + message + End)


class MadAnalysis5ExecutionError(Exception):
    """MadAnalysis 5 execution has failed"""

    def __init__(self, message="MadAnalysis 5 failed during execution!"):
        super(MadAnalysis5ExecutionError, self).__init__(Red + message + End)


def InvalidSyntax(message):
    warnings.warn(message, SyntaxWarning, stacklevel=2)
