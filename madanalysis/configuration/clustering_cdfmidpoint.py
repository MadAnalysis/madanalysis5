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
class ClusteringCDFMidPoint():

    default_radius       = 1.
    default_overlap      = 0.5
    default_seed         = 1.
    default_areafraction = 1.
    default_ptmin        = 5.

    userVariables = { "radius" : [str(default_radius)],\
                      "overlap" : [str(default_overlap)],\
                      "seed" : [str(default_seed)],\
                      "areafraction" : [str(default_areafraction)],\
                      "ptmin" : [str(default_ptmin)] }

    def __init__(self):
        self.radius       = ClusteringCDFMidPoint.default_radius
        self.ptmin        = ClusteringCDFMidPoint.default_ptmin
        self.overlap      = ClusteringCDFMidPoint.default_overlap
        self.seed         = ClusteringCDFMidPoint.default_seed
        self.areafraction = ClusteringCDFMidPoint.default_areafraction

        
    def Display(self):
        self.user_DisplayParameter("radius")
        self.user_DisplayParameter("overlap")
        self.user_DisplayParameter("seed")
        self.user_DisplayParameter("areafraction")
        self.user_DisplayParameter("ptmin")


    def user_DisplayParameter(self,parameter):
        if parameter=="radius":
            logging.info("  + cone radius = "+str(self.radius))
        elif parameter=="overlap":
            logging.info("  + overlap threshold = "+str(self.overlap))
        elif parameter=="seed":
            logging.info("  + seed threshold = "+str(self.seed))
        elif parameter=="ptmin":
            logging.info("  + PT min (GeV) for produced jets = "+str(self.ptmin))
        elif parameter=="areafraction":
            logging.info("  + cone area fraction = "+str(self.areafraction))
        else:
            logging.error("'clustering' has no parameter called '"+parameter+"'")


    def SampleAnalyzerConfigString(self):
        mydict = {}
        mydict['cluster.R']                = str(self.radius)
        mydict['cluster.PTmin']            = str(self.ptmin)
        mydict['cluster.OverlapThreshold'] = str(self.overlap)
        mydict['cluster.SeedThreshol']     = str(self.seed)
        mydict['cluster.ConeAreaFraction'] = str(self.areafraction)
        return mydict

        
    def user_GetValues(self,variable):
        try:
            return ClusteringCDFMidPoint.userVariables[variable]
        except:
            return []

    
    def user_GetParameters(self):
        return ClusteringCDFMidPoint.userVariables.keys()


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
        elif parameter=="seed":
            try:
                number = float(value)
            except:
                logging.error("the seed threshold  must be a float value.")
                return False
            if number<0:
                logging.error("the seed threshold cannot be negative.")
                return False
            self.seed=number

        # seed
        elif parameter=="areafraction":
            try:
                number = float(value)
            except:
                logging.error("the area fraction must be a float value.")
                return False
            if number<0:
                logging.error("the area fraction cannot be negative.")
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
