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


class FontType(object):
	values = {'none' : ['','','',''],\
			  'IT'   : ['\\textit{','}','<i>','</i>'],\
			  'BF'   : ['\\textbf{','}','<b>','</b>'],\
			  'TT'   : ['\\texttt{','}','  <tt>','</tt>'],\
			  'ITBF' : ['\\textit{\\textbf{','}}','<i><b>','</i></b>']}
        
	class __metaclass__(type):
		def __getattr__(self, name):
			return self.values.keys().index(name)
		
		def convert2latex(self,font):
			name = self.values.keys()[font]
			return self.values[name][0]

		def convert2latexclose(self,font):
			name = self.values.keys()[font]
			return self.values[name][1]

		def convert2html(self,font):
			name = self.values.keys()[font]
			return self.values[name][2]

		def convert2htmlclose(self,font):
			name = self.values.keys()[font]
			return self.values[name][3]
