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


from madanalysis.selection.instance_name      import InstanceName
from madanalysis.enumeration.combination_type import CombinationType
from madanalysis.enumeration.observable_type  import ObservableType
from madanalysis.enumeration.operator_type    import OperatorType
from madanalysis.enumeration.connector_type   import ConnectorType
from madanalysis.enumeration.cut_type         import CutType
from madanalysis.selection.condition_type     import ConditionType
from madanalysis.selection.condition_sequence import ConditionSequence
from madanalysis.enumeration.ma5_running_type import MA5RunningType

import logging

class Cut():

    userVariables = { "threshold"  : [], \
                      "rank"       : ["Eordering","Pordering","PTordering","ETordering","PXordering","PYordering","PZordering","ETAordering"], \
                      "statuscode" : ["finalstate","interstate","allstate","initialstate"]}

    userShortcuts = {"finalstate":   ["statuscode","finalstate"], \
                     "interstate":   ["statuscode","interstate"], \
                     "allstate":     ["statuscode","allstate"], \
                     "initialstate": ["statuscode","initialstate"], \
                     "Eordering":    ["rank",      "Eordering"], \
                     "Pordering":    ["rank",      "Pordering"], \
                     "PTordering":   ["rank",      "PTordering"], \
                     "ETordering":   ["rank",      "ETordering"], \
                     "PXordering":   ["rank",      "PXordering"], \
                     "PYordering":   ["rank",      "PYordering"], \
                     "PZordering":   ["rank",      "PZordering"], \
                     "ETAordering":  ["rank",      "ETAordering"] }

    def __init__(self,part,conditions,cut_type):
        import copy
        self.part       = part
        self.conditions = conditions
        self.rank       = "PTordering"
        self.statuscode = "finalstate"
        self.cut_type   = cut_type
        
    def user_GetParameters(self):
        return Cut.userVariables.keys()

    def user_GetShortcuts(self):
        return Cut.userShortcuts.keys()

    def user_GetValues(self,variable):
        try:
            return Cut.userVariables[variable]
        except:
            return []

    def user_SetShortcuts(self,name):
        if name in Cut.userShortcuts.keys():
            return self.user_SetParameter(Cut.userShortcuts[name][0],Cut.userShortcuts[name][1])
        else:
            logging.error("option '" + name + "' is unknown.")
            return False

    def user_SetParameter(self,variable,value):
        # rank
        if variable == "rank":
            if value in Cut.userVariables["rank"]:
                self.rank=value
            else:
                logging.error("'"+value+"' is not a possible value for the variable 'rank'.")
                return False
            
        # statuscode
        elif variable == "statuscode":
            if value in Cut.userVariables["statuscode"]:
                self.statuscode=value
            else:
                logging.error("'"+value+"' is not a possible value for the variable 'statuscode'.")
                return False

        else:
            logging.error("variable called '"+variable+"' is unknown")
            return False

        return True

    def user_DisplayParameter(self,variable):
        if variable=="rank":
            logging.info(" rank = "+self.rank)
        elif variable=="statuscode":
            logging.info(" statuscode = "+self.statuscode) 
        else:
            logging.error("no variable called '"+variable+"' is found")
                            

    def Display(self):
        logging.info(self.GetStringDisplay())
        logging.info(self.GetStringDisplay2())

    def GetStringDisplay(self):
        msg = "Cut: "

        # displaying command
        msg += CutType.convert2cmdname(self.cut_type)

        # displaying particles
        if len(self.part)!=0:
            msg += " ( " + self.part.GetStringDisplay()+" )"

        # displaying conditions
        msg += " "+self.conditions.GetStringDisplay()

        return msg 

    def DoYouUseMultiparticle(self,name):
        return self.part.DoYouUseMultiparticle(name)

    def GetStringDisplay2(self):
        return "Cut: "

