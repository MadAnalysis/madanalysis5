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


from madanalysis.multiparticle.multiparticle_collection import MultiParticleCollection
from madanalysis.dataset.dataset_collection             import DatasetCollection
from madanalysis.selection.selection                    import Selection
from madanalysis.enumeration.uncertainty_type           import UncertaintyType
from madanalysis.enumeration.normalize_type             import NormalizeType
from madanalysis.enumeration.sb_ratio_type              import SBratioType
from madanalysis.interpreter.cmd_base                   import CmdBase
from madanalysis.core.linux_architecture                import LinuxArchitecture
from madanalysis.core.config_checker                    import ConfigChecker
from madanalysis.core.library_builder                   import LibraryBuilder
from madanalysis.IOinterface.library_writer             import LibraryWriter
from madanalysis.enumeration.ma5_running_type           import MA5RunningType
from madanalysis.enumeration.stacking_method_type       import StackingMethodType
from madanalysis.observable.observable_manager          import ObservableManager
from madanalysis.configuration.fastsim_configuration    import FastsimConfiguration
from madanalysis.configuration.isolation_configuration  import IsolationConfiguration
from madanalysis.configuration.merging_configuration    import MergingConfiguration
from madanalysis.configuration.shower_configuration     import ShowerConfiguration
import logging
import os
import sys

        
class Main():

    userVariables = { "currentdir"      : [], \
                      "normalize"       : ["none","lumi","lumi_weight"], \
                      "lumi"            : [], \
                      "SBratio"         : ['"S/B"','"B/S"',\
                                           '"S/(S+B)"','"B/(B+S)"',\
                                           '"S/sqrt(S+B)"','"B/sqrt(B+S)"'], \
                      "SBerror"         : [], \
                      "stacking_method" : ["stack","superimpose","normalize2one"], \
                      "outputfile"      : ['"output.lhe.gz"','"output.lhco.gz"'] \
                      }

    SBformula = { 'S/B'         : '1./(B**2)*sqrt(B**2*ES**2+S**2*EB**2)', \
                  'S/(S+B)'     : '1./(S+B)**2*sqrt(B**2*ES**2+S**2*EB**2)', \
                  'S/sqrt(S+B)' : '1./pow(S+B,3./2.)*sqrt((S+2*B)**2*ES**2+S**2*EB**2)' }

    forced = False

    def __init__(self):
        self.currentdir     = os.getcwd()
        self.firstdir       = os.getcwd()
        self.version        = ""
        self.date           = ""
        self.configLinux    = LinuxArchitecture()
        self.mode           = MA5RunningType.PARTON
        self.forced         = False
        self.multiparticles = MultiParticleCollection()
        self.datasets       = DatasetCollection()
        self.selection      = Selection()
        self.script         = False
        self.mg5            = False
        self.libZIP         = False
        self.libDelphes     = False
        self.libDelfes      = False
        self.pdflatex       = False
        self.latex          = False
        self.dvipdf         = False
        self.libFastJet     = False
        self.fortran        = False
        self.observables    = ObservableManager(self.mode)
        self.mcatnloutils   = False
        self.expertmode     = False
        self.ResetParameters()


    def ResetParameters(self):
        self.merging        = MergingConfiguration()
        self.fastsim        = FastsimConfiguration()
        self.SBratio        = 'S/B'
        self.SBerror        = Main.SBformula['S/B']
        self.lumi           = 10
        self.lastjob_name   = ''
        self.lastjob_status = False
        self.stack          = StackingMethodType.STACK
        self.isolation      = IsolationConfiguration()
        self.output         = ""
        if self.mode==MA5RunningType.RECO:
            self.normalize = NormalizeType.NONE
        else:
            self.normalize = NormalizeType.LUMI_WEIGHT
        self.shower     = ShowerConfiguration()


    def InitObservables(self,mode):
        self.observables = ObservableManager(mode)
        

    def IsGoodFormat(self,file):
        for item in self.GetSampleFormat():
            if file.endswith(item):
                return True
        return False 
        

    def GetSampleFormat(self):
        samples = []
        # Adding format according to MA5 level mode
        if self.mode in [MA5RunningType.PARTON,MA5RunningType.HADRON]:
            samples.append('.lhe')
            samples.append('.hep')
            samples.append('.hepmc')
        else:
            if self.fastsim.package=="none":
                samples.append('.lhco')
                if self.libDelphes or self.libDelfes:
                    samples.append('.root')
            else:
                samples.append('.lhe')
                samples.append('.hep')
                samples.append('.hepmc')

        # Adding gzip file
        if self.libZIP:
            zipsamples=[]
            for item in samples:
                zipsamples.append(item+'.gz')
            samples.extend(zipsamples)

        return samples     


    def Display(self):
        logging.info(" *********************************" )
        logging.info("            main program          " )
        logging.info(" *********************************" )
        self.user_DisplayParameter("currentdir")
        self.user_DisplayParameter("normalize")
        self.user_DisplayParameter("lumi")
        self.user_DisplayParameter("outputfile")
        self.user_DisplayParameter("SBratio")
        self.user_DisplayParameter("SBerror")
        if self.libFastJet:
            self.merging.Display()
        self.fastsim.Display()    
        self.isolation.Display()
        self.shower.Display()
        logging.info(" *********************************" )


    def user_DisplayParameter(self,parameter):
        if  parameter=="currentdir":
            logging.info(" currentdir = "+self.get_currentdir())
        elif parameter=="stacking_method":
            sentence = " stacking methode for histograms = "
            if self.stack==StackingMethodType.STACK:
                sentence+="stack"
            elif self.stack==StackingMethodType.SUPERIMPOSE:
                sentence+="superimpose"
            else:
                sentence+="normalize2one"
            logging.info(sentence)
        elif parameter=="normalize":
            word=""
            if self.normalize==NormalizeType.NONE:
                word="none"
            elif self.normalize==NormalizeType.LUMI:
                word="lumi"
            elif self.normalize==NormalizeType.LUMI_WEIGHT:
                word="lumi_weight"
            logging.info(" histogram normalization mode = " + word)
        elif parameter=="outputfile":
            if self.output=="":
                msg="none"
            else:
                msg='"'+self.output+'"'
            logging.info(" output file = "+msg)
        elif parameter=="lumi":
            logging.info(" integrated luminosity = "+str(self.lumi)+" fb^{-1}" )
        elif parameter=="SBratio":
            logging.info(' S/B ratio formula = "' + self.SBratio + '"')
        elif parameter=="SBerror":
            logging.info(' S/B error formula = "' + self.SBerror + '"')
        else:
            logging.error("'main' has no parameter called '"+parameter+"'")


    def user_GetValues(self,variable):
        if variable=="currentdir":
            return CmdBase.directory_complete()
        else:
            try:
                return Main.userVariables[variable]
            except:
                return []

    def user_GetParameters(self):
        return Main.userVariables.keys()

    def user_SetParameter(self,parameter,value):
        # currentdir
        if parameter=="currentdir":
            self.set_currentdir(value)

        # stacked
        elif parameter=="stacking_method":
            if value == "stack":
                self.stack=StackingMethodType.STACK
            elif value == "superimpose":
                self.stack=StackingMethodType.SUPERIMPOSE
            elif value == "normalize2one":
                self.stack=StackingMethodType.NORMALIZE2ONE
            else:
                logging.error("'stack' possible values are : 'stack', 'superimpose', 'normalize2one'")
                return False

        # normalize
        elif parameter=="normalize":
            if value == "none":
                self.normalize = NormalizeType.NONE
            elif value == "lumi":
                self.normalize = NormalizeType.LUMI
            elif value == "lumi_weight":
                self.normalize = NormalizeType.LUMI_WEIGHT
            else:
                logging.error("'normalize' possible values are : 'none', 'lumi', 'lumi_weight'")
                return False

        # lumi    
        elif (parameter=="lumi"):
            try:
                tmp = float(value)
            except:
                logging.error("'lumi' is a positive float value")
                return
            if (tmp>0):
                self.lumi=tmp
            else:
                logging.error("'lumi' is a positive float value")
                return

        # sbratio    
        elif (parameter=="SBratio"):
            quoteTag=False
            if value.startswith("'") and value.endswith("'"):
                quoteTag=True
            if value.startswith('"') and value.endswith('"'):
                quoteTag=True
            if quoteTag:
                value=value[1:-1]
            if Main.checkSBratio(value):
                self.SBratio=value
                self.suggestSBerror()
            else:
                logging.error("Specified formula is not correct.")
                return False

        # sberror
        elif (parameter=="SBerror"):
            quoteTag=False
            if value.startswith("'") and value.endswith("'"):
                quoteTag=True
            if value.startswith('"') and value.endswith('"'):
                quoteTag=True
            if quoteTag:
                value=value[1:-1]
            if Main.checkSBratio(value):
                self.SBerror=value
            else:
                logging.error("Specified formula is not correct.")
                return False
            
        # output
        elif (parameter=="outputfile"):
            quoteTag=False
            if value.startswith("'") and value.endswith("'"):
                quoteTag=True
            if value.startswith('"') and value.endswith('"'):
                quoteTag=True
            if quoteTag:
                value=value[1:-1]
            valuemin = value.lower()

            # Compressed file
            if valuemin.endswith(".gz") and not self.libZIP:
                logging.error("Compressed formats (*.gz) are not available. "\
                              + "Please install zlib with the command line:")
                logging.error(" install zlib")
                return False

            # LHE
            if valuemin.endswith(".lhe") or valuemin.endswith(".lhe.gz"):
                self.output = value
                return

            # LHCO
            elif valuemin.endswith(".lhco") or valuemin.endswith(".lhco.gz"):
                if self.mode == MA5RunningType.RECO:
                    self.output = value
                    return
                elif self.mode == MA5RunningType.PARTON:
                    logging.error("LHCO format is not available in PARTON mode.")
                    return False
                elif self.mode == MA5RunningType.HADRON:
                    if self.fastsim.package == "none":
                        logging.error("Please select a fast-simulation package before requesting a LHCO file output.")
                        logging.error("Command: set main.fastsim.package = ")
                        return False
                    else:
                        self.output = value
                        return

            else:
                logging.error("Output format is not available. Extension allowed: " +\
                              ".lhe .lhe.gz .lhco .lhco.gz")
                return False

        # other    
        else:
            logging.error("'main' has no parameter called '"+parameter+"'")

    @staticmethod
    def checkSBratio(text):
        logging.info("Checking the formula ...")
        text = text.replace("ES","z")
        text = text.replace("EB","t")
        text = text.replace("S","x")
        text = text.replace("B","y")
        from ROOT import TFormula
        formula = TFormula()
        test = formula.Compile(text)
        return (test==0)

    def suggestSBerror(self):
        # create a TFormula with the SBratio formula
        text = self.SBratio.replace("S","x")
        text = text.replace("B","y")
        from ROOT import TFormula
        ref = TFormula('SBratio',text)
        ref.Optimize()

        # Loop over SBerror formula and comparing
        for k, v in Main.SBformula.iteritems():
            text = k.replace("S","x")
            text = text.replace("B","y")
            error = TFormula('SBerror',text)
            error.Optimize()
            if ref.GetExpFormula()==error.GetExpFormula():
                logging.info("Formula corresponding to the uncertainty calculation has been found and set to the variable main.SBerror :")
                logging.info('  '+v)
                self.SBerror=v
                return True

        # Loop over SBerror formula and comparing
        # reverse S and B
        for k, v in Main.SBformula.iteritems():
            text = k.replace("S","y")
            text = text.replace("B","x")
            error = TFormula('SBerror',text)
            error.Optimize()
            if ref.GetExpFormula()==error.GetExpFormula():
                logging.info("Formula corresponding to the uncertainty calculation has been found and set to the variable main.SBerror :")
                v=v.replace('ES','ZZ')
                v=v.replace('EB','TT')
                v=v.replace('S','SS')
                v=v.replace('B','BB')
                v=v.replace('SS','B')
                v=v.replace('BB','S')
                v=v.replace('ZZ','EB')
                v=v.replace('TT','ES')
                logging.info('  '+v)
                self.SBerror=v
                return True


    def get_currentdir(self):
        return os.getcwd()

    def set_currentdir(self,dir):
        theDir = os.path.expanduser(dir)
        try:
            os.chdir(theDir)
        except:
            logging.error("Impossible to access the directory : "+theDir) 
        self.user_DisplayParameter("currentdir")
        
    currentdir = property(get_currentdir, set_currentdir)

    def CheckLinuxConfig(self,detail=False):
        self.configLinux.ma5_version    = self.version
        self.configLinux.ma5_date       = self.date
        self.configLinux.python_version = sys.version.replace('\n','')
        self.configLinux.platform       = sys.platform

        if detail:
            logging.info("General     Platform identifier : " + str(self.configLinux.platform))
            import multiprocessing
            logging.info("            Number of cores : " + str(multiprocessing.cpu_count()))
            logging.info("")

            logging.info("Python      Python release : " + str(self.configLinux.python_version))
            import commands
            pythondir = commands.getstatusoutput('which python')
            if pythondir[0]==0:
                logging.info("            Path get by 'which' : "+pythondir[1])
            else:
                logging.info("            Path get by 'which' : ERROR")
            logging.info("            Search path for modules : ")
            for path in sys.path:
                logging.info("               "+path)
            logging.info("")

        # Initializing the config checker    
        checker = ConfigChecker(self.configLinux,self.ma5dir,self.script)
        checker.checkTextEditor()

        # Mandatory packages
        logging.info("Checking mandatory packages:")
        checker.PrintLibrary("python")
        checker.PrintOK()
        if not checker.checkNumPy():
            return False
        if not checker.checkGPP():
            return False
        if not checker.checkROOT():
            return False

        # Optional packages
        logging.info("Checking optional packages:")
        if not checker.checkGF():
            return False
        self.libZIP = checker.checkZLIB()
        self.libDelphes = checker.checkDelphes()
        self.libDelfes  = checker.checkDelfes()
        self.libFastJet = checker.checkFastJet()
        self.pdflatex = checker.checkPdfLatex()
        self.latex = checker.checkLatex()
        if self.latex:
            self.dvipdf = checker.checkdvipdf()
