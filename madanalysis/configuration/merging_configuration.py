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

    userVariables = { "check" : ["true","false"],\
                      "njets" : ["4"] }

    def __init__(self):
        self.enable = False
        self.njets  = 4

        
    def Display(self):
        self.user_DisplayParameter("check")
        if self.enable:
            self.user_DisplayParameter("njets")

        
    def user_DisplayParameter(self,parameter):
        if parameter=="check":
            if self.enable:
                value="true"
            else:
                value="false"
            logging.info(" enabling merging plots : "+value)
        elif parameter=="njets":
            logging.info("  + njets = "+str(self.njets))
        else:
            logging.error("'merging' has no parameter called '"+parameter+"'")
            return


    def user_SetParameter(self,parameter,value,level,fastjet):
        # enable
        if parameter=="check":
            if value=="true":
                # Only in reco mode
                if level==MA5RunningType.PARTON:
                    logging.error("clustering algorithm is only available in HADRON or RECO mode")
                    return
                
                # Fastjet ?
                if not fastjet:
                    logging.error("fastjet library is not installed. Merging plots not available.")
                    return
                
                self.enable=True
            elif value=="false":
                self.enable=False
            else:
                logging.error("only possible values are 'true' and 'false'")
                return

        # njets
        elif parameter=="njets":
            try:
                njets = int(value)
            except:
                logging.error("the value for 'njets' must be a positive non-null integer.")
                return
            if njets<=0:
                logging.error("the value for 'njets' cannot be negative or null.")
                return
            self.njets=njets

        # other
        else:
            logging.error("no parameter called '"+parameter+"' is found.")
            return

        
    def user_GetParameters(self):
        return MergingConfiguration.userVariables.keys()


    def user_GetValues(self,variable):
        try:
            return MergingConfiguration.userVariables[variable]
        except:
            return []
    
        
