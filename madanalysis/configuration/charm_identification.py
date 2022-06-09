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


class CharmIdentification:

    default_matching_dr = 0.5
    default_exclusive = True

    userVariables = {
        "cjet_id.matching_dr": [str(default_matching_dr)],
        "cjet_id.exclusive": [str(default_exclusive)],
    }

    def __init__(self):
        self.matching_dr = CharmIdentification.default_matching_dr
        self.exclusive = CharmIdentification.default_exclusive

    def Display(self):
        logging.getLogger("MA5").info("  + b-jet identification:")
        self.user_DisplayParameter("cjet_id.matching_dr")
        self.user_DisplayParameter("cjet_id.exclusive")

    def user_DisplayParameter(self, parameter):
        if parameter == "cjet_id.matching_dr":
            logging.getLogger("MA5").info("    + DeltaR matching = " + str(self.matching_dr))
        elif parameter == "cjet_id.exclusive":
            logging.getLogger("MA5").info(
                f"    + exclusive algo = {'true' if self.exclusive else 'false'}"
            )
        else:
            logging.getLogger("MA5").error(
                "'clustering' has no parameter called '" + parameter + "'"
            )

    def SampleAnalyzerConfigString(self):
        return {
            "cjet_id.matching_dr": str(self.matching_dr),
            "cjet_id.exclusive": "1" if self.exclusive else "0",
        }

    def user_GetValues(self, variable):
        try:
            return CharmIdentification.userVariables[variable]
        except:
            return []

    def user_GetParameters(self):
        return list(CharmIdentification.userVariables.keys())

    def user_SetParameter(self, parameter, value):
        # matching deltar
        if parameter == "cjet_id.matching_dr":
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
        elif parameter == "cjet_id.exclusive":
            if value not in ["true", "false"]:
                logging.getLogger("MA5").error("'exclusive' possible values are : 'true', 'false'")
                return False
            self.exclusive = value == "true"

        # other
        else:
            logging.getLogger("MA5").error(
                "'clustering' has no parameter called '" + parameter + "'"
            )
