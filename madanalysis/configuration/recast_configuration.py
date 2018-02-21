################################################################################
#  
#  Copyright (C) 2012-2016 Eric Conte, Benjamin Fuks
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


from madanalysis.enumeration.ma5_running_type   import MA5RunningType
from madanalysis.IOinterface.folder_writer      import FolderWriter
from shell_command import ShellCommand
import logging
import shutil
import os

class RecastConfiguration:

    default_CLs_numofexps = 100000

    userVariables ={
         "status"        : ["on","off"],\
         "CLs_numofexps" : [str(default_CLs_numofexps)],\
         "card_path"     : "",\
         "store_root"    : ["True", "False"]
    }

    def __init__(self):
        self.status     = "off"
        self.delphes    = False
        self.ma5tune    = False
        self.pad        = False
        self.padtune    = False
        self.store_root = False
        self.DelphesDic = {
          "delphes_card_cms_standard.tcl"         : ["cms_sus_14_001_monojet", "cms_sus_13_016", "cms_sus_13_012", "cms_sus_13_011"],
          "delphes_card_cms_sus14004.tcl"         : ["cms_sus_14_001_TopTag"],
          "delphes_card_atlas_sus_2013_05.tcl"    : ["atlas_susy_2013_21", "atlas_sus_13_05"],
          "delphes_card_atlas_sus_2013_05_pad.tcl": ["ATLAS_EXOT_2014_06"],
          "delphes_card_atlas_sus_2013_11.tcl"    : ["atlas_higg_2013_03", "atlas_susy_2013_11", "atlas_1405_7875"],
          "delphes_card_atlas_sus_2014_10.tcl"    : ["atlas_susy_2014_10"] ,
          "delphes_card_atlas_sus_2013_04.tcl"    : ["atlas_susy_2013_04"] ,
          "delphes_card_cms_b2g_12_012.tcl"       : ["CMS_B2G_12_012", "cms_exo_12_047", "cms_exo_12_048"],
          "delphes_card_cms_b2g_14_004.tcl"       : ["cms_b2g_12_022", "cms_b2g_14_004"],
          "delphes_card_ATLAS_1604_07773.tcl"     : ["ATLAS_1604_07773", "atlas_1605_03814"],
          "delphes_card_ATLAS_1711_03301.tcl"     : ["ATLAS_1711_03301"],
          "delphes_card_ATLAS_CONF_2016_086.tcl"  : ["ATLAS_CONF_2016_086"],
          "delphes_card_CMS_EXO_16_037.tcl"       : ["CMS_EXO_16_037"],
          "delphes_card_cms_exo_16_010.tcl"       : ["cms_exo_16_010"],
          "delphes_card_cms_exo_16_012.tcl"       : ["CMS_EXO_16_012_2gamma"],
          "delphes_card_cms_SUS_16_052.tcl"       : ["CMS_SUS_16_052"],
          "delphes_card_atlas_2016_32.tcl"        : ["ATLAS_EXOT_2016_32"]
        }

        self.description = {
          "ATLAS_1604_07773"       : "ATLAS - 13 TeV - monojet (3.2 fb-1)",
          "ATLAS_1711_03301"       : "ATLAS - 13 TeV - monojet (36.1 fb-1)",
          "ATLAS_EXOT_2016_32"     : "ATLAS - 13 TeV - monophoton (36.1 fb-1)",
          "atlas_1605_03814"       : "ATLAS - 13 TeV - multijet (2-6 jets) + met",
          "ATLAS_CONF_2016_086"    : "ATLAS - 13 TeV - Dark matter in the b+bar+met channel",
          "cms_exo_16_010"         : "CMS   - 13 TeV - Mono-Z",
          "CMS_EXO_16_012_2gamma"  : "CMS   - 13 TeV - Mono-Higgs with H in digamma (2.3 fb-1)",
          "CMS_EXO_16_037"         : "CMS   - 13 TeV - Monojet",
          "CMS_SUS_16_052"         : "CMS   - 13 TeV - SUSY 1 lepton + jets (36 fb-1)",
          "atlas_susy_2013_04"     : "ATLAS - 8 TeV - multijet + met", 
          "atlas_sus_13_05"        : "ATLAS - 8 TeV - stop/sbottom - 0 lepton + 2 bjets + met",
          "atlas_susy_2013_11"     : "ATLAS - 8 TeV - ewkinos - 2 leptons + met",
          "atlas_susy_2013_21"     : "ATLAS - 8 TeV - monojet",
          "atlas_susy_2014_10"     : "ATLAS - 8 TeV - squark-gluino - 2 leptons + jets + met",
          "atlas_1405_7875"        : "ATLAS - 8 TeV - squark-gluino - 0 leptons + 2-6 jets + met",
          "atlas_higg_2013_03"     : "ATLAS - 8 TeV - ZH to invisible + 2 leptons",
          "ATLAS_EXOT_2014_06"     : "ATLAS - 8 TeV - monophoton",
          "cms_sus_13_012"         : "CMS   - 8 TeV - squark-gluino - MET/MHT",
          "cms_sus_13_016"         : "CMS   - 8 TeV - gluinos - 2 leptons + bjets + met",
          "cms_sus_14_001_monojet" : "CMS   - 8 TeV - stop - the monojet channel",
          "cms_sus_14_001_TopTag"  : "CMS   - 8 TeV - stop - the top tagging channel",
          "cms_sus_13_011"         : "CMS   - 8 TeV - stop - 1 lepton + bjets + met",
          "cms_exo_12_047"         : "CMS   - 8 TeV - monophoton",
          "cms_exo_12_048"         : "CMS   - 8 TeV - monojet",
          "CMS_B2G_12_012"         : "CMS   - 8 TeV - T5/3 partners in the SSDL channel",
          "cms_b2g_12_022"         : "CMS   - 8 TeV - Monotop search",
          "cms_b2g_14_004"         : "CMS   - 8 TeV - Dark matter production with a ttbar pair"
        }

        self.CLs_numofexps= 100000
        self.card_path= ""
        self.logger = logging.getLogger('MA5')

    def Display(self):
        self.user_DisplayParameter("status")
        if self.status=="on":
            self.user_DisplayParameter("delphes")
            self.user_DisplayParameter("ma5tune")
            self.user_DisplayParameter("pad")
            self.user_DisplayParameter("padtune")
            self.user_DisplayParameter("CLs_numofexps")
            self.user_DisplayParameter("card_path")
            self.user_DisplayParameter("store_root")

    def user_DisplayParameter(self,parameter):
        if parameter=="status":
            self.logger.info(" recasting mode: "+self.status)
            return
        elif parameter=="delphes":
            if self.delphes:
                self.logger.info("   * analyses based on delphes    : allowed")
            else:
                self.logger.info("   * analyses based on delphes    : not allowed")
            return
        elif parameter=="ma5tune":
            if self.ma5tune:
                self.logger.info("   * analyses based on the ma5tune: allowed")
            else:
                self.logger.info("   * analyses based on the ma5tune: not allowed")
            return
        elif parameter=="pad":
            if self.pad:
                self.logger.info("   * the PAD is                   : available")
            else:
                self.logger.info("   * the PAD is                   : not available")
            return
        elif parameter=="padtune":
            if self.padtune:
                self.logger.info("   * the PADForMa5tune is         : available")
            else:
                self.logger.info("   * the PADForMa5tune is         : not available")
            return
        elif parameter=="CLs_numofexps":
            self.logger.info("   * Number of toy experiments for the CLs calculation: "+str(self.CLs_numofexps))
            return
        elif parameter=="card_path":
            self.logger.info("   * Path to a recasting card: "+str(self.card_path))
            return
        elif parameter=="store_root":
            self.logger.info("   * Keeping the root files: "+str(self.store_root))
            return
        return

    def user_SetParameter(self,parameter,value,level,hasroot,hasdelphes,hasMA5tune,datasets, hasPAD, hasPADtune):
        # algorithm
        if parameter=="status":
            # Switch on the clustering
            if value =="on":

                # Only in reco mode
                if level!=MA5RunningType.RECO:
                    self.logger.error("recasting is only available in the RECO mode")
                    return

                # Only if ROOT is install
                if not hasroot:
                    self.logger.error("recasting is only available if ROOT is installed")
                    return

                canrecast=False
                # Delphes and the PAD?
                if hasdelphes:
                    self.delphes=True
                if hasPAD:
                    self.pad=True
                if not hasPAD or not hasdelphes:
                    self.logger.warning("Delphes and/or the PAD are not installed (or deactivated): " + \
                        "the corresponding analyses will be unavailable")
                else:
                    canrecast=True

                # DelphesMA5tune and the PADFor MA5TUne?
                if hasMA5tune:
                    self.ma5tune=True
                if hasPADtune:
                    self.padtune=True
                if not hasPADtune or not hasMA5tune:
                    self.logger.warning("DelphesMA5tune and/or the PADForMA5tune are not installed " + \
                        "(or deactivated): the corresponding analyses will be unavailable")
                else:
                    canrecast=True

                # can we use the recasting mode
                if canrecast:
                    self.status="on"
                else:
                    self.logger.error("The recasting modules (PAD/Delphes, PADForMA5tune/DelphesMa5tune) " + \
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
                    self.logger.error("some datasets have a hadronic file format. "+\
                                  "The recasting mode cannot be switched off.")
                    return
                self.status="off"
            else:
                self.logger.error("Recasting can only be set to 'on' or 'off'.")

        # CLs module
        elif parameter=="CLs_numofexps":
            if self.status!="on":
                self.logger.error("Please first set the recasting mode to 'on'.")
                return
            self.CLs_numofexps = int(value)

        # path to a recasting card
        elif parameter=="card_path":
            if self.status!="on":
                self.logger.error("Please first set the recasting mode to 'on'.")
                return
            import os
            if os.path.isfile(value):
                self.card_path = value
            else:
                self.logger.error("Invalid path to a recasting card.")
                return

        # Keeping the root files
        elif parameter=="store_root":
            if self.status!="on":
                self.logger.error("Please first set the recasting mode to 'on'.")
                return
            if value == 'True':
                self.store_root=True
            elif value == 'False':
                self.store_root=False
            else:
                self.logger.error("Do the root files need to be stored? (True/False)")
                return

        # other rejection if no algo specified
        else:
            self.logger.error("the recast module has no parameter called '"+parameter+"'")
            return

    def user_GetParameters(self):
        if self.status=="on":
            table = ["CLs_numofexps", "card_path", "store_root"]
        else:
           table = []
        return table


    def user_GetValues(self,variable):
        table = []
        if variable=="status":
                table.extend(RecastConfiguration.userVariables["status"])
        elif variable =="CLs_numofexps":
                table.extend(RecastConfiguration.userVariables["CLs_numofexps"])
        elif variable =="card_path":
                table.extend(RecastConfiguration.userVariables["card_path"])
        elif variable =="store_root":
                table.extend(RecastConfiguration.userVariables["store_root"])
        return table


    def CreateCard(self,dirname,write=True):
        # using an existing card
        if self.card_path=="":
            if self.padtune and self.ma5tune:
                self.CreateMyCard(dirname,"PADForMA5tune",write)
            if self.pad and self.delphes:
                self.CreateMyCard(dirname,"PAD",write)
            return True
        #using and checking an existing card
        else:
            if not os.path.isfile(self.card_path):
                self.logger.error("Invalid path to a recasting card.")
                return False
            if not self.CheckCard(dirname):
                self.logger.error("Invalid recasting card")
                return False
        return True

    def CheckCard(self,dirname):
        self.logger.info('   Checking the recasting card...')
        ToLoopOver=[]
        padlist=[]
        tunelist=[]
        ma5dir = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath( __file__ )),os.pardir,os.pardir))
        if self.pad:
            padfile  = open(os.path.normpath(os.path.join(ma5dir,"PAD/Build/Main/main.cpp")), 'r')
            ToLoopOver.append([padfile, padlist])
        if self.padtune:
            tunefile = open(os.path.normpath(os.path.join(ma5dir,"PADForMA5tune/Build/Main/main.cpp")), 'r')
            ToLoopOver.append([tunefile, tunelist])
        for myfile,mylist in ToLoopOver:
            for line in myfile:
                if "manager.InitializeAnalyzer" in line:
                    analysis = str(line.split('\"')[1])
                    mydelphes="UNKNOWN"
                    for mycard,alist in self.DelphesDic.items():
                          if analysis in alist:
                              mydelphes=mycard
                              break
                    mylist.append([analysis,mydelphes])
        if self.pad:
            padfile.close()
        if self.padtune:
            tunefile.close()
        usercard = open(self.card_path)
        for line in usercard:
            if len(line.strip())==0:
                continue
            if line.lstrip()[0]=='#':
                continue
            myline=line.split()
            myana = myline[0]
            myver = myline[1]
            mydelphes = myline[3]
            # checking the presence of the analysis and the delphes card
            if myana in  [x[0] for x in padlist]:
                if myver!="v1.2":
                    self.logger.error("Recasting card: invalid analysis (not present in the PAD): " + myana)
                    return False
                if not os.path.isfile(os.path.normpath(os.path.join(ma5dir,'PAD/Input/Cards',mydelphes))):
                    self.logger.error("Recasting card: PAD analysis linked to an invalid delphes card: " + myana + " - " + mydelphes)
                    return False
            elif myana in  [x[0] for x in tunelist]:
                if myver!="v1.1":
                    self.logger.error("Recasting card: invalid analysis (not present in the PADForMA5tune): " + myana)
                    return False
                if not os.path.isfile(os.path.normpath(os.path.join(ma5dir,'PADForMA5tune/Input/Cards',mydelphes))):
                    self.logger.error("Recasting card: PADForMA5tune analysis linked to an invalid delphes card: " + myana + " - " + mydelphes)
                    return False
            else:
                self.logger.error("Recasting card: invalid analysis (not present in the PAD and in the PADForMA5tune): " + myana)
                return False
            # checking the matching between the delphes card and the analysis
            for mycard,alist in self.DelphesDic.items():
                if myana in alist:
                    if mydelphes!=mycard:
                        self.logger.error("Invalid delphes card associated with the analysis: " + myana)
                        return False
                    break
        usercard.close()
        try:
            shutil.copy(self.card_path,dirname+'/Input/recasting_card.dat')
        except:
            self.logger.error('impossible to copy the recasting card to the working directory')
            return False
        return True


    def CreateMyCard(self,dirname,padtype,write=True):
        ma5dir = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath( __file__ )),os.pardir,os.pardir))
        mainfile  = open(os.path.normpath(os.path.join(ma5dir,padtype,"Build/Main/main.cpp")), 'r')
        thecard=[]
        if write:
            exist=os.path.isfile(dirname+'/Input/recasting_card.dat')
            if not exist and write:
                thecard.append('# Delphes cards must be located in the PAD(ForMA5tune) directory')
                thecard.append('# Switches must be on or off')
                thecard.append('# AnalysisName               PADType    Switch     DelphesCard')
        if padtype=="PAD":
            mytype="v1.2"
        else:
            mytype="v1.1"
        for line in mainfile:
            if "manager.InitializeAnalyzer" in line:
                analysis = str(line.split('\"')[1])
                mydelphes="UNKNOWN"
                descr="UNKNOWN"
                for mycard,alist in self.DelphesDic.items():
                      if analysis in alist:
                          mydelphes=mycard
                          break
                for myana,mydesc in self.description.items():
                      if analysis == myana:
                          descr=mydesc
                          break
                thecard.append(analysis.ljust(30,' ') + mytype.ljust(12,' ') + 'on    ' + mydelphes.ljust(50, ' ')+\
                      ' # '+descr)
        mainfile.close()
        thecard.append('')
        if write:
            card = open(dirname+'/Input/recasting_card.dat','a')
            card.write('\n'.join(thecard))
            card.close()
        else:
            return thecard

    def CheckFile(self,dirname,dataset):
        filename=os.path.normpath(dirname+'/Output/'+dataset.name+'/CLs_output.dat')
        self.logger.debug('Check file "'+filename+'"...')
        if not os.path.isfile(filename):
            self.logger.error("The file '"+dirname+'/Output/'+dataset.name+'/CLs_output.dat" has not been found.')
            return False
        return True

    def collect_outputs(self,dirname,datasets):
        filename=os.path.normpath(os.path.join(dirname,'Output/CLs_output_summary.dat'))
        self.logger.debug('Check summary file "'+filename+'"...')
        out = open(filename,'w')
        counter=1
        for item in datasets:
            outset=open(os.path.normpath(os.path.join(dirname,'Output',item.name,'CLs_output.dat')))
            for line in outset:
                if counter==1 and '# analysis name' in line:
                    out.write('# dataset name'.ljust(30) + line[2:])
                    counter+=1
                if len(line.lstrip())==0:
                   continue
                if line.lstrip()[0]=='#':
                   continue
                out.write(item.name.ljust(30)+line)
            outset.close()
            out.write('\n')
        out.close()
