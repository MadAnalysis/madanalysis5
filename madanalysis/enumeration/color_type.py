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


class ColorType(object):
    values = { 'AUTO'   : ['auto',     '',         0, \
                           [0,0,0,0,0,0,0,0,0] ],\
               'NONE'   : ['invisible','',         0, \
                           [0,0,0,0,0,0,0,0,0] ],\
	       'BLACK'  : ['black',    '#000000',  1, \
                           [16,15,13,923,1,1,1,1,1] ],\
               'YELLOW' : ['yellow',   '#FFFF00',  400, \
                           [390,391,393,396,400,401,402,403,404] ],\
               'BLUE'   : ['blue',     '#0000CC',  600, \
                           [590,591,593,596,600,601,602,603,604] ],\
	       'RED'    : ['red',      '#FF0000',  632, \
                           [632-10,632-9,632-7,632-4,632,633,634,635,636] ],\
               'GREY'   : ['grey',     '#808080',  920, \
                           [0,19,18,17,16,15,13,923,1] ],\
	       'GREEN'  : ['green',    '#00FF00',  416, \
                           [416-10,416-9,416-7,416-4,416,417,418,419,420] ],\
	       'PURPLE' : ['purple',   '#660066',  616, \
                           [616-10,616-9,616-7,616-4,616,617,618,619,620] ],\
	       'CYAN'   : ['cyan',     '#00FFFF',  432, \
                           [432-10,432-9,432-7,432-4,432,433,434,435,436] ],\
	       'ORANGE' : ['orange',   '#FF6600', 800, \
                           [400-10,400-9,800-4,800,800-3,800+7,800+4,800+3,632+4] ],\
	       'WHITE'  : ['white',    '#FFFFFF',   0, \
                           [0,0,0,0,0,19,18,17,16] ] }

    class __metaclass__(type):

        def __getattr__(self, name):
            if (name)=='GRAY':
                self.values.keys().index('GREY')
	    else:
	        return self.values.keys().index(name)

        def convert2string(self,color):
            name = self.values.keys()[color]
            return self.values[name][0]
		  
        def convert2hexa(self,color):
            name = self.values.keys()[color]
            return self.values[name][1]

        def convert2rootcode(self,color):
            name = self.values.keys()[color]
            return self.values[name][2]

        def convert2root(self,color,shade=0):
            name = self.values.keys()[color]
            return self.values[name][3][shade+4]
