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


import logging
class FomConfiguration:

    userVariables = { "formula" : ['1','2','3','4','5','6'] }

    # 1: S/B
    # 2: S/sqrt(B)
    # 3: S/(S+B)
    # 4: S/sqrt(S+B)
    # 5: S/sqrt(S+B+(xB)**2)
    # 6: sqrt(2)*sqrt((S+B)log(1+S/B)-S)

    allformula = [ 'S/B', 'S/sqrt(B)', 'S/(S+B)', 'S/sqrt(S+B)', 'S/sqrt(S+B+(xB)**2)',\
                   'sqrt(2)*sqrt((S+B)log(1+S/B)-S)']


    def __init__(self):
        self.formula = 4
        self.x       = 0
        self.logger  = logging.getLogger('MA5')

        
    def Display(self):
        self.user_DisplayParameter("formula")
        self.user_DisplayParameter("x")


    def user_DisplayParameter(self,parameter):
        if parameter=="formula":
            self.logger.info(" figure of merit (fom) - formula num "+str(self.formula)+": "+FomConfiguration.allformula[self.formula-1])
        elif parameter=="x" and self.IsX():
            self.logger.info(" x parameter value: "+str(self.x))


    def user_SetParameter(self,parameter,value):
        # algorithm
        if parameter=="formula":
            if value in FomConfiguration.userVariables["formula"]:
                valueint = int(value)
                self.formula=valueint
                self.Display()
            else:
                self.logger.error("The only possible values for 'formula' are : "+' '.str(FomConfiguration.userVariables["formula"]+'.'))
                return False
        elif parameter=='x' and self.IsX():
            try:
                number = float(value)
            except:
                self.logger.error("The x parameter must be a float value.")
                return False
            if number<0:
                self.logger.error("The x parameter cannont be negative.")
                return False
            self.x=number
        # other    
        else:
            self.logger.error("'formula' has no parameter called '"+parameter+"'")

            
    def IsX(self):
        return ('x' in FomConfiguration.allformula[self.formula-1])
    
        
    def user_GetParameters(self):
        table = FomConfiguration.userVariables.keys()
        if self.IsX():
            table.extend("x")
        return table


    def user_GetValues(self,variable):
        if variable in FomConfiguration.userVariables.keys():
            table = FomConfiguration.userVariables[variable]
        if self.IsX() and variable=='x':
            table = ['0.']
        return table

    def getFormula(self):
        return FomConfiguration.allformula[self.formula-1]

