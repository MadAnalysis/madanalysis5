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


from madanalysis.configuration.isolation_cone  import IsolationCone
from madanalysis.configuration.isolation_sumpt import IsolationSumPt
import logging

class IsolationConfiguration():


    userVariables = { "algorithm" : ["cone","sumpt"] }

    def __init__(self):
        self.algorithm = 'cone'
        self.isolation = IsolationCone()

    def Display(self):
        self.user_DisplayParameter("algorithm")
        self.isolation.Display()

    def user_DisplayParameter(self,parameter):
        if parameter=="algorithm":
            logging.info(" isolation algorithm : "+self.algorithm)
        else:
            self.isolation.user_DisplayParameter(parameter)

    def user_GetParameters(self):
        table = IsolationConfiguration.userVariables.keys()
        table.extend(self.isolation.user_GetParameters())
        return table


    def user_GetValues(self,variable):
        table = []
        try:
            table.extend(IsolationConfiguration.userVariables[variable])
        except:
            pass
        try:
            table.extend(self.isolation.user_GetValues(variable))
        except:
            pass
        return table

    def user_SetParameter(self,parameter,value):
        # algorithm
        if parameter=="algorithm":
            if value=='cone':
                self.algorithm='cone'
                self.isolation=IsolationCone()
            elif value=='sumpt':
                self.algorithm='sumpt'
                self.isolation=IsolationSumPt()
            else:
                logging.error("Specified algorithm for muon isolation is not correct. "+\
                              "Only possible ones are : 'cone' and 'sumpt'.")
                return False
        # other    
        else:
            self.isolation.user_SetParameter(parameter,value)
