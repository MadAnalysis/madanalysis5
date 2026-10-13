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


"""Global stack of MadAnalysis 5 command scripts.

Scripts given on the command line (``./bin/ma5 [options] script1 script2 ...``) or
loaded internally (e.g. SFS detector cards during recasting or expert mode) are read
line by line and stored in :class:`ScriptStack`. The interpreter then pops the
commands one by one through :meth:`ScriptStack.Next`.

The stack is implemented with **class-level (static) state**, i.e. it is shared by
every interpreter instance living in the same Python process.
"""

from __future__ import absolute_import
from __future__ import annotations
import logging
import os

class ScriptStack:
    """Static FIFO container of script commands.

    Each element of :attr:`stack` is a two-element list ``[filename, commands]``
    where ``commands`` is the list of non-empty, stripped lines of the script.
    :attr:`main_index` points to the current script and :attr:`sub_index` to the
    current command inside that script. :attr:`first` is ``True`` until the first
    command has been delivered by :meth:`Next`.

    .. note::
        All members are class attributes; the class is never instantiated.
    """

    stack=[]
    main_index=0
    sub_index=0
    first=True


    @staticmethod
    def IsEmpty() -> bool:
        """Check whether no script has been stored.

        Returns:
            ``bool``:
            ``True`` if :attr:`ScriptStack.stack` is empty, ``False`` otherwise.
        """
        if len(ScriptStack.stack)==0:
            return True
        else:
            return False


    @staticmethod
    def Next() -> str:
        """Return the next command to execute.

        The first call returns the first command of the first script (and logs the
        name of the script). Subsequent calls advance the internal indices through
        :meth:`IncrementIndex`.

        Returns:
            ``str``:
            The next command line, or an empty string when the stack is empty or
            all commands have been consumed.
        """
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
    def IncrementIndex() -> bool | None:
        """Move the internal cursor to the next command.

        When the end of the current script is reached, the cursor moves to the first
        command of the next script (if any).

        Returns:
            ``bool | None``:
            ``True`` if a next command exists, ``False`` if all scripts have been
            consumed, and ``None`` if the stack is empty.
        """
        if ScriptStack.IsEmpty():
            # FIXME: returns None (not False) when the stack is empty; callers test the result with 'not'.
            return         
        if (ScriptStack.sub_index+1)>=len(ScriptStack.stack[ScriptStack.main_index][1]):
            if (ScriptStack.main_index+1)>=len(ScriptStack.stack):
                return False
            else:
                logging.getLogger('MA5').info("Executing the commands from the script")
                # FIXME: logs the name of the *last* stored script (stack[-1]) instead of the next one
                # (stack[main_index + 1]); wrong file name is printed when more than two scripts are stacked.
                logging.getLogger('MA5').info(ScriptStack.stack[-1][0] + "...")
                ScriptStack.main_index+=1
                ScriptStack.sub_index=0
        else:
            ScriptStack.sub_index+=1
        return True

    
    @staticmethod
    def IsFinished() -> bool:
        """Check whether all stored commands have been consumed.

        Returns:
            ``bool``:
            ``True`` if the stack is empty or the cursor is past the last command of
            the last script, ``False`` otherwise.
        """
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
    def AddScript(filename: str) -> bool:
        """Read a script file and push its commands on the stack.

        The path is expanded (``~``), made absolute and normalised. Empty lines are
        dropped and every line is stripped; comments are **not** removed here (this is
        done later by the interpreter pre-processing).

        Args:
            filename (``str``): path to the script file.

        Returns:
            ``bool``:
            ``True`` if the script has been stored, ``False`` if the file does not
            exist, cannot be opened or is empty.
        """

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
    def Reset() -> None:
        """Rewind the cursor to the first command of the first script.

        .. note::
            The stored scripts are **not** removed; callers wanting a fresh stack must
            also clear :attr:`ScriptStack.stack` (see
            :meth:`madanalysis.interpreter.ma5_interpreter.MA5Interpreter.load`).
        """
        ScriptStack.main_index = 0
        ScriptStack.sub_index  = 0
        ScriptStack.first      = True
