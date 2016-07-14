################################################################################
#  
#  Copyright (C) 2012-2016 Eric Conte, Benjamin Fuks
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


from madanalysis.layout.measure               import Measure
from math import sqrt


class FomCalculation:

    def __init__(self,main):
        self.formula = main.fom.formula
        self.x       = main.fom.x

    def Compute(self,S,ES,B,EB):

        # Initialization
        value = Measure()

        # Mean value
        if self.formula==1:
            value.mean = float(S)/float(B)
        elif self.formula==2:
            value.mean = float(S)/sqrt(B)
        elif self.formula==3:
            value.mean = float(S)/float(S+B)
        elif self.formula==4:
            value.mean = float(S)/sqrt(S+B)
        elif self.formula==5:
            value.mean = float(S)/sqrt(S+B+(self.x*B)**2)

        # Error value
        if self.formula==1:
            value.error = 1./(B**2)*\
                          sqrt(B**2*ES**2+S**2*EB**2)
        elif self.formula==2:
            value.error = 1./(S+B)**2*\
                          sqrt(B**2*ES**2+S**2*EB**2)
        elif self.formula==3:
            value.error = 1./(2*pow(B,3./2.))*\
                          sqrt((2*B)**2*ES**2+S**2*EB**2)
        elif self.formula==4:
            value.error = 1./(2*pow(S+B,3./2.))*\
                          sqrt((S+2*B)**2*ES**2+S**2*EB**2)
        elif self.formula==5:
            value.error = 1./(2*pow(S+B+self.x*B**2,3./2.))*\
                          sqrt((S+2*B+2*self.x*B**2)**2*ES**2+S**2*(2*self.x*B+1)**2*EB**2)

        return value

