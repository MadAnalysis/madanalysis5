################################################################################
#  
#  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <http://madanalysis.irmp.ucl.ac.be>
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
from madanalysis.enumeration.ma5_running_type           import MA5RunningType
import logging

class ClusteringConfiguration:

    userVariables = { "algorithm" : ["kt","antikt","genkt",\
                                     "cambridge","gridjet",\
                                     "siscone",\
                                     "cdfjetclu", "cdfmidpoint",\
                                     "none"] }

    def __init__(self):
        self.clustering = 0
        self.algorithm="none"

        
    def Display(self):
        self.user_DisplayParameter("algorithm")
        if self.algorithm!="none":
            self.clustering.Display()

        
    def user_DisplayParameter(self,parameter):
        if parameter=="algorithm":
            logging.info(" clustering algorithm : "+self.algorithm)
        else:
            if self.algorithm!="none":
                self.clustering.user_DisplayParameter(parameter)


    def SampleAnalyzerConfigString(self):
        if self.algorithm!="none":
            return self.clustering.SampleAnalyzerConfigString()
        else:
            return {}


    def user_SetParameter(self,parameter,value,datasets,level,fastjet):
        # algorithm
        if parameter=="algorithm":

            # Switch off the clustering
            if value=="none":
                test=True
                for dataset in datasets:
                    if not test:
                        break
                    for file in dataset.filenames:
                        if file.endswith('lhe') or \
                           file.endswith('lhe.gz') or \
                           file.endswith('hep') or \
                           file.endswith('hep.gz') or \
                           file.endswith('hepmc') or \
                           file.endswith('hepmc.gz'):
                            test=False
                            break
                if not test:
                    logging.error("some datasets contain partonic/hadronic file format. Clustering algorithm cannot be switched off.")
                    return

            # Switch on the clustering
            elif value in ["kt","antikt","cambridge","genkt",\
                           "gridjet","cdfmidpoint","cdfjetclu",\
                           "siscone"]:

                # Only in reco mode
                if level!=MA5RunningType.RECO:
                    logging.error("clustering algorithm is only available in RECO mode")
                    return
                
                # Fastjet ?
                if not fastjet:
                    logging.error("fastjet library is not installed. Clustering algorithms are not available.")
                    return

                test=True
                for dataset in datasets:
                    if not test:
                        break
                    for file in dataset.filenames:
                        if file.endswith('lhco') or \
                           file.endswith('lhco.gz') or \
                           file.endswith('root'):
                            test=False
                            break
                if not test:
                    logging.error("some datasets contain reconstructed file format. Clustering algorithm cannot be switched on.")
                    return
                 
            if value=="kt":
                self.algorithm="kt"
                self.clustering = ClusteringKt()
            elif value=="antikt":
                self.algorithm="antikt"
                self.clustering = ClusteringAntiKt()
            elif value=="cambridge":
                self.algorithm="cambridge"
                self.clustering = ClusteringCambridge()
            elif value=="genkt":
                self.algorithm="genkt"
                self.clustering = ClusteringGenKt()
            elif value=="gridjet":
                self.algorithm="gridjet"
                self.clustering = ClusteringGridJet()
            elif value=="cdfmidpoint":
                self.algorithm="cdfmidpoint"
                self.clustering = ClusteringCDFMidPoint()
            elif value=="cdfjetclu":
                self.algorithm="cdfjetclu"
                self.clustering = ClusteringCDFJetClu()
            elif value=="siscone":
                self.algorithm="siscone"
                self.clustering = ClusteringSisCone()
            elif value=="none":
                self.algorithm="none"
                self.clustering = 0
            else:
                logging.error("algorithm called '"+value+"' is not found.")
            return    

        # other
        if self.algorithm!="none":
            return self.clustering.user_SetParameter(parameter,value)

        
    def user_GetParameters(self):
        table = ClusteringConfiguration.userVariables.keys()
        if self.algorithm!="none":
            table.extend(self.clustering.user_GetParameters())
        return table


    def user_GetValues(self,variable):
        table = []
        try:
            table.extend(ClusteringConfiguration.userVariables[variable])
        except:
            pass
        if self.algorithm!="none":
            try:
                table.extend(self.clustering.user_GetValues(variable))
            except:
                pass
        return table
        
