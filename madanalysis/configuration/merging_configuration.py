################################################################################
#  
#  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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


from madanalysis.configuration.clustering_kt          import ClusteringKt
from madanalysis.configuration.clustering_antikt      import ClusteringAntiKt
from madanalysis.configuration.clustering_genkt       import ClusteringGenKt
from madanalysis.configuration.clustering_cambridge   import ClusteringCambridge
from madanalysis.configuration.clustering_gridjet     import ClusteringGridJet
from madanalysis.configuration.clustering_cdfmidpoint import ClusteringCDFMidPoint
from madanalysis.configuration.clustering_cdfjetclu   import ClusteringCDFJetClu
from madanalysis.configuration.clustering_siscone     import ClusteringSisCone
from madanalysis.enumeration.ma5_running_type         import MA5RunningType
import logging

class MergingConfiguration:

    userVariables = { "check"    : ["true","false"],\
                      "njets"    : ["4"],\
                      "ma5_mode" : ["true","false"]}

    def __init__(self):
        self.enable = False
        self.njets  = 4
        self.ma5_mode = False
        
    def Display(self):
        self.user_DisplayParameter("check")
        if self.enable:
            self.user_DisplayParameter("njets")
            self.user_DisplayParameter("ma5_mode")

        
    def user_DisplayParameter(self,parameter):
        if parameter=="check":
            if self.enable:
                value="true"
            else:
                value="false"
            logging.getLogger('MA5').info(" enabling merging plots : "+value)
        elif parameter=="njets":
            logging.getLogger('MA5').info("  + njets = "+str(self.njets))
        elif parameter=="ma5_mode":
            if self.ma5_mode:
                value="true"
            else:
                value="false"
            logging.getLogger('MA5').info(" ma5 mode : "+value)
        else:
            logging.getLogger('MA5').error("'merging' has no parameter called '"+parameter+"'")
            return


    def user_SetParameter(self,parameter,value,level,fastjet):
        # enable
        if parameter=="check":
            if value=="true":
                # Only in reco mode
                if level==MA5RunningType.PARTON:
                    logging.getLogger('MA5').error("clustering algorithm is only available in HADRON or RECO mode")
                    return
                
                # Fastjet ?
                if not fastjet:
                    logging.getLogger('MA5').error("fastjet library is not installed. Merging plots not available.")
                    return
                
                self.enable=True
            elif value=="false":
                self.enable=False
            else:
                logging.getLogger('MA5').error("only possible values are 'true' and 'false'")
                return

        # enable
        elif parameter=="ma5_mode":
            if value=="true":
                self.ma5_mode=True
            elif value=="false":
                self.ma5_mode=False
            else:
                logging.getLogger('MA5').error("only possible values are 'true' and 'false'")
                return

        # njets
        elif parameter=="njets":
            try:
                njets = int(value)
            except:
                logging.getLogger('MA5').error("the value for 'njets' must be a positive non-null integer.")
                return
            if njets<=0:
                logging.getLogger('MA5').error("the value for 'njets' cannot be negative or null.")
                return
            self.njets=njets

        # other
        else:
            logging.getLogger('MA5').error("no parameter called '"+parameter+"' is found.")
            return

        
    def user_GetParameters(self):
        return MergingConfiguration.userVariables.keys()


    def user_GetValues(self,variable):
        try:
            return MergingConfiguration.userVariables[variable]
        except:
            return []
    
        
