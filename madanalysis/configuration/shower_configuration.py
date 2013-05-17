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


from madanalysis.enumeration.ma5_running_type         import MA5RunningType
import logging

class ShowerConfiguration:
    def __init__(self):
        self.enable = False
        self.type   = 'none'

    userVariables = { 'type' : ['none', 'auto', 'pythia6', 'pythia8', 'herwig6'] }

    def Display(self):
        if self.enable:
            self.user_DisplayParameter('type')
        else:
            logging.info(' No shower algorithm is activated')
        return

    def user_DisplayParameter(self,parameter):
        if parameter=='type':
            if self.type=='auto':
                logging.info(' Shower information deduced from the event file')
            elif self.type=='none':
                logging.info(' No shower algorithm activated')
            else:
                logging.info(' Employing the shower algorithm ' + self.type + '.')
        else:
            logging.error("'shower' has no parameter called '"+parameter+"'")
            return

    def user_SetParameter(self,parameter,value,level,amcatnlo):
        # type
        if parameter=='type':
            if value!='none':
                # Only in parton mode
                if level!=MA5RunningType.PARTON:
                    logging.error("Showering can only be applied in PARTON mode")
                    return
                if value in [ 'auto', 'herwig6' ]: 
                    if amcatnlo:
                        self.enable=True
                        self.type=value
                    else:
                       logging.error('MCatNLO-for-ma5 is not installed.' +  
                         'Please type \'install mcatnlo-for-ma5')
                elif value in ['pythia6', 'pythia8']: 
                       logging.error('Pythia showers not supported so far.')
                else:
                    logging.error(' There is no shower type denoted by ' + value +'.')
                    return
            elif value=='none':
                self.enable=False
                self.type  ='none'
            return
        # other
        else:
            logging.error("The shower attribute '"+parameter+"' does not exist.")
            return
         
    def user_GetParameters(self):
        return ShowerConfiguration.userVariables.keys()

    def user_GetValues(self,variable):
       try:
           return ShowerConfiguration.userVariables[variable]
       except:
           return []
