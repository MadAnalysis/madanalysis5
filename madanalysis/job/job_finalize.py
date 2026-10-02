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


"""Writer of the ``user::Finalize`` method of the generated analysis."""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING, Any, TextIO

if TYPE_CHECKING:
    from madanalysis.core.main import Main
import logging
def WriteJobFinalize(file: TextIO,main: Main) -> None:
    """Write an empty ``user::Finalize`` method.

    Args:
        file (``TextIO``): output C++ file.
        main (``Main``): session state.
    """

    # Function header
    file.write('void user::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files)\n{\n')
    file.write('}\n')
