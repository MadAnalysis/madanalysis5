################################################################################
#  
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://github.com/MadAnalysis/madanalysis5>
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
class ClusteringKt():

    default_radius    = 0.4
    default_exclusive = False
    default_ptmin     = 5.
    default_collision = 'pp'

    userVariables = { "radius" : [str(default_radius)],\
                      "exclusive" : [str(default_exclusive)],\
                      "ptmin" : [str(default_ptmin)],\
                      "collision" : [str(default_collision)]}

    def __init__(self):
        self.radius     = ClusteringKt.default_radius
        self.exclusive  = ClusteringKt.default_exclusive
        self.ptmin      = ClusteringKt.default_ptmin
        self.collision  = ClusteringKt.default_collision

        
    def Display(self):
        self.user_DisplayParameter("radius")
        self.user_DisplayParameter("exclusive")
        self.user_DisplayParameter("ptmin")
        self.user_DisplayParameter("collision")


    def user_DisplayParameter(self,parameter):
        if parameter=="radius":
            logging.getLogger('MA5').info("  + cone radius = "+str(self.radius))
        elif parameter=="exclusive":
            msg="false"
            if self.exclusive:
                msg="true"
            logging.getLogger('MA5').info("  + exclusive algo = "+msg)
        elif parameter=="ptmin":
            logging.getLogger('MA5').info("  + PT min (GeV) for produced jets = "+str(self.ptmin))
        elif parameter=="collision":
            logging.getLogger('MA5').info("  + type of collisions described in the events = "+str(self.collision))
        else:
            logging.getLogger('MA5').error("'clustering' has no parameter called '"+parameter+"'")


    def SampleAnalyzerConfigString(self):
        mydict = {}
        mydict['cluster.R']     = str(self.radius)
        mydict['cluster.PTmin'] = str(self.ptmin)
        if self.exclusive:
            mydict['cluster.exclusive'] = '1'
        else:
            mydict['cluster.exclusive'] = '0'
        mydict['cluster.collision'] =str(self.collision)
        return mydict

        
    def user_GetValues(self,variable):
        try:
            return ClusteringKt.userVariables[variable]
        except:
            return []

    
    def user_GetParameters(self):
        return list(ClusteringKt.userVariables.keys())


    def user_SetParameter(self,parameter,value):
        # radius
        if parameter=="radius":
            try:
                number = float(value)
            except:
                logging.getLogger('MA5').error("the cone radius must be a float value.")
                return False
            if number<=0:
                logging.getLogger('MA5').error("the cone radius cannot be negative or null.")
                return False
            self.radius=number

        # exclusive
        elif parameter=="exclusive":
            if value == "true":
                self.exclusive=True
            elif value == "false":
                self.exclusive=False
            else:
                logging.getLogger('MA5').error("'exclusive' possible values are : 'true', 'false'")
                return False

        # ptmin
        elif parameter=="ptmin":
            try:
                number = float(value)
            except:
                logging.getLogger('MA5').error("the ptmin must be a float value.")
                return False
            if number<0:
                logging.getLogger('MA5').error("the ptmin cannot be negative.")
                return False
            self.ptmin=number

        # collision
        elif parameter=="collision":
            if value in['pp','ee']:
                self.collision = value
            else:
                logging.getLogger('MA5').error("the nature of the collisions described in the event file must be pp or ee.")
                return False

        # other    
        else:
            logging.getLogger('MA5').error("'clustering' has no parameter called '"+parameter+"'")
