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
                      "ymin"  : [], \
                      "ymax"  : [], \
                      "stacking_method" : ["auto","stack","superimpose","normalize2one"], \
                      "logX"  : ["true","false"], \
                      "logY"  : ["true","false"], \
                      "rank"  : ["Eordering","Pordering","PTordering","ETordering","PXordering","PYordering","PZordering","ETAordering"], \
                      "statuscode" : ["finalstate","interstate","allstate","initialstate"], \
                      "titleX" :[], \
                      "titleY" :[], \
                      "regions": []
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

    def __init__(self,observable,arguments,nbins,xmin,xmax,regions=[]):
        self.observable = observable
        self.arguments  = arguments
        self.nbins      = nbins
        self.xmin       = xmin
        self.xmax       = xmax
        self.ymin       = []
        self.ymax       = []
        self.logX       = False
        self.logY       = False
        self.rank       = "PTordering"
        self.statuscode = "finalstate"
        self.stack      = StackingMethodType.AUTO
        self.titleX     = ""
        self.titleY     = ""
        self.regions    = regions

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
            logging.getLogger('MA5').error("option '" + name + "' is unknown.")
            return False

    def user_SetParameter(self,variable,value):
        # nbins variable
        if variable == "nbins":
            try:
                tmp = int(value)
            except:
                logging.getLogger('MA5').error("variable 'nbins' must be an non-null integer positive value")
                return False
            if tmp <=0:
                logging.getLogger('MA5').error("variable 'nbins' must be an non-null integer positive value")
                return False
            else:
                self.nbins = tmp
                return True
        # xmin
        elif variable == "xmin":
            try:
                tmp = float(value)
            except:
                logging.getLogger('MA5').error("variable 'xmin' must be a float value")
                return False
            if tmp > self.xmax:
                logging.getLogger('MA5').error("'xmin' value must be less than 'xmax' value")
                return False
            else:
                self.xmin = tmp
        # xmax
        elif variable == "xmax":
            try:
                tmp = float(value)
            except:
                logging.getLogger('MA5').error("variable 'xmax' must be a float value")
                return False
            if tmp < self.xmin:
                logging.getLogger('MA5').error("'xmax' value must be greater than 'xmin' value")
                return False
            else:
                self.xmax = tmp
        # ymin
        elif variable == "ymin":
            try:
                tmp = float(value)
            except:
                logging.getLogger('MA5').error("variable 'ymin' must be a float value")
                return False
            if tmp > self.ymax:
                logging.getLogger('MA5').error("'ymin' value must be less than 'ymax' value")
                return False
            else:
                self.ymin = tmp
        # ymax
        elif variable == "ymax":
            try:
                tmp = float(value)
            except:
                logging.getLogger('MA5').error("variable 'ymax' must be a float value")
                return False
            if self.ymin!=[] and tmp < self.ymin:
                logging.getLogger('MA5').error("'ymax' value must be greater than 'ymin' value")
                return False
            else:
                self.ymax = tmp
        # logX
        elif variable == "logX":
            if value=="true":
                self.logX=True
            elif value=="false":
                self.logX=False
            else:
                logging.getLogger('MA5').error("variable 'logX' possible values are 'true' and 'false'")
                return False
        # logY
        elif variable == "logY":
            if value=="true":
                self.logY=True
            elif value=="false":
                self.logY=False
            else:
                logging.getLogger('MA5').error("variable 'logY' possible values are 'true' and 'false'")
                return False

        # rank
        elif variable == "rank":
            if value in Histogram.userVariables["rank"]:
                self.rank=value
            else:
                logging.getLogger('MA5').error("'"+value+"' is not a possible value for the variable 'rank'.")
                return False

        # statuscode
        elif variable == "statuscode":
            if value in Histogram.userVariables["statuscode"]:
                self.statuscode=value
            else:
                logging.getLogger('MA5').error("'"+value+"' is not a possible value for the variable 'statuscode'.")
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
                logging.getLogger('MA5').error("'"+value+"' is not a possible value for the variable 'stacking_method'.")
                return False

        # titleX
        elif variable == "titleX":
            if value[0] in [ '"', "'" ] and value[-1] in [ '"', "'" ]:
                self.titleX = value[1:-1]
            else:
                logging.getLogger('MA5').error("'"+value+"' is not a string, as necessary for the variable 'titleX'.")
                return False

        # titleY
        elif variable == "titleY":
            if value[0] in [ '"', "'" ] and value[-1] in [ '"', "'" ]:
                self.titleY = value[1:-1]
            else:
                logging.getLogger('MA5').error("'"+value+"' is not a string, as necessary for the variable 'titleY'.")
                return False        # other
        # regions
        elif variable == "regions":
            if isinstance(value,list) and all([isinstance(name,str) for name in value]):
                self.regions = value
            else:
                logging.getLogger('MA5').error("'"+value+"' is not a list of strings, ;"+\
                     "as necessary for the variable 'regions'.")
                return False        # other
        else:
            logging.getLogger('MA5').error("variable called '"+variable+"' is unknown")
            return False


        return True

    def user_DisplayParameter(self,variable):
        if variable=="nbins":
            logging.getLogger('MA5').info(" nbins = "+str(self.nbins))
        elif variable=="xmin":
            logging.getLogger('MA5').info(" xmin = "+str(self.xmin))
        elif variable=="xmax":
            logging.getLogger('MA5').info(" xmax = "+str(self.xmax))
        elif variable=="ymin":
            logging.getLogger('MA5').info(" ymin = "+str(self.ymin))
        elif variable=="ymax":
            logging.getLogger('MA5').info(" ymax = "+str(self.ymax))
        elif variable=="regions":
            logging.getLogger('MA5').info(" regions = '"+str(self.regions)+"'")
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
            logging.getLogger('MA5').info(" stacking method = "+msg)
        elif variable=="logX":
            word="false"
            if self.logX:
                word="true"
            logging.getLogger('MA5').info(" logX = "+word)
        elif variable=="logY":
            word="false"
            if self.logY:
                word="true"
            logging.getLogger('MA5').info(" logY = "+word)
        elif variable=="rank":
            logging.getLogger('MA5').info(" rank = "+self.rank)
        elif variable=="statuscode":
            logging.getLogger('MA5').info(" statuscode = "+self.statuscode)
        elif variable=="titleX":
            logging.getLogger('MA5').info(" titleX = '"+self.titleX+"'")
        elif variable=="titleY":
            logging.getLogger('MA5').info(" titleY = '"+self.titleY+"'")
        else:
            logging.getLogger('MA5').error("no variable called '"+variable+"' is found")


    def Display(self):
        logging.getLogger('MA5').info(self.GetStringDisplay())
        logging.getLogger('MA5').info(self.GetStringDisplay2())
        self.GetStringDisplayMore()

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
        strng= "  * Binning: nbins="+str(self.nbins)+", xmin="+str(self.xmin)+ ", xmax="+str(self.xmax)
        if self.ymin!=[]:
          strng = strng+', ymin='+str(self.ymin)
        if self.ymax!=[]:
          strng = strng+', ymax='+str(self.ymax)
        strng = strng + ", regions="+str(self.regions)
        return strng

    def GetStringDisplayMore(self):
        if self.logX or self.logY: 
            words = '  * Log scale: '
            if self.logX:
                words+= 'logX '
            if self.logY:
                words+= 'logY '
            logging.getLogger('MA5').info(words)
        if self.stack==StackingMethodType.AUTO or\
            self.stack==StackingMethodType.STACK:
            logging.getLogger('MA5').info('  * Stacking method: stacked')
        elif self.stack == StackingMethodType.SUPERIMPOSE:
            logging.getLogger('MA5').info('  * Stacking method: superimposition')
        elif self.stack == StackingMethodType.NORMALIZE2ONE:
            logging.getLogger('MA5').info('  * Stacking method: superimposition + normalization to one')
        if self.titleX !='':
            logging.getLogger('MA5').info('  * X-axis title: ' +  self.titleX)
        if self.titleY !='':
            logging.getLogger('MA5').info('  * Y-axis title: ' +  self.titleY)
        logging.getLogger('MA5').info('  * Particles under consideration: ' +  self.statuscode)
        logging.getLogger('MA5').info('  * Particle ordering: ' +  self.rank)
        return 

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

    def ReplaceAll_Matplotlib(self,word,dico):
        for i,j in dico.iteritems():
            word = word.replace(i,j)
        return word    

    def GetXaxis_Root(self):
        word = self.observable.tlatex + " "
        if len(self.arguments)!=0:
            word += "[ " + self.ReplaceAll(self.GetStringArguments(),self.dicoargs)+ " ] "
        if self.observable.plot_unitX_tlatex!="":
            word += "("+self.observable.plot_unitX_tlatex+") "
        return word    

    def GetXaxis_Matplotlib(self):
        word = "$"+self.observable.tlatex + "$ "
        if len(self.arguments)!=0:
            word += "$[ " + self.ReplaceAll(self.GetStringArguments(),self.dicoargs)+ " ]$ "
        if self.observable.plot_unitX_tlatex!="":
            word += "$("+self.observable.plot_unitX_tlatex+")$ "
        return word    

    def GetYaxis(self):
        word = "Events "

        part_string = self.ReplaceAll(self.GetStringArguments(),self.dicoargs)

        if self.observable.name in ['NPID', 'NAPID']:
            word = 'N. of particles';

        elif self.observable.name in ['DELTAR', 'DPHI_0_PI', 'DPHI_0_2PI', 'RECOIL']:
            if self.GetStringArguments().count('[')!=2 and self.GetStringArguments().count(']')!=2:
                word='N. of (' + part_string + ') pairs'; 

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
                       word = 'N. of ' + part_string + ' pairs '
               elif len(self.arguments[0][0])>2:
                   if self.GetStringArguments().count('[')==len(self.arguments[0][0]) and self.GetStringArguments().count(']')==len(self.arguments[0][0]) and \
                      self.observable.name!='N':
                       word = "Events "
                   else:
                       word = 'N. of ' + part_string + " combinations "
               elif self.GetStringArguments().count('[')==1 and self.GetStringArguments().count(']')==1 and self.observable.name!='N':
                   word = "Events "
               else:
                   word = 'N. of ' + part_string
            elif self.GetStringArguments().count('[')!=1 and self.GetStringArguments().count(']')!=1:
               word = 'N. of ' + part_string
        return word
    

    def GetYaxis_Matplotlib(self):
        word = "$#mathrm{Events}$"

        part_string = self.ReplaceAll_Matplotlib(self.GetStringArguments(),self.dicoargs)

        if self.observable.name in ['NPID', 'NAPID']:
            word = '$#mathrm{N.}# #mathrm{of}# #mathrm{particles}$';

        elif self.observable.name in ['DELTAR', 'DPHI_0_PI', 'DPHI_0_2PI']:
            if self.GetStringArguments().count('[')!=2 and self.GetStringArguments().count(']')!=2:
                word='$#mathrm{N.}# #mathrm{of}# (' + part_string + ')# #mathrm{pairs}$'; 

        elif len(self.arguments)!=0 and self.observable.name!='N':
            # Special case : display 'pair'/'combination' words
            if len(self.arguments)==1 and \
               len(self.arguments[0])==1 and \
               self.observable.args[0] in [ArgumentType.PARTICLE,\
                                           ArgumentType.COMBINATION] and \
               self.arguments[0].SameCombinationNumber():
               if len(self.arguments[0][0])==2: 
                   if self.GetStringArguments().count('[')==2 and self.GetStringArguments().count(']')==2 and self.observable.name!='N':
                       word = "$#mathrm{Events}$"
                   else:
                       word = '$#mathrm{N.} #mathrm{of}# ' + part_string + '# #mathrm{pairs}$'
               elif len(self.arguments[0][0])>2:
                   if self.GetStringArguments().count('[')==len(self.arguments[0][0]) and self.GetStringArguments().count(']')==len(self.arguments[0][0]) and \
                      self.observable.name!='N':
                       word = "$#mathrm{Events}$"
                   else:
                       word = '$#mathrm{N.} #mathrm{of}# ' + part_string + "# #mathrm{combinations}$"
               elif self.GetStringArguments().count('[')==1 and self.GetStringArguments().count(']')==1 and self.observable.name!='N':
                   word = "$#mathrm{Events}$"
               else:
                   word = '$#mathrm{N.}# #mathrm{of}# ' + part_string+'$'
            elif self.GetStringArguments().count('[')!=1 and self.GetStringArguments().count(']')!=1:
               word = '$#mathrm{N.}# #mathrm{of}# ' + part_string+'$'
               
        return word

