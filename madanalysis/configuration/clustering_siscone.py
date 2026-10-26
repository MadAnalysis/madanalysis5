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


"""Configuration of the SISCone jet-clustering algorithm (``set main.fastsim.<parameter>``).
"""

from __future__ import absolute_import
from __future__ import annotations
import logging
class ClusteringSisCone():
    """Parameters of the SISCone jet-clustering algorithm (FastJet).

    User-settable parameters (default values in the ``default_*`` class attributes):

        * ``radius``: cone radius R;
        * ``overlap``: overlap threshold (split-merge parameter);
        * ``npassmax``: maximum number of passes (0 = no limit);
        * ``input_ptmin``: minimum transverse momentum of the input particles (protojets);
        * ``ptmin``: minimum transverse momentum (GeV) of the jets;
    """
    
    default_radius       = 0.4
    default_overlap      = 0.5
    default_npassmax     = 0
    default_input_ptmin  = 0.
    default_ptmin        = 5.

    userVariables = { "radius"      : [str(default_radius)],\
                      "overlap"     : [str(default_overlap)],\
                      "npassmax"    : [str(default_npassmax)],\
                      "input_ptmin" : [str(default_input_ptmin)],\
                      "ptmin"       : [str(default_ptmin)] }

    def __init__(self) -> None:
        """Initialise all parameters to their default values."""
        self.radius       = ClusteringSisCone.default_radius
        self.ptmin        = ClusteringSisCone.default_ptmin
        self.overlap      = ClusteringSisCone.default_overlap
        self.npassmax     = ClusteringSisCone.default_npassmax
        self.input_ptmin  = ClusteringSisCone.default_input_ptmin

        
    def Display(self) -> None:
        """Log all parameters."""
        self.user_DisplayParameter("radius")
        self.user_DisplayParameter("overlap")
        self.user_DisplayParameter("npassmax")
        self.user_DisplayParameter("input_ptmin")
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
        elif parameter=="npassmax":
            logging.getLogger('MA5').info("  + number max of pass = "+str(self.npassmax))
        elif parameter=="ptmin":
            logging.getLogger('MA5').info("  + PT min (GeV) for produced jets = "+str(self.ptmin))
        elif parameter=="input_ptmin":
            logging.getLogger('MA5').info("  + PT min (GeV) for input particles = "+str(self.input_ptmin))
        else:
            logging.getLogger('MA5').error("'clustering' has no parameter called '"+parameter+"'")


    def SampleAnalyzerConfigString(self) -> dict[str, str]:
        """Get the options passed to the SampleAnalyzer jet clusterer.

        The keys are ``cluster.R``, ``cluster.PTmin``, ``cluster.OverlapThreshold``, ``cluster.NPassMax``, ``cluster.Protojet_ptmin``.

        Returns:
            ``dict[str, str]``:
            Option names and values, written in the generated ``main.cpp``.
        """
        mydict = {}
        mydict['cluster.R']                = str(self.radius)
        mydict['cluster.PTmin']            = str(self.ptmin)
        mydict['cluster.OverlapThreshold'] = str(self.overlap)
        mydict['cluster.NPassMax']         = str(self.npassmax)
        mydict['cluster.Protojet_ptmin']   = str(self.input_ptmin)
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
            return ClusteringSisCone.userVariables[variable]
        except:
            return []

    
    def user_GetParameters(self) -> list[str]:
        """Get the names of the user-settable parameters.

        Returns:
            ``list[str]``:
            Parameter names.
        """
        return list(ClusteringSisCone.userVariables.keys())


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
        elif parameter=="npassmax":
            try:
                number = int(value)
            except:
                logging.getLogger('MA5').error("the n max of pass  must be an integer value.")
                return False
            if number<0:
                logging.getLogger('MA5').error("the n max of pass cannot be negative.")
                return False
            self.npassmax=number

        # input_ptmin
        elif parameter=="input_ptmin":
            try:
                number = float(value)
            except:
                logging.getLogger('MA5').error("the input PT min must be a float value.")
                return False
            if number<0:
                logging.getLogger('MA5').error("the input PT min cannot be negative.")
                return False
            # FIXME: the input PT min is stored in 'self.areafraction' instead of 'self.input_ptmin'.
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
