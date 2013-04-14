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


from madanalysis.enumeration.operator_type     import OperatorType
from madanalysis.enumeration.connector_type    import ConnectorType
from madanalysis.selection.condition_connector import ConditionConnector
import logging


class ConditionSequence():

    def __init__(self,mother=False):
        self.sequence = []
        self.mother   = mother

    def GetStringDisplay(self):
        msg=""

        # Case : X < obs < Y
        if len(self.sequence)==3 and \
           self.sequence[0].__class__.__name__=='ConditionType' and \
           self.sequence[1].__class__.__name__=='ConditionConnector' and \
           self.sequence[2].__class__.__name__=='ConditionType' and \
           self.sequence[0].observable == self.sequence[2].observable and \
           self.sequence[1].value == ConnectorType.AND:
            
            msg+=str(self.sequence[0].threshold)+" "
            msg+=OperatorType.convert2string(self.sequence[0].operator)+" "
            msg+=self.sequence[0].observable.name+" "
            msg+=OperatorType.convert2string(self.sequence[2].operator)+" "
            msg+=str(self.sequence[2].threshold)
            
        # General case : loop over sequence
        else:
            
            if not self.mother:
                msg+="( "

            for i in range(0,len(self.sequence)):
                msg+=self.sequence[i].GetStringDisplay()
                if i!=(len(self.sequence)-1):
                    msg+=" "

            if not self.mother:
                msg+=" )"

        return msg
                
