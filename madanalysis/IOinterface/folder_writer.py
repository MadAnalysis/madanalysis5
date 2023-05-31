################################################################################
#  
#  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://github.com/MadAnalysis/madanalysis5>
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


from __future__ import absolute_import
import os
import shutil
import logging
from six.moves import input

class FolderWriter:

    @staticmethod
    def RemoveDirectory(path,question=False):

        from madanalysis.core.main import Main

        # Checking if the directory is already defined
        if not os.path.isdir(path):
            return True, True
            
        # Asking the safety question
        if question and not Main.forced:
            logging.getLogger('MA5').warning("Are you sure to remove the directory called '"+path+"' ? (Y/N)")
            allowed_answers=['n','no','y','yes']
            answer=""
            while answer not in  allowed_answers:
               answer=input("Answer: ")
               answer=answer.lower()
            if answer=="no" or answer=="n":
                return False, True

        # Removing the directory
        try:
            shutil.rmtree(path)
            return True, True
        except:
            logging.getLogger('MA5').error("Impossible to remove the directory :")
            logging.getLogger('MA5').error(" "+path)
            return False, False
        
        
    @staticmethod
    def CreateDirectory(path,question=False,overwrite=False):

        from madanalysis.core.main import Main

        # Checking if the directory is already defined
        if os.path.isdir(path) and (overwrite or Main.forced):
            if not FolderWriter.RemoveDirectory(path,False):
                return False
        
        elif os.path.isdir(path):
            if not question:
                logging.getLogger('MA5').error("Directory called '"+path+"' is already defined.")
                return False
            else:
                logging.getLogger('MA5').warning("A directory called '"+path+"' is already "+ \
                                "defined.\nWould you like to remove it ? (Y/N)")
                allowed_answers=['n','no','y','yes']
                answer=""
                while answer not in  allowed_answers:
                    answer=input("Answer: ")
                    answer=answer.lower()
                if answer=="no" or answer=="n":
                    return False
                else:
                    if not FolderWriter.RemoveDirectory(path,False):
                        return False

        # Creating the directory    
        try:
            os.mkdir(path)
            return True
        except:
            logging.getLogger('MA5').error("Impossible to create the directory :")
            logging.getLogger('MA5').error(" "+path)
            return False
                
        
        
        
        
