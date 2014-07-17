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

class CmdSet(CmdBase.CmdBase):
    """Command SET"""

    def __init__(self,main):
        CmdBase.CmdBase.__init__(self,main,"set")

    def do_other(self,object,operator,value):
        # Looking for '='
        if operator!='=' :
            logging.error("syntax error with the command 'set'.")
            self.help()
            return

        # Looking for one dot in the name
        objs = object.split('.')
        if len(objs)!=2 :
            logging.error("syntax error with the command 'set'.")
            self.help()
            return

        # Looking for dataset data
        elif self.main.datasets.Find(objs[0]):

            # good value
            theValue = value
            theValue2 = ""
            theValue3 = ""

            # Detecting "+ number"
            value2 = value.split('+')
            if len(value2)==2:
                if value2[1] in ['0','1','2','3','4']:
                    theValue = value2[0]
                    theValue2 = '+'
                    theValue3 = value2[1]

            # Detecting "- number"
            value2 = value.split('-')
            if len(value2)==2:
                if value2[1] in ['0','1','2','3','4']:
                    theValue = value2[0]
                    theValue2 = '-'
                    theValue3 = value2[1]

            self.main.datasets.Get(objs[0]).user_SetParameter(objs[1],theValue,theValue2,theValue3)
            return

        # Anything else
        else :
            logging.error("no object called '"+objs[0]+"' is found")
            return


    def do_main(self,args):

        # Looking for '='
        if args[1]!='=' :
            logging.error("syntax error with the command 'set'.")
            self.help()
            return

        # Looking for one dot in the name
        object = args[0]
