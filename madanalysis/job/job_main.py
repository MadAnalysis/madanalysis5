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


from madanalysis.selection.histogram          import Histogram
from madanalysis.selection.instance_name      import InstanceName
from madanalysis.enumeration.observable_type  import ObservableType
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.interpreter.cmd_cut          import CmdCut
import logging

class JobMain:

    def __init__(self,file,main):
        self.file = file
        self.main = main
        import madanalysis.job.job_particle as JobParticle
        self.parts=JobParticle.GetParticles(self.main)


    def WriteHeader(self):
        import madanalysis.job.job_header as JobHeader
        JobHeader.WriteHeader(self.file,self.main)
        JobHeader.WriteCore(self.file,self.main,self.parts)
        JobHeader.WriteFoot(self.file,self.main)

    
    def WriteSource(self):
        self.file.write('#include "SampleAnalyzer/User/Analyzer/user.h"\n')
        self.file.write('using namespace MA5;\n')
        self.file.write('\n')
        import madanalysis.job.job_initialize as JobInitialize
        JobInitialize.WriteJobInitialize(self.file,self.main)
        import madanalysis.job.job_execute as JobExecute
        JobExecute.WriteExecute(self.file,self.main,self.parts)
        import madanalysis.job.job_finalize as JobFinalize
        JobFinalize.WriteJobFinalize(self.file,self.main)
    
