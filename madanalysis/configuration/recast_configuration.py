################################################################################
#  
#  Copyright (C) 2012-2015 Eric Conte, Benjamin Fuks
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
import logging

class RecastConfiguration:

    userVariables = { "status" : ["on","off"] }

    def __init__(self):
        self.status  = "off"
        self.delphes = False
        self.ma5tune = False
        self.pad     = False
        self.padtune = False

    def Display(self):
        self.user_DisplayParameter("status")
        if self.status=="on":
            self.user_DisplayParameter("delphes")
            self.user_DisplayParameter("ma5tune")
            self.user_DisplayParameter("pad")
            self.user_DisplayParameter("padtune")

    def user_DisplayParameter(self,parameter):
        if parameter=="status":
            logging.info(" recasting mode: "+self.status)
            return
        elif parameter=="delphes":
            if self.delphes:
                logging.info("   * analyses based on delphes    : allowed")
            else:
                logging.info("   * analyses based on delphes    : not allowed")
            return
        elif parameter=="ma5tune":
            if self.ma5tune:
                logging.info("   * analyses based on the ma5tune: allowed")
            else:
                logging.info("   * analyses based on the ma5tune: not allowed")
            return
        elif parameter=="pad":
            if self.pad:
                logging.info("   * the PAD is                   : available")
            else:
                logging.info("   * the PAD is                   : not available")
            return
        elif parameter=="padtune":
            if self.padtune:
                logging.info("   * the PADForMa5Tune is         : available")
            else:
                logging.info("   * the PADForMa5Tune is         : not available")
            return

    def user_SetParameter(self,parameter,value,level,hasdelphes,hasMA5Tune,datasets, hasPAD, hasPADTune):
        # algorithm
        if parameter=="status":
            # Switch on the clustering
            if value =="on":
                # Only in reco mode
                if level!=MA5RunningType.RECO:
                    logging.error("recasting is only available in the RECO mode")
                    return

                canrecast=False
                # Delphes and the PAD?
                if hasdelphes:
                    self.delphes=True
                if hasPAD:
                    self.pad=True
                if not hasPAD or not hasdelphes:
                    logging.warning("Delphes and/or the PAD are not installed (or deactivated): " + \
                        "the corresponding analyses will be unavailable")
                else:
                    canrecast=True

                # DelphesMA5Tune and the PADFor MA5TUne?
                if hasMA5Tune:
                    self.ma5tune=True
                if hasPADTune:
                    self.padtune=True
                if not hasPADTune or not hasMA5Tune:
                    logging.warning("DelphesMA5Tune and/or the PADForMA5Tune are not installed " + \
                        "(or deactivated): the corresponding analyses will be unavailable")
                else:
                    canrecast=True

                # can we use the recasting mode
                if canrecast:
                    self.status="on"
                else:
                    logging.error("The recasting modules (PAD/Delphes, PADForMA5Tune/DelphesMa5Tune) " + \
                       "are not available. The recasting mode cannot be activated")
                    return

            elif value =="off":
                test=True
                for dataset in datasets:
                    if not test:
                        break
                    for file in dataset.filenames:
                        if file.endswith('hep') or \
                           file.endswith('hep.gz') or \
                           file.endswith('hepmc') or \
                           file.endswith('hepmc.gz'):
                            test=False
                            break
                if not test:
                    logging.error("some datasets have a hadronic file format. "+\
                                  "The recasting mode cannot be switched off.")
                    return
                self.status="off"
            else:
                logging.error("Recasting can only be set to 'on' or 'off'.")

        # other rejection if no algo specified
        else:
            logging.error("the recast module has no parameter called '"+parameter+"'")
            return

    def user_GetParameters(self):
        table = ["status"]
        return table


    def user_GetValues(self,variable):
        table = []
        if variable=="status":
                table.extend(FastsimConfiguration.userVariables["status"])
        return table


    def CreateCard(self,dirname):
        # getting the PAD analysis
        if self.padtune:
            self.CreateMyCard(dirname,"PADForMA5Tune")
        if self.pad:
            self.CreateMyCard(dirname,"PAD")

    def CreateMyCard(self,dirname,padtype):
        mainfile = open(dirname+"/../"+padtype+"/Build/Main/main.cpp")
        import os
        exist=os.path.isfile(dirname+'/Input/recasting_card.dat')
        card = open(dirname+'/Input/recasting_card.dat','a')
        if not exist:
            card.write('# AnalysisName               PADType    Switch\n')
        if padtype=="PAD":
            mytype="v1.2"
        else:
            mytype="v1.1"
        for line in mainfile:
            if "manager.InitializeAnalyzer" in line:
                analysis = str(line.split('\"')[1])
                card.write(analysis.ljust(30,' ') + mytype.ljust(12,' ') + 'on\n')
        mainfile.close()
        card.close()

    def CheckDir(self):
        logging.error("checkdir to be implemented")
        return False

    def CheckFile(self,dataset):
        logging.error("checkfile to be implemented")
        return False

    def LayoutWriter(self):
        logging.error("layout writer to be implemented")
        return False
