################################################################################
#  
#  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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
from math import log

class FomCalculation:

    def __init__(self,main):
        self.formula = main.fom.formula
        self.x       = main.fom.x

    def Compute(self,s,es,b,eb):

        # Initialization
        value = Measure()

        # Converting in floating value
        S=float(s)
        B=float(b)
        ES=float(es)
        EB=float(eb)
        
        # Mean value
        try:
            if self.formula==1:
                value.mean = S/B
            elif self.formula==2:
                value.mean = S/sqrt(B)
            elif self.formula==3:
                value.mean = S/(S+B)
            elif self.formula==4:
                value.mean = S/sqrt(S+B)
            elif self.formula==5:
                value.mean = S/sqrt(S+B+(self.x*B)**2)
            elif self.formula==6:
                value.mean = sqrt(2)*sqrt((S+B)*log(1+S/B)-S)
        except ZeroDivisionError:  # division by 0
            value.mean=-1
        except ValueError: # negative sqrt or log
            value.mean=-1

        # Error value
        try:
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
            elif self.formula==6:
                value.error = sqrt(2)/(2.*value.mean)*\
                              sqrt( ES**2 * (log(1+S/B))**2 +\
                                    EB**2 * (log(1+S/B)-S/B)**2 )

        except ZeroDivisionError: # division by 0
            value.mean=-1
        except ValueError: # negative sqrt or log
            value.mean=-1

        return value

