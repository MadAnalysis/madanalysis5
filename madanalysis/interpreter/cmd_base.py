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
import glob
import os

class CmdBase():
    """Command CMDBase"""

    reserved_words=["exit","quit","eof","history","shell","from","as","all","or","and","main"]

    def __init__(self,main,cmd_name):
        self.reserved_words.append(cmd_name)
        self.main=main

    def do(self,args):
        logging.error("To developpers: CmdBase.do method must be overloaded!")

    def help(self):
        logging.error("To developpers: CmdBase.help method must be overloaded!")

    def complete(self,text,line,begidx,endidx):
        logging.error("To developpers: CmdBase.complete method must be overloaded!")
        return
    
    @staticmethod 
    def directory_complete():
        output = []
        for file in glob.glob("*"):
            if os.path.isdir(file):
                output.append(file)
        return output        

    @staticmethod
    def finalize_complete(text,args):
        if not text:
            return args
        else:
            return [ item for item in args if item.startswith(text) ]
    
    @staticmethod
    def IsAuthorizedLabel(label):

        # Rejecting empty label
        if len(label)==0:
            return False

        # Checking first character
        if not (label[0].isalpha() or label[0]=='_'):
            return False

        # Checking forbidden character
        allowed = ['+','-','~','_']
        for i in range(0,len(label)):
            if not (label[i].isalpha() or label[i].isdigit()):
                test = False
                for j in range(0,len(allowed)):
                    if label[i]==allowed[j]:
                        test=True
                        break
                if not test:
                    return False

        # Ok
        return True
