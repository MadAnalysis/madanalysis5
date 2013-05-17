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


from madanalysis.enumeration.connector_type     import ConnectorType
from madanalysis.enumeration.operator_type      import OperatorType
from madanalysis.enumeration.combination_type   import CombinationType
import logging


class ConditionType():

    def __init__(self,observable, parts, operator, threshold ):
        self.observable = observable
        self.parts      = parts
        self.operator   = operator
        self.threshold  = threshold

    def GetStringDisplay(self):
        msg=self.observable.name
        if len(self.parts)!=0:
            msg+=" ("
        for i in range(len(self.parts)):
            msg+=" "+self.parts[i].GetStringDisplay()
            if i!=(len(self.parts)-1):
                msg+=" ,"
        if len(self.parts)!=0:
            msg+=" )"
        msg+=" "+OperatorType.convert2string(self.operator)
        msg+=" "+str(self.threshold)
        return msg
                
