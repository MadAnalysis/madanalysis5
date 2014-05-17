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


from madanalysis.interpreter.cmd_base           import CmdBase
from madanalysis.interpreter.cmd_selection_base import CmdSelectionBase
from madanalysis.multiparticle.particle_object  import ParticleObject
from madanalysis.multiparticle.extraparticle    import ExtraParticle
from madanalysis.enumeration.combination_type   import CombinationType
from madanalysis.enumeration.observable_type    import ObservableType
from madanalysis.enumeration.argument_type      import ArgumentType
from madanalysis.selection.histogram            import Histogram
import logging


class CmdPlot(CmdBase,CmdSelectionBase):
    """Command PLOT"""

    def __init__(self,main):
        CmdBase.__init__(self,main,"plot")

    def do(self,args):

        # Skipping the case with empty args
        if len(args)==0:
            logging.error("wrong syntax")
            self.help()
            return

        # Checking observable name exists
        obsName=self.extract_observable(args[0])
        if obsName==None:
            return

        # Getting reference to observable
        obsRef=self.main.observables.get(obsName)
        foundArguments = False
        foundOptions   = False
        foundBinning   = False 

        # 1. First check : counting number of braces ( ) and [ ]
        # 2. Detecting if there is argument or option
        endArguments=-1
        beginOptions=-1
        Nbracket1=0
        Nbracket2=0
        for i in range(0,len(args)):
            if args[i]=='(':
                Nbracket1+=1
                if i==1:
                    foundArguments=True
            elif args[i]==')':
                Nbracket1-=1
                endArguments=i
            elif args[i]=='[':
                Nbracket2+=1
                beginOptions=i
            elif args[i]==']':
                Nbracket2-=1
                if i==(len(args)-1):
                    foundOptions=True

        if Nbracket1!=0:
            logging.error("number of opening-bracket '(' and number of " +\
                          "closing-braket ')' does not match.")
            return
        if Nbracket2!=0:
            logging.error("number of opening-bracket '[' and number of " +\
                          "closing-braket ']' does not match.")
            return

        if not foundOptions:
            beginOptions=len(args)
        
        # Is there histo binning ?
        if not foundArguments:
            if beginOptions==4:
                foundBinning=True
            elif beginOptions==1:
                foundBinning=False
            else:
                logging.error("histogram parameters (binning and bounds) are not fully defined")
                self.help()
                return
        else:
            if beginOptions-endArguments==4:
                foundBinning=True
            elif beginOptions-endArguments==1:
                foundBinning=False
            else:
                logging.error('histogram parameters (binning and bounds) are not fully defined')
                self.help()
                return

        # Binning : warning for NPID and NAPID if binnings are defined
        if obsRef.plot_auto and foundBinning:
           logging.warning("histogram parameters (binning and bounds) " +
                           "are automatically determined for the observable '" +\
                           obsName+"'.")
           foundBinning=False

        # NO binning case : getting the default one
        if not foundBinning:
            nbins = obsRef.plot_nbins
            xmin  = obsRef.plot_xmin
            xmax  = obsRef.plot_xmax

        # WITH binning : extracting values specified by the user    
        else:
            
            # Converting nbins
            try:
                nbins = int(args[beginOptions-3])
            except:
                logging.error(str(beginOptions-2)+\
                          "th argument (nbins) must have a non-zero, positive, integer value.")
                return
            if (nbins<=0):
                logging.error(str(beginOptions-2)+\
                          "th argument (nbins) must have a non-zero, positive, integer value")
                return

            # Converting xmin
            try:
                xmin = float(args[beginOptions-2])
            except:
                logging.error(str(beginOptions-1)+\
                          "th argument (xmin) must have a floating value.")
                return

            # Converting xmax
            try:
                xmax = float(args[beginOptions-1])
            except:
                logging.error(str(beginOptions)+\
                          "th argument (xmax) have a floating value.")
                return
            
            # Checking xmin < xmax
            if xmin>xmax:
                logging.error("'xmin' must be less than 'xmax'.")
                return

        # Extracting arguments
        arguments=[]
        if foundArguments:
            arguments=self.extract_arguments(args[2:endArguments],obsName,obsRef)
            if arguments==None:
                return
        elif len(obsRef.args) !=0: # checks whether arguments should have been provided
          logging.error("the observable '"+obsName+"' requires "+
                str(len(obsRef.args))+" arguments whereas no arguments have been specified.")
          return
        

        # Creating histo
        histo = Histogram(obsRef,arguments,nbins,xmin,xmax)

        # Getting options
        if beginOptions!=len(args):
            if not self.extract_options(histo,args[beginOptions+1:len(args)-1]):
                return

        # Adding histo
        self.main.selection.Add(histo)


    def help(self):
        logging.info("   Syntax: plot observable_name ( multiparticle1 multiparticle2 ... ) nbins xmin xmax [ option1 option 2 ]")
        logging.info("   Declares an histogram: ")
        logging.info("    - describing the distribution of a given observable, related to one or a combination of (multi)particles,")
        logging.info("    - with nbins being the number of bins,")
        logging.info("    - xmin being the lower limit on the x-axis,")
        logging.info("    - xmax bing the upper limit on the x-axis.")


    def complete_arguments(self,text,args,obsRef):

        # Look after number of commas
        ncommas=0
        for item in args:
            if item==",":
                ncommas+=1

        tmp=[]
        arguments=[]

        # Split arguments with 'comma' separation
        for item in args:
            if item!=",":
                tmp.append(item)
            else:
                arguments.append(tmp)
                tmp=[]
        if len(tmp)!=0:
            arguments.append(tmp)

        # Checking consistency between number of arguments and number of comma
        if len(arguments)>len(obsRef.args):
            return []

        # Complete only the last argument
        # Case 1 : integer or float expected -> no completion
        if obsRef.args[len(arguments)-1] in [ArgumentType.INTEGER,\
                                             ArgumentType.FLOAT]:
            return []

        
        # Case 2 : integer or float expected -> no completion
        if obsRef.args[len(arguments)-1] in [ArgumentType.PARTICLE,\
                                             ArgumentType.COMBINATION]:

            # number of non-closed []
            nbracket2=0
            for item in arguments[-1]:
                if item=="[":
                    nbracket2+=1
                elif item=="]":
                    nbracket2-=1

            # PTrank
            if nbracket2==1:
                return self.finalize_complete(text,["1","2","3","-1","-2","-3"])
            elif nbracket2>1:
                return []

            # List of multiparticles
            output=[]
            if obsRef.args[len(arguments)-1]==ArgumentType.COMBINATION or \
               len(arguments[-1])==1:
                output.extend(self.main.multiparticles.GetNames())
            if len(arguments[-1])>=2:
                output.append(')')
                output.append('and')
            return self.finalize_complete(text,output)

        

    def complete(self,text,args,begidx,endidx):

        # plot PT ( mu+ ... mu+ )  100  0   1000 [ option1 ... optionN ]
        # 0    1  2 3  

        # Adding potential blank argument
        if not text:
            args.append('')

        # Safety but not necessary
        if len(args)<=1:
            return []
                
        # STEP 1 : complete observable name
        if len(args)==2:
            return self.finalize_complete(text,self.main.observables.plot_list)

        # Extracting observable
        obsName=self.extract_observable(args[1],False)
        if obsName==None:
            return []

        # Getting reference to observable
        obsRef=self.main.observables.get(obsName)

        # Counting number of () and []
        nbracket1=0  # number of non-closed ()
        nbracket2=0  # number of non-closed []
        for item in args:
            if item=="(":
                nbracket1+=1
            if item==")":
                nbracket1-=1
            if item=="[":
                nbracket2+=1
            if item=="]":
                nbracket2-=1

        # STEP 2 : in the case of observable with arguments
        # looking for opening brace '(' 
        if len(obsRef.args)!=0 and len(args)==3:
            return self.finalize_complete(text,["("])
        if len(obsRef.args)!=0 and args[2]!='(':
            return []

        # STEP 3 : in the case of observable with arguments
        # checking / completing arguments
        if len(obsRef.args)>=1 and nbracket1>0:
            return self.complete_arguments(text,args[3:],obsRef)

        # in the case of observable with arguments
        # looking for the last ')' bracket
        if len(obsRef.args)==0:
            binningPos = 2
        else:
            binningPos = 0
            for ind in range(len(args)):
                if args[ind]==')':
                    binningPos = ind+1
            if binningPos == 0:
                return []

        # Looking for options or first parameter
        if len(args)==binningPos+1:
            output=['[']
            output.append(str(obsRef.plot_nbins))
            return self.finalize_complete(text,output)

        # Checking option block
        optionMode=False
        if args[binningPos]=='[':
            optionMode=True

        # Checking first argument    
        else:
            try:
                nbins=int(args[binningPos])
            except:
                return []
        
        # Looking for seoond parameter
        if len(args)==binningPos+2 and nbracket2==0:
            output=[]
            output.append(str(obsRef.plot_xmin))
            return self.finalize_complete(text,output)

        # Checking second argument 
        if nbracket2==0 :
            try:
                xmin = float(args[binningPos+1])
            except:
                return []
            
        # Looking for third parameter
        if len(args)==binningPos+3 and nbracket2==0:
            output=[]
            output.append(str(obsRef.plot_xmax))
            return self.finalize_complete(text,output)

        # Checking third argument 
        if nbracket2==0 :
            try:
                xmax = float(args[binningPos+2])
            except:
                return []

        # Looking for option
        if len(args)==binningPos+4 and nbracket2==0 and not optionMode:
            output=['[']
            return self.finalize_complete(text,output)
        
        if nbracket2==0:
            return []

        # options mode
        if nbracket1==0 and nbracket2==1:
            output=Histogram.userShortcuts.keys()
            output.append("]")
            return self.finalize_complete(text,output)

        
