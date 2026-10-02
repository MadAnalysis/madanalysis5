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


"""Generation of the C++ analysis (``user.h``/``user.cpp``) corresponding to the selection.

The analysis inherits from ``MA5::AnalyzerBase``. The header declares the particle
containers and the particle-identification functions (:mod:`~madanalysis.job.job_header`);
the source file defines ``Initialize`` (:mod:`~madanalysis.job.job_initialize`),
``Execute`` (:mod:`~madanalysis.job.job_execute`) and ``Finalize``
(:mod:`~madanalysis.job.job_finalize`).
"""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING, Any, TextIO

if TYPE_CHECKING:
    from madanalysis.core.main import Main
from madanalysis.selection.histogram          import Histogram
from madanalysis.selection.instance_name      import InstanceName
from madanalysis.enumeration.observable_type  import ObservableType
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.interpreter.cmd_cut          import CmdCut
import logging

class JobMain:
    """Writer of the ``user`` analysis class.

    Attributes:
        file (``TextIO``): output file.
        main (``Main``): session state.
        parts (``list[list[Any]]``): particle containers used by the selection (see
            :func:`~madanalysis.job.job_particle.GetParticles`).
    """

    def __init__(self,file: TextIO,main: Main) -> None:
        """Collect the particle containers used by the selection.

        Args:
            file (``TextIO``): output file (header or source).
            main (``Main``): session state.
        """
        self.file = file
        self.main = main
        import madanalysis.job.job_particle as JobParticle
        self.parts=JobParticle.GetParticles(self.main)


    def WriteHeader(self) -> None:
        """Write the header ``user.h``."""
        import madanalysis.job.job_header as JobHeader
        JobHeader.WriteHeader(self.file,self.main)
        JobHeader.WriteCore(self.file,self.main,self.parts)
        JobHeader.WriteFoot(self.file,self.main)

    def WriteSource(self) -> None:
        """Write the source file ``user.cpp``."""
        self.file.write('#include "SampleAnalyzer/User/Analyzer/user.h"\n')
        self.file.write('using namespace MA5;\n')
        self.file.write('\n')
        import madanalysis.job.job_initialize as JobInitialize
        JobInitialize.WriteJobInitialize(self.file,self.main)
        import madanalysis.job.job_execute as JobExecute
        JobExecute.WriteExecute(self.file,self.main,self.parts)
        import madanalysis.job.job_finalize as JobFinalize
        JobFinalize.WriteJobFinalize(self.file,self.main)


