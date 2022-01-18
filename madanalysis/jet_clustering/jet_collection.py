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


from __future__ import absolute_import
from madanalysis.jet_clustering.jet_configuration import JetConfiguration
from collections import OrderedDict
from six.moves import range
import logging

from typing import Sequence, Text


class JetCollection:
    """
    Holds a collection of jets. This module is separate from the original jet clustering
    interface within Ma5. This module can be activated using following command

    .. code-block::

        ma5> define jet_algorithm my_jet antikt radius=0.5

    where `my_jet` is a user-defined jet identifier, `antikt` is the algorithm to be used which
    can be choosen from `antikt`, `cambridge`, `genkt`, `kt`, `gridjet`, `cdfjetclu`, `cdfmidpoint`,
    and `siscone`. Rest of the arguments are optional, if user won't define radius, ptmin etc.
    default parameters will be choosen. Each algorithm has its own unique set of parameters i.e.

    |       Algorithm       | Parameters & Default values                                                        |
    |:---------------------:|------------------------------------------------------------------------------------|
    | `antikt`, `cambridge` | `radius=0.4`, `ptmin=5.`                                                           |
    |        `genkt`        | `radius=0.4`, `ptmin=5.`, `exclusive=False`, `p=-1`                                |
    |         `kt`          | `radius=0.4`, `ptmin=5.`, `exclusive=False`                                        |
    |       `gridjet`       | `ymax=3.`, `ptmin=5.`                                                              |
    |      `cdfjetclu`      | `radius=0.4`, `ptmin=5.`, `overlap=0.5`, `seed=1.`, `iratch=0.`                    |
    |     `cdfmidpoint`     | `radius=0.4`, `ptmin=5.`, `overlap=0.5`, `seed=1.`, `iratch=0.`, `areafraction=1.` |
    |       `siscone`       | `radius=0.4`, `ptmin=5.`, `overlap=0.5`, `input_ptmin=5.`, `npassmax=1.`           |

    It is also possible to modify the entry after defining it

    .. code-block::

        ma5> define jet_algorithm my_jet cambridge
        ma5> set my_jet.ptmin = 200.
        ma5> set my_jet.radius = 0.8

    Note that when a `jet_algorithm` is defined MadAnalysis interface will automatically swithch
    to constituent smearing mode. `set my_jet.+tab` will show the dedicated options available
    for that particular algorithm.

    It is possible to display all the jets available in the current session by using `display jet_algorithm`
    command:

    .. code-block::

        $ ./bin/ma5 -R
        ma5>set main.fastsim.package = fastjet
        ma5>define jet_algorithm my_jet cdfmidpoint
        ma5>display jet_algorithm
        MA5: * Primary Jet Definition :
        MA5:  fast-simulation package : fastjet
        MA5:  clustering algorithm : antikt
        MA5:   + Jet ID : Ma5Jet
        MA5:   + cone radius = 0.4
        MA5:   + PT min (GeV) for produced jets = 5.0
        MA5:   + exclusive identification = true
        MA5:   + b-jet identification:
        MA5:     + DeltaR matching = 0.5
        MA5:     + exclusive algo = true
        MA5:     + id efficiency = 1.0
        MA5:     + mis-id efficiency (c-quark)      = 0.0
        MA5:     + mis-id efficiency (light quarks) = 0.0
        MA5:   + hadronic-tau identification:
        MA5:     + id efficiency = 1.0
        MA5:     + mis-id efficiency (light quarks) = 0.0
        MA5:    --------------------
        MA5: * Other Jet Definitions:
        MA5:    1. Jet ID = my_jet
        MA5:       - algorithm       : cdfmidpoint
        MA5:       - radius          : 0.4
        MA5:       - ptmin           : 5.0
        MA5:       - overlap         : 0.5
        MA5:       - seed            : 1.0
        MA5:       - iratch          : 0.0
        MA5:       - areafraction    : 1.0

    Here primary jet is defined with the original jet definition syntax of MadAnalysis 5 where
    since we did not specify anything, it uses default `antikt` configuration. For more info on
    how to define primary jet see [arXiv:2006.09387](https://arxiv.org/abs/2006.09387). Other jet
    definitions shows all the jets which are defined via `jet_algorithm` keyword.

    To remove a `jet_algorithm` definition one can use `remove my_jet` command.
    """

    def __init__(self):
        self.logger = logging.getLogger("MA5")
        self.collection = OrderedDict()
        self.algorithms = JetConfiguration().GetJetAlgorithms()

    def help(self):
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
            "                         it can be radius=0.4, ptmin=20 etc."
        )

    def define(self, args: Sequence[Text], dataset_names: Sequence = None) -> bool:
        """
        Definition of a new jet

        Parameters
        ----------
        args : Sequence[Text]
            input arguments:
            args[0]  -> jet_algorithm
            args[1]  -> JetID
            args[2]  -> jet algorithm
            args[3:] -> options: no need to cherry pick them only the relevant ones will be used.
        dataset_names : Sequence
            names for the datasets and primary jet to avoid overlaps

        Returns
        -------
        bool:
            if true new jet has been successfully created, if false not.
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

    def Set(self, obj, value):
        if len(obj) == 2:
            self.collection[obj[0]].user_SetParameter(obj[1], value)
        else:
            self.logger.error("Invalid syntax!")
        return

    def Delete(self, JetID):
        if JetID in self.collection.keys():
            self.collection.pop(JetID)
        else:
            self.logger.error(JetID + " does not exist.")

    def Display(self):
        for ix, (key, item) in enumerate(self.collection.items()):
            self.logger.info("   " + str(ix + 1) + ". Jet ID = " + key)
            item.Display()

    def __len__(self):
        return len(self.collection.keys())

    def GetNames(self):
        return list(self.collection.keys())

    def Get(self, JetID):
        return self.collection[JetID]
