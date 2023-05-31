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
import madanalysis.interpreter.cmd_base as CmdBase
import logging

class CmdDisplayMultiparticles(CmdBase.CmdBase):
    """Command DISPLAY_MULTIPARTICLES"""

    def __init__(self,main):
        CmdBase.CmdBase.__init__(self,main,"display_multiparticles")

    def do(self,args):
        self.main.multiparticles.DisplayMultiparticles()

    def help(self):
        logging.getLogger('MA5').info("   Syntax: display_multiparticles")
        logging.getLogger('MA5').info("   Displays the list of all (pre)defined multiparticles.")

    def complete(self,text,line,begidx,endidx,main):
        return