#       COMMENTED BY BENJ: not used for the moment
#        self.mcatnloutils = checker.checkMCatNLOUtils()

        os.environ['LD_LIBRARY_PATH'] = os.environ['LD_LIBRARY_PATH'] + \
                                        ":" + self.ma5dir+'/tools/SampleAnalyzer/Lib/'
        os.environ['LIBRARY_PATH'] = os.environ['LIBRARY_PATH'] + \
                                        ":" + self.ma5dir+'/tools/SampleAnalyzer/Lib/'

        if self.mcatnloutils:
            os.environ['LD_LIBRARY_PATH'] = os.environ['LD_LIBRARY_PATH'] + \
                      ":" + self.ma5dir+'/tools/MCatNLO-utilities/MCatNLO/lib/'
            os.environ['LIBRARY_PATH'] = os.environ['LIBRARY_PATH'] + \
                      ":" + self.ma5dir+'/tools/MCatNLO-utilities/MCatNLO/lib/'
            
        return True 
   
    def BuildLibrary(self,forced=False):
        builder = LibraryBuilder(self.configLinux,self.ma5dir)
        UpdateNeed=False
        
        FirstUse = builder.checkMA5()
        if not FirstUse:
            UpdateNeed = not builder.compare()

        rebuild = forced or FirstUse or UpdateNeed
        if not rebuild:
            return True

        # Compile library
        if FirstUse:
            logging.info("  => First use of MadAnalysis (or the library is missing).")
        elif UpdateNeed:
            logging.info("  => System configuration has changed since the last use. Need to rebuild the library.")
        elif forced:
            logging.info("  => The user forces to rebuild the library.")

        # Initializing the JobWriter
        compiler = LibraryWriter(self.ma5dir,'lib',self.libZIP,self.libFastJet,self.forced,self.fortran,self.libDelphes,self.libDelfes)
    
        # Dumping architecture
        if not self.configLinux.Export(self.ma5dir+'/tools/architecture.ma5'):
            sys.exit()

        # Writing the Makefiles
        logging.info("   Creating the general 'Makefile'...")
        if not compiler.WriteMakefile('shared'):
            logging.error("library building aborted.")
            sys.exit()

        if self.libFastJet:
            logging.info("   Creating the 'Makefile' devoted to the FastJet interface ...")
            if not compiler.WriteMakefileForInterfaces(self,'fastjet'):
                logging.error("library building aborted.")
                sys.exit()
        
        if self.libZIP:
            logging.info("   Creating the 'Makefile' devoted to the zlib interface ...")
            if not compiler.WriteMakefileForInterfaces(self,'zlib'):
                logging.error("library building aborted.")
                sys.exit()

        if self.libDelphes:
            logging.info("   Creating the 'Makefile' devoted to the Delphes interface ...")
            if not compiler.WriteMakefileForInterfaces(self,'delphes'):
                logging.error("library building aborted.")
                sys.exit()

        if self.libDelfes:
            logging.info("   Creating the 'Makefile' devoted to the Delfes interface ...")
            if not compiler.WriteMakefileForInterfaces(self,'delfes'):
                logging.error("library building aborted.")
                sys.exit()

        # Cleaning the project
        logging.info("   Cleaning the project before building the library ...")
        if not compiler.MrProper():
            logging.error("impossible to clean the general folder.")
            sys.exit()

        if self.libFastJet:
            if not compiler.MrProperForInterfaces(self,'fastjet'):
                logging.error("impossible to clean the folder devoted to the FastJet interface.")
                sys.exit()

        if self.libZIP:
            if not compiler.MrProperForInterfaces(self,'zlib'):
                logging.error("impossible to clean the folder devoted to the zlib interface.")
                sys.exit()

        if self.libDelphes:
            if not compiler.MrProperForInterfaces(self,'delphes'):
                logging.error("impossible to clean the folder devoted to the Delphes interface.")
                sys.exit()

        if self.libDelfes:
            if not compiler.MrProperForInterfaces(self,'delfes'):
                logging.error("impossible to clean the folder devoted to the Delfes interface.")
                sys.exit()

        # Building the interface to FastJet
        if self.libFastJet:
            logging.info("   Compiling the interface to FastJet library ...")
            if not compiler.CompileForInterfaces('fastjet'):
                logging.error("library building aborted.")
                sys.exit()

            logging.info("   Linking the interface to FastJet library ...")
            if not compiler.LinkForInterfaces('fastjet'):
                logging.error("library building aborted.")
                sys.exit()

            logging.info("   Checking that the interface to FastJet library is properly built ...")
  
            if not os.path.isfile(self.ma5dir+'/tools/SampleAnalyzer/Lib/libfastjet_for_ma5.so'):
                logging.error("the library 'libfastjet_for_ma5.so' is not produced.")
                sys.exit()
        
        # Building the interface to zlib
        if self.libZIP:
            logging.info("   Compiling the interface to zlib library ...")
            if not compiler.CompileForInterfaces('zlib'):
                logging.error("library building aborted.")
                sys.exit()

            logging.info("   Linking the interface to zlib library ...")
            if not compiler.LinkForInterfaces('zlib'):
                logging.error("library building aborted.")
                sys.exit()

            logging.info("   Checking that the interface to zlib library is properly built ...")
  
            if not os.path.isfile(self.ma5dir+'/tools/SampleAnalyzer/Lib/libzlib_for_ma5.so'):
                logging.error("the library 'libzlib_for_ma5.so' is not produced.")
                sys.exit()

        # Building the interface to delphes
        if self.libDelphes:
            logging.info("   Compiling the interface to Delphes library ...")
            if not compiler.CompileForInterfaces('delphes'):
                logging.error("library building aborted.")
                sys.exit()

            logging.info("   Linking the interface to Delphes library ...")
            if not compiler.LinkForInterfaces('delphes'):
                logging.error("library building aborted.")
                sys.exit()

            logging.info("   Checking that the interface to Delphes library is properly built ...")
  
            if not os.path.isfile(self.ma5dir+'/tools/SampleAnalyzer/Lib/libdelphes_for_ma5.so'):
                logging.error("the library 'libdelphes_for_ma5.so' is not produced.")
                sys.exit()

        # Building the interface to delfes
        if self.libDelfes:
            logging.info("   Compiling the interface to Delfes library ...")
            if not compiler.CompileForInterfaces('delfes'):
                logging.error("library building aborted.")
                sys.exit()

            logging.info("   Linking the interface to Delfes library ...")
            if not compiler.LinkForInterfaces('delphes'):
                logging.error("library building aborted.")
                sys.exit()

            logging.info("   Checking that the interface to Delfes library is properly built ...")
  
            if not os.path.isfile(self.ma5dir+'/tools/SampleAnalyzer/Lib/libdelfes_for_ma5.so'):
                logging.error("the library 'libdelfes_for_ma5.so' is not produced.")
                sys.exit()

        # Building the SampleAnalyzer
        logging.info("   Compiling the SampleAnalyzer library ...")
        if not compiler.Compile():
            logging.error("library building aborted.")
            sys.exit()

        logging.info("   Linking the SampleAnalyzer library ...")
        if not compiler.Link():
            logging.error("library building aborted.")
            sys.exit()

        logging.info("   Checking that the SampleAnalyzer library is properly built ...")

        if not os.path.isfile(self.ma5dir+'/tools/SampleAnalyzer/Lib/libSampleAnalyzer.a'):
            logging.error("the library 'libSampleAnalyzer.a' is not produced.")
            sys.exit()
    
        # Cleaning the project
        logging.info("   Cleaning the project after building the library ...")

        if not compiler.Clean():
            logging.error("impossible to remove temporary files created during the building.")
            sys.exit()

        if self.libFastJet:
            if not compiler.CleanForInterfaces(self,'fastjet'):
                logging.error("impossible to clean the folder devoted to the FastJet interface.")
                sys.exit()

        if self.libZIP:
            if not compiler.CleanForInterfaces(self,'zlib'):
                logging.error("impossible to clean the folder devoted to the zlib interface.")
                sys.exit()

        if self.libDelphes:
            if not compiler.CleanForInterfaces(self,'delphes'):
                logging.error("impossible to clean the folder devoted to the Delphes interface.")
                sys.exit()

        if self.libDelfes:
            if not compiler.CleanForInterfaces(self,'delfes'):
                logging.error("impossible to clean the folder devoted to the Delfes interface.")
                sys.exit()

        return True    
        