#        object = object.lower()
        object = object.replace('fastsim.bjet_id.','fastsim.bjet_idXXX')
        object = object.replace('fastsim.tau_id.','fastsim.tau_idXXX')
        objs = object.split('.')
        for i in range(len(objs)):
            objs[i] = objs[i].replace('XXX','.')

        if len(objs)==2 and objs[0].lower()=='main':
            self.main.user_SetParameter(objs[1],args[2])
        elif len(objs)==3 and objs[0].lower()=='main' and objs[1].lower()=='isolation':
            self.main.isolation.user_SetParameter(objs[2],args[2])
        elif len(objs)==3 and objs[0].lower()=='main' and objs[1].lower()=='merging':
            self.main.merging.user_SetParameter(objs[2],args[2],self.main.mode,self.main.archi_info.has_fastjet)
        elif len(objs)==3 and objs[0].lower()=='main' and objs[1].lower()=='fastsim':
            self.main.fastsim.user_SetParameter(objs[2],args[2],self.main.datasets,self.main.mode,self.main.archi_info.has_fastjet,self.main.archi_info.has_delphes,self.main.archi_info.has_delphesMA5tune) 
        elif len(objs)==3 and objs[0].lower()=='main' and objs[1].lower()=='shower':
            self.main.shower.user_SetParameter(objs[2],args[2],self.main.mode,self.main.mcatnloutils)
        else:
            logging.error("syntax error with the command 'set'.")
            self.help()
            return

        
    def do_selection(self,args):
        # set selection [ i ] .variable = value
        #     0         1 2 3 4         5 6

        # Checking number of arguments
        if len(args)<7:
            logging.error("wrong number of arguments")
            return

        # Looking for '=', '[' and ']' 
        if args[0]!='selection' or args[1]!='[' or not args[2].isdigit() or \
               args[3]!=']' or not args[4].startswith('.') or args[5]!='=' :
            logging.error("syntax error with the command 'set'.")
            self.help()
            return

        # Looking for selection
        index = int(args[2])
        if index>=1 and index<=len(self.main.selection):
            variable = args[4].replace('.','')
            self.main.selection[index-1].user_SetParameter(variable,args[6])
            return
        else:
            logging.error("selection['" + str(index) + "'] does not exist")
            return

        return


    def do(self,args,line=""):

        # Isolating " " or ' '
        first=True
        arguments=[]
        begin=-1
        for ind in range(len(line)):
            if line[ind] not in ['"',"'"]:
                continue
            if first:
                if len(line[:ind])!=0:
                    arguments.append(line[:ind])
                first=False
                begin=ind
                continue
            else:
                if len(line[begin:ind+1]):
                    arguments.append(line[begin:ind+1])
                begin=ind
                continue
        if begin==-1:
            arguments.append(line)
        elif len(line[begin+1:])!=0:
            arguments.append(line[begin+1:])

        # Splitting arguments
        args2 = []
        for item in arguments:
            if item[0] in ['"',"'"]:
                args2.append(item)
            else:
                item=item.replace("+ ","+")
                item=item.replace(" +","+")
                item=item.replace("- ","-")
                item=item.replace(" -","-")
                args2.extend(item.split())

        # First check of argument
        if len(args2)<3:
            logging.error("wrong number of arguments for the command 'set'.")
            return

        # Checking argument number
        if args2[0].startswith("selection"):
            self.do_selection(args2)
        elif args2[0].startswith("main"):
            self.do_main(args2)
        elif len(args2)==3:
            self.do_other(args2[0],args2[1],args2[2])
        else:
            logging.error("object called '" + args2[0] +\
                          "' is unknown or has no options to set.")
            self.help()
            return


    def help(self):
        logging.info("   Syntax: set <object>.<variable> = <value>")
        logging.info("   Modifies or sets an attribute of an object to a specific value.")



    def complete_name2(self,text,object,subobject,variable,withValue):
        # Main object
        if object.lower()=='main':
            if not withValue:
                output=[ object+".isolation."+ item \
                         for item in self.main.isolation.user_GetParameters() ]
                output.extend([ object+".fastsim."+ item \
                         for item in self.main.fastsim.user_GetParameters() ])
                output.extend([ object+".merging."+ item \
                         for item in self.main.merging.user_GetParameters() ])
                output.extend([ object+".shower."+ item \
                         for item in self.main.shower.user_GetParameters() ])
                return self.finalize_complete(text,output)
            else:
                if subobject=="isolation":
                    output = self.main.isolation.user_GetValues(variable)
                elif subobject=="fastsim":
                    output = self.main.fastsim.user_GetValues(variable)
                elif subobject=="merging":
                    output = self.main.merging.user_GetValues(variable)
                elif subobject=="shower":
                    output = self.main.shower.user_GetValues(variable)
                return self.finalize_complete(text,output)
        # Other cases
        else:
            return []


    def complete_name(self,text,object,variable,withValue):
        
        # Only object name
        if variable==None:
            output = ["main"]
            output.extend(self.main.datasets.GetNames())
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
                    if not withValue:
                        output=[ object + "." + item \
                                 for item in \
                                 self.main.selection[index-1].user_GetParameters() ]
                        return self.finalize_complete(text,output)
                    else :
                        return self.finalize_complete(text,self.main.selection[index-1].user_GetValues(variable))
            return []
        
        # Main object
        elif object.lower()=='main':
            if not withValue:
                output=[ object+"."+ item \
                         for item in self.main.user_GetParameters() ]
                output.extend([ object+".isolation."+ item \
                               for item in self.main.isolation.user_GetParameters() ])
                output.extend([ object+".fastsim."+ item \
                               for item in self.main.fastsim.user_GetParameters() ])
                output.extend([ object+".merging."+ item \
                               for item in self.main.merging.user_GetParameters() ])
                output.extend([ object+".shower."+ item \
                                for item in self.main.shower.user_GetParameters() ])
                return self.finalize_complete(text,output)
            else:
                return self.finalize_complete(text,self.main.user_GetValues(variable))

        # Dataset object    
        elif self.main.datasets.Find(object):
            if not withValue:
                output=[ object+"."+ item \
                         for item in \
                         self.main.datasets.Get(object).user_GetParameters() ]
                return self.finalize_complete(text,output)
            else:
                return self.finalize_complete(text,self.main.datasets.Get(object).user_GetValues(variable))

        # Other cases
        else:
            return []


    def complete(self,text,line,begidx,endidx):
        # set  object.variable = value
        # 0    1               2 3 
        args = line.split()
        nargs = len(args)
        if not text:
            nargs +=1

        # Checking number of arguments
        if nargs==2 and len(args)==1:
            return self.complete_name(text,None,None,None)
        elif nargs==3:
            return self.finalize_complete(text,["="])
        elif nargs>4:
            return []

        # Splitting
        object = args[1]
#        object = object.lower()
        object = object.replace('fastsim.bjet_id.','fastsim.bjet_idXXX')
        object = object.replace('fastsim.tau_id.','fastsim.tau_idXXX')
        objs = object.split('.')
        for i in range(len(objs)):
            objs[i] = objs[i].replace('XXX','.')
        
        if len(objs)==1:
            return self.complete_name(text,objs[0],None,False)
        elif len(objs)==2:
            withValue = False
            if nargs==4:
                withValue=True
            return self.complete_name(text,objs[0],objs[1],withValue)
        elif len(objs)==3:
            withValue = False
            if nargs==4:
                withValue=True
            return self.complete_name2(text,objs[0],objs[1],objs[2],withValue) 
        else:
            return []
        

        
