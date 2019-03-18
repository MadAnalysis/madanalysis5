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
        elif self.type in ['bin_op', 'un_op']:
            return 'OP[ ' + self.name+': { id:' + str(self.id) + ' }]'


    # Printing all the info on a leaf
    def info(self):
        self.logger.info('  ** Leaf nr. ' + str(self.id))
        self.logger.info('    -> type/name = ' + self.type + ' ('+ self.name +')')
        self.logger.info('    -> mother: ' + str(self.mother))
        self.logger.info('    -> daugthers: '+ str(self.daughters))
