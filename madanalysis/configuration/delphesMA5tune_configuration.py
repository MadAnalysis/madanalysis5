################################################################################
#  
#  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://github.com/MadAnalysis/madanalysis5>
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


from __future__ import absolute_import
from madanalysis.enumeration.ma5_running_type         import MA5RunningType
import os
import logging

class DelphesMA5tuneConfiguration:

    userVariables = { "detector" : ["cms","atlas"],\
                      "output": ["true","false"],\
                      "pileup": ["none"],\
                      "rootfile" : ["none"] }

    def __init__(self):
        self.detector  = "cms"
        self.output    = True
        self.pileup    = ""
        self.card      = ""
        self.rootfile  = ""
        self.SetCard()

    def SetCard(self):
        if self.detector=='cms' and self.pileup=="":
            self.card = "delphesMA5tune_card_CMS.tcl"
        elif self.detector=='cms' and self.pileup!="":
            self.card = "delphesMA5tune_card_CMS_PileUp.tcl"
        elif self.detector=='atlas' and self.pileup=="":
            self.card = "delphesMA5tune_card_ATLAS.tcl"
        elif self.detector=='atlas' and self.pileup!="":
            self.card = "delphesMA5tune_card_ATLAS_PileUp.tcl"
        
    def Display(self):
        self.user_DisplayParameter("detector")
        self.user_DisplayParameter("rootfile")
        self.user_DisplayParameter("output")
        self.user_DisplayParameter("pileup")


    def user_DisplayParameter(self,parameter):
        if parameter=="detector":
            logging.getLogger('MA5').info(" detector : "+self.detector)
            return
        elif parameter=="output":
            if self.output:
                msg="true"
            else:
                msg="false"
            logging.getLogger('MA5').info(" ROOT output : "+msg)
            return
        elif parameter=="rootfile":
            if self.rootfile not in ['', "none"]:
                logging.getLogger('MA5').getLogger('MA5').info(" ROOT outputfile: "+msg)
            return
        elif parameter=="pileup":
            if self.pileup=="":
                msg="none"
            else:
                msg='"'+self.pileup+'"'
            logging.getLogger('MA5').info(" pile-up source = "+msg)

    def SampleAnalyzerConfigString(self):
            mydict = {}
            if self.output:
                mydict['output'] = '1'
            else:
                mydict['output'] = '0'
            if self.rootfile not in ['', 'none']:
                mydict['rootfile'] = self.rootfile
            return mydict

    def user_SetParameter(self,parameter,value,datasets,level):
        
        # algorithm
        if parameter=="detector":

            if value.lower()=="cms":
                self.detector=value
                self.SetCard()
            elif value.lower()=="atlas":
                self.detector=value
                self.SetCard()
            else:
                logging.getLogger('MA5').error("algorithm called '"+value+"' is not found.")
            return

        # output
        elif parameter=="output":

            if value.lower()=="true":
                self.output = True
            elif value.lower()=="false":
                self.output = False
            else:
                logging.getLogger('MA5').error("allowed values for output are: true false")
            return

        elif parameter=="rootfile":
            if value.lower().endswith('root'):
                self.rootfile=os.path.normpath(value)
            else:
                logging.getLogger('MA5').error("Wrong output file format (root file necessary)")
                return False
            return

        # pileup
        elif parameter=="pileup":
            quoteTag=False
            if value.startswith("'") and value.endswith("'"):
                quoteTag=True
            if value.startswith('"') and value.endswith('"'):
                quoteTag=True
            if quoteTag:
                value=value[1:-1]
            valuemin = value.lower()

            # none
            if valuemin=="none":
                self.pileup = ""
                self.SetCard()
                
            # .pileup
            elif valuemin.endswith(".pileup"):
                if not os.path.isfile(value):
                    logging.getLogger('MA5').error('File called "'+value+'" is not found')
                    return
                self.pileup = value
                self.SetCard()
                return

            # other case: error
            else:
                logging.getLogger('MA5').error("The file format for the pile-up source is not known. "+\
                                               "Only files with .pileup extension can be used.")
                return False

        else:
            logging.getLogger('MA5').error("parameter called '"+parameter+"' does not exist")
            return

        
    def user_GetParameters(self):
        return list(DelphesMA5tuneConfiguration.userVariables.keys())


    def user_GetValues(self,variable):
        table = []
        try:
            table.extend(DelphesMA5tuneConfiguration.userVariables[variable])
        except:
            pass
        return table
        
