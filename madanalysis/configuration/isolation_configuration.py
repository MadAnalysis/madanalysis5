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


"""Lepton isolation configuration (``set main.isolation.*``)."""

from __future__ import absolute_import
from __future__ import annotations
from madanalysis.configuration.isolation_cone  import IsolationCone
from madanalysis.configuration.isolation_sumpt import IsolationSumPt
import logging

class IsolationConfiguration():
    """Selection of the isolation algorithm.

    Attributes:
        algorithm (``str``): ``"cone"`` or ``"sumpt"``.
        isolation (``IsolationCone | IsolationSumPt``): parameters of the algorithm.
    """


    userVariables = { "algorithm" : ["cone","sumpt"] }

    def __init__(self) -> None:
        """Initialise a cone isolation."""
        self.algorithm = 'cone'
        self.isolation = IsolationCone()

    def Display(self) -> None:
        """Log all parameters of the isolation algorithm."""
        self.user_DisplayParameter("algorithm")
        self.isolation.Display()

    def user_DisplayParameter(self,parameter: str) -> None:
        """Log the value of one parameter.

        Args:
            parameter (``str``): name of the parameter.
        """
        if parameter=="algorithm":
            logging.getLogger('MA5').info(" isolation algorithm : "+self.algorithm)
        else:
            self.isolation.user_DisplayParameter(parameter)

    def user_GetParameters(self) -> list[str]:
        """Get the names of the settable parameters.

        Returns:
            ``list[str]``:
            Parameter names.
        """
        table = list(IsolationConfiguration.userVariables.keys())
        table.extend(self.isolation.user_GetParameters())
        return table


    def user_GetValues(self,variable: str) -> list[str]:
        """Get suggested values of a parameter (tab completion).

        Args:
            variable (``str``): name of the parameter.

        Returns:
            ``list[str]``:
            Suggested values, or an empty list.
        """
        table = []
        try:
            table.extend(IsolationConfiguration.userVariables[variable])
        except:
            pass
        try:
            table.extend(self.isolation.user_GetValues(variable))
        except:
            pass
        return table

    def user_SetParameter(self,parameter: str,value: str) -> bool | None:
        """Set the isolation algorithm (``cone`` or ``sumpt``) or one of its parameters.

        Args:
            parameter (``str``): ``algorithm`` or a parameter of the selected algorithm.
            value (``str``): value typed by the user.

        Returns:
            ``bool | None``:
            ``False`` for an invalid algorithm, ``None`` otherwise.
        """
        # algorithm
        if parameter=="algorithm":
            if value=='cone':
                self.algorithm='cone'
                self.isolation=IsolationCone()
            elif value=='sumpt':
                self.algorithm='sumpt'
                self.isolation=IsolationSumPt()
            else:
                logging.getLogger('MA5').error("Specified algorithm for muon isolation is not correct. "+\
                              "Only possible ones are : 'cone' and 'sumpt'.")
                return False
        # other    
        else:
            # NOTE: the result of the sub-setter is not returned.
            self.isolation.user_SetParameter(parameter,value)
