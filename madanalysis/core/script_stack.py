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
import glob
import os
import commands
import sys
import shutil

class ScriptStack:

    stack=[]
    main_index=0
    sub_index=0
    first=True


    @staticmethod
    def IsEmpty():
        if len(ScriptStack.stack)==0:
            return True
        else:
            return False


    @staticmethod
    def Next():
        if ScriptStack.IsEmpty():
            return ""
        if ScriptStack.first:
            ScriptStack.first=False
            logging.getLogger('MA5').info("Executing the commands from the script")
            logging.getLogger('MA5').info(ScriptStack.stack[ScriptStack.main_index][0] + "...")
        else:
            if not ScriptStack.IncrementIndex():
                return ""
        return ScriptStack.stack[ScriptStack.main_index][1][ScriptStack.sub_index]
    

    @staticmethod
    def IncrementIndex():
        if ScriptStack.IsEmpty():
            return         
        if (ScriptStack.sub_index+1)>=len(ScriptStack.stack[ScriptStack.main_index][1]):
            if (ScriptStack.main_index+1)>=len(ScriptStack.stack):
                return False
            else:
                logging.getLogger('MA5').info("Executing the commands from the script")
                logging.getLogger('MA5').info(ScriptStack.stack[ScriptStack.main_index][0] + "...")
                ScriptStack.main_index+=1
                ScriptStack.sub_index=0
        else:
            ScriptStack.sub_index+=1
        return True

    
    @staticmethod
    def IsFinished():
        if ScriptStack.IsEmpty():
            return True 
        if (ScriptStack.sub_index+1)>len(ScriptStack.stack[ScriptStack.main_index][1]):
            if (ScriptStack.main_index+1)>=len(ScriptStack.stack):
                return True
            else:
                return False
        else:
            return False


    @staticmethod
    def AddScript(filename):

        # Filename
        filename=os.path.expanduser(filename)
        filename=os.path.abspath(filename)
        filename=os.path.normpath(filename)
        logging.getLogger('MA5').debug("Storing the commands from the script '" + \
                                        filename + "'...")
        
        # Check
        if not os.path.isfile(filename):
            logging.getLogger('MA5').warning("The file called '"+filename+\
                                             "' is not found and will be skipped.")
            return False
        
        # Open the file
        try:
            input = open(filename)
        except:
            logging.getLogger('MA5').warning("The file called '"+filename+\
                                             "' cannot be opened and will be skipped.")
            return False

        # Mycommands
        mycommands = []
        
        # Loop over the file
        for line in input:
            line=line.rstrip('\r\n')
            line=line.rstrip()
            line=line.lstrip()
            if len(line)==0:
                continue
            mycommands.append(line)

        # Close the file
        input.close()

        # Empty
        if len(mycommands)==0:
            logging.getLogger('MA5').warning("The file called '"+filename+\
                                             "' is empty and will be skipped.")
            return False

        # Fill
        ScriptStack.stack.append([filename,mycommands])

        #Ok
        return True
        


    @staticmethod
    def Reset():
        ScriptStack.main_index = 0
        ScriptStack.sub_index  = 0
        ScriptStack.first      = True
