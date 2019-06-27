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


import logging


class Tagger:

    # Initialization
    def __init__(self):
        self.logger = logging.getLogger('MA5');
        self.rules = {}


    # Adding a rule to the tagger
    # The bounds and function are written as ASTs
    def add_rule(self,id_true, id_reco, function, bounds):
        ## Checking wether the tagger is supported
        if not self.is_supported(id_true, id_reco):
            return
        ## Checking whether the reco/true pair already exists
        key_number=len(self.rules.keys())+1
        for key, value in self.rules.items():
            if value['id_true']==id_true and value['id_reco']==id_reco:
                key_number = key
        if not key_number in self.rules.keys():
            self.rules[key_number] = { 'id_true':id_true, 'id_reco':id_reco,
              'efficiencies':{} }

        ## Defining a new rule ID for an existing tagger
        eff_key = len(self.rules[key_number]['efficiencies'])+1
        self.rules[key_number]['efficiencies'][eff_key] = { 'function':function,
            'bounds': bounds }


    def display(self):
        self.logger.info('*********************************')
        self.logger.info('       Tagger  information       ')
        self.logger.info('*********************************')
        for key in self.rules.keys():
            myrule = self.rules[key]
            self.logger.info(str(key) + ' - Tagging a true PDG-' + str(myrule['id_true']) + \
               ' as a PDG-' + str(myrule['id_reco']))
            for eff_key in myrule['efficiencies'].keys():
                cpp_name = 'eff_'+str(myrule['id_true'])+'_'+str(myrule['id_reco'])+\
                  '_'+str(eff_key)
                bnd_name = 'bnd_'+str(myrule['id_true'])+'_'+str(myrule['id_reco'])+\
                  '_'+str(eff_key)
                myeff = myrule['efficiencies'][eff_key]
                self.logger.info('  ** function: ' + myeff['function'].tostring())
                self.logger.info('  ** bounds:   ' + myeff['bounds'].tostring())
                self.logger.debug(' C++ version for the function: \n        '  + \
                   myeff['function'].tocpp('MAdouble64', cpp_name).replace('\n','\n        '))
                self.logger.debug(' C++ version for the bounds: \n        '  + \
                   myeff['bounds'].tocpp('MAbool', bnd_name).replace('\n','\n        '))
                self.logger.info('  --------------------')
            self.logger.info('  --------------------')


    def is_supported(self,id_true, id_reco):
        supported = { '5':['21','4','5'], '4':['21','4','5'], '15':['15','21'],
                      '21' : ['11','13','22'], '11' : ['13','22'],
                      '13' : ['11','22'],      '22' : ['11','13']}
        if id_reco not in supported.keys() or id_true not in supported[id_reco]:
            self.logger.error('This tagger is currently not supported (tagging '+ id_true + ' as ' + id_reco + '). Tagger ignored.')
            return False
        return True
