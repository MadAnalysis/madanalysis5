################################################################################
#  
#  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


"""Removal and creation of folders (with optional user confirmation)."""

from __future__ import absolute_import
from __future__ import annotations
import os
import shutil
import logging
from six.moves import input

class FolderWriter:
    """Static helpers to remove and create folders."""

    @staticmethod
    def RemoveDirectory(path: str,question: bool = False) -> tuple[bool, bool]:
        """Remove a folder and its content.

        Args:
            path (``str``): folder to remove.
            question (``bool``, default ``False``): ask for confirmation (not in forced mode).

        Returns:
            ``tuple[bool, bool]``:
            ``(done, no_error)``: ``(True, True)`` if the folder has been removed or does not
            exist, ``(False, True)`` if the user refused, ``(False, False)`` if the removal
            failed.

        .. warning::
            A tuple is always truthy: ``if not FolderWriter.RemoveDirectory(...)`` never
            detects a failure (see the FIXME notes at the call sites).
        """

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
    def CreateDirectory(path: str,question: bool = False,overwrite: bool = False) -> bool:
        """Create a folder, possibly replacing an existing one.

        Args:
            path (``str``): folder to create.
            question (``bool``, default ``False``): ask whether an existing folder can be removed
                (otherwise an existing folder is an error).
            overwrite (``bool``, default ``False``): silently replace an existing folder (also
                done in forced mode).

        Returns:
            ``bool``:
            ``True`` if the folder has been created.
        """

        from madanalysis.core.main import Main

        # Checking if the directory is already defined
        if os.path.isdir(path) and (overwrite or Main.forced):
            # FIXME: RemoveDirectory returns a (non-empty, hence truthy) tuple: this test never fails
            # (same below).
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
                
        
        
        
        
