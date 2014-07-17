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
class ClusteringSisCone():
    
    default_radius       = 1.
    default_overlap      = 0.5
    default_npassmax     = 0
    default_input_ptmin  = 0.
    default_ptmin        = 5.

    userVariables = { "radius"      : [str(default_radius)],\
                      "overlap"     : [str(default_overlap)],\
                      "npassmax"    : [str(default_npassmax)],\
                      "input_ptmin" : [str(default_input_ptmin)],\
                      "ptmin"       : [str(default_ptmin)] }

    def __init__(self):
        self.radius       = ClusteringSisCone.default_radius
        self.ptmin        = ClusteringSisCone.default_ptmin
        self.overlap      = ClusteringSisCone.default_overlap
        self.npassmax     = ClusteringSisCone.default_npassmax
        self.input_ptmin  = ClusteringSisCone.default_input_ptmin

        
    def Display(self):
        self.user_DisplayParameter("radius")
        self.user_DisplayParameter("overlap")
        self.user_DisplayParameter("npassmax")
        self.user_DisplayParameter("input_ptmin")
        self.user_DisplayParameter("ptmin")


    def user_DisplayParameter(self,parameter):
        if parameter=="radius":
            logging.info("  + cone radius = "+str(self.radius))
        elif parameter=="overlap":
            logging.info("  + overlap threshold = "+str(self.overlap))
        elif parameter=="npassmax":
            logging.info("  + number max of pass = "+str(self.npassmax))
        elif parameter=="ptmin":
            logging.info("  + PT min (GeV) for produced jets = "+str(self.ptmin))
        elif parameter=="input_ptmin":
            logging.info("  + PT min (GeV) for input particles = "+str(self.input_ptmin))
        else:
            logging.error("'clustering' has no parameter called '"+parameter+"'")


    def SampleAnalyzerConfigString(self):
        mydict = {}
        mydict['cluster.R']                = str(self.radius)
        mydict['cluster.PTmin']            = str(self.ptmin)
        mydict['cluster.OverlapThreshold'] = str(self.overlap)
        mydict['cluster.NPassMax']         = str(self.npassmax)
        mydict['cluster.Protojet_ptmin']   = str(self.input_ptmin)
        return mydict

        
    def user_GetValues(self,variable):
        try:
            return ClusteringSisCone.userVariables[variable]
        except:
            return []

    
    def user_GetParameters(self):
        return ClusteringSisCone.userVariables.keys()


    def user_SetParameter(self,parameter,value):
        # radius
        if parameter=="radius":
            try:
                number = float(value)
            except:
                logging.error("the cone radius must be a float value.")
                return False
            if number<=0:
                logging.error("the cone radius cannot be negative or null.")
                return False
            self.radius=number

        # overlap
        elif parameter=="overlap":
            try:
                number = float(value)
            except:
                logging.error("the overlap threshold must be a float value.")
                return False
            if number<0:
                logging.error("the overlap threshold cannot be negative.")
                return False
            self.overlap=number

        # seed
        elif parameter=="npassmax":
            try:
                number = int(value)
            except:
                logging.error("the n max of pass  must be an integer value.")
                return False
            if number<0:
                logging.error("the n max of pass cannot be negative.")
                return False
            self.npassmax=number

        # input_ptmin
        elif parameter=="input_ptmin":
            try:
                number = float(value)
            except:
                logging.error("the input PT min must be a float value.")
                return False
            if number<0:
                logging.error("the input PT min cannot be negative.")
                return False
            self.areafraction=number

        # ptmin
        elif parameter=="ptmin":
            try:
                number = float(value)
            except:
                logging.error("the ptmin must be a float value.")
                return False
            if number<0:
                logging.error("the ptmin cannot be negative.")
                return False
            self.ptmin=number

        # other    
        else:
            logging.error("'clustering' has no parameter called '"+parameter+"'")
