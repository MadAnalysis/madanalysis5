################################################################################
#  
#  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <http://madanalysis.irmp.ucl.ac.be>
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


class CombinationType(object):
    values = { 'UNKNOWN'    : [],\
               'DEFAULT'    : [''],\
               'SUMSCALAR'  : ['s'],\
	       'SUMVECTOR'  : ['v'],\
               'DIFFSCALAR' : ['ds','sd'],\
               'DIFFVECTOR' : ['d','dv','vd'],\
               'RATIO'      : ['r'] }
               
    class __metaclass__(type):

        def __getattr__(self, name):
            return self.values.keys().index(name)

        def convert_from_string(self,lowerletters):
            for i,j in self.values.items():
                if lowerletters in j:
                    return self.__getattr__(i)
            return self.__getattr__('UNKNOWN')

        def convert2string(self,index):
            if index==self.__getattr__('UNKNOWN'):
                return 'ERROR'
            else:
                name = self.values.keys()[index]
                return self.values[name][0]
