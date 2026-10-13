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


"""Sum-PT-based lepton isolation settings (legacy ``set main.isolation`` interface)."""

from __future__ import absolute_import
from __future__ import annotations
import logging
class IsolationSumPt():
    """Isolation based on the scalar sum of the transverse momenta around the lepton.

    Attributes:
        sumPT (``float``): threshold on the sum of the track transverse momenta.
        ET_PT (``float``): threshold on the ratio of calorimeter E_T to lepton p_T.
    """

    default_sumPT  = 1.
    default_ET_PT  = 1.

    userVariables = { "ET_PT"  : [str(default_ET_PT)], \
                      "sumPT"  : [str(default_sumPT)] }

    def __init__(self) -> None:
        """Initialise the thresholds to their default values."""
        self.sumPT = IsolationSumPt.default_sumPT
        self.ET_PT = IsolationSumPt.default_ET_PT

        
    def Display(self) -> None:
        """Log all parameters of the sum-PT isolation."""
        self.user_DisplayParameter("ET_PT")
        self.user_DisplayParameter("sumPT")


    def user_DisplayParameter(self,parameter: str) -> None:
        """Log the value of one parameter.

        Args:
            parameter (``str``): name of the parameter.
        """
        if parameter=='sumPT':
            logging.getLogger('MA5').info('  + sumPT = ' + str(self.sumPT) )
        elif parameter=='ET_PT':
            logging.getLogger('MA5').info('  + ET_PT = ' + str(self.ET_PT) )
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
            return IsolationSumPt.userVariables[variable]
        except:
            return []

    
    def user_GetParameters(self) -> list[str]:
        """Get the names of the settable parameters.

        Returns:
            ``list[str]``:
            Parameter names.
        """
        return list(IsolationSumPt.userVariables.keys())


    def user_SetParameter(self,parameter: str,value: str) -> bool | None:
        """Set a parameter (``set main.isolation.<parameter> = <value>``).

        Args:
            parameter (``str``): ``sumPT`` or ``ET_PT``.
            value (``str``): float value.

        Returns:
            ``bool | None``:
            ``False`` for invalid values, ``None`` otherwise.
        """

        # ET_PT
        if parameter=="ET_PT":
            try:
                tmp=float(value)
                self.ET_PT=tmp
            except:
                logging.getLogger('MA5').error("'"+value+"' is not a float value.")
                return False

        # sumPT
        elif parameter=="sumPT":
            try:
                tmp=float(value)
                self.sumPT=tmp
            except:
                logging.getLogger('MA5').error("'"+value+"' is not a float value.")
                return False

        # other    
        else:
            logging.getLogger('MA5').error("'isolation' has no parameter called '"+parameter+"'")
