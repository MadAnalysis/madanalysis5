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
class BeautyIdentification():

    default_matching_dr = 0.5
    default_exclusive   = True
    default_efficiency  = 1.
    default_misid_cjet  = 0.
    default_misid_ljet  = 0.
    

    userVariables = { "bjet_id.matching_dr" : [str(default_matching_dr)],\
                      "bjet_id.exclusive"   : [str(default_exclusive)],\
                      "bjet_id.efficiency"  : [str(default_efficiency)],\
                      "bjet_id.misid_cjet"  : [str(default_misid_cjet)],\
                      "bjet_id.misid_ljet"  : [str(default_misid_ljet)]\
                    }

    def __init__(self):
        self.matching_dr = BeautyIdentification.default_matching_dr
        self.exclusive   = BeautyIdentification.default_exclusive
        self.efficiency  = BeautyIdentification.default_efficiency
        self.misid_cjet  = BeautyIdentification.default_misid_cjet
        self.misid_ljet  = BeautyIdentification.default_misid_ljet

        
    def Display(self):
        logging.info("  + b-jet identification:")
        self.user_DisplayParameter("bjet_id.matching_dr")
        self.user_DisplayParameter("bjet_id.exclusive")
        self.user_DisplayParameter("bjet_id.efficiency")
        self.user_DisplayParameter("bjet_id.misid_cjet")
        self.user_DisplayParameter("bjet_id.misid_ljet")


    def user_DisplayParameter(self,parameter):
        if parameter=="bjet_id.matching_dr":
            logging.info("    + DeltaR matching = "+str(self.matching_dr))
        elif parameter=="bjet_id.exclusive":
            msg="false"
            if self.exclusive:
                msg="true"
            logging.info("    + exclusive algo = "+msg)
        elif parameter=="bjet_id.efficiency":
            logging.info("    + id efficiency = "+str(self.efficiency))
        elif parameter=="bjet_id.misid_cjet":
            logging.info("    + mis-id efficiency (c-quark)      = "+str(self.misid_cjet))
        elif parameter=="bjet_id.misid_ljet":
            logging.info("    + mis-id efficiency (light quarks) = "+str(self.misid_ljet))
        else:
            logging.error("'clustering' has no parameter called '"+parameter+"'")


    def SampleAnalyzerConfigString(self):
        mydict = {}
        mydict['bjet_id.matching_dr'] = str(self.matching_dr)
        mydict['bjet_id.efficiency']  = str(self.efficiency)
        if self.exclusive:
            mydict['bjet_id.exclusive'] = '1'
        else:
            mydict['bjet_id.exclusive'] = '0'
        mydict['bjet_id.misid_cjet']  = str(self.misid_cjet)
        mydict['bjet_id.misid_ljet']  = str(self.misid_ljet)
        return mydict

        
    def user_GetValues(self,variable):
        try:
            return BeautyIdentification.userVariables[variable]
        except:
            return []

    
    def user_GetParameters(self):
        return BeautyIdentification.userVariables.keys()


    def user_SetParameter(self,parameter,value):
        # matching deltar
        if parameter=="bjet_id.matching_dr":
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
        elif parameter=="bjet_id.exclusive":
            if value == "true":
                self.exclusive=True
            elif value == "false":
                self.exclusive=False
            else:
                logging.error("'exclusive' possible values are : 'true', 'false'")
                return False

        # efficiency
        elif parameter=="bjet_id.efficiency":
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

        # efficiency
        elif parameter=="bjet_id.efficiency":
            try:
                number = float(value)
            except:
                logging.error("the efficiency must be a float value.")
                return False
            if number<0:
                logging.error("the efficiency cannot be negative.")
                return False
            if number>1:
                logging.error("the efficiency cannot be greater to 1.")
                return False
            self.efficiency=number

        # mis efficiency (cjet)
        elif parameter=="bjet_id.misid_cjet":
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
            self.misid_cjet=number

        # mis efficiency (ljet)
        elif parameter=="bjet_id.misid_ljet":
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
