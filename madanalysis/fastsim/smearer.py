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


class Smearer:

    # Initialization
    def __init__(self):
        self.logger = logging.getLogger('MA5');
        self.rules = {}
        self.vars = ['PT','ETA','PHI','E','PX','PY','PZ'] 


    # Adding a rule to the tagger
    # The bounds and function are written as ASTs
    def add_rule(self, id_true, obs, function, bounds):
        ## Checking whether the smearer is supported
        check, id_true = self.is_supported(id_true, obs)
        if not check:
            return
        ## Checking whether the reco/true pair already exists
        key_number=len(self.rules.keys())+1
        for key, value in self.rules.items():
            if value['id_true']==id_true and value['obs']==obs:
                key_number = key
        if not key_number in self.rules.keys():
            self.rules[key_number] = { 'id_true':id_true, 'obs':obs,
              'efficiencies':{} }

        ## Defining a new rule ID for an existing tagger
        eff_key = len(self.rules[key_number]['efficiencies'])+1
        self.rules[key_number]['efficiencies'][eff_key] = { 'function':function,
            'bounds': bounds }


    def display(self, jetrecomode):
        self.logger.info('*********************************')
        self.logger.info('       Smearer information       ')
        self.logger.info('*********************************')
        if self.rules.keys() != []:
            self.logger.info(' - Running in the '+jetrecomode+' reconstruction mode.')
        for key in self.rules.keys():
            myrule = self.rules[key]
            self.logger.info(str(key) + ' - Smearing an object of PDG ' + str(myrule['id_true']) + \
               ' from  the observable ' + str(myrule['obs']))
            for eff_key in myrule['efficiencies'].keys():
                cpp_name = 'eff_'+str(myrule['id_true'])+'_'+str(myrule['obs'])+\
                  '_'+str(eff_key)
                bnd_name = 'bnd_'+str(myrule['id_true'])+'_'+str(myrule['obs'])+\
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


    def is_supported(self,id_true,obs):
        supported = {'e':'11', 'mu':'13', 'ta':'15', 'j':'21', 'a':'22'}
        if not obs in self.vars:
            self.logger.error('Unsupported smearer. The smeared variable must be part of ' + \
              ', '.join(self.vars))
            self.logger.error('Smearer ignored')
            return False, id_true
        if id_true in supported.keys():
            return True, supported[id_true]
        elif id_true in supported.values():
            return True, id_true
        else:
            self.logger.error('Unsupported smearer ('+id_true+'). Only the following objects can be smeared: '\
                 + ', '.join(supported.keys()))
            self.logger.error('Smearer ignored')
            return False, id_true
