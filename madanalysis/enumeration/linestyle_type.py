################################################################################
#  
#  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
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


class LineStyleType(object):
	values = { 'SOLID'      : [1,'solid'],\
		   'DASHED'     : [2,'dashed'],\
		   'DOTTED'     : [3,'dotted'],\
		   'DASHDOTTED' : [4,'dash-dotted'] }
	
        class __metaclass__(type):

		def __getattr__(self, name):
			return self.values.keys().index(name)

		def convert2code(self,color):
			name = self.values.keys()[color]
			return self.values[name][0]

		def convert2string(self,color):
			name = self.values.keys()[color]
			return self.values[name][1]
