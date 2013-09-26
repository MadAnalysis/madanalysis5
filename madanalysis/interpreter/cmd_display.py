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


import madanalysis.interpreter.cmd_base as CmdBase
import logging

class CmdDisplay(CmdBase.CmdBase):
    """Command DISPLAY"""

    def __init__(self,main):
        CmdBase.CmdBase.__init__(self,main,"display")

    def do_other(self,object):

        # Looking for one dot in the name
#        object = object.lower()
        object = object.replace('fastsim.bjet_id.','fastsim.bjet_idXXX')
        object = object.replace('fastsim.tau_id.','fastsim.tau_idXXX')
        objs = object.split('.')
        for i in range(len(objs)):
            objs[i] = objs[i].replace('XXX','.')
        if len(objs)>3 or len(objs)==0:
            logging.error("syntax error with the command 'display'.")
            self.help()
            return

        # Main
        if objs[0].lower()=='main':
            if len(objs)==1:
                self.main.Display()
                return
            elif len(objs)==2:
                if objs[1].lower()=="fastsim":
                    self.main.fastsim.Display()
                elif objs[1].lower()=="isolation":
                    self.main.isolation.Display()
                elif objs[1].lower()=="merging":
                    self.main.merging.Display()
                else:
                    self.main.user_DisplayParameter(objs[1])
                return
            else:
                if objs[1].lower()!='isolation' and objs[1].lower()!='fastsim' and objs[1].lower()!='merging':
                    logging.error("'main' has no variable set called '"+objs[1]+"'")
                    return
                elif objs[1].lower()=='fastsim':
                    self.main.fastsim.user_DisplayParameter(objs[2])
                elif objs[1].lower()=='isolation':
                    self.main.isolation.user_DisplayParameter(objs[2])
                elif objs[1].lower()=='merging':
                    self.main.merging.user_DisplayParameter(objs[2])
                else:
                    self.main.user_DisplayParameter(objs[2])
                return

        # Selection general content
        elif objs[0].lower()=='selection':
            if len(objs)==1:
                self.main.selection.Display()
                return
            else:
                logging.error("'selection' has no variable to be set.")
                return

        # Dataset display
        elif self.main.datasets.Find(objs[0]):
            if len(objs)==1:
                self.main.datasets.Get(objs[0]).Display()
                return
            else:
                self.main.datasets.Get(objs[0]).user_DisplayParameter(objs[1].lower())
                return

        # Multiparticle display
        elif self.main.multiparticles.Find(objs[0]):
            if len(objs)==1:
                self.main.multiparticles.Get(objs[0]).Display()
                return
            else:
                logging.error("'"+objs[0]+"' has no variable to be set.")
                return
                
        # Adding ids to the multiparticle
        logging.error("no object called '"+objs[0]+"' found.")


    def do_selection(self,args):
        # Looking for '=', '[' and ']' 
        if args[0]!='selection' or args[1]!='[' or not args[2].isdigit() or \
               args[3]!=']' :
            logging.error("syntax error with the command 'display'.")
            self.help()
            return

        if len(args)==5 and not args[4].startswith('.') :
            logging.error("syntax error with the command 'display'.")
            self.help()
            return

        # Looking for selection
        index = int(args[2])
        if index>=1 and index<=len(self.main.selection):
            if len(args)==5:
                variable = args[4].replace('.','')
                self.main.selection[index-1].user_DisplayParameter(variable)
                return
            else:
                self.main.selection[index-1].Display()
        else:
            logging.error("selection['" + str(index) + "'] not found.")
            return

        return

    
    def do(self,args):
        # Checking argument number
        if len(args)==1:
            return self.do_other(args[0])
        elif len(args)==5 or len(args)==4:
            return self.do_selection(args)
        else:
            logging.error("wrong number of arguments for the command 'display'.")
            self.help()
            return


    def help(self):
        logging.info("   Syntax: display <object name>")
        logging.info("   Syntax 2: display <object name>.<properties>")
        logging.info("   Displays the definition of an object, or one of its properties.")

    def complete(self,text,line,begidx,endidx):
        # set  object.variable = value
        # 0    1               2 3 
        args = line.split()
        nargs = len(args)
        if not text:
            nargs +=1

        # Checking number of arguments
        if nargs>2:
            return []
        elif nargs==2 and len(args)==1:
            return self.complete_name(text,None,None)

        # Splitting
        object = args[1]
#        object = object.lower()
        object = object.replace('fastsim.bjet_id.','fastsim.bjet_idXXX')
        object = object.replace('fastsim.tau_id.','fastsim.tau_idXXX')
        objs = object.split('.')
        for i in range(len(objs)):
            objs[i] = objs[i].replace('XXX','.')

        if len(objs)==1:
            return self.complete_name(text,objs[0],None)
        elif len(objs)==2:
            return self.complete_name(text,objs[0],objs[1])
        elif len(objs)==3:
            return self.complete_name(text,objs[0],objs[1])
        else:
            return []
        

    def complete_name(self,text,object,variable):
        
        # Only object name
        if variable==None:
            output = ["main","selection"]
            output.extend(self.main.datasets.GetNames())
            output.extend(self.main.multiparticles.GetNames())
            output.extend( [ "selection["+str(ind+1)+"]" \
                             for ind in \
                             range(0,len(self.main.selection)) ] )
            return self.finalize_complete(text,output)

        # Selection object
        if object.startswith("selection[") and object.endswith("]"):
            tmp = object.replace("["," [ ")
            tmp = tmp.replace("]"," ] ")
            tmp = tmp.lstrip()
            vec = tmp.split()
            if len(vec)==4 and vec[2].isdigit():
                index = int(vec[2])
                if index>=1 and index<=len(self.main.selection):
                    output=[ object + "." + item \
                             for item in \
                             self.main.selection[index-1].user_GetParameters() ]
                    return self.finalize_complete(text,output)
            return []
        
        # Main object
        elif object.lower()=='main':
            output=[ object+"."+ item \
                     for item in self.main.user_GetParameters() ]
            output.extend([ object+".isolation."+ item \
                     for item in self.main.isolation.user_GetParameters() ])
            output.extend([ object+".fastsim."+ item \
                     for item in self.main.fastsim.user_GetParameters() ])
            output.extend([ object+".merging."+ item \
                     for item in self.main.merging.user_GetParameters() ])
            return self.finalize_complete(text,output)

        # Dataset object    
        elif self.main.datasets.Find(object):
            output=[ object+"."+ item \
                     for item in \
                     self.main.datasets.Get(object).user_GetParameters() ]
            return self.finalize_complete(text,output)

        # Other cases
        else:
            return []


