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


class ConditionConnector():

    def __init__(self,name):
        if name=="or":
            self.value=ConnectorType.OR
        elif name=="and":
            self.value=ConnectorType.AND

    def GetStringDisplay(self):
        if self.value==ConnectorType.OR:
            return "or"
        elif self.value==ConnectorType.AND:
            return "and"

    def GetStringCode(self):
        if self.value==ConnectorType.OR:
            return "||"
        elif self.value==ConnectorType.AND:
            return "&&"
