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
class ClusteringGridJet():

    default_ymax    = 3.
    default_spacing = 0.1
    default_ptmin   = 5.

    userVariables = { "ymax" : [str(default_ymax)],\
                      "spacing" : [str(default_spacing)],\
                      "ptmin" : [str(default_ptmin)] }

    def __init__(self):
        self.ymax    = ClusteringGridJet.default_ymax
        self.spacing = ClusteringGridJet.default_spacing
        self.ptmin   = ClusteringGridJet.default_ptmin

        
    def Display(self):
        self.user_DisplayParameter("ymax")
        self.user_DisplayParameter("spacing")
        self.user_DisplayParameter("ptmin")


    def user_DisplayParameter(self,parameter):
        if parameter=="ymax":
            logging.info("  + ymax = "+str(self.ymax))
        elif parameter=="ptmin":
            logging.info("  + ptmin = "+str(self.ptmin))
        elif parameter=="spacing":
            logging.info("  + requested grid spacing = "+str(self.spacing))
        else:
            logging.error("'clustering' has no parameter called '"+parameter+"'")


    def SampleAnalyzerConfigString(self):
        mydict = {}
        mydict['cluster.Ymax']                 = str(self.ymax)
        mydict['cluster.RequestedGridSpacing'] = str(self.spacing)
        mydict['cluster.Ptmin']                = str(self.ptmin)
        return mydict

        
    def user_GetValues(self,variable):
        try:
            return ClusteringGridJet.userVariables[variable]
        except:
            return []

    
    def user_GetParameters(self):
        return ClusteringGridJet.userVariables.keys()


    def user_SetParameter(self,parameter,value):
        # ymax
        if parameter=="ymax":
            try:
                number = float(value)
            except:
                logging.error("the rapidity maximum must be a float value.")
                return False
            self.radius=number

        # spacing
        elif parameter=="spacing":
            try:
                number = float(value)
            except:
                logging.error("the requested grid spacing must be a float value.")
                return False
            if number<=0:
                logging.error("the requested grid spacing cannot be negative or null.")
                return False
            self.spacing=number

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
