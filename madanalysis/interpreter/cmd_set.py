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


from madanalysis.system.config_checker            import ConfigChecker
from madanalysis.system.user_info                 import UserInfo
from madanalysis.enumeration.ma5_running_type     import MA5RunningType
from madanalysis.IOinterface.particle_reader      import ParticleReader
from madanalysis.IOinterface.multiparticle_reader import MultiparticleReader
from madanalysis.interpreter.cmd_define           import CmdDefine
import madanalysis.interpreter.cmd_base as CmdBase
import logging

class CmdSet(CmdBase.CmdBase):
    """Command SET"""

    def __init__(self,main):
        CmdBase.CmdBase.__init__(self,main,"set")

    def do_other(self,object,operator,value):
        # Looking for '='
        if operator!='=' :
            logging.getLogger('MA5').error("syntax error with the command 'set'.")
            self.help()
            return

        # Looking for one dot in the name
        objs = object.split('.')
        if len(objs)!=2 :
            logging.getLogger('MA5').error("syntax error with the command 'set'.")
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
            logging.getLogger('MA5').error("no object called '"+objs[0]+"' is found")
            return


    def do_main(self,args):

        # Looking for '='
        if args[1]!='=' :
            logging.getLogger('MA5').error("syntax error with the command 'set'.")
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

        if len(objs)==2 and objs[0].lower()=='main' and objs[1].lower()=='recast':
            user_info    = UserInfo()
            user_info.ReadUserOptions(self.main.archi_info.ma5dir+'/madanalysis/input/installation_options.dat')
            checker = ConfigChecker(self.main.archi_info, user_info, self.main.session_info, self.main.script, False)
            bkp_delphes = self.main.archi_info.has_delphes
            bkp_ma5tune = self.main.archi_info.has_delphesMA5tune
            self.main.archi_info.has_delphes = checker.checkDelphes(True)
            self.main.archi_info.has_delphesMA5tune = checker.checkDelphesMA5tune(True)
            self.main.recasting.user_SetParameter("status",args[2],self.main.mode,self.main.archi_info,self.main.session_info, self.main.datasets)
            self.main.archi_info.has_delphes = bkp_delphes
            self.main.archi_info.has_delphesMA5tune = bkp_ma5tune
            if args[2]=='on' and self.main.fastsim.package!='none':
                logging.getLogger('MA5').warning("Fastsim package switched off and internally handled")
                self.main.fastsim.package="none"
        elif len(objs)==2 and objs[0].lower()=='main':
            if objs[1]=='mode' and args[2]=='parton':
                self.main.mode=MA5RunningType.PARTON
                self.main.ResetParameters()
                self.main.AutoSetGraphicalRenderer()
                self.main.InitObservables(self.main.mode)
                lvl = logging.getLogger('MA5').getEffectiveLevel()
                logging.getLogger('MA5').setLevel(100)
                self.main.multiparticles.Reset()
                cmd_define = CmdDefine(self.main)
                input = ParticleReader(self.main.archi_info.ma5dir,cmd_define,self.main.mode,self.main.forced)
                input.Load()
                input = MultiparticleReader(self.main.archi_info.ma5dir,cmd_define,self.main.mode,self.main.forced)
                input.Load()
                logging.getLogger('MA5').setLevel(lvl)
            else:
                self.main.user_SetParameter(objs[1],args[2])
        elif len(objs)==3 and objs[0].lower()=='main' and objs[1].lower()=='isolation':
            self.main.isolation.user_SetParameter(objs[2],args[2])
        elif len(objs)==3 and objs[0].lower()=='main' and objs[1].lower()=='fom':
            self.main.fom.user_SetParameter(objs[2],args[2])
        elif len(objs)==3 and objs[0].lower()=='main' and objs[1].lower()=='merging':
            self.main.merging.user_SetParameter(objs[2],args[2],self.main.mode,self.main.archi_info.has_fastjet)
        elif len(objs)==3 and objs[0].lower()=='main' and objs[1].lower()=='fastsim':
            if objs[2] == 'jetrecomode':
                if args[2] in ['jets', 'constituents']:
                    self.main.superfastsim.jetrecomode = args[2]
                else:
                    logging.getLogger('MA5').error("Jet smearing can only be based on the jet ('jets') or on its constituents ('constituents').")
            else:
                user_info    = UserInfo()
                user_info.ReadUserOptions(self.main.archi_info.ma5dir+'/madanalysis/input/installation_options.dat')
                checker = ConfigChecker(self.main.archi_info, user_info, self.main.session_info, self.main.script, False)
                bkp_delphes = self.main.archi_info.has_delphes
                bkp_ma5tune = self.main.archi_info.has_delphesMA5tune
                self.main.archi_info.has_delphes = checker.checkDelphes(True)
                self.main.archi_info.has_delphesMA5tune = checker.checkDelphesMA5tune(True)
                self.main.fastsim.user_SetParameter(objs[2],args[2],self.main.datasets,self.main.mode,self.main.archi_info) 
                self.main.archi_info.has_delphes = bkp_delphes
                self.main.archi_info.has_delphesMA5tune = bkp_ma5tune
            if objs[2]=='package' and args[2] in ['fastjet', 'delphes', 'delphesMA5tune'] and self.main.recasting.status=='on':
                logging.getLogger('MA5').warning("Recasting mode switched off")
                self.main.recasting.status ="off"
        elif (len(objs)==3 or (len(objs)>3 and objs[2].lower()=='add')) and \
          objs[0].lower()=='main' and objs[1].lower()=='recast':
            user_info    = UserInfo()
            user_info.ReadUserOptions(self.main.archi_info.ma5dir+'/madanalysis/input/installation_options.dat')
            checker = ConfigChecker(self.main.archi_info, user_info, self.main.session_info, self.main.script, False)
            bkp_delphes = self.main.archi_info.has_delphes
            bkp_ma5tune = self.main.archi_info.has_delphesMA5tune
            self.main.archi_info.has_delphes = checker.checkDelphes(True)
            self.main.archi_info.has_delphesMA5tune = checker.checkDelphesMA5tune(True)
            self.main.recasting.user_SetParameter(objs[2:],args[2:],self.main.mode,self.main.archi_info,self.main.session_info, self.main.datasets)
            self.main.archi_info.has_delphes = bkp_delphes
            self.main.archi_info.has_delphesMA5tune = bkp_ma5tune
        else:
            logging.getLogger('MA5').error("syntax error with the command 'set'.")
            self.help()
            return

    def do_selection(self,args):
        # set selection [ i ] .variable = value
        #     0         1 2 3 4         5 6

        # Checking number of arguments
        if len(args)<7:
            logging.getLogger('MA5').error("wrong number of arguments")
            return

        # Looking for '=', '[' and ']' 
        if args[0]!='selection' or args[1]!='[' or not args[2].isdigit() or \
               args[3]!=']' or not args[4].startswith('.') or args[5]!='=' :
            logging.getLogger('MA5').error("syntax error with the command 'set'.")
            self.help()
            return

        # Looking for selection
        index = int(args[2])
        if index>=1 and index<=len(self.main.selection):
            variable = args[4].replace('.','')
            self.main.selection[index-1].user_SetParameter(variable,args[6])
            return
        else:
            logging.getLogger('MA5').error("selection['" + str(index) + "'] does not exist")
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
            logging.getLogger('MA5').error("wrong number of arguments for the command 'set'.")
            return

        # Checking argument number
        if args2[0].startswith("selection"):
            self.do_selection(args2)
        elif args2[0].startswith("main"):
            self.do_main(args2)
        elif len(args2)==3:
            self.do_other(args2[0],args2[1],args2[2])
        else:
            logging.getLogger('MA5').error("object called '" + args2[0] +\
                          "' is unknown or has no options to set.")
            self.help()
            return


    def help(self):
        logging.getLogger('MA5').info("   Syntax: set <object>.<variable> = <value>")
        logging.getLogger('MA5').info("   Modifies or sets an attribute of an object to a specific value.")


    def complete_name2(self,text,object,subobject,variable,withValue):
        # Main object
        if object.lower()=='main':
            if not withValue:
                output=[ object+".isolation."+ item \
                         for item in self.main.isolation.user_GetParameters() ]
                output.extend([ object+".fom."+ item \
                         for item in self.main.fom.user_GetParameters() ])
                output.extend([ object+".fastsim."+ item \
                         for item in self.main.fastsim.user_GetParameters() ])
                output.extend([ object+".merging."+ item \
                         for item in self.main.merging.user_GetParameters() ])
                output.extend([ object+".recast."+ item \
                         for item in self.main.recasting.user_GetParameters() ])
                output.extend([ object+".recast.add."+ item \
                         for item in self.main.recasting.user_GetParameters('add') ])
                return self.finalize_complete(text,output)
            else:
                if subobject=="isolation":
                    output = self.main.isolation.user_GetValues(variable)
                elif subobject=="fom":
                    output = self.main.fom.user_GetValues(variable)
                elif subobject=="fastsim":
                    output = self.main.fastsim.user_GetValues(variable)
                elif subobject=="merging":
                    output = self.main.merging.user_GetValues(variable)
                elif subobject=="recast":
                    output = self.main.recasting.user_GetValues(variable)
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
                output.extend([ object+".fom."+ item \
                               for item in self.main.fom.user_GetParameters() ])
                output.extend([ object+".fastsim."+ item \
                               for item in self.main.fastsim.user_GetParameters() ])
                output.extend([ object+".merging."+ item \
                               for item in self.main.merging.user_GetParameters() ])
                output.extend([ object+".recast."+ item \
                               for item in self.main.recasting.user_GetParameters() ])
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
        elif len(objs)==4:
            withValue = False
            if nargs==4:
                withValue=True
            return self.complete_name2(text,objs[0],objs[1],objs[2], withValue)
        else:
            return []

