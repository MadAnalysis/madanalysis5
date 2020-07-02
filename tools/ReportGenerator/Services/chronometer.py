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


# Standard modules
import logging
import os
import sys
import datetime
import time


class Chronometer():

    def __init__(self):
        self.start = None
        self.end   = None

    # Starting the chronometer
    def Start(self):
        self.start = datetime.datetime.now()
        self.end=None  # mandatory reset

    # Ending the chronometer
    def Stop(self):
        self.end = datetime.datetime.now()
        return (self.start != None) # has the chronometer been started?

    # Chronometer
    def Display(self):
        # Safety 1
        if self.start==None and self.end==None:
            return "Chronometer has not been started!!"
        elif self.start!=None and self.end==None:
            return "Chronometer has been started but not stopped!!"        
        elif self.start==None and self.end!=None:
            return "Chronometer has been stopped where as it was not started!!" 

        # Safety 2
        if self.start > self.end:
            return "Chronometer has been started after it was stopped!! " +\
                   "Time travel is forbidden!!"

        # time duration
        diff = datetime.datetime(1,1,1) + (self.end-self.start)
       
        # display
        onlySeconds=True
        msg = ''

        # - display on the optional options
        if diff.year>1:
            msg+=' '+str(diff.year-1)+' year'
            onlySeconds=False
            if diff.year>2:
                msg+='s'
        if diff.month>1:
            msg+=' '+str(diff.month-1)+' month'
            onlySeconds=False
            if diff.month>2:
                msg+='s'
        if diff.day>1:
            msg+=' '+str(diff.day-1)+' day'
            onlySeconds=False
            if diff.day>2:
                msg+='s'
        if diff.hour>0:
            msg+=' '+str(diff.hour)+' hour'
            onlySeconds=False
            if diff.hour>1:
                msg+='s'
        if diff.minute>0:
            msg+=' '+str(diff.minute)+' minute'
            onlySeconds=False
            if diff.minute>1:
                msg+='s'

        # - display on the seconds
        if not onlySeconds:
            msg+=' '+str(diff.second)+' second'
        else:
            value = float(diff.second+diff.microsecond/1e6)
            msg+=' %.2f'%value
            msg+=' second'
        if diff.second>1:
            msg+='s'

        #return the string
        msg=msg.lstrip()
        return msg
