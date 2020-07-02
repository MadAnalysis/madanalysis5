################################################################################
#  
#  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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


import logging

class Leaf:

    # Initialization
    def __init__(self, id_, type_, name_, id_mother, id_daughters):
        self.logger    = logging.getLogger('MA5')
        self.id        = id_
        self.type      = type_
        self.name      = name_
        self.mother    = id_mother
        self.daughters = id_daughters


    # Leaf representation as a string
    def __repr__(self):
        if self.type in ['var', 'cst']:
            return 'LEAF[ ' + self.name+': { id:' + str(self.id) + ' }]'
        elif self.type in ['bin2_op', 'bin1_op', 'un_op', 'bool']:
            return 'OP[ ' + self.name+': { id:' + str(self.id) + ' }]'


    # Printing all the info on a leaf
    def info(self):
        self.logger.info('  ** Leaf nr. ' + str(self.id))
        self.logger.info('    -> type/name = ' + self.type + ' ('+ self.name +')')
        self.logger.info('    -> mother: ' + str(self.mother))
        self.logger.info('    -> daugthers: '+ str(self.daughters))


    ## Connecting leaves as mother and daughters
    def connect(self, daughter_leafs):
        for daughter in daughter_leafs:
            if daughter.mother==[]:
                daughter.mother = [self.id]
            else:
                self.logger.error('daughter leaf already having a mother')
                daughter.info()
                return
            self.daughters.append(daughter.id)

    ## Method to get a string out of an ast leaf
    def write(self, tree):
        if self.type in ['cst', 'var', 'bool']:
            return self.name
        elif self.type == 'un_op' and len(self.daughters)==1:
            return self.name + '(' + tree.get(self.daughters[0]).write(tree) + ')'
        elif self.type == 'bin2_op' and len(self.daughters)==2:
            return self.name + '(' + tree.get(self.daughters[0]).write(tree) + ', ' + \
             tree.get(self.daughters[1]).write(tree) + ')'
        elif self.type == 'bin1_op' and len(self.daughters)==2:
            return '(' + tree.get(self.daughters[0]).write(tree) + ' ' + \
               self.name + ' ' + tree.get(self.daughters[1]).write(tree) + ')'
        else:
            self.logger.warning('cannot write this ast')
            self.info
            return


    ## From AT to c++
    def write_cpp(self, tree):
        if self.type == 'cst':
            return str(float(self.name))
        elif self.type in ['var', 'bool']:
            return self.name
        elif self.type == 'un_op' and len(self.daughters)==1 and self.name!='-':
            return 'std::'+self.name.replace('gamma','tgamma') + '(' +\
              tree.get(self.daughters[0]).write_cpp(tree) + ')'
        elif self.type == 'un_op' and len(self.daughters)==1 and self.name=='-':
            return self.name + '(' +\
              tree.get(self.daughters[0]).write_cpp(tree) + ')'
        elif self.type == 'bin2_op' and len(self.daughters)==2:
            return 'std::'+self.name + '(' +\
              tree.get(self.daughters[0]).write_cpp(tree) + ', ' + \
              tree.get(self.daughters[1]).write_cpp(tree) + ')'
        elif self.type == 'bin1_op' and len(self.daughters)==2 and self.name != '^':
            op = self.name.replace('and',' && ').replace('or',' || ')
            return '(' + tree.get(self.daughters[0]).write_cpp(tree) + ' ' + \
               op + ' ' + tree.get(self.daughters[1]).write_cpp(tree) + ')'
        elif self.name=='^' and len(self.daughters)==2:
            return 'pow(' + tree.get(self.daughters[0]).write_cpp(tree) + ', ' + \
               tree.get(self.daughters[1]).write_cpp(tree) + ')'
        else:
            self.logger.warning('cannot write this ast')
            self.info
            return



