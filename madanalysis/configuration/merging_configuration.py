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


"""Configuration of the jet-merging validation plots (``set main.merging.*``)."""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any
from madanalysis.configuration.clustering_kt          import ClusteringKt
from madanalysis.configuration.clustering_antikt      import ClusteringAntiKt
from madanalysis.configuration.clustering_genkt       import ClusteringGenKt
from madanalysis.configuration.clustering_cambridge   import ClusteringCambridge
from madanalysis.configuration.clustering_gridjet     import ClusteringGridJet
from madanalysis.configuration.clustering_cdfmidpoint import ClusteringCDFMidPoint
from madanalysis.configuration.clustering_cdfjetclu   import ClusteringCDFJetClu
from madanalysis.configuration.clustering_siscone     import ClusteringSisCone
from madanalysis.enumeration.ma5_running_type         import MA5RunningType
import logging

class MergingConfiguration:
    """Differential jet rate (DJR) plots used to validate matrix-element/parton-shower merging.

    Attributes:
        enable (``bool``): whether the merging plots are produced (``check``).
        njets (``int``): maximum number of jets considered.
        ma5_mode (``bool``): use the MadAnalysis 5 implementation of the DJR computation.
    """

    userVariables = { "check"    : ["true","false"],\
                      "njets"    : ["4"],\
                      "ma5_mode" : ["true","false"]}

    def __init__(self) -> None:
        """Disable the merging plots (``njets = 4``)."""
        self.enable = False
        self.njets  = 4
        self.ma5_mode = False
        
    def Display(self) -> None:
        """Log the merging-plot settings."""
        self.user_DisplayParameter("check")
        if self.enable:
            self.user_DisplayParameter("njets")
            self.user_DisplayParameter("ma5_mode")

        
    def user_DisplayParameter(self,parameter: str) -> None:
        """Log the value of one parameter.

        Args:
            parameter (``str``): ``check``, ``njets`` or ``ma5_mode``.
        """
        if parameter=="check":
            if self.enable:
                value="true"
            else:
                value="false"
            logging.getLogger('MA5').info(" enabling merging plots : "+value)
        elif parameter=="njets":
            logging.getLogger('MA5').info("  + njets = "+str(self.njets))
        elif parameter=="ma5_mode":
            if self.ma5_mode:
                value="true"
            else:
                value="false"
            logging.getLogger('MA5').info(" ma5 mode : "+value)
        else:
            logging.getLogger('MA5').error("'merging' has no parameter called '"+parameter+"'")
            return


    def user_SetParameter(self,parameter: str,value: str,level: int,fastjet: bool) -> None:
        """Set a merging-plot parameter.

        Enabling the plots requires the hadron or reco mode and FastJet. Errors are logged.

        Args:
            parameter (``str``): ``check``, ``njets`` or ``ma5_mode``.
            value (``str``): value typed by the user.
            level (``int``): running mode.
            fastjet (``bool``): whether FastJet is available.
        """
        # enable
        if parameter=="check":
            if value=="true":
                # Only in reco mode
                if level==MA5RunningType.PARTON:
                    # NOTE: misleading message: this concerns the merging plots, not the clustering.
                    logging.getLogger('MA5').error("clustering algorithm is only available in HADRON or RECO mode")
                    return
                
                # Fastjet ?
                if not fastjet:
                    logging.getLogger('MA5').error("fastjet library is not installed. Merging plots not available.")
                    return
                
                self.enable=True
            elif value=="false":
                self.enable=False
            else:
                logging.getLogger('MA5').error("only possible values are 'true' and 'false'")
                return

        # enable
        elif parameter=="ma5_mode":
            if value=="true":
                self.ma5_mode=True
            elif value=="false":
                self.ma5_mode=False
            else:
                logging.getLogger('MA5').error("only possible values are 'true' and 'false'")
                return

        # njets
        elif parameter=="njets":
            try:
                njets = int(value)
            except:
                logging.getLogger('MA5').error("the value for 'njets' must be a positive non-null integer.")
                return
            if njets<=0:
                logging.getLogger('MA5').error("the value for 'njets' cannot be negative or null.")
                return
            self.njets=njets

        # other
        else:
            logging.getLogger('MA5').error("no parameter called '"+parameter+"' is found.")
            return

        
    def user_GetParameters(self) -> list[str]:
        """Get the names of the settable parameters.

        Returns:
            ``list[str]``:
            ``["check", "njets", "ma5_mode"]``.
        """
        return list(MergingConfiguration.userVariables.keys())


    def user_GetValues(self,variable: str) -> list[str]:
        """Get suggested values of a parameter (tab completion).

        Args:
            variable (``str``): name of the parameter.

        Returns:
            ``list[str]``:
            Suggested values, or an empty list.
        """
        try:
            return MergingConfiguration.userVariables[variable]
        except:
            return []
    
        
