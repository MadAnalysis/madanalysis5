################################################################################
#  
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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


from __future__ import absolute_import
import logging
class TauIdentification():

    default_matching_dr = 0.5
    default_exclusive   = True
    default_efficiency  = 1.
    default_misid_ljet  = 0.

    userVariables = {"tau_id.matching_dr": [str(default_misid_ljet)]}


    def __init__(self):
        self.efficiency  = TauIdentification.default_efficiency
        self.misid_ljet  = TauIdentification.default_misid_ljet
        self.matching_dr = TauIdentification.default_matching_dr

        
    def Display(self):
        logging.getLogger('MA5').info("  + hadronic-tau identification:")
        self.user_DisplayParameter("tau_id.matching_dr")
        # self.user_DisplayParameter("tau_id.efficiency")
        # self.user_DisplayParameter("tau_id.misid_ljet")


    def user_DisplayParameter(self,parameter):
        if parameter=="tau_id.matching_dr":
            logging.getLogger('MA5').info("    + DeltaR matching = "+str(self.matching_dr))
        # if parameter=="tau_id.efficiency":
        #     logging.getLogger('MA5').info("    + id efficiency = "+str(self.efficiency))
        # elif parameter=="tau_id.misid_ljet":
        #     logging.getLogger('MA5').info("    + mis-id efficiency (light quarks) = "+str(self.misid_ljet))
        else:
            logging.getLogger('MA5').error("'clustering' has no parameter called '"+parameter+"'")


    def SampleAnalyzerConfigString(self):
        return {
            'tau_id.efficiency': str(self.efficiency),
            'tau_id.misid_ljet': str(self.misid_ljet),
            'tau_id.matching_dr': str(self.matching_dr),
        }

        
    def user_GetValues(self,variable):
        try:
            return TauIdentification.userVariables[variable]
        except:
            return []

    
    def user_GetParameters(self):
        return list(TauIdentification.userVariables.keys())


    def user_SetParameter(self,parameter,value):
        # matching deltar
        if parameter == "tau_id.matching_dr":
            try:
                number = float(value)
            except:
                logging.getLogger('MA5').error("the 'matching deltaR' must be a float value.")
                return False
            if number <= 0:
                logging.getLogger('MA5').error("the 'matching deltaR' cannot be negative or null.")
                return False
            self.matching_dr = number

        # efficiency
        elif parameter == "tau_id.efficiency":
            logging.getLogger('MA5').error("This function has been deprecated, please use SFS functionality instead.")
            logging.getLogger('MA5').error("Same functionality can be captured via following command in SFS:")
            logging.getLogger('MA5').error(f"     -> define tagger ta as ta {value}")
            return False

        # mis efficiency (ljet)
        elif parameter == "tau_id.misid_ljet":
            logging.getLogger('MA5').error("This function has been deprecated, please use SFS functionality instead.")
            logging.getLogger('MA5').error("Same functionality can be captured via following command in SFS:")
            logging.getLogger('MA5').error(f"     -> define tagger ta as j {value}")
            return False

        # other    
        else:
            logging.getLogger('MA5').error("'clustering' has no parameter called '"+parameter+"'")
