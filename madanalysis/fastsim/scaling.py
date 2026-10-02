################################################################################
#  
#  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


"""Energy/momentum scaling of the SFS (``define jes``, ``define energy_scaling``, ``define scaling``).
"""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any
import logging
class Scaling:
    """Scaling functions applied to reconstructed objects (e.g. jet energy scale).

    Each rule is stored in :attr:`rules` as ``{key: {'id_true', 'obs', 'efficiencies': {n: {'function':
    AST, 'bounds': AST}}}}``: several (function, bounds) pairs can be attached to the
    same object and observable, each applying in its own domain.
    """

    # Initialization
    def __init__(self) -> None:
        """Create an empty set of rules."""
        self.logger    = logging.getLogger('MA5');
        self.rules     = {}
        self.vars      = ['PT','ETA','PHI','E','PX','PY','PZ']


    # Adding a rule to the tagger
    # The bounds and function are written as ASTs
    # JES option is to make sure that the jet scaling is only applied on 
    # clustered jets
    def add_rule(self, id_true: str, obs: str, function: Any, bounds: Any) -> None:
        """Add a scaling rule.

        Args:
            id_true (``str``): PDG code or label of the object (``JES`` for the jet energy
                scale applied to clustered jets).
            obs (``str``): scaled observable (``PT``, ``ETA``, ``PHI``, ``E``, ``PX``, ``PY``, ``PZ``).
            function (``AST``): scaling formula.
            bounds (``AST``): domain of validity.
        """
        ## Checking whether the smearer is supported
        check, id_true = self.is_supported(id_true, obs)
        if not check:
            return
        ## Checking whether the reco/true pair already exists
        key_number=len(list(self.rules.keys()))+1
        for key, value in self.rules.items():
            if value['id_true']==id_true and value['obs']==obs:
                key_number = key
        if not key_number in list(self.rules.keys()):
            self.rules[key_number] = { 'id_true':id_true, 'obs':obs,
              'efficiencies':{}}

        ## Defining a new rule ID for an existing tagger
        eff_key = len(self.rules[key_number]['efficiencies'])+1
        self.rules[key_number]['efficiencies'][eff_key] = { 'function':function,
            'bounds': bounds }


    def display(self, jetrecomode: str) -> None:
        """Log the defined scaling rules (the C++ translation is logged at debug level).

        Args:
            jetrecomode (``str``): jet reconstruction mode (``jets`` or ``constituents``).
        """
        self.logger.info('*********************************')
        self.logger.info('       Scaling information       ')
        self.logger.info('*********************************')
        if list(self.rules.keys()) != []:
            self.logger.info(' - Running in the '+jetrecomode+' reconstruction mode.')
        for key in self.rules.keys():
            myrule = self.rules[key]
            if myrule['id_true'] == 'JES':
                self.logger.info(str(key) + ' - Jet Energy Scaling')
            else:
                self.logger.info(str(key) + ' - Scaling an object of PDG ' + str(myrule['id_true']) + \
                   ' from  the observable ' + str(myrule['obs']))
            for eff_key in myrule['efficiencies'].keys():
                cpp_name = 'scale_'+str(myrule['id_true'])+'_'+str(myrule['obs'])+\
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


    def is_supported(self,id_true: str,obs: str) -> tuple[bool, str]:
        """Check whether an object and observable can be scaled.

        Args:
            id_true (``str``): PDG code or label of the object.
            obs (``str``): observable.

        Returns:
            ``tuple[bool, str]``:
            Whether the scaling is supported, and the PDG code of the object.
        """
        supported = {'e':'11', 'mu':'13', 'ta':'15', 'j':'21', 'a':'22', 'track':'track', 'JES':'JES'}
        if not obs in self.vars:
            self.logger.error('Unsupported scaling. The scaling variable must be part of ' + \
              ', '.join(self.vars))
            self.logger.error('Scaling ignored')
            return False, id_true
        if id_true in list(supported.keys()):
            return True, supported[id_true]
        elif id_true in list(supported.values()):
            return True, id_true
        else:
            self.logger.error('Unsupported scaling ('+id_true+'). Only the following objects can be scaled: '\
                 + ', '.join(list(supported.keys())))
            self.logger.error('Scaling ignored')
            return False, id_true
