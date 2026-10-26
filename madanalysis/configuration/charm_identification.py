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


"""Configuration of the c-jet identification of the SampleAnalyzer jet clusterer."""

from __future__ import absolute_import
from __future__ import annotations
import logging


class CharmIdentification:
    """c-tagging based on the geometrical matching (Delta R) between jets and C hadrons.

    Disabled by default (``cjet_id.status = off``).

    Attributes:
        matching_dr (``float``): Delta R used to match a jet with a C hadron.
        exclusive (``bool``): whether a C hadron can be matched to a single jet only.
        status (``bool``): whether c-tagging is enabled.

    .. note::
        :attr:`userVariables` is a **class** attribute modified by
        :meth:`user_SetParameter`.
    """

    default_matching_dr = 0.5
    default_exclusive = True

    userVariables = {"cjet_id.status": ["on", "off"]}

    def __init__(self) -> None:
        """Initialise the parameters to their default values."""
        self.matching_dr = CharmIdentification.default_matching_dr
        self.exclusive = CharmIdentification.default_exclusive
        self.status = False

    def Display(self) -> None:
        """Log the c-jet identification parameters."""
        logging.getLogger("MA5").info("  + c-jet identification:")
        if self.status:
            self.user_DisplayParameter("cjet_id.matching_dr")
            self.user_DisplayParameter("cjet_id.exclusive")
        else:
            logging.getLogger("MA5").info("    + Disabled")

    def user_DisplayParameter(self, parameter: str) -> None:
        """Log the value of one parameter.

        Args:
            parameter (``str``): full name of the parameter (e.g. ``cjet_id.matching_dr``).
        """
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

    def SampleAnalyzerConfigString(self) -> dict[str, str]:
        """Get the options passed to the SampleAnalyzer jet clusterer.

        Returns:
            ``dict[str, str]``:
            Options ``cjet_id.matching_dr``, ``cjet_id.exclusive``, ``cjet_id.enable_ctagging``.
        """
        return {
            "cjet_id.matching_dr": str(self.matching_dr),
            "cjet_id.exclusive": "1" if self.exclusive else "0",
            "cjet_id.enable_ctagging": "1" if self.status else "0",
        }

    def user_GetValues(self, variable: str) -> list[str]:
        """Get suggested values of a parameter (tab completion).

        Args:
            variable (``str``): full name of the parameter.

        Returns:
            ``list[str]``:
            Suggested values, or an empty list.
        """
        return CharmIdentification.userVariables.get(variable, [])

    def user_GetParameters(self) -> list[str]:
        """Get the names of the currently settable parameters.

        Returns:
            ``list[str]``:
            Keys of :attr:`userVariables`.
        """
        return list(CharmIdentification.userVariables.keys())

    # FIXME: annotated '-> bool' but None is returned on success and for unknown parameters.
    def user_SetParameter(self, parameter: str, value: str) -> bool:
        """Set a c-tagging parameter (``set main.fastsim.cjet_id.<parameter> = <value>``).

        Setting ``cjet_id.status`` to ``on`` enables c-tagging and makes the other
        parameters available for completion.

        Args:
            parameter (``str``): ``cjet_id.status``, ``cjet_id.matching_dr`` or
                ``cjet_id.exclusive``.
            value (``str``): value typed by the user.

        Returns:
            ``bool``:
            ``False`` on error (``None`` on success, see FIXME).
        """
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

        # Enable ctagger
        elif parameter == "cjet_id.status":
            if value.lower() not in ["on", "off"]:
                logging.getLogger("MA5").error("C-Jet tagging status can only be `on` or `off`.")
                return False
            self.status = value.lower() == "on"
            # FIXME: when c-tagging is switched on while the options are already registered, the else
            # branch below resets userVariables to the 'status' entry only.
            if self.status and "cjet_id.matching_dr" not in CharmIdentification.userVariables.keys():
                CharmIdentification.userVariables.update(
                    {
                        "cjet_id.matching_dr": [str(CharmIdentification.default_matching_dr)],
                        "cjet_id.exclusive": ["True", "False"],
                    }
                )
            else:
                CharmIdentification.userVariables = {"cjet_id.status": ["on", "off"]}

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
