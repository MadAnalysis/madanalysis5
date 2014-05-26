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


"""A user friendly command line interface to access MadAnalysis features.
   Uses the cmd package for command interpretation and tab completion.
"""

import logging
import readline
import os

# Import Interpreter core
from madanalysis.interpreter.interpreter_base import InterpreterBase

# Import MadAnalysis main class
from madanalysis.core.main import Main

# Import Readers for multiparticles initializing
from madanalysis.IOinterface.particle_reader import ParticleReader
from madanalysis.IOinterface.multiparticle_reader import MultiparticleReader
from madanalysis.enumeration.report_format_type import ReportFormatType
from madanalysis.enumeration.cut_type import CutType

# List of command
from madanalysis.interpreter.cmd_set     import CmdSet
from madanalysis.interpreter.cmd_define  import CmdDefine
from madanalysis.interpreter.cmd_import  import CmdImport
from madanalysis.interpreter.cmd_remove  import CmdRemove
from madanalysis.interpreter.cmd_swap    import CmdSwap
from madanalysis.interpreter.cmd_display import CmdDisplay
from madanalysis.interpreter.cmd_display_particles      import CmdDisplayParticles
from madanalysis.interpreter.cmd_display_multiparticles import CmdDisplayMultiparticles
from madanalysis.interpreter.cmd_display_datasets       import CmdDisplayDatasets
from madanalysis.interpreter.cmd_plot     import CmdPlot
from madanalysis.interpreter.cmd_cut      import CmdCut
from madanalysis.interpreter.cmd_submit   import CmdSubmit
from madanalysis.interpreter.cmd_open     import CmdOpen
from madanalysis.interpreter.cmd_reset    import CmdReset
from madanalysis.interpreter.cmd_install  import CmdInstall


