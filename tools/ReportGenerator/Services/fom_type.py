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


from math import sqrt
import parser

import logging

class fomType():
    def __init__(self):
        self.S = None
        self.B = None
        self.ES = None
        self.EB = None
        self.formula = None
        self.expr = None
#        self.r1 = "S/B"
#        self.r2 = "S/(S+B)"
#        self.r3 = "S/sqrt(S+B)"
#        self.er1 = "sqrt(B**2*ES**2+S**2*EB**2)/B**2"
#        self.er2 = "sqrt(B**2*ES**2 + S**2*EB**2)/(S+B)**2"
#        self.er3 = "sqrt((S+2*B)**2*ES**2 + S**2*EB**2)/2*(S+B)**(3.0/2)"
        
    def initialize(self, expression):
        self.expr = expression
        try:
            self.formula = parser.expr(expression).compile()
        except Exception as e:
            logging.error("The formula is not correct " + self.expr + ":")
            logging.error(e)
            return False
            
    def eval_expression(self):
        try:
            res = eval(self.formula)
            return res
        except Exception as e:
            logging.error("Impossible to evaluate the formula " + self.expr + ":")
            logging.error(e)
            return False
        
#fom = fomType()
#fom.initialize("sqrt((S+2*B)**2*ES**2 + S**2*EB**2)/2*(S+B)**(3.0/2)")
#B = 1.
#S = 1.
#ES = 2.
#EB = 3.
#res = fom.eval_expression()
#print res
#
