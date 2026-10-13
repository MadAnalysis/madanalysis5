################################################################################
#
#  Copyright (C) 2012-2020 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#
#  This file is part of MadAnalysis 5.
#  Official website: <https://launchpad.net/madanalysis5>
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


"""Collection of the additional jet definitions of the session."""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from madanalysis.jet_clustering.jet_configuration import JetConfiguration as _JetConfiguration
from madanalysis.jet_clustering.jet_configuration import JetConfiguration
from collections import OrderedDict
from six.moves import range
import logging

from typing import Sequence, Text


class JetCollection:
    """Collection of the jet definitions created with ``define jet_algorithm``.

    This module is independent of the original (primary) jet clustering of MadAnalysis 5,
    defined through ``set main.fastsim.*``. Additional jets are defined as::

        ma5> define jet_algorithm my_jet antikt radius=0.5

    where ``my_jet`` is a user-defined identifier and ``antikt`` the algorithm. The
    remaining keyword arguments are optional; each algorithm has its own parameters
    (default values in parentheses):

    * ``antikt``, ``cambridge``: ``radius`` (0.4), ``ptmin`` (5);
    * ``genkt``: ``radius`` (0.4), ``ptmin`` (5), ``exclusive`` (False), ``p`` (-1);
    * ``kt``: ``radius`` (0.4), ``ptmin`` (5), ``exclusive`` (False);
    * ``gridjet``: ``ymax`` (3), ``ptmin`` (5);
    * ``cdfjetclu``: ``radius`` (0.4), ``ptmin`` (5), ``overlap`` (0.5), ``seed`` (1),
      ``iratch`` (0);
    * ``cdfmidpoint``: same as ``cdfjetclu`` plus ``areafraction`` (1);
    * ``siscone``: ``radius`` (0.4), ``ptmin`` (5), ``overlap`` (0.5), ``input_ptmin`` (5),
      ``npassmax`` (1);
    * ``VariableR``: ``rho`` (2000), ``minR`` (0), ``maxR`` (2), ``ptmin`` (20),
      ``exclusive`` (False), ``clustertype`` (``AKTLIKE``), ``strategy`` (``Best``).

    The definitions can be modified afterwards::

        ma5> set my_jet.ptmin = 200.
        ma5> set my_jet.radius = 0.8

    As soon as a jet algorithm is defined, the SFS module switches to its
    constituent-smearing mode (see arXiv:2006.09387). ``display jet_algorithm`` lists the
    primary and additional jet definitions, and ``remove my_jet`` deletes a definition.

    Attributes:
        collection (``OrderedDict[str, JetConfiguration]``): jet definitions by identifier.
        algorithms (``list[str]``): supported algorithms.
    """

    def __init__(self) -> None:
        """Create an empty collection."""
        self.logger = logging.getLogger("MA5")
        self.collection = OrderedDict()
        self.algorithms = JetConfiguration().GetJetAlgorithms()

    def help(self) -> None:
        """Log the syntax of ``define jet_algorithm`` (at error level)."""
        self.logger.error("   * define jet_algorithm <name> <algorithm> <keyword args>")
        self.logger.error("      - <name>         : Name to be assigned to the jet.")
        self.logger.error(
            "      - <algorithm>    : Clustering algorithm of the jet. Available algorithms are: "
        )
        self.logger.error("                         " + ", ".join(self.algorithms))
        self.logger.error(
            "      - <keyword args> : (Optional) depending on the nature of the algorithm."
        )
        self.logger.error(
            "                         it can be radius=0.4, ptmin=20, etc."
        )

    def define(self, args: Sequence[Text], dataset_names: Sequence = None) -> bool:
        """Define a new jet collection.

        Args:
            args (``Sequence[Text]``): command arguments: ``args[0]`` is ``jet_algorithm``,
                ``args[1]`` the jet identifier, ``args[2]`` the algorithm and ``args[3:]`` the
                options as ``key = value`` triplets (commas are ignored).
            dataset_names (``Sequence``, default ``None``): names that cannot be used as jet
                identifier (datasets, primary jet).

        Returns:
            ``bool``:
            ``True`` if the jet collection has been created, ``False`` otherwise.
        """

        if dataset_names is None:
            dataset_names = []

        if len(args) < 3:
            self.logger.error("Invalid syntax! Correct syntax is as follows:")
            self.help()
            return False

        if args[2] not in self.algorithms:
            self.logger.error("Clustering algorithm '" + args[2] + "' does not exist.")
            self.logger.error(
                "Available algorithms are : " + ", ".join(self.algorithms)
            )
            return False

        if args[1] in dataset_names + list(self.collection.keys()):
            self.logger.error(
                args[1] + " has been used as a dataset or jet identifier."
            )
            if args[1] in self.collection.keys():
                self.logger.error(
                    "To modify clustering properties please use 'set' command."
                )
            return False

        JetID = args[1]
        algorithm = args[2]

        # remove commas from options
        chunks = args[3:]
        for i in range(len([x for x in chunks if x == ","])):
            chunks.remove(",")

        # Decode keyword arguments
        chunks = [chunks[x : x + 3] for x in range(0, len(chunks), 3)]
        if any([len(x) != 3 for x in chunks]) or any([("=" != x[1]) for x in chunks]):
            self.logger.error("Invalid syntax!")
            self.help()
            return False

        # Extract options
        options = {}
        for item in chunks:
            try:
                if item[0] == "exclusive":
                    if item[2].lower() in ["true", "t"]:
                        options[item[0]] = True
                    elif item[2].lower() in ["false", "f"]:
                        options[item[0]] = False
                    else:
                        raise ValueError("Exclusive can only be True or False.")
                elif item[0] == "strategy" and algorithm == "VariableR":
                    strategy = ["Best", "N2Tiled", "N2Plain", "NNH", "Native"]
                    if item[2] in strategy:
                        options[item[0]] = item[2]
                    else:
                        # NOTE: an invalid strategy/cluster type is only logged; the jet is still created with the
                        # default value. The cluster type is case-sensitive here, but not in JetConfiguration.user_SetParameter.
                        self.logger.error(f"Invalid strategy: {item[2]}")
                        self.logger.error("Available types are: " + ", ".join(strategy))
                elif item[0] == "clustertype" and algorithm == "VariableR":
                    ctype = ["CALIKE", "KTLIKE", "AKTLIKE"]
                    if item[2] in ctype:
                        options[item[0]] = item[2]
                    else:
                        self.logger.error(f"Invalid cluster type: {item[2]}")
                        self.logger.error("Available types are: " + ", ".join(ctype))
                else:
                    options[item[0]] = float(item[2])
            except ValueError as err:
                if item[0] == "exclusive":
                    self.logger.error("Invalid syntax! " + str(err))
                else:
                    self.logger.error(
                        "Invalid syntax! "
                        + item[0]
                        + " requires to have a float value."
                    )
                return False

        self.collection[JetID] = JetConfiguration(
            JetID=JetID, algorithm=algorithm, options=options
        )
        return True

    def Set(self, obj: list[str], value: str) -> None:
        """Set a parameter of a jet definition (``set <JetID>.<parameter> = <value>``).

        Args:
            obj (``list[str]``): ``[JetID, parameter]``.
            value (``str``): value typed by the user.
        """
        if len(obj) == 2:
            # NOTE: KeyError if obj[0] is not a defined jet.
            self.collection[obj[0]].user_SetParameter(obj[1], value)
        else:
            self.logger.error("Invalid syntax!")
        return

    def Delete(self, JetID: str) -> None:
        """Remove a jet definition.

        Args:
            JetID (``str``): identifier of the jet collection.
        """
        if JetID in self.collection.keys():
            self.collection.pop(JetID)
        else:
            self.logger.error(JetID + " does not exist.")

    def Display(self) -> None:
        """Log all additional jet definitions."""
        for ix, (key, item) in enumerate(self.collection.items()):
            self.logger.info("   " + str(ix + 1) + ". Jet ID = " + key)
            item.Display()

    def __len__(self) -> int:
        """Get the number of additional jet definitions.

        Returns:
            ``int``:
            Number of definitions.
        """
        return len(self.collection.keys())

    def GetNames(self) -> list[str]:
        """Get the identifiers of the jet definitions.

        Returns:
            ``list[str]``:
            Jet identifiers in definition order.
        """
        return list(self.collection.keys())

    def Get(self, JetID: str) -> _JetConfiguration:
        """Get a jet definition.

        Args:
            JetID (``str``): identifier of the jet collection.

        Raises:
            ``KeyError``: if the identifier does not exist.

        Returns:
            ``JetConfiguration``:
            The jet definition.
        """
        return self.collection[JetID]
