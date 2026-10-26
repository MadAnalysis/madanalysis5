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


"""Figure of merit displayed in the cut-flow tables of the reports (``set main.fom.*``).
"""

from __future__ import absolute_import
from __future__ import annotations
import logging
class FomConfiguration:
    r"""Figure of merit used to quantify the signal significance.

    Available formulas (``set main.fom.formula = <n>``):

    1. :math:`S/B`
    2. :math:`S/\sqrt{B}`
    3. :math:`S/(S+B)`
    4. :math:`S/\sqrt{S+B}` (default)
    5. :math:`S/\sqrt{S+B+(xB)^2}`
    6. :math:`\sqrt{2}\sqrt{(S+B)\log(1+S/B)-S}`

    Attributes:
        formula (``int``): selected formula (1-6).
        x (``float``): relative background uncertainty used in formula 5.
    """

    userVariables = { "formula" : ['1','2','3','4','5','6'] }

    # 1: S/B
    # 2: S/sqrt(B)
    # 3: S/(S+B)
    # 4: S/sqrt(S+B)
    # 5: S/sqrt(S+B+(xB)**2)
    # 6: sqrt(2)*sqrt((S+B)log(1+S/B)-S)

    allformula = [ 'S/B', 'S/sqrt(B)', 'S/(S+B)', 'S/sqrt(S+B)', 'S/sqrt(S+B+(xB)**2)',\
                   'sqrt(2)*sqrt((S+B)log(1+S/B)-S)']


    def __init__(self) -> None:
        """Initialise the figure of merit to ``S/sqrt(S+B)`` (formula 4) with ``x = 0``.
        """
        self.formula = 4
        self.x       = 0
        self.logger  = logging.getLogger('MA5')

        
    def Display(self) -> None:
        """Log all parameters of the figure of merit."""
        self.user_DisplayParameter("formula")
        self.user_DisplayParameter("x")


    def user_DisplayParameter(self,parameter: str) -> None:
        """Log the value of one parameter.

        Args:
            parameter (``str``): name of the parameter.
        """
        if parameter=="formula":
            self.logger.info(" figure of merit (fom) - formula num "+str(self.formula)+": "+FomConfiguration.allformula[self.formula-1])
        elif parameter=="x" and self.IsX():
            self.logger.info(" x parameter value: "+str(self.x))


    def user_SetParameter(self,parameter: str,value: str) -> bool | None:
        """Set the formula number (1-6) or the ``x`` parameter of formula 5.

        Args:
            parameter (``str``): ``formula`` or ``x``.
            value (``str``): value typed by the user.

        Returns:
            ``bool | None``:
            ``False`` for invalid values, ``None`` otherwise.
        """
        # algorithm
        if parameter=="formula":
            if value in FomConfiguration.userVariables["formula"]:
                valueint = int(value)
                self.formula=valueint
                self.Display()
            else:
                # FIXME: "' '.str(...)" raises AttributeError (and a list cannot be concatenated with '.'):
                # ' '.join(...) was intended.
                self.logger.error("The only possible values for 'formula' are : "+' '.str(FomConfiguration.userVariables["formula"]+'.'))
                return False
        elif parameter=='x' and self.IsX():
            try:
                number = float(value)
            except:
                self.logger.error("The x parameter must be a float value.")
                return False
            if number<0:
                self.logger.error("The x parameter cannont be negative.")
                return False
            self.x=number
        # other    
        else:
            self.logger.error("'formula' has no parameter called '"+parameter+"'")

            
    def IsX(self) -> bool:
        """Check whether the selected formula depends on the ``x`` parameter.

        Returns:
            ``bool``:
            ``True`` for formula 5 (relative background uncertainty ``x``).
        """
        return ('x' in FomConfiguration.allformula[self.formula-1])
    
        
    def user_GetParameters(self) -> list[str]:
        """Get the names of the settable parameters.

        Returns:
            ``list[str]``:
            Parameter names.
        """
        table = list(FomConfiguration.userVariables.keys())
        if self.IsX():
            # NOTE: extend('x') works only because 'x' is a single character (append intended).
            table.extend("x")
        return table


    def user_GetValues(self,variable: str) -> list[str]:
        """Get suggested values of a parameter (tab completion).

        Args:
            variable (``str``): name of the parameter.

        Returns:
            ``list[str]``:
            Suggested values, or an empty list.
        """
        # FIXME: 'table' is unbound (UnboundLocalError) for an unknown variable.
        if variable in list(FomConfiguration.userVariables.keys()):
            table = FomConfiguration.userVariables[variable]
        if self.IsX() and variable=='x':
            table = ['0.']
        return table

    def getFormula(self) -> str:
        """Get the selected formula.

        Returns:
            ``str``:
            E.g. ``"S/sqrt(S+B)"``.
        """
        return FomConfiguration.allformula[self.formula-1]