#===============================================================================
# Interpreter
#===============================================================================
class Interpreter(InterpreterBase):
    """Particularisation of the cmd command for MA5"""

    def __init__(self, main,*arg, **opt):

        # Calling constructor from InterpreterBase
        InterpreterBase.__init__(self, *arg, **opt)

        # Getting back main
        self.main = main

        # Getting back all commands
        self.cmd_set                    = CmdSet(main)
        self.cmd_define                 = CmdDefine(main)
        self.cmd_display                = CmdDisplay(main)
        self.cmd_display_datasets       = CmdDisplayDatasets(main)
        self.cmd_display_particles      = CmdDisplayParticles(main)
        self.cmd_display_multiparticles = CmdDisplayMultiparticles(main)
        self.cmd_import                 = CmdImport(main)
        self.cmd_remove                 = CmdRemove(main)
        self.cmd_swap                   = CmdSwap(main) 
        self.cmd_plot                   = CmdPlot(main)
        self.cmd_reject                 = CmdCut(main,CutType.REJECT)
        self.cmd_select                 = CmdCut(main,CutType.SELECT)
        self.cmd_reset                  = CmdReset(main)
        self.cmd_open                   = CmdOpen(main)
        self.cmd_submit                 = CmdSubmit(main)
        self.cmd_resubmit               = CmdSubmit(main,resubmit=True)
        self.cmd_install                = CmdInstall(main)
        
        # Initializing multiparticle
        self.InitializeParticle()
        self.InitializeMultiparticle()

        # Importing history
        self.history_file = os.path.normpath(self.main.archi_info.ma5dir + '/.ma5history')
        try:
            readline.read_history_file(self.history_file)
        except:
            pass
                                    
    def __del__(self):
        try:
            readline.set_history_length(100)
            readline.write_history_file(self.history_file)
        except:
            pass
                

    def do_set(self,line):
        self.cmd_set.do(self.split_arg(line),line)

    def help_set(self):
        self.cmd_set.help()

    def complete_set(self,text,line,begidx,endidx):
        return self.cmd_set.complete(text,line,begidx,endidx)

    def do_define(self,line):
        self.cmd_define.do(self.split_arg(line))

    def help_define(self):
        self.cmd_define.help()

    def complete_define(self,text,line,begidx,endidx):
        return self.cmd_define.complete(text,line,begidx,endidx)

    def do_display(self,line):
        self.cmd_display.do(self.split_arg(line))

    def help_display(self):
        self.cmd_display.help()

    def complete_display(self,text,line,begidx,endidx):
        return self.cmd_display.complete(text,line,begidx,endidx)

    def do_display_particles(self,line):
        self.cmd_display_particles.do(self.split_arg(line))

    def help_display_particles(self):
        self.cmd_display_particles.help()

    def complete_display_particles(self,text,line,begidx,endidx):
        return self.cmd_display_particles.complete(text,line,begidx,endidx)

    def do_display_multiparticles(self,line):
        self.cmd_display_multiparticles.do(self.split_arg(line))

    def help_display_multiparticles(self):
        self.cmd_display_multiparticles.help()

    def complete_display_multiparticles(self,text,line,begidx,endidx):
        return self.cmd_display_multiparticles.complete(text,line,begidx,endidx)

    def do_display_datasets(self,line):
        self.cmd_display_datasets.do(self.split_arg(line))

    def help_display_datasets(self):
        self.cmd_display_datasets.help()

    def complete_display_datasets(self,text,line,begidx,endidx):
        return self.cmd_display_datasets.complete(text,line,begidx,endidx)

    def do_import(self,line):
        self.cmd_import.do(self.split_arg(line),self,self.history)

    def help_import(self):
        self.cmd_import.help()

    def complete_import(self,text,line,begidx,endidx):
        return self.cmd_import.complete(text,line,begidx,endidx)

     # Restart
    def do_restart(self, line):
        """ sending a signal allowing to restart the interpreter """
        if self.main.script:
            logging.warning("'restart' command is not allowed in script mode.")
        else:
            YES=False
            # Asking the safety question
            if not Main.forced:
                logging.warning("Are you sure to restart the MadAnalysis 5 session? (Y/N)")
                allowed_answers=['n','no','y','yes']
                answer=""
                while answer not in  allowed_answers:
                   answer=raw_input("Answer: ")
                   answer=answer.lower()
                   if answer=="no" or answer=="n":
                       YES=False
                       break
                   elif answer=='yes' or answer=='y':
                       YES=True
                       break
            if YES:
                self.main.repeatSession=True
                return True
            else:
                pass

    def help_restart(self):
        logging.info("   Syntax: restart ")
        logging.info("   Quit the current MadAnalysis sessiona and open a new one.")
        logging.info("   All the information will be discarded.")

    def do_remove(self,line):
        self.cmd_remove.do(self.split_arg(line))

    def help_remove(self):
        self.cmd_remove.help()

    def complete_remove(self,text,line,begidx,endidx):
        return self.cmd_remove.complete(text,line,begidx,endidx)

    def do_swap(self,line):
        self.cmd_swap.do(self.split_arg(line))

    def help_swap(self):
        self.cmd_swap.help()

    def complete_swap(self,text,line,begidx,endidx):
        return self.cmd_swap.complete(text,line,begidx,endidx)

    def do_install(self,line):
        result = self.cmd_install.do(self.split_arg(line))
        if result=='restart':
            logging.info(" ")
            logging.info("MadAnalysis 5 must be restarted for taking into account the present installation.")
            return self.do_restart('restart')

    def help_install(self):
        self.cmd_install.help()

    def complete_install(self,text,line,begidx,endidx):
        return self.cmd_install.complete(text,self.split_arg(line),begidx,endidx)

    def do_open(self,line):
        self.cmd_open.do(self.split_arg(line))

    def help_open(self):
        self.cmd_open.help()

    def complete_open(self,text,line,begidx,endidx):
        return self.cmd_open.complete(text,line,begidx,endidx)

    def do_reset(self,line):
        self.cmd_reset.do(self.split_arg(line),self)

    def help_reset(self):
        self.cmd_reset.help()

    def complete_reset(self,text,line,begidx,endidx):
        return self.cmd_reset.complete(text,line,begidx,endidx)

    def do_submit(self,line):
        self.cmd_submit.do(self.split_arg(line),self.history)

    def help_submit(self):
        self.cmd_submit.help()

    def complete_submit(self,text,line,begidx,endidx):
        return self.cmd_submit.complete(text,line,begidx,endidx)

    def do_resubmit(self,line):
        self.cmd_resubmit.do(self.split_arg(line),self.history)

    def help_resubmit(self):
        self.cmd_resubmit.help()

    def complete_resubmit(self,text,line,begidx,endidx):
        return self.cmd_resubmit.complete(text,line,begidx,endidx)

    def do_plot(self,line):
        self.cmd_plot.do(self.split_arg(line))

    def help_plot(self):
        self.cmd_plot.help()

    def complete_plot(self,text,line,begidx,endidx):
        tmp = line.replace("["," [ ")
        tmp = tmp.replace("]"," ] ")
        tmp = tmp.replace(")"," ) ")
        tmp = tmp.replace("("," ( ")
        tmp = tmp.replace(","," , ")
        return self.cmd_plot.complete(text,self.split_arg(tmp),begidx,endidx)

    def do_reject(self,line):
        self.cmd_reject.do(self.split_arg(line))

    def help_reject(self):
        self.cmd_reject.help()

    def complete_reject(self,text,line,begidx,endidx):
        tmp = line.replace("["," [ ")
        tmp = tmp.replace("]"," ] ")
        tmp = tmp.replace("("," ( ")
        tmp = tmp.replace(")"," ) ")
        return self.cmd_reject.complete(text,self.split_arg(tmp),begidx,endidx)

    def do_select(self,line):
        self.cmd_select.do(self.split_arg(line))

    def help_select(self):
        self.cmd_select.help()

    def complete_select(self,text,line,begidx,endidx):
        tmp = line.replace("["," [ ")
        tmp = tmp.replace("]"," ] ")
        tmp = tmp.replace("("," ( ")
        tmp = tmp.replace(")"," ) ")
        return self.cmd_select.complete(text,self.split_arg(tmp),begidx,endidx)

    def InitializeParticle(self):
        input = ParticleReader(self.main.archi_info.ma5dir,self.cmd_define,self.main.mode)
        input.Load()

    def InitializeMultiparticle(self):
        input = MultiparticleReader(self.main.archi_info.ma5dir,self.cmd_define,self.main.mode,self.main.forced)
        input.Load()

    # PreLoop
    def preloop(self):
        """Initializing before starting the main loop"""
        self.prompt = 'ma5>'
