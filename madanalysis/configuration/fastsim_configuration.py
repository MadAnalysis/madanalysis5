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


from madanalysis.enumeration.ma5_running_type           import MA5RunningType
from madanalysis.configuration.clustering_configuration import ClusteringConfiguration
from madanalysis.configuration.delphes_configuration    import DelphesConfiguration
from madanalysis.configuration.delphesMA5tune_configuration     import DelphesMA5tuneConfiguration
import logging

class FastsimConfiguration:

    userVariables = { "package" : ["fastjet","delphes","delphesMA5tune","none"] }

    def __init__(self):
        self.clustering = 0
        self.delphes = 0
        self.delphesMA5tune = 0
        self.package    = "none"

        
    def Display(self):
        self.user_DisplayParameter("package")
        if self.package=="fastjet":
            self.clustering.Display()
        elif self.package=="delphes":
            self.delphes.Display()
        elif self.package=="delphesMA5tune":
            self.delphesMA5tune.Display()


    def user_DisplayParameter(self,parameter):
        if parameter=="package":
            logging.info(" fast-simulation package : "+self.package)
            return
        if self.package=="fastjet":
            self.clustering.user_DisplayParameter(parameter)
        elif self.package=="delphes":
            self.delphes.user_DisplayParameter(parameter)
        elif self.package=="delphesMA5tune":
            self.delphesMA5tune.user_DisplayParameter(parameter)


    def SampleAnalyzerConfigString(self):
        if self.package=="fastjet":
            mydict = {}
            mydict.update(self.clustering.SampleAnalyzerConfigString())
            return mydict
        elif self.package=="delphes":
            mydict = {}
            mydict.update(self.delphes.SampleAnalyzerConfigString())
            return mydict
        elif self.package=="delphesMA5tune":
            mydict = {}
            mydict.update(self.delphesMA5tune.SampleAnalyzerConfigString())
            return mydict
        else:
            return {}


    def user_SetParameter(self,parameter,value,datasets,level,fastjet,delphes,delphesMA5tune):

        # algorithm
        if parameter=="package":

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
                    logging.error("some datasets contain partonic/hadronic file format. "+\
                                  "Fast-simulation package cannot be switched off.")
                    return

            # Switch on the clustering
            elif value in ["fastjet","delphes","delphesMA5tune"]:

                # Only in reco mode
                if level!=MA5RunningType.RECO:
                    logging.error("fast-simulation algorithm is only available in RECO mode")
                    return
                
                # Fastjet ?
                if value=='fastjet' and not fastjet:
                    logging.error("fastjet library is not installed. Clustering algorithms are not available.")
                    return

                # Delphes ?
                if value=='delphes' and not delphes:
                    logging.error("delphes library is not installed. This fast-simulation package is not available.")
                    return

                # DelphesMA5tune ?
                if value=='delphesMA5tune' and not delphesMA5tune:
                    logging.error("delphesMA5tune library is not installed. This fast-simulation package is not available.")
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
                    logging.error("some datasets contain reconstructed file format. Fast-simulation cannot be switched on.")
                    return
                 
            if value=="fastjet":
                self.package="fastjet"
                self.clustering = ClusteringConfiguration()
                self.delphes = 0
                self.delphesMA5tune = 0
            elif value=="delphes":
                self.package="delphes"
                self.clustering = 0
                self.delphes = DelphesConfiguration()
                self.delphesMA5tune = 0
            elif value=="delphesMA5tune":
                self.package="delphesMA5tune"
                self.clustering = 0
                self.delphes = 0
                self.delphesMA5tune = DelphesMA5tuneConfiguration()
            elif value=="none":
                self.package="none"
                self.clustering = 0
                self.delphes = 0
                self.delphesMA5tune = 0
            else:
                logging.error("parameter called '"+value+"' is not found.")
            return    

        # other rejection if no algo specified
        if self.package=="none":
            logging.error("'fastsim' has no parameter called '"+parameter+"'")
            return

        # other
        if self.package=="fastjet":
            return self.clustering.user_SetParameter(parameter,value,datasets,level)
        elif self.package=="delphes":
            return self.delphes.user_SetParameter(parameter,value,datasets,level)
        elif self.package=="delphesMA5tune":
            return self.delphesMA5tune.user_SetParameter(parameter,value,datasets,level)

        
    def user_GetParameters(self):
        if self.package=="fastjet":
            table = FastsimConfiguration.userVariables.keys()
            table.extend(self.clustering.user_GetParameters())
        elif self.package=="delphes":
            table = FastsimConfiguration.userVariables.keys()
            table.extend(self.delphes.user_GetParameters())
        elif self.package=="delphesMA5tune":
            table = FastsimConfiguration.userVariables.keys()
            table.extend(self.delphesMA5tune.user_GetParameters())
        else:
            table = ["package"]
        return table


    def user_GetValues(self,variable):
        table = []
        if self.package=="fastjet":
            try:
                table.extend(FastsimConfiguration.userVariables[variable])
            except:
                pass
            try:
                table.extend(self.clustering.user_GetValues(variable))
            except:
                pass
        elif self.package=="delphes":
            try:
                table.extend(FastsimConfiguration.userVariables[variable])
            except:
                pass
            try:
                table.extend(self.delphes.user_GetValues(variable))
            except:
                pass
        elif self.package=="delphesMA5tune":
            try:
                table.extend(FastsimConfiguration.userVariables[variable])
            except:
                pass
            try:
                table.extend(self.delphesMA5tune.user_GetValues(variable))
            except:
                pass
        else:
            if variable=="package":
                table.extend(FastsimConfiguration.userVariables["package"])
        return table
        

