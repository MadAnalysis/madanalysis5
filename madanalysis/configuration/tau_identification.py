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


class TauIdentification:

    default_matching_dr = 0.3
    default_exclusive = False
    default_reconstruction_method = "hadron-based"

    userVariables = {
        "tau_id.reconstruction_method": ["jet-based", "hadron-based"],
    }

    def __init__(self):
        self.matching_dr = TauIdentification.default_matching_dr
        self.reconstruction_method = TauIdentification.default_reconstruction_method
        self.exclusive = TauIdentification.default_exclusive

    def Display(self):
        logging.getLogger("MA5").info("  + hadronic-tau identification:")
        if self.reconstruction_method == "jet-based":
            self.user_DisplayParameter("tau_id.matching_dr")
        self.user_DisplayParameter("tau_id.reconstruction_method")

    def user_DisplayParameter(self, parameter):
        if parameter == "tau_id.matching_dr":
            logging.getLogger("MA5").info(f"    + DeltaR matching = {self.matching_dr:.2f}")
        elif parameter == "tau_id.reconstruction_method":
            logging.getLogger("MA5").info(
                f"    + Reconstruction method: {self.reconstruction_method}"
            )
        elif parameter == "tau_id.exclusive":
            logging.getLogger("MA5").info(
                f"    + exclusive algo = {'true' if self.exclusive else 'false'}"
            )
        else:
            logging.getLogger("MA5").error(
                "'clustering' has no parameter called '" + parameter + "'"
            )

    def SampleAnalyzerConfigString(self):
        return {
            "tau_id.matching_dr": str(self.matching_dr),
            "tau_id.reconstruction_method": "1" if self.reconstruction_method == "jet-based" else "0",
            "tau_id.exclusive": "1" if self.exclusive else "0",
        }

    def user_GetValues(self, variable):
        return TauIdentification.userVariables.get(variable, [])

    def user_GetParameters(self):
        return list(TauIdentification.userVariables.keys())

    def user_SetParameter(self, parameter, value):
        # matching deltar
        if parameter == "tau_id.matching_dr":
            try:
                number = float(value)
            except:
                logging.getLogger("MA5").error("the 'matching deltaR' must be a float value.")
                return False
            if number <= 0:
                logging.getLogger("MA5").error("the 'matching deltaR' cannot be negative or null.")
                return False
            if self.reconstruction_method == "hadron-based":
                logging.getLogger("MA5").warning("Hadronic tau matching is only available in jet-based tagging mode.")
                logging.getLogger("MA5").warning("To activate jet-based tagging type "
                                                 "`set main.fastsim.tau_id.reconstruction_method = jet-based`")
            self.matching_dr = number

        # Exclusive algorithm
        elif parameter == "tau_id.exclusive":
            if value not in ["true", "false"]:
                logging.getLogger('MA5').error("'exclusive' possible values are : 'true', 'false'")
                return False
            if self.reconstruction_method == "hadron-based":
                logging.getLogger("MA5").warning("Exclusive Hadronic tau matching is only available "
                                                 "in jet-based tagging mode.")
                logging.getLogger("MA5").warning("To activate jet-based tagging type "
                                                 "`set main.fastsim.tau_id.reconstruction_method = jet-based`")
            self.exclusive = (value == "true")

        # reconstruction method
        if parameter == "tau_id.reconstruction_method":
            if value in TauIdentification.userVariables["tau_id.reconstruction_method"]:
                self.reconstruction_method = value
            else:
                logging.getLogger("MA5").error(
                    "Available reconstruction methods are: "
                    + ", ".join(TauIdentification.userVariables["tau_id.reconstruction_method"])
                )
                return False
            if self.reconstruction_method == "jet-based":
                TauIdentification.userVariables.update(
                    {"tau_id.matching_dr": [str(TauIdentification.default_matching_dr)],
                     "tau_id.exclusive": ["True", "False"]}
                )
            else:
                TauIdentification.userVariables = {
                    "tau_id.reconstruction_method": ["jet-based", "hadron-based"]
                }

        # efficiency
        elif parameter == "tau_id.efficiency":
            logging.getLogger("MA5").error(
                "This function has been deprecated, please use SFS functionality instead."
            )
            logging.getLogger("MA5").error(
                "Same functionality can be captured via following command in SFS:"
            )
            logging.getLogger("MA5").error(f"     -> define tagger ta as ta {value}")
            return False

        # mis efficiency (ljet)
        elif parameter == "tau_id.misid_ljet":
            logging.getLogger("MA5").error(
                "This function has been deprecated, please use SFS functionality instead."
            )
            logging.getLogger("MA5").error(
                "Same functionality can be captured via following command in SFS:"
            )
            logging.getLogger("MA5").error(f"     -> define tagger ta as j {value}")
            return False

        # other
        else:
            logging.getLogger("MA5").error(
                "'clustering' has no parameter called '" + parameter + "'"
            )
