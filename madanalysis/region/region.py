################################################################################
#  
#  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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


import logging
class Region:

    def __init__(self,name):
        self.logger       = logging.getLogger('MA5')
        self.name              = name.lower()
        self.selections        = []
        self.histos            = []

    def Display(self):
        self.logger.info(" *******************************************" )
        self.logger.info(" ** name = "+self.name)
        self.user_DisplayParameter("selections")
        self.user_DisplayParameter("histos")
        self.logger.info(" ********************************************" )

    def user_GetParameters(self):
        return ['histos', 'selections']

    def user_DisplayParameter(self,parameter):
        if parameter=="selections":
            if len(self.selections)>0:
                self.logger.info(" ** List of cuts")
                ii=1
                for mysel in self.selections:
                    self.logger.info("     " + str(ii) + ". " + mysel)
                    ii=ii+1
            else:
                self.logger.info(" ** No cut attached to this region")
        elif parameter=="histos":
            if len(self.histos)>0:
                self.logger.info(" ** List of histograms")
                ii=1
                for myhist in self.histos:
                    self.logger.info("     " + str(ii) + ". " + myhist)
                    ii=ii+1
            else:
                self.logger.info(" ** No histogram attached to this region")
        else:
            self.logger.error(" the class Region has no attribute denoted by '"+parameter+"'")


