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


from madanalysis.enumeration.ma5_running_type   import MA5RunningType
from madanalysis.IOinterface.folder_writer      import FolderWriter
from shell_command import ShellCommand
import logging
import shutil
import os

class RecastConfiguration:

    default_CLs_numofexps = 100000

    userVariables ={
         "status"              : ["on","off"],\
         "CLs_numofexps"       : [str(default_CLs_numofexps)],\
         "card_path"           : "",\
         "store_root"          : ["True", "False"] , \
         "THerror_combination" : ["quadratic","linear"], \
         "error_extrapolation" : ["linear", "sqrt"]
    }

    def __init__(self):
        self.status                     = "off"
        self.delphes                    = False
        self.ma5tune                    = False
        self.pad                        = False
        self.padtune                    = False
        self.store_root                 = False
        self.systematics                = []
        self.extrapolated_luminosities  = []
        self.THerror_combination        = "linear"
        self.error_extrapolation        = "linear"
        self.DelphesDic                 = { }
        self.description                = { }
        self.ma5dir = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath( __file__ )),os.pardir,os.pardir))
        for mypad in ['PAD', 'PADForMa5tune']:
            if os.path.isfile(os.path.join(self.ma5dir,'tools',mypad,'Input','recast_config.dat')):
                dico_file = open(os.path.join(self.ma5dir,'tools',mypad,'Input','recast_config.dat'), 'r')
                for line in dico_file:
                    if line.strip().startswith('#'):
                        continue
                    self.DelphesDic[line.split('|')[0].strip()] = line.split('|')[1].split()
                dico_file.close()
            if os.path.isfile(os.path.join(self.ma5dir,'tools',mypad,'Input','analysis_description.dat')):
                dico_file = open(os.path.join(self.ma5dir,'tools',mypad,'Input','analysis_description.dat'), 'r')
                for line in dico_file:
                    if line.strip().startswith('#'):
                        continue
                    self.description[line.split('|')[0].strip()] = line.split('|')[1][:-1]
                dico_file.close()

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
            self.user_DisplayParameter("extrapolated_luminosities")
            self.user_DisplayParameter("systematics")
            self.user_DisplayParameter("THerror_combination")
            self.user_DisplayParameter("error_extrapolation")

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
        elif parameter=="systematics":
            if len(self.systematics) > 0:
                for i in range(0,len(self.systematics)):
                    up, dn = self.systematics[i]
                    self.logger.info("   * Systematics "+str(i+1)+": [+{:.1%}, -{:.1%}]".format(up,dn))
            return
        elif parameter=="extrapolated_luminosity":
            if len(self.extrapolated_luminosities) > 0:
                tmp = ["{:.1f}".format(x)+" fb^{-1}" for x in self.extrapolated_lumi]
                self.logger.info("   * Results extrapolated for the luminosities: "+', '.join(tmp))
            return
        elif parameter=="THerror_combination":
            self.logger.info("   * Theory errors (if provided) are combined in a " + self.THerror_combination + " way")
            return
        elif parameter=="error_extrapolation":
            self.logger.info("   * Errors on the background will be extrapolated " + self.error_extrapolation + "ly (if necessary)")
            return
        return

    def user_SetParameter(self,parameters,values,level,archi_info,session_info,datasets):
        # Make sure that previous features are unchanged:  the 'add' keyword is properly dealt with
        if isinstance(parameters, list):
            parameter = parameters[0]
            value = values[0]
        else:
            parameter=parameters
            value=values
        # algorithm
        if parameter=="status":
            # Switch on the clustering
            if value =="on":

                # Only in reco mode
                if level!=MA5RunningType.RECO:
                    self.logger.error("recasting is only available in the RECO mode")
                    return

                # Only if ROOT is install
                if not archi_info.has_root:
                    self.logger.error("recasting is only available if ROOT is installed")
                    return

                canrecast=False
                # Delphes and the PAD?
                if archi_info.has_delphes:
                    self.delphes=True
                if session_info.has_pad:
                    self.pad=True
                if not archi_info.has_delphes or not session_info.has_pad:
                    self.logger.warning("Delphes and/or the PAD are not installed (or deactivated): " + \
                        "the corresponding analyses will be unavailable")
                else:
                    canrecast=True

                # DelphesMA5tune and the PADFor MA5TUne?
                if archi_info.has_delphesMA5tune:
                    self.ma5tune=True
                if session_info.has_padma5:
                    self.padtune=True
                if not archi_info.has_delphesMA5tune or not session_info.has_padma5:
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

        # Systematic uncertainties and Luminosity extrapolation
        elif parameter=="add":
            if self.status!="on":
                self.logger.error("Please first set the recasting mode to 'on'.")
                return
            ## Checking the values
            try:
                vals = [float(x) for x in values if x!=',']
            except:
                self.logger.error("Values for the systematic uncertainties and extrapolated luminosities should be real")
                return
            ## Systematics
            if len(parameters)>1 and parameters[1]=='systematics':
                if len(vals) == 1 and vals[0] >= 0. and vals[0] <= 1.:
                    self.systematics.append((vals[0],vals[0]))
                elif len(vals) == 2 and vals[0]>=0. and vals[0]<=1. and vals[1]>= 0. and vals[1]<= 1.:
                    self.systematics.append((vals[0],vals[1]))
                else:
                    self.logger.error("Invalid syntax for adding systematics uncertainties.")
                    return
            ## Extrapolated lumis
            elif len(parameters)>1 and  parameters[1]=='extrapolated_luminosity':
                if len(vals) >= 1:
                    self.extrapolated_luminosities += vals
                else:
                    self.logger.error("Invalid syntax for adding extrapolated luminosities.")
                    return
            ## protection
            else:
                self.logger.error("Invalid syntax with the \'add\' keyword")
                return

        # Error combination
        elif parameter=="THerror_combination":
            if self.status!="on":
                self.logger.error("Please first set the recasting mode to 'on'.")
                return
            if value in ["quadratic","linear"]:
                self.THerror_combination = value
            else:
                self.logger.error("Theoretical uncertainties can only be combined")
                self.logger.error("linearly [linear] or quadratically [quadratic].")
                return

        # Error extrapolation
        elif parameter=="error_extrapolation":
            if self.status!="on":
                self.logger.error("Please first set the recasting mode to 'on'.")
                return
            if value in ["linear", "sqrt"]:
                self.error_extrapolation = value
            else:
                self.logger.error("When extrapolating to different luminosities, uncertainties")
                self.logger.error("can only be extrapolated linearly [linear] or sqrtly [sqrt].")
                return

        # other rejection if no algo specified
        else:
            self.logger.error("the recast module has no parameter called '"+str(parameter)+"'")
            return

    def user_GetParameters(self,var=''):
        if self.status=="on":
            if var == "add":
                table = ["extrapolated_luminosity", "systematics"]
            else:
                table = ["CLs_numofexps", "card_path", "store_root", "add", "THerror_combination", "error_extrapolation"]
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
        elif variable =="THerror_combination":
                table.extend(RecastConfiguration.userVariables["THerror_combination"])
        elif variable =="error_extrapolation":
                table.extend(RecastConfiguration.userVariables["error_extrapolation"])
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
        if self.pad:
            padfile  = open(os.path.normpath(os.path.join(self.ma5dir,"tools/PAD/Build/Main/main.cpp")), 'r')
            ToLoopOver.append([padfile, padlist])
        if self.padtune:
            tunefile = open(os.path.normpath(os.path.join(self.ma5dir,"tools/PADForMA5tune/Build/Main/main.cpp")), 'r')
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
                if not os.path.isfile(os.path.normpath(os.path.join(self.ma5dir,'tools/PAD/Input/Cards',mydelphes))):
                    self.logger.error("Recasting card: PAD analysis linked to an invalid delphes card: " + myana + " - " + mydelphes)
                    return False
            elif myana in  [x[0] for x in tunelist]:
                if myver!="v1.1":
                    self.logger.error("Recasting card: invalid analysis (not present in the PADForMA5tune): " + myana)
                    return False
                if not os.path.isfile(os.path.normpath(os.path.join(self.ma5dir,'tools/PADForMA5tune/Input/Cards',mydelphes))):
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
        mainfile  = open(os.path.normpath(os.path.join(self.ma5dir,'tools',padtype,"Build/Main/main.cpp")), 'r')
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
        thecard.sort()
        if write:
            card = open(dirname+'/Input/recasting_card.dat','a')
            card.write('\n'.join(thecard))
            card.write('#\n')
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
