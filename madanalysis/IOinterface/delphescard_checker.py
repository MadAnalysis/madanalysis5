################################################################################
#  
#  Copyright (C) 2012-2016 Eric Conte, Benjamin Fuks
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
import os


class DelphesCardChecker():

    def __init__(self,dirname,main):
        self.dirname = dirname
        self.main    = main


    def getNameCard(self):
        if self.main.fastsim.package=="delphes":
            cardname = self.main.fastsim.delphes.card
        elif self.main.fastsim.package=="delphesMA5tune":
            cardname = self.main.fastsim.delphesMA5tune.card
        return self.dirname+"/Input/"+cardname        
        

    def checkPresenceCard(self):
        logging.getLogger('MA5').debug("Check the presence of the Delphes card: "+self.getNameCard()+' ...')
        if not os.path.isfile(self.getNameCard()):
            logging.getLogger('MA5').error('DelphesCard is not found: '+self.getNameCard())
            return False
        return True
                          

    def editCard(self):
        logging.getLogger('MA5').debug("Invite the user to edit the Delphes card: "+self.getNameCard()+' ...')
        if self.main.forced or self.main.script:
            return True

        logging.getLogger('MA5').info("Would you like to edit the Delphes Card? (Y/N)")
        allowed_answers=['n','no','y','yes']
        answer=""
        while answer not in  allowed_answers:
            answer=raw_input("Answer: ")
            answer=answer.lower()
        if answer=="no" or answer=="n":
            return True
        else:
            # TO BE CODED PROPERLY AND CHECK IF THE COMMAND HAS WORKED
            os.system(self.main.session_info.editor+" "+self.getNameCard())
            return True


    def checkContentCard(self):
        logging.getLogger('MA5').debug("Check the content of the Delphes card: "+self.getNameCard()+' ...')
        test=False
        if test or self.main.forced or self.main.script:
            return test
        else:
            logging.getLogger('MA5').info("Are you sure to go on with this Card? (Y/N)")
            allowed_answers=['n','no','y','yes']
            answer=""
            while answer not in  allowed_answers:
                answer=raw_input("Answer: ")
                answer=answer.lower()
            if answer=="no" or answer=="n":
                return False # no, give up
            else:
                return True # go on       
    
