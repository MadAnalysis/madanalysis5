################################################################################
#  
#  Copyright (C) 2012-2018 Eric Conte, Benjamin Fuks
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

import logging

class Tagger:

    def __init__(self):
        self.logger = logging.getLogger('MA5')
        self.dico = {}

    def __len__(self):
        return len(self.table)

    def __getitem__(self,i):
        return self.table[i][1]

    def Display(self, args):
        ## what to display
        if len(args) == 0:
            to_display = self.dico.keys()
        else:
            if not (self.module_finder(args) in self.dico.keys()):
                self.logger.error("Cannot display a non-existing module")
                return None
            if self.module_finder(args) == None:
                self.logger.error("Cannot display a non-supported module")
                return None
            to_display = [self.module_finder(args)]

        ## Display
        def PrintEff(title,tag):
            if tag not in self.dico.keys():
                return
            val=self.dico[tag]
            self.logger.info(title)
            for x in val:
                if x[1] !='ALL':
                    self.logger.info('       ' + x[0] + ' for ' + '; '.join(x[1]))
                else:
                    self.logger.info('       ' + x[0])

        if [x for x in to_display if x[0]=='b']!=[]:
            self.logger.info(" ****************** B-tagging ******************" )
            PrintEff("Tagging efficiency",        'bTagEff')
            PrintEff("C-jet mistagging rate",     'bMistagC')
            PrintEff("Light-jet mistagging rate", 'bMistagLight')
        if [x for x in to_display if x[0]=='c']!=[]:
            self.logger.info(" ****************** C-tagging ******************" )
            PrintEff("Tagging efficiency", 'cTagEff')
            PrintEff("Mistagging rate",    'cMistag')
        if [x for x in to_display if x.startswith('tau')]!=[]:
            self.logger.info(" ****************** Tau-tagging ******************" )
            PrintEff("Tagging efficiency", 'tauTagEff')
            PrintEff("Mistagging rate",    'tauMistag')
#        for elem in sorted(to_display):
        print "FIX EFFICIENCIES"
#            self.SetEfficiency.user_DisplayRules(module)

    def Add(self,mod,fct,bounds):
        ## Checking bounds
        if not mod in self.dico.keys():
            self.dico[mod]=[[fct, bounds]]
        else:
            self.dico[mod].append([fct, bounds])

    def module_finder(self,args):
        ## Security
        if len(args)!=2:
            return None

        ## Getting the PDG codes
        try:
            arg0 = int(args[0])
            arg1 = int(args[1])
        except:
            return None

        ## Is it an alowed tagger?
        if not arg0 in [1,2,3,4,5,21,15]:
            return None
        if arg0==15 and arg1!=15:
            return None
        elif arg0 in [1,2,3,4,5,21] and arg1 not in [4,5,15]:
            return None

        ### Find the module
        if   arg0==5 and arg1==5:
                return 'bTagEff'
        elif arg0==4 and arg1==5:
                return 'bMistagC'
        elif arg0 in [1,2,3,21] and arg1==5:
                return 'bMistagLight'
        elif arg0==4 and arg1==4:
                return 'cTagEff'
        elif arg0 in [1,2,3,21] and arg1==4:
                return 'cMistag'
        elif arg0==15 and arg1==15:
                return 'tauTagEff'
        elif arg0 in [1,2,3,4,21] and arg1==15:
                return 'tauMistag'
        else:
            return None



