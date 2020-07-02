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


class RecoEfficiency:

    # Initialization
    def __init__(self):
        self.logger = logging.getLogger('MA5');
        self.rules = {}


    # Adding a rule to the reconstruction efficiencies
    # The bounds and function are written as ASTs
    def add_rule(self, id_reco, function, bounds):
        ## Checking wether the object is supported
        if not self.is_supported(id_reco):
            return
        ## Checking whether the reco/true pair already exists
        key_number=len(self.rules.keys())+1
        for key, value in self.rules.items():
            if value['id_reco']==id_reco:
                key_number = key
        if not key_number in self.rules.keys():
            self.rules[key_number] = { 'id_reco':id_reco, 'efficiencies':{} }

        ## Defining a new rule ID for an existing tagger
        eff_key = len(self.rules[key_number]['efficiencies'])+1
        self.rules[key_number]['efficiencies'][eff_key] = { 'function':function,
            'bounds': bounds }


    def display(self):
        self.logger.info('************************************************')
        self.logger.info(' Information on the reconstruction efficiencies ')
        self.logger.info('************************************************')
        for key in self.rules.keys():
            myrule = self.rules[key]
            self.logger.info(str(key) + ' - Efficiency to reconstruct an object of PDG-' + str(myrule['id_reco']))
            for eff_key in myrule['efficiencies'].keys():
                cpp_name = 'reco_'+str(myrule['id_reco'])+'_'+str(eff_key)
                bnd_name = 'reco_bnd_'+str(myrule['id_reco'])+'_'+str(eff_key)
                myeff = myrule['efficiencies'][eff_key]
                self.logger.info('  ** function: ' + myeff['function'].tostring())
                self.logger.info('  ** bounds:   ' + myeff['bounds'].tostring())
                self.logger.debug(' C++ version for the function: \n        '  + \
                   myeff['function'].tocpp('MAdouble64', cpp_name).replace('\n','\n        '))
                self.logger.debug(' C++ version for the bounds: \n        '  + \
                   myeff['bounds'].tocpp('MAbool', bnd_name).replace('\n','\n        '))
                self.logger.info('  --------------------')
            self.logger.info('  --------------------')


    def is_supported(self, id_reco):
        supported = {'e':'11', 'mu':'13', 'ta':'15', 'j':'21', 'a':'22'}
        if id_reco in (supported.keys()+supported.values()):
            return True
        self.logger.error('The reconstruction of such an object (' + id_reco + ') is currently not supported.'+\
                              ' Reconstruction ignored.')
        return False
