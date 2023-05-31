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


"""  A file containing different extension of the cmd basic python library"""
from __future__ import absolute_import
from madanalysis.core.script_stack            import ScriptStack
from madanalysis.interpreter.history          import History

# Python import
import cmd
import logging
import os
import subprocess
import readline
from six.moves import range


#===============================================================================
# InterpreterBase
#===============================================================================
class InterpreterBase(cmd.Cmd):

    """Extension of the cmd.Cmd command line.
    This extensions supports line breaking, history, comments,
    internal call to cmdline, path completion,...
    this class should be MG5 independent"""

    class InvalidCmd(Exception):
        """expected error for wrong command"""
        pass    
 
        debug_output = 'debug'
        error_debug = """Please report this bug to developers\nMore information can be found in '%s'."""
        error_debug += """\nPlease attach this file to your report."""
        keyboard_stop_msg = """Stopping all current operations. Program must exited. Please type 'exit'"""

    interpreter_operators = ['(',')','[',']','&','|','&',\
                             '^','!','=','>','<',',']

    def load(self, verbose=True):
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

    def __init__(self, *arg, **opt):
        """Init history and line continuation"""
        
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
    def precmd(self, line):
        """ A suite of additional function needed for in the cmd
        this implement history, line breaking, comment treatment,...
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
        if ';' in line and not (line.startswith('!') or line.startswith('shell')):
            for subline in line.split(';'):
                stop = self.onecmd(subline)
                stop = self.postcmd(stop, subline)
            return ''

        # debug
        self.logger.debug(self.split_arg(line))

        # execute the line command
        return line

    def exec_cmd(self, line, errorhandling=False):
        """for third party call, call the line with pre and postfix treatment
        without global error handling """

        self.logger.info(line)
        line = self.precmd(line)
        if errorhandling:
            stop = self.onecmd(line)
        else:
            stop = cmd.Cmd.onecmd(self, line)
        stop = self.postcmd(stop, line)
        return stop

    def run_cmd(self, line):
        """for third party call, call the line with pre and postfix treatment
        with global error handling"""
        
        return self.exec_cmd(line, errorhandling=True)
    
    def emptyline(self):
        """If empty line, do nothing. Default is repeat previous command."""
        pass
    
    def default(self, line):
        """Default action if line is not recognized"""

        # Faulty command
        self.logger.warning("Command \"%s\" not implemented, please try again." % \
                                                                line.split()[0])
    # Quit
    def do_quit(self, line):
        """ exit the mainloop() """
        self.logger.info("")
        return True

    # Aliases
    do_EOF = do_quit
    do_exit = do_quit

    @staticmethod
    def list_completion(text, list):
        """Propose completions of text in list"""
        if not text:
            completions = list
        else:
            completions = [ f
                            for f in list
                            if f.startswith(text)
                            ]
        return completions

    @staticmethod
    def path_completion(text, base_dir = None, only_dirs = False, relative=True):
        """Propose completions of text to compose a valid path"""

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
    def do_history(self, line):
        """write in a file the suite of command that was used"""
        
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

    def help_help(self):
        self.logger.info("   Syntax: help [<command>]")
        self.logger.info("   Display the list of all available commands.");
        self.logger.info("   If a command is passed as an argument, its manual is displayed to the screen.")
        
        
    def help_history(self):
        self.logger.info("   Syntax: history [clean] ")
        self.logger.info("   Displays the history of the commands type-in by")
        self.logger.info("   the user.")
        self.logger.info("   The option \"clean\" removes all the entries from the history.")

    def do_shell(self, line):
        "run a shell command"

        if line.strip() == '':
            self.help_shell()
        else:
            self.logger.info("   Running the shell command: " + line + ".")
            subprocess.call(line, shell=True)

    def help_shell(self):
        self.logger.info("   Syntax: shell <command> (or !CMD)")
        self.logger.info("   Runs the command CMD on a shell and retrieves the output.")


    def complete_history(self, text, line, begidx, endidx):
        "complete the history command"

        output = ["clean"]
        if text:
            output = [ f for f in output if f.startswith(text) ]
        return output    

    def complete_shell(self, text, line, begidx, endidx):
        """ add path for shell """

        if len(self.split_arg(line[0:begidx])) > 1 and line[begidx -1] == os.path.sep:
            if not text:
                text = ''
            output = self.path_completion(text, base_dir=self.split_arg(line[0:begidx])[-1])
        else:
            output = self.path_completion(text)

        return output    

    @staticmethod
    def split_arg(line):
        """Split a line of arguments"""

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


    def pattern_design(self):
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


