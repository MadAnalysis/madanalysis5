################################################################################
#  
#  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <http://madanalysis.irmp.ucl.ac.be>
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


import threading
import time

class Timer:
    def __init__(self,tempo,target,args=[],kwargs={}):
        self.target = target
        self.args = args
        self.kwargs = kwargs
        self.tempo = tempo

    def run(self):
        self.timer = threading.Timer(self.tempo,self.run)
        self.timer.start()
        self.target(*self.args,**self.kwargs)

    def start(self):
        self.timer = threading.Timer(self.tempo,self.run)
        self.timer.start()

    def stop(self):
        self.timer.cancel()
        
