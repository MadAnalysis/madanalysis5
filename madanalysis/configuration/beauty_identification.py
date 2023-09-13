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
import logging


class BeautyIdentification:

    default_matching_dr = 0.5
    default_exclusive = True
    default_efficiency = 1.0
    default_misid_cjet = 0.0
    default_misid_ljet = 0.0

    userVariables = {
        "bjet_id.matching_dr": [str(default_matching_dr)],
        "bjet_id.exclusive": ["True", "False"],
    }

    def __init__(self):
        self.matching_dr = BeautyIdentification.default_matching_dr
        self.exclusive = BeautyIdentification.default_exclusive
        self.efficiency = BeautyIdentification.default_efficiency
        self.misid_cjet = BeautyIdentification.default_misid_cjet
        self.misid_ljet = BeautyIdentification.default_misid_ljet

    def Display(self):
        logging.getLogger("MA5").info("  + b-jet identification:")
        self.user_DisplayParameter("bjet_id.matching_dr")
        self.user_DisplayParameter("bjet_id.exclusive")

    def user_DisplayParameter(self, parameter):
        if parameter == "bjet_id.matching_dr":
            logging.getLogger("MA5").info("    + DeltaR matching = " + str(self.matching_dr))
        elif parameter == "bjet_id.exclusive":
            logging.getLogger("MA5").info(
                f"    + exclusive algo = {'true' if self.exclusive else 'false'}"
            )
        else:
            logging.getLogger("MA5").error(
                "'clustering' has no parameter called '" + parameter + "'"
            )

    def SampleAnalyzerConfigString(self):
        return {
            "bjet_id.matching_dr": str(self.matching_dr),
            "bjet_id.exclusive": "1" if self.exclusive else "0",
        }

    def user_GetValues(self, variable):
        return BeautyIdentification.userVariables.get(variable, [])

    def user_GetParameters(self):
        return list(BeautyIdentification.userVariables.keys())

    def user_SetParameter(self, parameter: str, value: str) -> bool:
        # matching deltar
        if parameter == "bjet_id.matching_dr":
            try:
                number = float(value)
            except Exception as err:
                logging.getLogger("MA5").error("the 'matching deltaR' must be a float value.")
                return False
            if number <= 0:
                logging.getLogger("MA5").error("the 'matching deltaR' cannot be negative or null.")
                return False
            self.matching_dr = number

        # exclusive
        elif parameter == "bjet_id.exclusive":
            if value.lower() not in ["true", "false"]:
                logging.getLogger("MA5").error("'exclusive' possible values are : 'true', 'false'")
                return False
            self.exclusive = value == "true"

        # efficiency
        elif parameter == "bjet_id.efficiency":
            logging.getLogger("MA5").error("This function is deprecated; please use the corresponding SFS functionality instead.")
            logging.getLogger("MA5").error("This can be achieved by typing the following command:")
            logging.getLogger("MA5").error(f"     -> define tagger b as b {value}")
            return False

        # mis efficiency (cjet)
        elif parameter == "bjet_id.misid_cjet":
            logging.getLogger("MA5").error("This function is deprecated; please use the corresponding SFS functionality instead.")
            logging.getLogger("MA5").error("This can be achieved by typing the following command:")
            logging.getLogger("MA5").error(f"     -> define tagger b as c {value}")
            return False

        # mis efficiency (ljet)
        elif parameter == "bjet_id.misid_ljet":
            logging.getLogger("MA5").error("This function is deprecated; please use the corresponding SFS functionality instead.")
            logging.getLogger("MA5").error("This can be achieved by typing the following command:")
            logging.getLogger("MA5").error(f"     -> define tagger b as j {value}")
            return False

        # other
        else:
            logging.getLogger("MA5").error(
                "'clustering' has no parameter called '" + parameter + "'"
            )
