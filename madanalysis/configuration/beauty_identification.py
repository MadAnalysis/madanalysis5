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


"""Configuration of the b-jet identification of the SampleAnalyzer jet clusterer."""

from __future__ import absolute_import
from __future__ import annotations
import logging


class BeautyIdentification:
    """b-tagging based on the geometrical matching (Delta R) between jets and B hadrons.

    Attributes:
        matching_dr (``float``): Delta R used to match a jet with a B hadron.
        exclusive (``bool``): whether a B hadron can be matched to a single jet only.
        efficiency / misid_cjet / misid_ljet (``float``): legacy (deprecated) tagging rates.
    """

    default_matching_dr = 0.5
    default_exclusive = True
    default_efficiency = 1.0
    default_misid_cjet = 0.0
    default_misid_ljet = 0.0

    userVariables = {
        "bjet_id.matching_dr": [str(default_matching_dr)],
        "bjet_id.exclusive": ["True", "False"],
    }

    def __init__(self) -> None:
        """Initialise the parameters to their default values."""
        self.matching_dr = BeautyIdentification.default_matching_dr
        self.exclusive = BeautyIdentification.default_exclusive
        self.efficiency = BeautyIdentification.default_efficiency
        self.misid_cjet = BeautyIdentification.default_misid_cjet
        self.misid_ljet = BeautyIdentification.default_misid_ljet

    def Display(self) -> None:
        """Log the b-jet identification parameters."""
        logging.getLogger("MA5").info("  + b-jet identification:")
        self.user_DisplayParameter("bjet_id.matching_dr")
        self.user_DisplayParameter("bjet_id.exclusive")

    def user_DisplayParameter(self, parameter: str) -> None:
        """Log the value of one parameter.

        Args:
            parameter (``str``): full name of the parameter (e.g. ``bjet_id.matching_dr``).
        """
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

    def SampleAnalyzerConfigString(self) -> dict[str, str]:
        """Get the options passed to the SampleAnalyzer jet clusterer.

        Returns:
            ``dict[str, str]``:
            Options ``bjet_id.matching_dr``, ``bjet_id.exclusive``.
        """
        return {
            "bjet_id.matching_dr": str(self.matching_dr),
            "bjet_id.exclusive": "1" if self.exclusive else "0",
        }

    def user_GetValues(self, variable: str) -> list[str]:
        """Get suggested values of a parameter (tab completion).

        Args:
            variable (``str``): full name of the parameter.

        Returns:
            ``list[str]``:
            Suggested values, or an empty list.
        """
        return BeautyIdentification.userVariables.get(variable, [])

    def user_GetParameters(self) -> list[str]:
        """Get the names of the currently settable parameters.

        Returns:
            ``list[str]``:
            Keys of :attr:`userVariables`.
        """
        return list(BeautyIdentification.userVariables.keys())

    # FIXME: annotated '-> bool' but None is returned on success and for unknown parameters.
    def user_SetParameter(self, parameter: str, value: str) -> bool:
        """Set a b-tagging parameter (``set main.fastsim.bjet_id.<parameter> = <value>``).

        The efficiency and mistagging rates are deprecated here and must be defined with SFS
        taggers (``define tagger b as ...``).

        Args:
            parameter (``str``): ``bjet_id.matching_dr`` (Delta R between the jet and the
                B hadron) or ``bjet_id.exclusive``.
            value (``str``): value typed by the user.

        Returns:
            ``bool``:
            ``False`` on error (see FIXME: ``None`` is returned on success).
        """
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
            # FIXME: 'True' passes the check above (lower()) but sets exclusive to False.
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
