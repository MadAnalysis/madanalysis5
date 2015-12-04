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


from madanalysis.enumeration.ma5_running_type   import MA5RunningType
from madanalysis.IOinterface.library_writer     import LibraryWriter
from madanalysis.IOinterface.folder_writer      import FolderWriter
from shell_command import ShellCommand
import logging
import shutil
import os
import math

def CleanRegionName(mystr):
    newstr = mystr.replace("/",  "_slash_")
    newstr = newstr.replace("->", "_to_")
    newstr = newstr.replace(">=", "_greater_than_or_equal_to_")
    newstr = newstr.replace(">",  "_greater_than_")
    newstr = newstr.replace("<=", "_smaller_than_or_equal_to_")
    newstr = newstr.replace("<",  "_smaller_than_")
    newstr = newstr.replace(" ",  "_")
    newstr = newstr.replace(",",  "_")
    newstr = newstr.replace("+",  "_")
    newstr = newstr.replace("-",  "_")
    newstr = newstr.replace("(",  "_lp_")
    newstr = newstr.replace(")",  "_rp_")
    return newstr

def CLs(NumObserved, ExpectedBG, BGError, SigHypothesis, NumToyExperiments):
    ## testing whether scipy is there
    try:
        import scipy.stats
    except ImportError:
        logging.warning('scipy is not installed... the CLs module cannot be used.')
        logging.warning('Please install scipy.')
        return False
    # generate a set of expected-number-of-background-events, one for each toy
    # experiment, distributed according to a Gaussian with the specified mean
    # and uncertainty
    ExpectedBGs = scipy.stats.norm.rvs(loc=ExpectedBG, scale=BGError, size=NumToyExperiments)

    # Ignore values in the tail of the Gaussian extending to negative numbers
    ExpectedBGs = [value for value in ExpectedBGs if value > 0]

    # For each toy experiment, get the actual number of background events by
    # taking one value from a Poisson distribution created using the expected
    # number of events.
    ToyBGs = scipy.stats.poisson.rvs(ExpectedBGs)
    ToyBGs = map(float, ToyBGs)

    # The probability for the background alone to fluctutate as LOW as
    # observed = the fraction of the toy experiments with backgrounds as low as
    # observed = p_b.
    # NB (1 - this p_b) corresponds to what is usually called p_b for CLs.
    p_b = scipy.stats.percentileofscore(ToyBGs, NumObserved, kind='weak')*.01

    # Toy MC for background+signal
    ExpectedBGandS = [expectedbg + SigHypothesis for expectedbg in ExpectedBGs]
    ToyBplusS = scipy.stats.poisson.rvs(ExpectedBGandS)
    ToyBplusS = map(float, ToyBplusS)

    # Calculate the fraction of these that are >= the number observed,
    # giving p_(S+B). Divide by (1 - p_b) a la the CLs prescription.
    p_SplusB = scipy.stats.percentileofscore(ToyBplusS, NumObserved, kind='weak')*.01

    if p_SplusB>p_b:
        return 0.
    else:
        return 1.-(p_SplusB / p_b) # 1 - CLs

