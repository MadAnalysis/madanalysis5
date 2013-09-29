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


from madanalysis.enumeration.ma5_running_type         import MA5RunningType
import logging

class DelphesConfiguration:

    userVariables = { "detector" : ["cms","atlas"] }

    def __init__(self):
        self.detector  = "cms"

        
    def Display(self):
        self.user_DisplayParameter("detector")


    def user_DisplayParameter(self,parameter):
        if parameter=="detector":
            logging.info(" detector : "+self.detector)
            return

    def SampleAnalyzerConfigString(self):
            return {}


    def user_SetParameter(self,parameter,value,datasets,level):
        
        # algorithm
        if parameter=="detector":

            if value=="cms":
                self.detector=value
            elif value=="atlas":
                self.detector=value
            else:
                logging.error("algorithm called '"+value+"' is not found.")
            return    

        else:
            logging.error("parameter called '"+parameter+"' does not exist")
            return

        
    def user_GetParameters(self):
        return DelphesConfiguration.userVariables.keys()


    def user_GetValues(self,variable):
        table = []
        try:
            table.extend(DelphesConfiguration.userVariables[variable])
        except:
            pass
        return table
        
