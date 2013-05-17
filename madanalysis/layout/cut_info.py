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


import logging
class CutInfo:

    def __init__(self):
        self.Reset()
        
    def Reset(self):
        self.nentries_pos = 0
        self.nentries_neg = 0
        self.sumw_pos     = 0.
        self.sumw_neg     = 0.
        self.sumw2_pos    = 0.
        self.sumw2_neg    = 0.

    def Print(self):
        logging.info("nentries_pos = " + str(self.nentries_pos))
        logging.info("nentries_neg = " + str(self.nentries_neg))
        logging.info("sumw_pos     = " + str(self.sumw_pos))
        logging.info("sumw_neg     = " + str(self.sumw_neg))
        logging.info("sumw2_pos    = " + str(self.sumw2_pos))
        logging.info("sumw2_neg    = " + str(self.sumw2_neg))
        
        
