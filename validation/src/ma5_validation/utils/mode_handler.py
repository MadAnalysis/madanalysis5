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

from enum import Enum, auto
from typing import Text


class _AutoName(Enum):
    def _generate_next_value_(name, start, count, last_values):
        return name


class MA5Mode(_AutoName):
    """
    MadAnalysis 5 mode handler. Available modes are;

    PARTON
    HADRON
    RECO
    RECOFAC
    EXPERTRECO
    EXPERTHADRON
    EXPERTPARTON
    EXPERT

    See MadAnalysis 5 packege for details.
    """

    PARTON = auto()
    HADRON = auto()
    RECO = auto()
    RECOFAC = auto()
    EXPERTRECO = auto()
    EXPERTHADRON = auto()
    EXPERTPARTON = auto()
    EXPERT = auto()

    @staticmethod
    def get_mode(mode: Text):
        """
        Get mode indicator
        Parameters
        ----------
        mode : Text
            MadAnalysis 5 mode

        Returns
        -------
        Enumerated MadAnalysis Mode

        Raises
        ------
        InvalidMode
            If an unknown mode has been given.
        """
        if mode.upper() not in MA5Mode._member_names_:
            raise InvalidMode(
                f"Unknown mode: {mode}. Available modes are: " + ", ".join(MA5Mode._member_names_)
            )

        else:
            return MA5Mode.__dict__.get(mode.upper(), False)

    @staticmethod
    def get_flag(mode):
        flags = {
            MA5Mode.PARTON: "--partonlevel",
            MA5Mode.HADRON: "--hadronlevel",
            MA5Mode.RECO: "--recolevel",
            MA5Mode.RECOFAC: "--FAC --recolevel",
            MA5Mode.EXPERTRECO: "-Re",
            MA5Mode.EXPERTHADRON: "--hadronlevel -e",
            MA5Mode.EXPERTPARTON: "--partonlevel -e",
            MA5Mode.EXPERT: "-e",
        }

        return flags.get(mode, " ")
