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


from madanalysis.IOinterface.particle_reader      import ParticleReader
from madanalysis.IOinterface.multiparticle_reader import MultiparticleReader
from madanalysis.enumeration.ma5_running_type     import MA5RunningType
from madanalysis.interpreter.cmd_define           import CmdDefine
from madanalysis.interpreter.cmd_base             import CmdBase
import logging

class CmdReset(CmdBase):
    """Command RESET"""


    def __init__(self,main):
        CmdBase.__init__(self,main,"reset")


    def do(self,args,myinterpreter):

        # Checking argument number
        if len(args) != 0:
            logging.error("wrong number of arguments for the command 'reset'.")
            self.help()
            return

        # Ask question
        if not self.main.forced:
            logging.warning("You are going to reinitialize MadAnalysis 5. The current configuration will be lost.")
            logging.warning("Are you sure to do that ? (Y/N)")
            allowed_answers=['n','no','y','yes']
            answer=""
            while answer not in  allowed_answers:
               answer=raw_input("Answer: ")
               answer=answer.lower()
            if answer=="no" or answer=="n":
                return False

        # Reset datasets
        self.main.datasets.Reset()

        # Reset selection
        self.main.selection.Reset()

        # Reset main
        self.main.ResetParameters()

        # Reset multiparticles
        self.ResetMultiparticles()

        # Reset history
        myinterpreter.history=[] 

        return

    def ResetMultiparticles(self):

        # Reset multiparticles
        self.main.multiparticles.Reset()

        # Opening a CmdDefine
        cmd_define = CmdDefine(self.main)

        # Loading particles
        input = ParticleReader(self.main.archi_info.ma5dir,cmd_define,self.main.mode)
        input.Load()
        input = MultiparticleReader(self.main.archi_info.ma5dir,cmd_define,self.main.mode,self.main.forced)
        input.Load()
        

    def help(self):
        logging.info("   Syntax: reset")
        logging.info("   Reinitializing all variables")


    def complete(self,text,line,begidx,endidx):
        return []