#        if readline and not 'libedit' in readline.__doc__:
#            readline.set_completion_display_matches_hook(self.print_suggestions)
            

    def deal_multiple_categories(self, dico):
        """convert the multiple category in a formatted list understand by our
        specific readline parser"""

        if 'libedit' in readline.__doc__:
            # No parser in this case, just send all the valid options
            out = []
            for name, opt in dico.items():
                out += opt
            return out

        # That's the real work
        out = []
        valid=0
        # if the key starts with number order the key with that number.
        for name, opt in dico.items():
            if not opt:
                continue
            name = name.replace(' ', '_')
            valid += 1
            out.append(opt[0].rstrip()+'@@'+name+'@@')
            # Remove duplicate
            d = {}
            for x in opt:
                d[x] = 1    
            opt = list(d.keys())
            opt.sort()
            out += opt

            
        if valid == 1:
            out = out[1:]
        return out
    
    def print_suggestions(self, substitution, matches, longest_match_length) :
        """print auto-completions by category"""
        longest_match_length += len(self.completion_prefix)
        try:
            if len(matches) == 1:
                self.stdout.write(matches[0]+' ')
                return
            self.stdout.write('\n')
            l2 = [a[-2:] for a in matches]
            if '@@' in l2:
                nb_column = self.getTerminalSize()//(longest_match_length+1)
                pos=0
                for val in self.completion_matches:
                    if val.endswith('@@'):
                        category = val.rsplit('@@',2)[1]
                        category = category.replace('_',' ')
                        self.stdout.write('\n %s:\n%s\n' % (category, '=' * (len(category)+2)))
                        start = 0
                        pos = 0
                        continue
                    elif pos and pos % nb_column ==0:
                        self.stdout.write('\n')
                    self.stdout.write(self.completion_prefix + val + \
                                      ' ' * (longest_match_length +1 -len(val)))
                    pos +=1
                self.stdout.write('\n')
            else:
                # nb column
                nb_column = self.getTerminalSize()//(longest_match_length+1)
                for i,val in enumerate(matches):
                    if i and i%nb_column ==0:
                        self.stdout.write('\n')
                    self.stdout.write(self.completion_prefix + val + \
                                     ' ' * (longest_match_length +1 -len(val)))
                self.stdout.write('\n')
    
            self.stdout.write(self.prompt+readline.get_line_buffer())
            self.stdout.flush()
        except Exception, error:
            if __debug__:
                 print error
            
    def getTerminalSize(self):
        def ioctl_GWINSZ(fd):
            try:
                import fcntl, termios, struct, os
                cr = struct.unpack('hh', fcntl.ioctl(fd, termios.TIOCGWINSZ,
                                                     '1234'))
            except:
                return None
            return cr
        cr = ioctl_GWINSZ(0) or ioctl_GWINSZ(1) or ioctl_GWINSZ(2)
        if not cr:
            try:
                fd = os.open(os.ctermid(), os.O_RDONLY)
                cr = ioctl_GWINSZ(fd)
                os.close(fd)
            except:
                pass
        if not cr:
            try:
                cr = (env['LINES'], env['COLUMNS'])
            except:
                cr = (25, 80)
        return int(cr[1])

    #def complete(self, text, state):
    def complete2(self,text,state):
        """Return the next possible completion for 'text'.
         If a command has not been entered, then complete against command list.
         Otherwise try to call complete_<command> to get list of completions.
        """
                
        if state == 0:
            import readline
            origline = readline.get_line_buffer()
            line = origline.lstrip()
            stripped = len(origline) - len(line)
            begidx = readline.get_begidx() - stripped
            endidx = readline.get_endidx() - stripped
            
            if ';' in line:
                begin, line = line.rsplit(';',1)
                begidx = begidx - len(begin) - 1
                endidx = endidx - len(begin) - 1
                if line[:begidx] == ' ' * begidx:
                    begidx=0

            if begidx>0:
                cmd, args, foo = self.parseline(line)
                if cmd == '':
                    compfunc = self.completedefault
                else:
                    try:
                        compfunc = getattr(self, 'complete_' + cmd)
                    except AttributeError:
                        compfunc = self.completedefault
            else:
                compfunc = self.completenames
                
            # correct wrong splittion with '\ '
            if line and begidx > 2 and line[begidx-2:begidx] == '\ ':
                Ntext = line.split(os.path.sep)[-1]
                self.completion_prefix = Ntext.rsplit('\ ', 1)[0] + '\ '
                to_rm = len(self.completion_prefix) - 1
                Nbegidx = len(line.rsplit(os.path.sep, 1)[0]) + 1
                data = compfunc(Ntext.replace('\ ', ' '), line, Nbegidx, endidx)
                self.completion_matches = [p[to_rm:] for p in data 
                                              if len(p)>to_rm]                
            # correct wrong splitting with '-'
            elif line and line[begidx-1] == '-':
             try:    
                Ntext = line.split()[-1]
                self.completion_prefix = Ntext.rsplit('-',1)[0] +'-'
                to_rm = len(self.completion_prefix)
                Nbegidx = len(line.rsplit(None, 1)[0])
                data = compfunc(Ntext, line, Nbegidx, endidx)
                self.completion_matches = [p[to_rm:] for p in data 
                                              if len(p)>to_rm]
             except Exception, error:
                 print error
            else:
                self.completion_prefix = ''
                self.completion_matches = compfunc(text, line, begidx, endidx)
        #print self.completion_matches

        self.completion_matches = [ (l[-1] in [' ','@','=',os.path.sep] 
                      and l or (l+' ')) for l in self.completion_matches if l]
        
        try:
            return self.completion_matches[state]
        except IndexError, error:
            #if __debug__:
            #    print '\n Completion ERROR:'
            #    print error
            #    print '\n'
            return None    


    def correct_splitting(line):
        """if the line finish with a '-' the code splits in a weird way
           on GNU_SPLITTING"""
                
        line = line.lstrip()
        if line[-1] in [' ','\t']:
            return '', line, len(line),len(enidx)
        return text, line, begidx, endidx

        
