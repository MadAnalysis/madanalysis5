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


r"""Extension of the :mod:`cmd` library used by the MadAnalysis 5 interpreter.

The extensions support multi-line commands (trailing ``\``), comments (``#``),
several commands per line (``;``), a command history, execution of scripts from the
:class:`~madanalysis.core.script_stack.ScriptStack`, and path completion.
"""
from __future__ import absolute_import
from __future__ import annotations

# Python import
import cmd
import logging
import os
import readline
import subprocess

from six.moves import range

from madanalysis.core.script_stack import ScriptStack
from madanalysis.interpreter.history import History


class InvalidCmd(Exception):
    """Error raised for a wrong command."""


#===============================================================================
# InterpreterBase
#===============================================================================
class InterpreterBase(cmd.Cmd):

    """Extension of :class:`cmd.Cmd` independent of MadAnalysis 5 specifics.

    Attributes:
        interpreter_operators (``list[str]``): characters surrounded by spaces before a
            command is parsed.
        history (``History``): history of the commands of the session.
        save_line (``str``): beginning of a command continued on the next line.

    .. note::
        The docstring of :meth:`do_quit` is displayed by ``help quit`` (there is no
        ``help_quit`` method): it must be kept short and user-oriented.
    """

    # NOTE: '&' appears twice in the list below.
    interpreter_operators = ['(',')','[',']','&','|','&',\
                             '^','!','=','>','<',',']

    def load(self, verbose: bool = True) -> bool:
        """Execute all commands stored in the :class:`~madanalysis.core.script_stack.ScriptStack`.

        Execution stops at the end of the stack or when a command stops the loop (``quit``,
        ``restart``).

        Args:
            verbose (``bool``, default ``True``): echo each command (``ma5>...``).

        Returns:
            ``bool``:
            Always ``True``.
        """
        ok = True
        while ok:
            line = ScriptStack.Next()
            if line=='':
                ok=False
            else:
                if verbose:
                    self.logger.info("ma5>"+line)
                line = self.precmd(line)
                stop = self.onecmd(line)
                stop = self.postcmd(stop, line)
                if stop==True: #Restart
                    ok=False
        return True

    def __init__(self, *arg, **opt) -> None:
        """Initialise the history, the line continuation and the ``readline`` delimiters.

        Args:
            *arg: arguments of :class:`cmd.Cmd`.
            **opt: keyword arguments of :class:`cmd.Cmd`.
        """
        
        self.log = True
        self.logger=logging.getLogger('MA5')

        # string table for history
        self.history = History()

        # beginning of the incomplete line (line break with '\') 
        self.save_line = ''
        cmd.Cmd.__init__(self, *arg, **opt)
        self.__initpos = os.path.abspath(os.getcwd())

        # set completer delimiter
        delims = readline.get_completer_delims().replace("[","")
        delims = delims.replace("]","")
        delims = delims.replace("{","")
        delims = delims.replace("}","")
        delims = delims.replace("=","")
        readline.set_completer_delims(delims)

    # FORMATTING THE LINE BEFORE INTERPRETING
    def precmd(self, line: str) -> str:
        r"""Pre-process a command line before its execution.

        The line is left-stripped, continued lines (trailing ``\``) are joined, comments
        are removed (outside quotes), the command is added to the history, the operators
        are surrounded by spaces and ``;``-separated commands are executed one by one.

        Args:
            line (``str``): raw input line.

        Returns:
            ``str``:
            The processed line, or ``''`` when there is nothing left to execute.
        """

        # nothing to do with empty line
        if not line:
            return line

        # cleaning the line
        # --> removing additionnal whitespace characters
        line = line.lstrip()

        # pattern design
        if len(line)==4 and \
           line[0]=='m' and line[1]=='u' and line[2]=='f' and line[3]=='!':
            self.pattern_design()
            return ''

        # Check if we are continuing a line:
        if self.save_line:
            line = self.save_line + line 
            self.save_line = ''
        
        # Check if the line is complete
        if line.endswith('\\'):
            self.save_line = line[:-1]
            return '' # do nothing   
        
        # Remove comment
        open_singlequote = False
        open_doublequote = False
        for ind in range(len(line)):
            if line[ind]=="'" and not open_doublequote:
                open_singlequote = not open_singlequote
            elif line[ind]=='"' and not open_singlequote:
                 open_doublequote = not open_doublequote
            elif line[ind]=='#' and not open_singlequote and not open_doublequote:
               line = line[0:ind]
               break

        # Add the line to the history
        if isinstance(self.history, list):
            if len(self.history) > 0:
                tmp = History()
                # FIXME: the loop variable shadows 'line': the current command is replaced by the last history
                # entry and is not added to the history.
                for line in self.history:
                    tmp.Add(line)
                self.history = tmp
            else:
                self.history = History()
        else:
            self.history.Add(line)

        # Isolating operator
        if not line.startswith('shell'):
            for item in self.interpreter_operators:
                line=line.replace(item,' '+item+' ')

        # Deal with line splitting
        # NOTE: the stop flags of the sub-commands are ignored (e.g. 'quit' in a ';'-separated line).
        if ';' in line and not (line.startswith('!') or line.startswith('shell')):
            for subline in line.split(';'):
                stop = self.onecmd(subline)
                stop = self.postcmd(stop, subline)
            return ''

        # debug
        self.logger.debug(self.split_arg(line))

        # execute the line command
        return line

    def exec_cmd(self, line: str, errorhandling: bool = False) -> bool | None:
        """Execute a command from a third-party code (with pre/post-processing).

        Args:
            line (``str``): command line.
            errorhandling (``bool``, default ``False``): use :meth:`onecmd` of this class
                (``True``) or of :class:`cmd.Cmd` (``False``).

        Returns:
            ``bool | None``:
            The stop flag of the command.
        """

        self.logger.info(line)
        line = self.precmd(line)
        if errorhandling:
            stop = self.onecmd(line)
        else:
            stop = cmd.Cmd.onecmd(self, line)
        stop = self.postcmd(stop, line)
        return stop

    def run_cmd(self, line: str) -> bool | None:
        """Execute a command from a third-party code with error handling.

        Args:
            line (``str``): command line.

        Returns:
            ``bool | None``:
            The stop flag of the command.
        """
        
        return self.exec_cmd(line, errorhandling=True)
    
    def emptyline(self) -> None:
        """Do nothing for an empty line (the default of :class:`cmd.Cmd` repeats the last command).
        """
        pass
    
    def default(self, line: str) -> None:
        """Log a warning for an unknown command.

        Args:
            line (``str``): command line.
        """

        # Faulty command
        self.logger.warning("Command \"%s\" not implemented, please try again." % \
                                                                line.split()[0])
    # Quit
    # The docstring below is displayed by 'help quit' (no help_quit method): keep it unchanged.
    # Exits the command loop (returns True); do_EOF and do_exit are aliases.
    def do_quit(self, line: str) -> bool:
        """ exit the mainloop() """
        self.logger.info("")
        return True

    # Aliases
    do_EOF = do_quit
    do_exit = do_quit

    @staticmethod
    def list_completion(text: str, list: list[str]) -> list[str]:
        """Propose the completions of a text among a list of words.

        Args:
            text (``str``): beginning of the word.
            list (``list[str]``): possible words.

        Returns:
            ``list[str]``:
            Words starting with ``text`` (all words if ``text`` is empty).
        """
        if not text:
            completions = list
        else:
            completions = [ f
                            for f in list
                            if f.startswith(text)
                            ]
        return completions

    @staticmethod
    def path_completion(text: str, base_dir: str | None = None, only_dirs: bool = False, relative: bool = True) -> list[str]:
        """Propose the completions of a text to form a valid path.

        Args:
            text (``str``): beginning of the path.
            base_dir (``str | None``, default ``None``): folder the path is relative to (current
                directory if ``None``).
            only_dirs (``bool``, default ``False``): only propose folders.
            relative (``bool``, default ``True``): also propose ``./`` and ``../``.

        Returns:
            ``list[str]``:
            Possible completions (folders end with a separator).
        """

        if base_dir is None:
            base_dir = os.getcwd()

        prefix, text = os.path.split(text)
        base_dir = os.path.join(base_dir, prefix)

        if prefix:
            prefix += os.path.sep

        if only_dirs:
            completion = [prefix + f
                          for f in os.listdir(base_dir)
                          if f.startswith(text) and \
                          os.path.isdir(os.path.join(base_dir, f)) and \
                          (not f.startswith('.') or text.startswith('.'))
                          ]
        else:
            completion = [ prefix + f
                          for f in os.listdir(base_dir)
                          if f.startswith(text) and \
                          os.path.isfile(os.path.join(base_dir, f)) and \
                          (not f.startswith('.') or text.startswith('.'))
                          ]

            completion = completion + \
                         [prefix + f + os.path.sep
                          for f in os.listdir(base_dir)
                          if f.startswith(text) and \
                          os.path.isdir(os.path.join(base_dir, f)) and \
                          (not f.startswith('.') or text.startswith('.'))
                          ]

        if relative:
            completion += [prefix + f for f in ['.'+os.path.sep, '..'+os.path.sep] if \
                       f.startswith(text) and not prefix.startswith('.')]

        return completion


    # Write the list of command line use in this session
    def do_history(self, line: str) -> None:
        """Display, clean or save the command history (``history [clean|<file>]``).

        Args:
            line (``str``): arguments of the command.
        """
        
        args = self.split_arg(line)

        if len(args) == 0:
            self.logger.info(self.history.Print())
            return
        elif args[0] == 'clean':
            self.history.Reset()
            self.logger.info('History is cleaned')
            return
        elif len(args)==1:
            if not self.history.Save(args[0]):
                self.logger.error('The file ' + args[0] + ' already exists.' + \
                                  ' Please chose another filename.')
            else:
                self.logger.info('Command history written to the file ' + \
                              args[0] + '.')

            return
        else:
            self.logger.error("'history' takes either zero or one argument")
            return

    def help_help(self) -> None:
        """Display the help of the ``help`` command."""
        self.logger.info("   Syntax: help [<command>]")
        self.logger.info("   Display the list of all available commands.");
        self.logger.info("   If a command is passed as an argument, its manual is displayed to the screen.")
        
        
    def help_history(self) -> None:
        """Display the help of the ``history`` command."""
        self.logger.info("   Syntax: history [clean] ")
        self.logger.info("   Displays the history of the commands type-in by")
        self.logger.info("   the user.")
        self.logger.info("   The option \"clean\" removes all the entries from the history.")

    def do_shell(self, line: str) -> None:
        """Run a shell command (``shell <command>`` or ``!<command>``).

        Args:
            line (``str``): shell command.
        """

        if line.strip() == '':
            self.help_shell()
        else:
            self.logger.info("   Running the shell command: " + line + ".")
            subprocess.call(line, shell=True)

    def help_shell(self) -> None:
        """Display the help of the ``shell`` command."""
        self.logger.info("   Syntax: shell <command> (or !CMD)")
        self.logger.info("   Runs the command CMD on a shell and retrieves the output.")


    def complete_history(self, text: str, line: str, begidx: int, endidx: int) -> list[str]:
        """Tab completion of the ``history`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text``.
            endidx (``int``): end index of ``text``.

        Returns:
            ``list[str]``:
            ``['clean']`` if it matches.
        """

        output = ["clean"]
        if text:
            output = [ f for f in output if f.startswith(text) ]
        return output    

    def complete_shell(self, text: str, line: str, begidx: int, endidx: int) -> list[str]:
        """Tab completion of the ``shell`` command (paths).

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text``.
            endidx (``int``): end index of ``text``.

        Returns:
            ``list[str]``:
            Possible paths.
        """

        if len(self.split_arg(line[0:begidx])) > 1 and line[begidx -1] == os.path.sep:
            if not text:
                text = ''
            output = self.path_completion(text, base_dir=self.split_arg(line[0:begidx])[-1])
        else:
            output = self.path_completion(text)

        return output    

    @staticmethod
    def split_arg(line: str) -> list[str]:
        r"""Split a command line into arguments.

        Curly braces are isolated, ``' ^ '`` is joined and escaped spaces (``\ ``) are kept
        inside an argument.

        Args:
            line (``str``): command line.

        Returns:
            ``list[str]``:
            The arguments.
        """

        myline = line.replace('{', ' { ')
        myline = myline.replace('}', ' } ')
        myline = myline.replace(' ^ ', '^')
        split = myline.split()
        out=[]
        tmp=''
        for data in split:
            if data[-1] == '\\':
                tmp += data[:-1]+' '
            elif tmp:
                out.append(tmp+data)
            else:
                out.append(data)
        return out


    def pattern_design(self) -> None:
        """Display an ASCII-art easter egg (typed as ``muf!``)."""
        pattern=[]
        pattern.append('32-32-32-32-32-32-32-32-95-95-95-95-95-32')
        pattern.append('32-32-32-32-32-95-45-126-126-32-32-32-32-32-126-126-45-95-47-47-32')
        pattern.append('32-32-32-47-126-32-32-32-32-32-32-32-32-32-32-32-32-32-126-92-32')
        pattern.append('32-32-124-32-32-32-32-32-32-32-32-32-32-32-32-32-32-95-32-32-124-95-32')
        pattern.append('32-124-32-32-32-32-32-32-32-32-32-95-45-45-126-126-126-32-41-126-126-32-41-95-95-95-32')
        pattern.append('92-124-32-32-32-32-32-32-32-32-47-32-32-32-95-95-95-32-32-32-95-45-126-32-32-32-126-45-39-95-32')
        pattern.append('92-32-32-32-32-32-32-32-32-32-32-95-45-126-32-32-32-126-45-95-32-32-32-32-32-32-32-32-32-92-32')
        pattern.append('124-32-32-32-32-32-32-32-32-32-47-32-32-32-32-32-32-32-32-32-92-32-32-32-32-32-32-32-32-32-124-32')
        pattern.append('124-32-32-32-32-32-32-32-32-124-32-32-32-32-32-32-32-32-32-32-32-124-32-32-32-32-32-40-79-32-32-124-32')
        pattern.append('32-124-32-32-32-32-32-32-124-32-32-32-32-32-32-32-32-32-32-32-32-32-124-32-32-32-32-32-32-32-32-124-32')
        pattern.append('32-32-124-32-32-32-32-32-32-124-32-32-32-79-41-32-32-32-32-32-32-32-32-124-32-32-32-32-32-32-32-124-32')
        pattern.append('32-32-47-124-32-32-32-32-32-32-124-32-32-32-32-32-32-32-32-32-32-32-124-32-32-32-32-32-32-32-47-32')
        pattern.append('32-32-47-32-92-32-95-45-45-95-32-92-32-32-32-32-32-32-32-32-32-47-45-95-32-32-32-95-45-126-41-32')
        pattern.append('32-32-32-32-47-126-32-32-32-32-92-32-126-45-95-32-32-32-95-45-126-32-32-32-126-126-126-95-95-47-32')
        pattern.append('32-32-32-124-32-32-32-124-92-32-32-126-45-95-32-126-126-126-32-95-45-126-126-45-45-45-126-32-32-92-32')
        pattern.append('32-32-32-124-32-32-32-124-32-124-32-32-32-32-126-45-45-126-126-32-32-47-32-92-32-32-32-32-32-32-126-45-39-95-32')
        pattern.append('32-32-32-124-32-32-32-92-32-124-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32')
        pattern[-1]='-32-126-45-39-95-32'
        pattern.append('32-32-32-32-92-32-32-32-126-45-124-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32')
        pattern[-1]+='-32-32-32-126-126-45-45-95-95-32-95-45-126-126-45-44-32'
        pattern.append('32-32-32-32-32-126-45-95-32-32-32-124-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32')
        pattern[-1]+='-32-32-32-32-32-32-32-32-47-32-32-32-32-32-124-32'
        pattern.append('32-32-32-32-32-32-32-32-126-126-45-45-124-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32')
        pattern[-1]+='-32-32-32-32-32-32-32-32-32-32-32-32-32-47-32'
        pattern.append('32-32-32-32-32-32-32-32-32-32-124-32-32-124-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32')
        pattern[-1]+='-32-32-32-32-32-32-32-32-32-32-32-47-32'
        pattern.append('32-32-32-32-32-32-32-32-32-32-124-32-32-32-124-32-32-32-32-32-32-32-32-32-32-32-32-32-32-95-32-32-32-32')
        pattern[-1]+='-32-32-32-32-32-32-32-32-95-45-126-32'
        pattern.append('32-32-32-32-32-32-32-32-32-32-124-32-32-47-126-126-45-45-95-32-32-32-95-95-45-45-45-126-126-32-32-32-32')
        pattern[-1]+='-32-32-32-32-32-32-95-45-126-32'
        pattern.append('32-32-32-32-32-32-32-32-32-32-124-32-32-92-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-32-95-95')
        pattern[-1]+='-45-45-126-126-32'
        pattern.append('32-32-32-32-32-32-32-32-32-32-124-32-32-124-126-126-45-45-95-95-32-32-32-32-32-95-95-95-45-45-45-126-126-32')
        pattern.append('32-32-32-32-32-32-32-32-32-32-124-32-32-124-32-32-32-32-32-32-126-126-126-126-126-32')
        pattern.append('32-32-32-32-32-32-32-32-32-32-124-32-32-124-32')

        for word in pattern:
            msg=""
            words = word.split('-')
            for i in range(len(words)):
                msg+=chr((int(words[i])))
            self.logger.info("\x1b[1m"+"\x1b[32m"+msg+"\x1b[0m")
        self.logger.info("")


