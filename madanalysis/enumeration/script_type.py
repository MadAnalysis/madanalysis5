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


class ScriptType(object):
        values = {'none' : ['','','',''],\
                          'SUB'   : ['$_{','}$','<sub>','</sub>'],\
                          'SUP'   : ['$^{','}$','<sup>','</sup>']}

        class __metaclass__(type):
                def __getattr__(self, name):
                        return self.values.keys().index(name)

                def latexscript(self,script):
                        name = self.values.keys()[script]
                        return self.values[name][0]

                def latexscriptclose(self,script):
                        name = self.values.keys()[script]
                        return self.values[name][1]

                def htmlscript(self,script):
                        name = self.values.keys()[script]
                        return self.values[name][2]

                def htmlscriptclose(self,script):
                        name = self.values.keys()[script]
                        return self.values[name][3]
