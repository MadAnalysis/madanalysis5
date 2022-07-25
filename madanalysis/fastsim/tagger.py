################################################################################
#  
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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
import logging
from typing import Any, Optional
from enum import Enum, auto

AST = Any

class TaggerStatus(Enum):
    NONE = auto()
    LOOSE = auto()
    MID = auto()
    TIGHT = auto()

    @staticmethod
    def get_status(status: str):
        """Convert string to tagger criterion"""
        if status.lower() == "loose":
            return TaggerStatus.LOOSE
        elif status.lower() in ["med", "mid", "medium"]:
            return TaggerStatus.MID
        elif status.lower() == "tight":
            return TaggerStatus.TIGHT
        else:
            return TaggerStatus.NONE

    @staticmethod
    def to_str(status):
        if status == TaggerStatus.LOOSE:
            return "loose"
        elif status == TaggerStatus.MID:
            return "medium"
        elif status == TaggerStatus.TIGHT:
            return "tight"
        else:
            return ""


class Tagger:
    # Initialization
    def __init__(self):
        self.logger = logging.getLogger('MA5')
        self.rules = {}

    def add_rule(self, id_true: str, id_reco: str, function: AST, bounds: AST, tag: Optional[TaggerStatus] = None) -> None:
        """
        Adding a rule to the tagger. The bounds and function are written as ASTs

        :param id_true: true particle id
        :param id_reco: particle id to be reconstructed
        :param function: efficiency function
        :param bounds: bounds of the function
        :param tag: loose/medium/tight criterion of the tagger
        :return:
        """

        ## Checking wether the tagger is supported
        if not self.is_supported(id_true, id_reco, tag):
            return
        ## Default tag for jets and taus is loose tag. If tag is not given convert to loose
        if id_reco in ["21", "4", "5", "15"] and id_true in ["21", "4", "5", "15"]:
            tag = tag if tag != TaggerStatus.NONE else TaggerStatus.LOOSE
        ## Checking whether the reco/true pair already exists
        key_number = len(list(self.rules.keys())) + 1
        for key, value in self.rules.items():
            if value['id_true'] == id_true and value['id_reco'] == id_reco and value["tag"] == tag:
                key_number = key
        if not key_number in list(self.rules.keys()):
            self.rules[key_number] = dict(
                id_true=id_true,
                id_reco=id_reco,
                efficiencies=dict(),
                tag=tag
            )

        ## Defining a new rule ID for an existing tagger
        eff_key = len(self.rules[key_number]['efficiencies']) + 1
        self.rules[key_number]['efficiencies'][eff_key] = {'function': function,
                                                           'bounds': bounds}


    def display(self):
        self.logger.info('*********************************')
        self.logger.info('       Tagger  information       ')
        self.logger.info('*********************************')
        for key in self.rules.keys():
            myrule = self.rules[key]
            self.logger.info(f"{key} - Tagging a true PDG-{myrule['id_true']} as a PDG-{myrule['id_reco']}" \
                             + (myrule["tag"] != TaggerStatus.NONE)*f" with {TaggerStatus.to_str(myrule['tag'])} tag.")
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


    def is_supported(self,id_true: str, id_reco: str, tag: TaggerStatus):
        supported = {'5': ['21', '4', '5'], '4': ['21', '4', '5'], '15': ['15', '21'],
                     '21': ['11', '13', '22'], '11': ['13', '22', '21'],
                     '13': ['11', '22'], '22': ['11', '13', '21']}
        if id_reco not in list(supported.keys()) or id_true not in supported[id_reco]:
            self.logger.error(
                f"This tagger is currently not supported (tagging {id_true} as {id_reco}). Tagger ignored."
            )
            return False
        if tag in [TaggerStatus.LOOSE, TaggerStatus.MID, TaggerStatus.TIGHT]:
            if id_reco not in ["21", "4", "5", "15"] or id_true not in ["21", "4", "5", "15"]:
                self.logger.error(
                    f"This tagger is currently not supported. {id_true} can not be tagged as {id_reco} with "
                    f"{TaggerStatus.to_str(tag)} tag."
                )
                return False
        return True
