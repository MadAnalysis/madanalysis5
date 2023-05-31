################################################################################
#  
#  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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


import six

class metaclass(type):

        def __getattr__(self, name):
                return list(self.values.keys()).index(name)

        def convert2code(self,color):
                name = list(self.values.keys())[color]
                return self.values[name][0]

        def convert2string(self,color):
                name = list(self.values.keys())[color]
                return self.values[name][1]

        def convert2matplotlib(self,color):
                name = list(self.values.keys())[color]
                return self.values[name][2]

@six.add_metaclass(metaclass)
class BackStyleType(object):
        values = { 'AUTO'   : [0,   'auto',  'None'],\
                   'SOLID'  : [1001,'solid', 'None'],\
                   'DOTTED' : [3002,'dotted','"."'],\
                   'HLINE'  : [3007,'hline', '"-"'],\
                   'DLINE'  : [3004,'dline', '"/"'],\
                   'VLINE'  : [3006,'vline', '"|"']  }

#            matplotlib -> hatch   = ['/', '\\', '|', '-', '+', 'x', 'o', 'O', '.', '*']

