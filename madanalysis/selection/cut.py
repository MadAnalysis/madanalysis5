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


"""Definition of a cut (``select``/``reject`` commands)."""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any
from madanalysis.selection.instance_name      import InstanceName
from madanalysis.enumeration.combination_type import CombinationType
from madanalysis.enumeration.observable_type  import ObservableType
from madanalysis.enumeration.operator_type    import OperatorType
from madanalysis.enumeration.connector_type   import ConnectorType
from madanalysis.enumeration.cut_type         import CutType
from madanalysis.selection.condition_type     import ConditionType
from madanalysis.selection.condition_sequence import ConditionSequence
from madanalysis.enumeration.ma5_running_type import MA5RunningType

import logging

class Cut():
    """Event cut (no particle argument) or candidate cut (applied on a particle collection).

    The options that can be set on a cut are listed in :attr:`userVariables`; the
    shortcuts accepted between square brackets in the command are listed in
    :attr:`userShortcuts`.

    Attributes:
        part (``ParticleObject``): particles on which a candidate cut applies (empty for
            an event cut).
        conditions (``ConditionSequence``): criterion of the cut.
        rank (``str``): ordering of the particles.
        statuscode (``str``): particle status selection (``finalstate``, ...).
        cut_type (``int``): :class:`~madanalysis.enumeration.cut_type.CutType` code.
        regions (``list[str] | str``): regions the cut applies to (``"all"`` by default).
    """

    # NOTE: 'regions' (last entry) holds a string while the other entries are lists.
    userVariables = { "threshold"  : [], \
                      "rank"       : ["Eordering","Pordering","PTordering","ETordering",\
                                      "PXordering","PYordering","PZordering","ETAordering"], \
                      "statuscode" : ["finalstate","interstate","allstate","initialstate"], \
                      "regions": "all"
    }

    userShortcuts = {"finalstate":   ["statuscode","finalstate"], \
                     "interstate":   ["statuscode","interstate"], \
                     "allstate":     ["statuscode","allstate"], \
                     "initialstate": ["statuscode","initialstate"], \
                     "Eordering":    ["rank",      "Eordering"], \
                     "Pordering":    ["rank",      "Pordering"], \
                     "PTordering":   ["rank",      "PTordering"], \
                     "ETordering":   ["rank",      "ETordering"], \
                     "PXordering":   ["rank",      "PXordering"], \
                     "PYordering":   ["rank",      "PYordering"], \
                     "PZordering":   ["rank",      "PZordering"], \
                     "ETAordering":  ["rank",      "ETAordering"] }

    def __init__(self,part: Any,conditions: Any,cut_type: int,regions: list[str] | str = "all") -> None:
        """Create a cut.

        Args:
            part (``ParticleObject``): particles for a candidate cut (empty for an event cut).
            conditions (``ConditionSequence``): criterion of the cut.
            cut_type (``int``): ``CutType.SELECT`` or ``CutType.REJECT``.
            regions (``list[str] | str``, default ``"all"``): regions the cut applies to.
        """
        # NOTE: 'copy' is imported but not used.
        import copy
        self.part       = part
        self.conditions = conditions
        self.rank       = "PTordering"
        self.statuscode = "finalstate"
        self.cut_type   = cut_type
        self.regions    = regions

    def user_GetParameters(self) -> list[str]:
        """Get the names of the settable options.

        Returns:
            ``list[str]``:
            Keys of :attr:`userVariables`.
        """
        return list(Cut.userVariables.keys())

    def user_GetShortcuts(self) -> list[str]:
        """Get the option shortcuts.

        Returns:
            ``list[str]``:
            Keys of :attr:`userShortcuts`.
        """
        return list(Cut.userShortcuts.keys())

    def user_GetValues(self,variable: str) -> list[str] | str:
        """Get the possible values of an option.

        Args:
            variable (``str``): name of the option.

        Returns:
            ``list[str] | str``:
            Possible values (``"all"`` for ``regions``), or an empty list.
        """
        try:
            return Cut.userVariables[variable]
        except:
            return []

    def user_SetShortcuts(self,name: str) -> bool:
        """Apply an option shortcut (e.g. ``PTordering``).

        Args:
            name (``str``): shortcut.

        Returns:
            ``bool``:
            ``True`` on success, ``False`` for an unknown shortcut or invalid value.
        """
        if name in list(Cut.userShortcuts.keys()):
            return self.user_SetParameter(Cut.userShortcuts[name][0],Cut.userShortcuts[name][1])
        else:
            logging.getLogger('MA5').error("option '" + name + "' is unknown.")
            return False

    def user_SetParameter(self,variable: str,value: str | list[str]) -> bool:
        """Set an option of the cut.

        Args:
            variable (``str``): ``rank``, ``statuscode`` or ``regions``.
            value (``str | list[str]``): value (a list of region names for ``regions``).

        Returns:
            ``bool``:
            ``True`` on success, ``False`` otherwise.
        """
        # rank
        if variable == "rank":
            if value in Cut.userVariables["rank"]:
                self.rank=value
            else:
                logging.getLogger('MA5').error("'"+value+"' is not a possible value for the variable 'rank'.")
                return False

        # statuscode
        elif variable == "statuscode":
            if value in Cut.userVariables["statuscode"]:
                self.statuscode=value
            else:
                logging.getLogger('MA5').error("'"+value+"' is not a possible value for the variable 'statuscode'.")
                return False

        # regions
        elif variable == "regions":
            if isinstance(value,list) and all([isinstance(name,str) for name in value]):
                self.regions = value
            else:
                # FIXME: TypeError if 'value' is not a string (e.g. a list containing non-strings).
                logging.getLogger('MA5').error("'"+value+"' is not a list of strings, ;"+\
                     "as necessary for the variable 'regions'.")
                return False

        else:
            logging.getLogger('MA5').error("variable called '"+variable+"' is unknown")
            return False

        return True

    def user_DisplayParameter(self,variable: str) -> None:
        """Log the value of an option.

        Args:
            variable (``str``): name of the option.
        """
        if variable=="rank":
            logging.getLogger('MA5').info(" rank = "+self.rank)
        elif variable=="statuscode":
            logging.getLogger('MA5').info(" statuscode = "+self.statuscode) 
        elif variable=="regions":
            logging.getLogger('MA5').info(" regions = '"+str(self.regions)+"'")
        else:
            logging.getLogger('MA5').error("no variable called '"+variable+"' is found")


    def Display(self) -> None:
        """Log the cut."""
        logging.getLogger('MA5').info(self.GetStringDisplay())

        
    def GetStringDisplay(self) -> str:
        """Get the user-level representation of the cut.

        Returns:
            ``str``:
            E.g. ``"  * Cut: select ( j ) PT > 20.0, regions = all"``.
        """
        msg = "  * Cut: "

        # displaying command
        msg += CutType.convert2cmdname(self.cut_type)

        # displaying particles
        if len(self.part)!=0:
            msg += " ( " + self.part.GetStringDisplay()+" )"

        # displaying conditions
        msg += " "+self.conditions.GetStringDisplay()

        # displaying regions
        msg += ", regions = " + str(self.regions)

        return msg 

    # NOTE: only the candidate particles are checked, not the particles used in the conditions.
    def DoYouUseMultiparticle(self,name: str) -> bool:
        """Check whether a (multi)particle is used by the candidate selection.

        Args:
            name (``str``): label of the (multi)particle.

        Returns:
            ``bool``:
            ``True`` if the (multi)particle is used.
        """
        return self.part.DoYouUseMultiparticle(name)
