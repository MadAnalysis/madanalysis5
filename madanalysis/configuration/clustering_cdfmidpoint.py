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


"""Configuration of the CDF MidPoint jet-clustering algorithm (``set main.fastsim.<parameter>``).
"""

from __future__ import absolute_import
from __future__ import annotations
import logging
class ClusteringCDFMidPoint():
    """Parameters of the CDF MidPoint jet-clustering algorithm (FastJet).

    User-settable parameters (default values in the ``default_*`` class attributes):

        * ``radius``: cone radius R;
        * ``overlap``: overlap threshold;
        * ``seed``: seed threshold (GeV);
        * ``areafraction``: cone area fraction;
        * ``ptmin``: minimum transverse momentum (GeV) of the jets;
    """

    default_radius       = 1.
    default_overlap      = 0.5
    default_seed         = 1.
    default_areafraction = 1.
    default_ptmin        = 5.

    userVariables = { "radius" : [str(default_radius)],\
                      "overlap" : [str(default_overlap)],\
                      "seed" : [str(default_seed)],\
                      "areafraction" : [str(default_areafraction)],\
                      "ptmin" : [str(default_ptmin)] }

    def __init__(self) -> None:
        """Initialise all parameters to their default values."""
        self.radius       = ClusteringCDFMidPoint.default_radius
        self.ptmin        = ClusteringCDFMidPoint.default_ptmin
        self.overlap      = ClusteringCDFMidPoint.default_overlap
        self.seed         = ClusteringCDFMidPoint.default_seed
        self.areafraction = ClusteringCDFMidPoint.default_areafraction

        
    def Display(self) -> None:
        """Log all parameters."""
        self.user_DisplayParameter("radius")
        self.user_DisplayParameter("overlap")
        self.user_DisplayParameter("seed")
        self.user_DisplayParameter("areafraction")
        self.user_DisplayParameter("ptmin")


    def user_DisplayParameter(self,parameter: str) -> None:
        """Log the value of one parameter.

        Args:
            parameter (``str``): name of the parameter.
        """
        if parameter=="radius":
            logging.getLogger('MA5').info("  + cone radius = "+str(self.radius))
        elif parameter=="overlap":
            logging.getLogger('MA5').info("  + overlap threshold = "+str(self.overlap))
        elif parameter=="seed":
            logging.getLogger('MA5').info("  + seed threshold = "+str(self.seed))
        elif parameter=="ptmin":
            logging.getLogger('MA5').info("  + PT min (GeV) for produced jets = "+str(self.ptmin))
        elif parameter=="areafraction":
            logging.getLogger('MA5').info("  + cone area fraction = "+str(self.areafraction))
        else:
            logging.getLogger('MA5').error("'clustering' has no parameter called '"+parameter+"'")


    def SampleAnalyzerConfigString(self) -> dict[str, str]:
        """Get the options passed to the SampleAnalyzer jet clusterer.

        The keys are ``cluster.R``, ``cluster.PTmin``, ``cluster.OverlapThreshold``, ``cluster.SeedThreshol`` (sic), ``cluster.ConeAreaFraction``.

        Returns:
            ``dict[str, str]``:
            Option names and values, written in the generated ``main.cpp``.
        """
        mydict = {}
        mydict['cluster.R']                = str(self.radius)
        mydict['cluster.PTmin']            = str(self.ptmin)
        mydict['cluster.OverlapThreshold'] = str(self.overlap)
        # FIXME: typo in the key: the C++ clusterer (ClusterAlgoCDFMidPoint.cpp) expects
        # 'seedthreshold', so the seed threshold is never passed to SampleAnalyzer.
        mydict['cluster.SeedThreshol']     = str(self.seed)
        mydict['cluster.ConeAreaFraction'] = str(self.areafraction)
        return mydict

        
    def user_GetValues(self,variable: str) -> list[str]:
        """Get suggested values of a parameter (tab completion).

        Args:
            variable (``str``): name of the parameter.

        Returns:
            ``list[str]``:
            The default value as a one-element list, or an empty list.
        """
        try:
            return ClusteringCDFMidPoint.userVariables[variable]
        except:
            return []

    
    def user_GetParameters(self) -> list[str]:
        """Get the names of the user-settable parameters.

        Returns:
            ``list[str]``:
            Parameter names.
        """
        return list(ClusteringCDFMidPoint.userVariables.keys())


    def user_SetParameter(self,parameter: str,value: str) -> bool | None:
        """Set a parameter (``set main.fastsim.<parameter> = <value>``).

        Args:
            parameter (``str``): name of the parameter.
            value (``str``): value typed by the user.

        Returns:
            ``bool | None``:
            ``False`` for invalid values, ``None`` otherwise (success or unknown parameter).
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

        # overlap
        elif parameter=="overlap":
            try:
                number = float(value)
            except:
                logging.getLogger('MA5').error("the overlap threshold must be a float value.")
                return False
            if number<0:
                logging.getLogger('MA5').error("the overlap threshold cannot be negative.")
                return False
            self.overlap=number

        # seed
        elif parameter=="seed":
            try:
                number = float(value)
            except:
                logging.getLogger('MA5').error("the seed threshold  must be a float value.")
                return False
            if number<0:
                logging.getLogger('MA5').error("the seed threshold cannot be negative.")
                return False
            self.seed=number

        # seed
        elif parameter=="areafraction":
            try:
                number = float(value)
            except:
                logging.getLogger('MA5').error("the area fraction must be a float value.")
                return False
            if number<0:
                logging.getLogger('MA5').error("the area fraction cannot be negative.")
                return False
            self.areafraction=number

        # ptmin
        elif parameter=="ptmin":
            try:
                number = float(value)
            except:
                logging.getLogger('MA5').error("the ptmin must be a float value.")
                return False
            if number<0:
                logging.getLogger('MA5').error("the ptmin cannot be negative.")
                return False
            self.ptmin=number

        # other    
        else:
            logging.getLogger('MA5').error("'clustering' has no parameter called '"+parameter+"'")
