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


import logging
class IsolationSumPt():

    default_sumPT  = 1.
    default_ET_PT  = 1.

    userVariables = { "ET_PT"  : [str(default_ET_PT)], \
                      "sumPT"  : [str(default_sumPT)] }

    def __init__(self):
        self.sumPT = IsolationSumPt.default_sumPT
        self.ET_PT = IsolationSumPt.default_ET_PT

        
    def Display(self):
        self.user_DisplayParameter("ET_PT")
        self.user_DisplayParameter("sumPT")


    def user_DisplayParameter(self,parameter):
        if parameter=='sumPT':
            logging.info('  + sumPT = ' + str(self.sumPT) )
        elif parameter=='ET_PT':
            logging.info('  + ET_PT = ' + str(self.ET_PT) )
        else:
            logging.error("'isolation' has no parameter called '"+parameter+"'")

        
    def user_GetValues(self,variable):
        try:
            return IsolationSumPt.userVariables[variable]
        except:
            return []

    
    def user_GetParameters(self):
        return IsolationSumPt.userVariables.keys()


    def user_SetParameter(self,parameter,value):

        # ET_PT
        if parameter=="ET_PT":
            try:
                tmp=float(value)
                self.ET_PT=tmp
            except:
                logging.error("'"+value+"' is not a float value.")
                return False

        # sumPT
        elif parameter=="sumPT":
            try:
                tmp=float(value)
                self.sumPT=tmp
            except:
                logging.error("'"+value+"' is not a float value.")
                return False

        # other    
        else:
            logging.error("'isolation' has no parameter called '"+parameter+"'")
