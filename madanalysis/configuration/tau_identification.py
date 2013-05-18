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
class TauIdentification():

    default_matching_dr = 0.5
    default_exclusive   = False
    default_efficiency  = 1.
    default_misid_ljet  = 0.

    userVariables = { "tau_id.matching_dr" : [str(default_matching_dr)],\
                      "tau_id.exclusive"   : [str(default_exclusive)],\
                      "tau_id.efficiency"  : [str(default_efficiency)],\
                      "tau_id.misid_ljet"  : [str(default_misid_ljet)]\
                    }


    def __init__(self):
        self.matching_dr = TauIdentification.default_matching_dr
        self.exclusive   = TauIdentification.default_exclusive
        self.efficiency  = TauIdentification.default_efficiency
        self.misid_ljet  = TauIdentification.default_misid_ljet

        
    def Display(self):
        logging.info("  + hadronic-tau identification:")
        self.user_DisplayParameter("tau_id.matching_dr")
        self.user_DisplayParameter("tau_id.exclusive")
        self.user_DisplayParameter("tau_id.efficiency")
        self.user_DisplayParameter("tau_id.misid_ljet")


    def user_DisplayParameter(self,parameter):
        if parameter=="tau_id.matching_dr":
            logging.info("    + DeltaR matching = "+str(self.matching_dr))
        elif parameter=="tau_id.exclusive":
            msg="false"
            if self.exclusive:
                msg="true"
            logging.info("    + exclusive algo = "+msg)
        elif parameter=="tau_id.efficiency":
            logging.info("    + id efficiency = "+str(self.efficiency))
        elif parameter=="tau_id.misid_ljet":
            logging.info("    + mis-id efficiency (light quarks) = "+str(self.misid_ljet))
        else:
            logging.error("'clustering' has no parameter called '"+parameter+"'")


    def SampleAnalyzerConfigString(self):
        mydict = {}
        mydict['tau_id.matching_dr'] = str(self.matching_dr)
        mydict['tau_id.efficiency']  = str(self.efficiency)
        if self.exclusive:
            mydict['tau_id.exclusive'] = '1'
        else:
            mydict['tau_id.exclusive'] = '0'
        mydict['tau_id.misid_ljet']  = str(self.misid_ljet)
        return mydict

        
    def user_GetValues(self,variable):
        try:
            return TauIdentification.userVariables[variable]
        except:
            return []

    
    def user_GetParameters(self):
        return TauIdentification.userVariables.keys()


    def user_SetParameter(self,parameter,value):
        # matching deltar
        if parameter=="tau_id.matching_dr":
            try:
                number = float(value)
            except:
                logging.error("the 'matching deltaR' must be a float value.")
                return False
            if number<=0:
                logging.error("the 'matching deltaR' cannot be negative or null.")
                return False
            self.matching_dr=number

        # exclusive
        elif parameter=="tau_id.exclusive":
            if value == "true":
                self.exclusive=True
            elif value == "false":
                self.exclusive=False
            else:
                logging.error("'exclusive' possible values are : 'true', 'false'")
                return False

        # efficiency
        elif parameter=="tau_id.efficiency":
            try:
                number = float(value)
            except:
                logging.error("the efficiency must be a float value.")
                return False
            if number<0:
                logging.error("the efficiency cannot be negative.")
                return False
            if number>1:
                logging.error("the efficiency cannot not greater to 1.")
                return False
            self.efficiency=number

        # mis efficiency (ljet)
        elif parameter=="tau_id.misid_ljet":
            try:
                number = float(value)
            except:
                logging.error("the mis-id efficiency must be a float value.")
                return False
            if number<0:
                logging.error("the mis-id efficiency cannot be negative.")
                return False
            if number>1:
                logging.error("the mis-id efficiency cannot be greater to 1.")
                return False
            self.misid_ljet=number

        # other    
        else:
            logging.error("'clustering' has no parameter called '"+parameter+"'")
