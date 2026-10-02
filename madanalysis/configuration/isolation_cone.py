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


"""Cone-based lepton isolation settings (legacy ``set main.isolation`` interface)."""

from __future__ import absolute_import
from __future__ import annotations
import logging
class IsolationCone():
    """Isolation defined by a cone of radius :attr:`radius` around the lepton."""

    default_radius  = 0.5

    userVariables = { "radius" : [str(default_radius)] }


    def __init__(self) -> None:
        """Initialise the cone radius to its default value."""
        self.radius  = IsolationCone.default_radius

        
    def Display(self) -> None:
        """Log all parameters of the cone isolation."""
        self.user_DisplayParameter("radius")


    def user_DisplayParameter(self,parameter: str) -> None:
        """Log the value of one parameter.

        Args:
            parameter (``str``): name of the parameter.
        """
        if parameter=="radius":
            logging.getLogger('MA5').info("  + cone radius = "+str(self.radius))
        else:
            logging.getLogger('MA5').error("'isolation' has no parameter called '"+parameter+"'")

        
    def user_GetValues(self,variable: str) -> list[str]:
        """Get suggested values of a parameter (tab completion).

        Args:
            variable (``str``): name of the parameter.

        Returns:
            ``list[str]``:
            Suggested values, or an empty list.
        """
        try:
            return IsolationCone.userVariables[variable]
        except:
            return []

    
    def user_GetParameters(self) -> list[str]:
        """Get the names of the settable parameters.

        Returns:
            ``list[str]``:
            Parameter names.
        """
        return list(IsolationCone.userVariables.keys())


    def user_SetParameter(self,parameter: str,value: str) -> bool | None:
        """Set a parameter (``set main.isolation.radius = <value>``).

        Args:
            parameter (``str``): ``radius``.
            value (``str``): strictly positive cone radius.

        Returns:
            ``bool | None``:
            ``False`` for invalid values, ``None`` otherwise.
        """
        # radius
        if parameter=="radius":
            try:
                number = float(value)
            except:
                logging.getLogger('MA5').error("the cone radius must be a float value.")
                return False
            if number<=0:
                logging.getLogger('MA5').error("the cone radius cannot be negative or null.")
                return False
            self.radius=number

        # other    
        else:
            logging.getLogger('MA5').error("'isolation' has no parameter called '"+parameter+"'")