class RecastConfiguration:

    default_CLs_numofexps = 100000

    userVariables ={
         "status"        : ["on","off"],\
         "CLs_numofexps" : [str(default_CLs_numofexps)]
    }

    def __init__(self):
        self.status  = "off"
        self.delphes = False
        self.ma5tune = False
        self.pad     = False
        self.padtune = False
        self.DelphesDic = {
          "delphes_card_cms_standard.tcl":      ["cms_sus_14_001_monojet", "cms_sus_13_016", "cms_sus_13_012", "cms_sus_13_011"],
          "delphes_card_atlas_sus_2013_05.tcl": ["ATLAS_EXOT_2014_06", "atlas_susy_2013_21", "atlas_sus_13_05"],
          "delphes_card_atlas_sus_2013_11.tcl": ["atlas_higg_2013_03", "atlas_susy_2013_11", "atlas_1405_7875"],
          "delphes_card_atlas_sus_2014_10.tcl": ["atlas_susy_2014_10"] ,
          "delphes_card_atlas_sus_2013_04.tcl": ["atlas_susy_2013_04"] ,
          "delphes_card_cms_b2g_12_012.tcl":    ["cms_B2G_12_012", "cms_exo_12_047", "cms_exo_12_048"] }

        self.description = {
          "atlas_susy_2013_04"     : "ATLAS - multijet + met", 
          "atlas_sus_13_05"        : "ATLAS - stop/sbottom - 0 lepton + 2 bjets + met",
          "atlas_susy_2013_11"     : "ATLAS - ewkinos - 2 leptons + met",
          "atlas_susy_2013_21"     : "ATLAS - monojet",
          "atlas_susy_2014_10"     : "ATLAS - squark-gluino - 2 leptons + jets + met",
          "atlas_1405_7875"        : "ATLAS - squark-gluino - 0 leptons + 2-6 jets + met",
          "atlas_higg_2013_03"     : "ATLAS - ZH to invisible + 2 leptons",
          "ATLAS_EXOT_2014_06"     : "ATLAS - monophoton",
          "cms_sus_13_012"         : "CMS   - squark-gluino - MET/MHT",
          "cms_sus_13_016"         : "CMS   - gluinos - 2 leptons + bjets + met",
          "cms_sus_14_001_monojet" : "CMS   - stop - the monojet channel",
          "cms_sus_13_011"         : "CMS   - stop - 1 lepton + bjets + met",
          "cms_exo_12_047"         : "CMS   - monophoton",
          "cms_exo_12_048"         : "CMS   - monojet",
          "cms_B2G_12_012"         : "CMS   - T5/3 partners in the SSDL channel"
        }

        self.delphesruns  = []
        self.analysisruns = []
        self.CLs_numofexps= 100000

    def Display(self):
        self.user_DisplayParameter("status")
        if self.status=="on":
            self.user_DisplayParameter("delphes")
            self.user_DisplayParameter("ma5tune")
            self.user_DisplayParameter("pad")
            self.user_DisplayParameter("padtune")
            self.user_DisplayParameter("CLs_numofexps")

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
                logging.info("   * the PADForMa5tune is         : available")
            else:
                logging.info("   * the PADForMa5tune is         : not available")
            return
        elif parameter=="CLs_numofexps":
            logging.info("   * Number of toy experiments for the CLs calculation: "+self.CLs_numofexps)
            return
        return

    def user_SetParameter(self,parameter,value,level,hasdelphes,hasMA5tune,datasets, hasPAD, hasPADtune):
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

                # DelphesMA5tune and the PADFor MA5TUne?
                if hasMA5tune:
                    self.ma5tune=True
                if hasPADtune:
                    self.padtune=True
                if not hasPADtune or not hasMA5tune:
                    logging.warning("DelphesMA5tune and/or the PADForMA5tune are not installed " + \
                        "(or deactivated): the corresponding analyses will be unavailable")
                else:
                    canrecast=True

                # can we use the recasting mode
                if canrecast:
                    self.status="on"
                else:
                    logging.error("The recasting modules (PAD/Delphes, PADForMA5tune/DelphesMa5tune) " + \
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

        # CLs module
        elif parameter=="CLs_numofexps":
            if self.status!="on":
                logging.error("Please first set the recasting mode to 'on'.")
                return
            self.CLs_numofexps = value
        # other rejection if no algo specified
        else:
            logging.error("the recast module has no parameter called '"+parameter+"'")
            return

    def user_GetParameters(self):
        if self.status=="on":
            table = ["CLs_numofexps"]
        else:
           table = []
        return table


    def user_GetValues(self,variable):
        table = []
        if variable=="status":
                table.extend(RecastConfiguration.userVariables["status"])
        elif variable =="CLs_numofexps":
                table.extend(RecastConfiguration.userVariables["CLs_numofexps"])
        return table


    def CreateCard(self,dirname):
        # getting the PAD analysis
        if self.padtune and self.ma5tune:
            self.CreateMyCard(dirname,"PADForMA5tune")
        if self.pad and self.delphes:
            self.CreateMyCard(dirname,"PAD")

    def CreateMyCard(self,dirname,padtype):
        mainfile = open(dirname+"/../"+padtype+"/Build/Main/main.cpp")
        import os
        exist=os.path.isfile(dirname+'/Input/recasting_card.dat')
        card = open(dirname+'/Input/recasting_card.dat','a')
        if not exist:
            card.write('# Delphes cards must be located in the PAD(ForMA5tune) directory\n')
            card.write('# Switches must be on or off\n')
            card.write('# AnalysisName               PADType    Switch     DelphesCard\n')
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
                card.write(analysis.ljust(30,' ') + mytype.ljust(12,' ') + 'on    ' + mydelphes.ljust(50, ' ')+\
                      ' # '+descr+'\n')
        mainfile.close()
        card.close()

    def UpdatePADMain(self,analysislist,PADdir):
        ## backuping the main file
        logging.info("   Updating the PAD main executable")
        if os.path.isfile(PADdir+'/Build/Main/main.bak'):
            os.remove(PADdir+'/Build/Main/main.bak')
        shutil.move(PADdir+'/Build/Main/main.cpp',PADdir+'/Build/Main/main.bak')
        ## creating the main file with the desired analyses inside
        mainfile = open(PADdir+"/Build/Main/main.bak",'r')
        newfile  = open(PADdir+"/Build/Main/main.cpp",'w')
        ignore = False
        for line in mainfile:
            if '// Getting pointer to the analyzer' in line:
                ignore = True
                newfile.write(line)
                for analysis in analysislist:
                    newfile.write('  std::map<std::string, std::string> prm'+analysis+';\n')
                    newfile.write('  AnalyzerBase* analyzer_'+analysis+'=\n')
                    newfile.write('    manager.InitializeAnalyzer(\"'+analysis+'\",\"'+analysis+'.saf\",'+\
                       'prm'+analysis+');\n')
                    newfile.write(  '  if (analyzer_'+analysis+'==0) return 1;\n\n')
            elif '// Post initialization (creates the new output directory structure)' in line:
                ignore=False
                newfile.write(line)
            elif '!analyzer_' in line and not ignore:
                ignore=True
                for analysis in analysislist:
                    newfile.write('      if (!analyzer_'+analysis+'->Execute(mySample,myEvent)) continue;\n')
            elif '!analyzer1' in line:
                ignore=False
            elif not ignore:
                newfile.write(line)
        mainfile.close()
        newfile.close()
        return True

    def RestorePADMain(self,PADdir,dirname,main):
        logging.info('   Restoring the PAD in '+PADdir)
        ## Restoring the main file
        shutil.move(PADdir+'/Build/Main/main.bak',PADdir+'/Build/Main/main.cpp')
        self.MakePAD(PADdir,dirname,main,True)
        return True

    def MakePAD(self,PADdir,dirname,main,silent=False):
        if not silent:
            logging.info('   Compiling the PAD in '+PADdir)
        compiler = LibraryWriter('lib',main)
        ncores = compiler.get_ncores2()
        if ncores>1:
            strcores='-j'+str(ncores)
        command = ['make',strcores]
        logfile = PADdir+'/Build/PADcompilation.log'
        result, out = ShellCommand.ExecuteWithLog(command,logfile,PADdir+'/Build')
        if not result:
            logging.error('Impossible to compile the PAD....'+\
              ' For more details, see the log file:')
            logging.error(logfile)
            return False
        return True

    def RunPAD(self,PADdir,eventfile):
        ## input file
        if os.path.isfile(PADdir+'/Input/PADevents.list'):
            os.remove(PADdir+'/Input/PADevents.list')
        infile = open(PADdir+'/Input/PADevents.list','w')
        infile.write(eventfile)
        infile.close()
        ## cleaning the output directory
        if not FolderWriter.RemoveDirectory(os.path.normpath(PADdir+'/Output/PADevents.list')):
            return False
        ## running
        command = ['MadAnalysis5job', '../Input/PADevents.list']
        ok = ShellCommand.Execute(command,PADdir+'/Build')
        if not ok:
            logging.error('Problem with the run of the PAD on the file: '+ eventfile)
            return False
        os.remove(PADdir+'/Input/PADevents.list')
        return True

    def SavePADOutput(self,PADdir,dirname,analysislist,setname):
        if not os.path.isfile(dirname+'/Output/PADevents.list.saf'):
            shutil.move(PADdir+'/Output/PADevents.list/PADevents.list.saf',dirname+'/Output/'+setname+'.saf')
        for analysis in analysislist:
            shutil.move(PADdir+'/Output/PADevents.list/'+analysis+'_0',dirname+'/Output/'+setname+'/'+analysis)
        return True

    def GetDelphesRuns(self,recastcard):
        self.delphesruns=[]
        runcard = open(recastcard,'r')
        for line in runcard:
            myline=line.split()
            if myline[2].lower() =='on' and myline[3] not in self.delphesruns:
                self.delphesruns.append(myline[1]+'_'+myline[3])
        return True

    def GetAnalysisRuns(self,recastcard):
        self.analysisruns=[]
        runcard = open(recastcard,'r')
        for line in runcard:
            myline=line.split()
            if myline[2].lower() =='on':
                self.analysisruns.append(myline[1]+'_'+myline[0])
        return True

    def ReadInfoFile(self, mytree, myanalysis):
        ## checking the header of the file
        info_root = mytree.getroot()
        if info_root.tag != "analysis":
            logging.warning('Invalid info file (' + myanalysis+ '): <analysis> tag.')
            return -1,-1,-1
        if info_root.attrib["id"].lower() != myanalysis.lower():
            logging.warning('Invalid info file (' + myanalysis+ '): <analysis id> tag.')
            return -1,-1,-1
        ## extracting the information
        lumi    = 0
        regions = []
        regiondata = {}
        for child in info_root:
            # Luminosity
            if child.tag == "lumi":
                try:
                    lumi = float(child.text)
                except:
                    logging.warning('Invalid info file (' + myanalysis+ '): ill-defined lumi')
                    return -1,-1,-1
                logging.debug('The luminosity of ' + myanalysis + ' is ' + str(lumi) + ' fb-1.')
            # regions
            if child.tag == "region" and ("type" not in child.attrib or child.attrib["type"] == "signal"):
                if "id" not in child.attrib:
                    logging.warning('Invalid info file (' + myanalysis+ '): <region id> tag.')
                    return -1,-1,-1
                if child.attrib["id"] in regions:
                    logging.warning('Invalid info file (' + myanalysis+ '): doubly-defined region.')
                    return -1,-1,-1
                regions.append(child.attrib["id"])
                nobs    = -1
                nb      = -1
                deltanb = -1
                for rchild in child:
                    try:
                        myval=float(rchild.text)
                    except:
                        logging.warning('Invalid info file (' + myanalysis+ '): region data ill-defined.')
                        return -1,-1,-1
                    if rchild.tag=="nobs":
                        nobs = myval
                    elif rchild.tag=="nb":
                        nb = myval
                    elif rchild.tag=="deltanb":
                        deltanb = myval
                    else:
                        logging.warning('Invalid info file (' + myanalysis+ '): unknown region subtag.')
                        return -1,-1,-1
                regiondata[child.attrib["id"]] = { "nobs":nobs, "nb":nb, "deltanb":deltanb }
        return lumi, regions, regiondata

    def ReadCutflow(self, dirname,regions,regiondata):
        for reg in regions:
            regname = CleanRegionName(reg)
            ## getting the initial and final number of events
            IsInitial = False
            IsCounter = False
            N0 = 0.
            Nf = 0.
            ## checking if regions must be combined
            theregs=regname.split(';')
            for regiontocombine in theregs:
                if not os.path.isfile(dirname+'/'+regiontocombine+'.saf'):
                    logging.warning('Cannot find a cutflow for the region '+regiontocombine+' in ' + dirname)
                    logging.warning('Skipping the CLs calculation.')
                    return -1
                mysaffile = open(dirname+'/'+regiontocombine+'.saf')
                myN0=-1
                myNf=-1
                for line in mysaffile:
                    if "<InitialCounter>" in line:
                        IsInitial = True
                        continue
                    elif "</InitialCounter>" in line:
                        IsInitial = False
                        continue
                    elif "<Counter>" in line:
                        IsCounter = True
                        continue
                    elif "</Counter>" in line:
                        IsCounter = False
                        continue
                    if IsInitial and "sum of weights" in line and not '^2' in line:
                        myN0 = float(line.split()[0])
                    if IsCounter and "sum of weights" in line and not '^2' in line:
                        myNf = float(line.split()[0])
                mysaffile.close()
                if myNf==-1 or myN0==-1:
                    logging.warning('Invalid cutflow for the region ' + reg +'('+regname+') in ' + dirname)
                    logging.warning('Skipping the CLs calculation.')
                    return -1
                Nf+=myNf
                N0+=myN0
            if Nf==0 and N0==0:
                logging.warning('Invalid cutflow for the region ' + reg +'('+regname+') in ' + dirname)
                logging.warning('Skipping the CLs calculation.')
                return -1
            regiondata[reg]["N0"]=N0
            regiondata[reg]["Nf"]=Nf
        return regiondata


    def ComputesigCLs(self,regiondata,regions,lumi):
        for reg in regions:
            nb      = regiondata[reg]["nb"]
            nobs    = regiondata[reg]["nobs"]
            deltanb = regiondata[reg]["deltanb"]
            def GetSig95(xsection):
                nsignal=xsection * lumi * 1000. * regiondata[reg]["Nf"] / regiondata[reg]["N0"]
                return CLs(nobs,nb,deltanb,nsignal,self.CLs_numofexps)-0.95
            nslow = lumi * 1000. * regiondata[reg]["Nf"] / regiondata[reg]["N0"]
            nshig = lumi * 1000. * regiondata[reg]["Nf"] / regiondata[reg]["N0"]
            if nslow == 0 and nshig == 0:
               regiondata[reg]["s95"]="Inf"
               continue
            low = 1.
            hig = 1.
            while CLs(nobs,nb,deltanb,nslow,self.CLs_numofexps)>0.95:
              logging.debug('region ' + reg + ', lower bound = ' + str(low))
              nslow=nslow*0.1
              low  =  low*0.1
            while CLs(nobs,nb,deltanb,nshig,self.CLs_numofexps)<0.95:
              logging.debug('region ' + reg + ', upper bound = ' + str(hig))
              nshig=nshig*10.
              hig  =  hig*10.
            ## testing whether scipy is there
            try:
                import scipy.stats
            except ImportError:
                logging.warning('scipy is not installed... the CLs module cannot be used.')
                logging.warning('Please install scipy.')
                return False
            s95 = scipy.optimize.brentq(GetSig95,low,hig)
            logging.debug('region ' + reg + ', s95 = ' + str(s95) + ' pb')
            regiondata[reg]["s95"]= ("%.7f" % s95)
        return regiondata

    def ComputeCLs(self,regiondata,regions,xsection,lumi):
        ## computing fi a region belongs to the best expected ones, and derive the CLs in all cases
        bestreg=[]
        limit=-1
        for reg in regions:
            nsignal = xsection * lumi * 1000. * regiondata[reg]["Nf"] / regiondata[reg]["N0"]
            nb      = regiondata[reg]["nb"]
            nobs    = regiondata[reg]["nobs"]
            deltanb = regiondata[reg]["deltanb"]
            limitSR = CLs(nb,   nb, deltanb, nsignal, self.CLs_numofexps)
            myCLs   = CLs(nobs, nb, deltanb, nsignal, self.CLs_numofexps)
            regiondata[reg]["limitSR"] = limitSR
            regiondata[reg]["CLs"]     = myCLs
            if limitSR >= limit:
                regiondata[reg]["best"]=1
                if limitSR > limit:
                    for mybr in bestreg:
                        regiondata[mybr]["best"]=0
                    bestreg = [reg]
                    limit = limitSR
                else:
                    bestreg.append(reg)
            else:
                regiondata[reg]["best"]=0
        return regiondata

    def WriteCLs(self, dirname, analysis, regions,regiondata, summary, xsflag):
        for reg in regions:
            eff    = (regiondata[reg]["Nf"] / regiondata[reg]["N0"])
            stat   = (math.sqrt(eff*(1-eff)/regiondata[reg]["N0"]))
            syst   = 0.
            myeff  = "%.7f" % eff
            mystat = "%.7f" % stat
            mysyst = "%.7f" % syst
            mytot  = "%.7f" % (math.sqrt(stat**2+syst**2))
            if not xsflag:
                mycls  = "%.7f" % regiondata[reg]["CLs"]
                summary.write(analysis.ljust(30,' ') + reg.ljust(50,' ') +\
                   str(regiondata[reg]["best"]).ljust(10, ' ') + mycls.ljust(10,' ') + \
                   ' ||    ' + myeff.ljust(15,' ') + mystat.ljust(15,' ') + mysyst.ljust(15, ' ') +\
                   mytot.ljust(15,' ') + '\n')
            else:
                myxs = regiondata[reg]["s95"]
                summary.write(analysis.ljust(30,' ') + reg.ljust(50,' ') +\
                   myxs.ljust(10,' ') + \
                   ' ||    ' + myeff.ljust(15,' ') + mystat.ljust(15,' ') + mysyst.ljust(15, ' ') +\
                   mytot.ljust(15,' ') + '\n')

    def GetCLs(self,PADdir, dirname, analysislist, name,  xsection, setname):
        logging.info('   Calculation of the exclusion CLs')
        if xsection<=0:
            logging.info('   Signal xsection not defined. The 95% excluded xsecton will be calculated.')
        try:
            from lxml import ET
        except:
            try:
                import xml.etree.ElementTree as ET
            except:
                logging.warning('lxml or xml not available... the CLs module cannot be used')
                return False
        ## preparing the output file
        if os.path.isfile(dirname+'/Output/'+setname+'/CLs_output.saf'):
            mysummary=open(dirname+'/Output/'+setname+'/CLs_output.saf','a')
        else:
            mysummary=open(dirname+'/Output/'+setname+'/CLs_output.saf','w')
            if xsection <=0:
                mysummary.write("# analysis name".ljust(30, ' ') + "signal region".ljust(50,' ') + \
                 'sig95'.ljust(10, ' ') + ' ||    ' + 'efficiency'.ljust(15,' ') +\
                 "stat. unc.".ljust(15,' ') + "syst. unc.".ljust(15," ") + "tot. unc.".ljust(15," ") + '\n')

            else:
                mysummary.write("# analysis name".ljust(30, ' ') + "signal region".ljust(50,' ') + \
                 "best?".ljust(10,' ') + 'CLs'.ljust(10,' ') + ' ||    ' + 'efficiency'.ljust(15,' ') +\
                 "stat. unc.".ljust(15,' ') + "syst. unc.".ljust(15," ") + "tot. unc.".ljust(15," ") + '\n')
        ## running over all analysis
        for analysis in analysislist:
            ## Reading the info file
            if not os.path.isfile(PADdir+'/Build/SampleAnalyzer/User/Analyzer/'+analysis+'.info'):
                logging.warning('Info file missing for the '+ analysis+ ' analysis. Skipping the CLs calculation')
                return False
            info_input = open(PADdir+'/Build/SampleAnalyzer/User/Analyzer/'+analysis+'.info')
            try:
                info_tree = ET.parse(info_input)
            except:
                logging.warning('Info file for '+analysis+' corrupted. Skipping the CLs calculation.')
                return False
            info_input.close()
            lumi, regions, regiondata = self.ReadInfoFile(info_tree,analysis)
            if lumi==-1 or regions==-1 or regiondata==-1:
                logging.warning('Info file for '+analysis+' corrupted. Skipping the CLs calculation.')
                return False
            ## reading the cutflow information
            regiondata=self.ReadCutflow(dirname+'/Output/'+name+'/'+analysis+'/Cutflows',regions,regiondata)
            if regiondata==-1:
                logging.warning('Info file for '+analysis+' corrupted. Skipping the CLs calculation.')
                return False
            ## performing the alculation
            if xsection <=0:
                xsflag=True
                regiondata=self.ComputesigCLs(regiondata,regions,lumi)
            else:
                xsflag=False
                regiondata=self.ComputeCLs(regiondata,regions,xsection,lumi)
            ## writing the output file
            self.WriteCLs(dirname,analysis,regions, regiondata,mysummary,xsflag)
            mysummary.write('\n')

        ## closing the output file
        mysummary.close()

        return True


    def CheckDir(self,dirname):
        if not os.path.isdir(dirname):
            logging.error("The directory '"+dirname+"' has not been found.")
            return False
        elif not os.path.isdir(dirname+'/Output'):
            logging.error("The directory '"+dirname+"/Output' has not been found.")
            return False
        return True

    def CheckFile(self,dirname,dataset):
        if not os.path.isfile(dirname+'/Output/'+dataset.name+'/CLs_output.saf'):
            logging.error("The file '"+dirname+'/Output/'+dataset.name+'/CLs_output.saf" has not been found.')
            return False
        return True
