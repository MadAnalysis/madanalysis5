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


from madanalysis.selection.instance_name          import InstanceName
from madanalysis.enumeration.combination_type     import CombinationType
from madanalysis.enumeration.observable_type      import ObservableType
from madanalysis.enumeration.ma5_running_type     import MA5RunningType
from madanalysis.enumeration.stacking_method_type import StackingMethodType
from madanalysis.enumeration.argument_type        import ArgumentType
import logging

class Histogram():

    userVariables = { "nbins" : [], \
                      "xmin"  : [], \
                      "xmax"  : [], \
                      "stacking_method" : ["auto","stack","superimpose","normalize2one"], \
                      "logX"  : ["true","false"], \
                      "logY"  : ["true","false"], \
                      "rank"  : ["Eordering","Pordering","PTordering","ETordering","PXordering","PYordering","PZordering","ETAordering"], \
                      "statuscode" : ["finalstate","interstate","allstate","initialstate"], \
                      "titleX" :[], \
                      "titleY" :[]
  }

    userShortcuts = {"logX": ["logX","true"], \
                     "logY": ["logY","true"], \
                     "finalstate": ["statuscode","finalstate"], \
                     "interstate": ["statuscode","interstate"], \
                     "allstate":   ["statuscode","allstate"], \
                     "initialstate": ["statuscode","initialstate"], \
                     "Eordering": ["rank","Eordering"], \
                     "Pordering": ["rank","Pordering"], \
                     "PTordering": ["rank","PTordering"], \
                     "ETordering": ["rank","ETordering"], \
                     "PXordering": ["rank","PXordering"], \
                     "PYordering": ["rank","PYordering"], \
                     "PZordering": ["rank","PZordering"], \
                     "ETAordering": ["rank","ETAordering"], \
                     "stack":       ["stacking_method","stack"], \
                     "superimpose": ["stacking_method","superimpose"], \
                     "normalize2one" : ["stacking_method","normalize2one"]}

    def __init__(self,observable,arguments,nbins,xmin,xmax):
        self.observable = observable
        self.arguments  = arguments
        self.nbins      = nbins
        self.xmin       = xmin
        self.xmax       = xmax
        self.logX       = False
        self.logY       = False
        self.rank       = "PTordering"
        self.statuscode = "finalstate"
        self.stack      = StackingMethodType.AUTO
        self.titleX     = ""
        self.titleY     = ""

    def user_GetParameters(self):
        return Histogram.userVariables.keys()

    def user_GetShortcuts(self):
        return Histogram.userShortcuts.keys()

    def user_GetValues(self,variable):
        try:
            return Histogram.userVariables[variable]
        except:
            return []

    def user_SetShortcuts(self,name):
        if name in Histogram.userShortcuts.keys():
            return self.user_SetParameter(Histogram.userShortcuts[name][0],Histogram.userShortcuts[name][1])
        else:
            logging.error("option '" + name + "' is unknown.")
            return False

    def user_SetParameter(self,variable,value):
        # nbins variable
        if variable == "nbins":
            try:
                tmp = int(value)
            except:
                logging.error("variable 'nbins' must be an non-null integer positive value")
                return False
            if tmp <=0:
                logging.error("variable 'nbins' must be an non-null integer positive value")
                return False
            else:
                self.nbins = tmp
                return True
        # xmin
        elif variable == "xmin":
            try:
                tmp = float(value)
            except:
                logging.error("variable 'xmin' must be a float value")
                return False
            if tmp > self.xmax:
                logging.error("'xmin' value must be less than 'xmax' value")
                return False
            else:
                self.xmin = tmp
        # xmax
        elif variable == "xmax":
            try:
                tmp = float(value)
            except:
                logging.error("variable 'xmax' must be a float value")
                return False
            if tmp < self.xmin:
                logging.error("'xmax' value must be greater than 'xmin' value")
                return False
            else:
                self.xmax = tmp
        # logX
        elif variable == "logX":
            if value=="true":
                self.logX=True
            elif value=="false":
                self.logX=False
            else:
                logging.error("variable 'logX' possible values are 'true' and 'false'")
                return False
        # logY
        elif variable == "logY":
            if value=="true":
                self.logY=True
            elif value=="false":
                self.logY=False
            else:
                logging.error("variable 'logY' possible values are 'true' and 'false'")
                return False

        # rank
        elif variable == "rank":
            if value in Histogram.userVariables["rank"]:
                self.rank=value
            else:
                logging.error("'"+value+"' is not a possible value for the variable 'rank'.")
                return False
            
        # statuscode
        elif variable == "statuscode":
            if value in Histogram.userVariables["statuscode"]:
                self.statuscode=value
            else:
                logging.error("'"+value+"' is not a possible value for the variable 'statuscode'.")
                return False

        # stack
        elif variable == "stacking_method":
            if value=="auto":
                self.stack = StackingMethodType.AUTO
            elif value=="stack":
                self.stack = StackingMethodType.STACK
            elif value=="superimpose":
                self.stack = StackingMethodType.SUPERIMPOSE
            elif value=="normalize2one":
                self.stack = StackingMethodType.NORMALIZE2ONE
            else:
                logging.error("'"+value+"' is not a possible value for the variable 'stacking_method'.")
                return False

        # titleX
        elif variable == "titleX":
            if value[0] in [ '"', "'" ] and value[-1] in [ '"', "'" ]:
                self.titleX = value[1:-1]
            else:
                logging.error("'"+value+"' is not a string, as necessary for the variable 'titleX'.")
                return False

        # titleY
        elif variable == "titleY":
            if value[0] in [ '"', "'" ] and value[-1] in [ '"', "'" ]:
                self.titleY = value[1:-1]
            else:
                logging.error("'"+value+"' is not a string, as necessary for the variable 'titleY'.")
                return False        # other
        else:
            logging.error("variable called '"+variable+"' is unknown")
            return False
        

        return True

    def user_DisplayParameter(self,variable):
        if variable=="nbins":
            logging.info(" nbins = "+str(self.nbins))
        elif variable=="xmin":
            logging.info(" xmin = "+str(self.xmin))
        elif variable=="xmax":
            logging.info(" xmax = "+str(self.xmax))
        elif variable=="stacking_method":
            msg=""
            if self.stack==StackingMethodType.AUTO:
                msg="auto"
            elif self.stack==StackingMethodType.STACK:
                msg="stack"
            elif self.stack==StackingMethodType.SUPERIMPOSE:
                msg="superimpose"
            elif self.stack==StackingMethodType.NORMALIZE2ONE:
                msg="normalize2one"
            logging.info(" stacking method = "+msg)
        elif variable=="logX":
            word="false"
            if self.logX:
                word="true"
            logging.info(" logX = "+word)
        elif variable=="logY":
            word="false"
            if self.logY:
                word="true"
            logging.info(" logY = "+word)
        elif variable=="rank":
            logging.info(" rank = "+self.rank)
        elif variable=="statuscode":
            logging.info(" statuscode = "+self.statuscode)
        elif variable=="titleX":
            logging.info(" titleX = '"+self.titleX+"'")
        elif variable=="titleY":
            logging.info(" titleY = '"+self.titleY+"'")
        else:
            logging.error("no variable called '"+variable+"' is found")
                            

    def Display(self):
        logging.info(self.GetStringDisplay())
        logging.info(self.GetStringDisplay2())
        logging.info(self.GetStringDisplayMore())

    def GetStringDisplay(self):
        msg = "  * Plot: "+self.observable.name
        if len(self.arguments)!=0:
            msg += ' ( '
            for i in range(len(self.arguments)):
                msg += self.arguments[i].GetStringDisplay() + ' '
                if i!=len(self.arguments)-1:
                    msg += ', '
            msg += ') '
        return msg 

    def DoYouUseMultiparticle(self,name):
        return self.combination.DoYouUseMultiparticle(name)

    def GetStringDisplay2(self):
        return "  * Binning: nbins="+str(self.nbins)+\
               ", xmin="+str(self.xmin)+\
               ", xmax="+str(self.xmax)

    def GetStringDisplayMore(self):
        words=''
        if self.logX or self.logY: 
            words += '  * Log scale: '
            if self.logX:
                words+= 'logX '
            if self.logY:
                words+= 'logY '
            words+='\n'
        if self.stack==StackingMethodType.AUTO or\
            self.stack==StackingMethodType.STACK:
            words+='  * Stacking method: stacked\n'
        elif self.stack == StackingMethodType.SUPERIMPOSE:
            words+='  * Stacking method: superimposition\n'
        elif self.stack == StackingMethodType.NORMALIZE2ONE:
            words+='  * Stacking method: superimposition + normalization to one\n'
        if self.titleX !='':
            words += '  * X-axis title: ' +  self.titleX + '\n'
        if self.titleY !='':
            words += '  * Y-axis title: ' +  self.titleY + '\n'
        words += '  * Particles under consideration: ' +  self.statuscode + '\n'
        words += '  * Particle ordering: ' +  self.rank
        return words

    def GetStringArguments(self):
        word=''
        for ind in range(len(self.arguments)):
            if self.observable.args[ind] in [ArgumentType.PARTICLE,\
                                             ArgumentType.COMBINATION]:
                word+=self.arguments[ind].GetStringDisplay()
            else:
                word+=str(self.arguments[ind])
            if ind!=(len(self.arguments)-1):
                word+=', '
        return word    

    dicoargs = { '[':'_{',']':'}' } 

    def ReplaceAll(self,word,dico):
        for i,j in dico.iteritems():
            word = word.replace(i,j)
        return word    

    def GetXaxis(self):
        word = self.observable.tex + " "
        if len(self.arguments)!=0:
            word += "[ " + self.ReplaceAll(self.GetStringArguments(),self.dicoargs)+ " ] "
        if self.observable.plot_unitX!="":
            word += "("+self.observable.plot_unitX+") "

        return word    

    def GetYaxis(self):
        word = "Events "

        if self.observable.name in ['NPID', 'NAPID']:
            word = 'N. of particles';

        if self.observable.name in ['DELTAR', 'DPHI_0_PI', 'DPHI_0_2PI']:
            if self.GetStringArguments().count('[')!=2 and self.GetStringArguments().count(']')!=2:
                word='N. of (' + self.ReplaceAll(self.GetStringArguments(),self.dicoargs) + ') pairs'; 

        elif len(self.arguments)!=0 and self.observable.name!='N':
            # Special case : display 'pair'/'combination' words
            if len(self.arguments)==1 and \
               len(self.arguments[0])==1 and \
               self.observable.args[0] in [ArgumentType.PARTICLE,\
                                           ArgumentType.COMBINATION] and \
               self.arguments[0].SameCombinationNumber():
               if len(self.arguments[0][0])==2: 
                   if self.GetStringArguments().count('[')==2 and self.GetStringArguments().count(']')==2 and self.observable.name!='N':
                       word = "Events "
                   else:
                       word = 'N. of ' + self.ReplaceAll(self.GetStringArguments(),self.dicoargs) + ' pairs '
               elif len(self.arguments[0][0])>2:
                   if self.GetStringArguments().count('[')==len(self.arguments[0][0]) and self.GetStringArguments().count(']')==len(self.arguments[0][0]) and \
                      self.observable.name!='N':
                       word = "Events "
                   else:
                       word = 'N. of ' + self.ReplaceAll(self.GetStringArguments(),self.dicoargs) + " combinations "
               elif self.GetStringArguments().count('[')==1 and self.GetStringArguments().count(']')==1 and self.observable.name!='N':
                   word = "Events "
               else:
                   word = 'N. of ' + self.ReplaceAll(self.GetStringArguments(),self.dicoargs)
            elif self.GetStringArguments().count('[')!=1 and self.GetStringArguments().count(']')!=1:
               word = 'N. of ' + self.ReplaceAll(self.GetStringArguments(),self.dicoargs)
        return word
    

