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


from __future__                                   import absolute_import
from madanalysis.jet_clustering.jet_configuration import JetConfiguration
from collections                                  import OrderedDict
from six.moves                                    import range
import logging


class JetCollection:
    # Initialization
    def __init__(self):
        self.logger     = logging.getLogger('MA5')
        self.collection = OrderedDict()
        self.algorithms = JetConfiguration().GetJetAlgorithms()

    def help(self):
        self.logger.error('   * define jet_algorithm <name> <algorithm> <keyword args>')
        self.logger.error('      - <name>         : Name to be assigned to the jet.')
        self.logger.error('      - <algorithm>    : Clustering algorithm of the jet. Available algorithms are: ')
        self.logger.error('                         '+', '.join(self.algorithms))
        self.logger.error('      - <keyword args> : (Optional) depending on the nature of the algorithm.')
        self.logger.error("                         it can be radius=0.4, ptmin=20 etc.")

    # Definition of a new jet
    def define(self, args, dataset_names=[]):
        # args[0]  -> jet_algorithm
        # args[1]  -> JetID
        # args[2]  -> jet algorithm
        # args[3:] -> options: no need to cherry pick them only the relevant ones will be used.
        # dataset_names: names for the datasets and primary jet to avoid overlaps

        if len(args) < 3:
            self.logger.error('Invalid syntax! Correct syntax is as follows:')
            self.help()
            return False

        if args[2] not in self.algorithms:
            self.logger.error("Clustering algorithm '"+args[2]+"' does not exist.")
            self.logger.error("Available algorithms are : "+", ".join(self.algorithms))
            return False

        if args[1] in dataset_names+list(self.collection.keys()):
            self.logger.error(args[1]+' has been used as a dataset or jet identifier.')
            if args[1] in self.collection.keys():
                self.logger.error("To modify clustering properties please use 'set' command.")
            return False

        JetID     = args[1]
        algorithm = args[2]

        # remove commas from options
        chunks = args[3:]
        for i in range(len([x for x in chunks if x==','])):
            chunks.remove(',')

        # Decode keyword arguments
        chunks = [chunks[x:x+3] for x in range(0, len(chunks), 3)]
        if any([len(x)!=3 for x in chunks]) or any([('=' != x[1]) for x in chunks]):
            self.logger.error('Invalid syntax!')
            self.help()
            return False

        # Extract options
        options = {}
        for item in chunks:
            try:
                if item[0] == 'exclusive':
                    if item[2].lower() in ['true','t']:
                        options[item[0]] = True
                    elif item[2].lower() in ['false','f']:
                        options[item[0]] = False
                    else:
                        raise ValueError('Exclusive can only be True or False.')
                else:
                    options[item[0]] = float(item[2])
            except ValueError as err:
                if item[0] == 'exclusive':
                    self.logger.error('Invalid syntax! '+str(err))
                else:
                    self.logger.error('Invalid syntax! '+item[0]+' requires to have a float value.')
                return False


        self.collection[JetID] = JetConfiguration(JetID=JetID,
                                                  algorithm=algorithm, 
                                                  options=options)
        return True

    def Set(self,obj,value):
        if len(obj) == 2:
            self.collection[obj[0]].user_SetParameter(obj[1],value)
        else:
            self.logger.error('Invalid syntax!')
        return

    def Delete(self,JetID):
        if JetID in self.collection.keys():
            self.collection.pop(JetID)
        else:
            self.logger.error(JetID+' does not exist.')

    def Display(self):
        for ix, (key, item) in enumerate(self.collection.items()):
            self.logger.info('   '+str(ix+1)+'. Jet ID = '+key)
            item.Display()

    def __len__(self):
        return len(self.collection.keys())

    def GetNames(self):
        return list(self.collection.keys())

    def Get(self,JetID):
        return self.collection[JetID]