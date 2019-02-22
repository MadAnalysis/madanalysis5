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


"""
This module has been initially developed by Jack Araz <jack.araz@concordia.ca>
and integrated into MadAnalysis 5 by Benjamin Fuks

Updated on Fri Jun 29 11:38:00 2018
  BF: Integration in v1.7beta

Updated on Fri May 18 22:40:20 2018
    Display module integrated to MA5s display command. Error handlers are 
    reregulated.

Updated on Tue May 29 11:58:27 2018
    Error handling expanded for sake of more stable program. ALL set as default
    if user doesnt specify a bout function will be applied to all events.
"""

import madanalysis.interpreter.cmd_base as CmdBase
import logging

#from madanalysis.interpreter.cmd_selection_base import CmdSelectionBase
#from madanalysis.interpreter.SetEfficiency      import SetEfficiency
#from madanalysis.IOinterface.library_writer     import LibraryWriter

#import glob, os


class CmdDefineTagger(CmdBase.CmdBase):
    """Command DEFINE_TAGGER"""

    ###################################
    #                                 #
    #     INITIALIZATION AND RESET    #
    #                                 #
    ###################################
    def __init__(self,main):
        self.logger = logging.getLogger('MA5')
        tag_algo    = ['bTagEff','bMistagC','bMistagLight','tauTagEff',\
                  'tauMistag','cTagEff','cMistag']
        CmdBase.CmdBase.__init__(self,main,"define_tagger")
        self.reset()

    def reset(self):
        self.jet_selection = []
        self.module        = ''
        self.function      = ''
        self.bounds        = []
        self.bound1        = ''
        self.bound2        = ''

    ###################################
    #                                 #
    #           MAIN METHOD           #
    #                                 #
    ###################################
    def do(self,args):
        self.reset()
 ###       if args[0].upper() == 'TEST':
 ###            print "AAAAA"
###            print self.SetEfficiency.dev_RuleDict()
### 
### 
###         ## run change algo: update <PID> <PID> <Nrule> <function> [<bound> and <bound>]
###         elif args[0].upper() == 'UPDATE':
###             args.remove(args[0])
###             if len(args) <= 3:
###                 self.logger.error('Please specify the module, rule number,')
###                 self.logger.error('fuction and/or bound that you wish to change.')
###                 self.logger.error('define_tagger update <PIDtrue> <PIDreco> <Nrule>'+\
###                                   ' <function> [<bound> and <bound>]')
###                 return None
###             else: 
###                 module, args = self.__module_finder(args)
###                 try:
###                     NRule = int(args[0])
###                     args.remove(args[0])
###                 except:
###                     self.logger.error('Please specify the module, rule number,')
###                     self.logger.error('fuction and/or bound that you wish to change.')
###                     self.logger.error('define_tagger update <PIDtrue> <PIDreco> <Nrule>'+\
###                                   ' <function> [<bound> and <bound>]')
###                     self.logger.error('Nrule can be learned via display option')
###                     return None
###                 
###                 function, bound1, bound2 = self.__function_bound_finder(args)
###                 try:
###                     self.SetEfficiency.user_ChangeRule(module,NRule,\
###                                                        bound1=bound1,\
###                                                        bound2=bound2,\
###                                                        function=function)
###                 except:
###                     self.logger.error('An unexpected error occured please contact')
###                     self.logger.error('the developer with your `define_tagger`')
###                     self.logger.error('history.')
###                     return None
### 
### 
###         ##define_tagger remove <PID> <PID> Nrule
###         elif args[0].upper() == 'REMOVE':
###             args.remove(args[0])
###             if len(args) < 3:
###                 self.logger.error('Please specify the module and the rule number')
###                 self.logger.error('define_tagger remove <PIDtrue> <PIDreco> <Nrule>')
###                 return None
###             else:
###                 module, args = self.__module_finder(args)
###                 try:
###                     Nrule = int(args[0])
###                 except:
###                     self.logger.error('Invalid syntax: Rule number has to be an'+\
###                                       ' integer.')
###                     return None
###                 try:
###                     self.SetEfficiency.user_RemoveRule(module,Nrule)
###                 except:
###                     self.logger.error('An unexpected error occured please contact')
###                     self.logger.error('the developer with your `define_tagger`')
###                     self.logger.error('history. ')
###                     return None
### 
###         ## define_tagger clear <PID> <PID>
###         elif args[0].upper() == 'CLEAR':
###             args.remove(args[0])
###             if len(args) < 2:
###                 self.logger.error('Please specify the module.')
###                 self.logger.error('define_tagger clear <PIDtrue> <PIDreco>')
###                 return None
###             else:
###                 module, args = self.__module_finder(args)
###                 try:
###                     self.SetEfficiency.user_ClearRules(module)
###                 except:
###                     self.logger.error('An unexpected error occured please contact')
###                     self.logger.error('the developer with your `define_tagger`')
###                     self.logger.error('history. ')
###                     return None
### 
### 
###         elif args[0].upper() == 'HELP':
###             self.help()
### 
### 
### #        elif args[0].upper() == 'SUBMIT':
### #            try:
### #                self.SetEfficiency.HeaderWriter()
### #            except:
### #                self.logger.error('An unexpected error occured please contact')
### #                self.logger.error('the developer with your `define_tagger`')
### #                self.logger.error('history. ')
### #                return None
### 
### 
###         # define_tagger save name
###         elif args[0].upper() == 'SAVE':
###             args.remove(args[0])
###             try:
###                 os.mkdir(self.main.archi_info.ma5dir+\
###                          '/tools/SavedAlgo')
###             except: pass
###             try: os.mkdir(self.main.archi_info.ma5dir+\
###                          '/tools/SavedAlgo/Tagger')
###             except: pass
###             if len(args) > 0:
###                 name = args[0]
###             else:
###                 name_list = os.listdir(self.main.archi_info.ma5dir+\
###                                        '/tools/SavedAlgo/Tagger/')
###                 numbering = []
###                 for j in name_list:
###                     numbering.append(int(j.replace('.json','').split('_')[1]))
###                 if numbering != []:
###                     name = 'Algo_'+str(max(numbering)+1)
###                 else: name = 'Algo_0'
###             try:
###                 self.SetEfficiency.user_SaveRule(name,\
###                                              self.main.archi_info.ma5dir+\
###                                              '/tools/SavedAlgo/Tagger/')
###             except:
###                 self.logger.error('An unexpected error occured please contact')
###                 self.logger.error('the developer with your `define_tagger`')
###                 self.logger.error('history. ')
###                 return
### 
### 
###         # define_tagger load NAME MODULE
###         elif args[0].upper() == 'LOAD':
###             args.remove(args[0])
###             try:
###                 os.mkdir(self.main.archi_info.ma5dir+\
###                          '/tools/SavedAlgo')
###             except: pass
###             try: os.mkdir(self.main.archi_info.ma5dir+\
###                          '/tools/SavedAlgo/Tagger')
###             except: pass
### 
###             name_list = os.listdir(self.main.archi_info.ma5dir+\
###                                              '/tools/SavedAlgo/Tagger/')
###             name_str  = str(name_list).replace('[','').replace(']','').replace('.json','')
###             if name_list == []:
###                 self.logger.error('There is no preinstalled algorithm.')
###                 return
###             if len(args) > 0:
###                 name = args[0]
###                 args.remove(name)
###             else:
###                 self.logger.error('Please define the name of the preinstalled algorithm.')
###                 if name_list != []: self.logger.error('Valid names are; ' + name_str)
###                 return
### 
###             if len(args) > 0:
###                 module, args = self.__module_finder(args)
###             else: module = 'ALL'
###             if module == None: module = 'ALL'
### 
###             name_check = False
###             for i in name_list:
###                 if name == i.replace('.json',''):
###                     name_check = True
###                     break
###             if not name_check:
###                 self.logger.error(name + ' does not exist.')
###                 if name_list != []:
###                     self.logger.error('Valid names are; ' + name_str)
###                 return
###             else:
###                 try:
###                     self.SetEfficiency.user_LoadRule(module,name,\
###                      self.main.archi_info.ma5dir+'/tools/SavedAlgo/Tagger/')
###                 except:
###                     self.logger.error('An unexpected error occured please contact')
###                     self.logger.error('the developer with your `define_tagger`')
###                     self.logger.error('history. ')
###                     return 
### 
### 

        if len(args)>=2:
            module = self.main.tagger.module_finder(args[:2])
            function, bounds = self.decode_args(args[2:])

            if function == None:
                self.logger.error('Ooops, can not find the function.')
                return None
            if module in [None, False]:
                self.logger.error('Invalid tagging combination: ' + str(module)+'.')
                return None
            if bounds == []:
                bounds = 'ALL'
                self.logger.debug('No condition (function to be applied to all.')
            self.main.tagger.Add(module,function,bounds)
            print "FIX EFFICIENCIES"
#            self.SetEfficiency.user_Addrule(module,function,bounds)
        else:
            self.help()

### ###################################
### #                                 #
### #             Submit              #
### #                                 #
### ###################################
### 
###     def submit(self,path):
###         return self.SetEfficiency.HeaderWriter(filepath=path)


    ###################################
    #                                 #
    #   Get functions and conditions  #
    #                                 #
    ###################################
    def decode_args(self,args):
        Nbracket1=0
        Nbracket2=0
        beginOptions = -1
        endOptions = -1
        foundOptions = False

        for i in range(0,len(args)):
            if args[i]=='(':
                Nbracket1+=1
            elif args[i]==')':
                Nbracket1-=1
            elif args[i] == '[':
                Nbracket2+=1
                if Nbracket1==0:
                    beginOptions = i
                    foundOptions=True
            elif args[i] == ']':
                Nbracket2-=1
                if Nbracket1==0:
                    endOptions = i

        if Nbracket1!=0:
            self.logger.error("number of opening brackets '(' and number of " +\
                          "closing brakets ')' does not match.")
            return None, []
        if Nbracket2!=0:
            self.logger.error("number of opening squared-brackets '[' and number of " +\
                          "closing squared-brakets ']' does not match.")
            return None, []
        if beginOptions==0:
            return None, []

        if not foundOptions:
            beginOptions=len(args)

        bounds   = []
        function = ''
        for elem in args[:beginOptions]:
            function += elem.upper()
        current_bound = ''
        for elem in args[beginOptions+1:endOptions]:
            if elem.upper() != 'AND':
                current_bound += elem.upper()
            else:
                bounds.append(current_bound)
                current_bound = ''
        if current_bound!='':
            bounds.append(current_bound)

        return function, bounds


    ###################################
    #                                 #
    #               HELP              #
    #                                 #
    ###################################
    def help(self):
        self.logger.info("")
        self.logger.info("   **********************************************************")
        self.logger.info("")
        self.logger.info("                        Tagger Module                        ")
        self.logger.info("")
        self.logger.info("   **********************************************************")
        self.logger.info("")
        self.logger.info("     - Adding a tagging algorithm:")
        self.logger.info('       define_tagger <ID_true> <ID_reco> <function> [<bounds>]')
        self.logger.info("")
        self.logger.info('        ** <ID_true> is the true object identifier')
        self.logger.info('        ** <ID_reco> is the reconstructed object identifier')
        self.logger.info('        ** <function>: any polynomial/trigonometric/hyperbolic/')
        self.logger.info('                       logaritmic function of PT/ETA/ABSETA')
        self.logger.info('        ** <bounds>: pT/eta range (optional)')
        self.logger.info("")
        self.logger.info("   **********************************************************")
        self.logger.info("")
        self.logger.info('     - Additional methods')
        self.logger.info('        ** display_tagger [<ID_true> <ID_reco>]')
        self.logger.info('           [displays all (or a specific) user-defined tagging rules]')
        self.logger.info('        ** update_tagger <ID_true> <ID_reco> <n_rule> <function> [<bounds>]')
        self.logger.info('           [update a given rule (labeled by <n_rule>, cf. the output]')
        self.logger.info('           [of the display command) of a given tagging algorithm    ]')
        self.logger.info('        ** remove_tagger <ID_true> <ID_reco> <n_rule>')
        self.logger.info('           [remove a given rule (labeled by <n_rule>, cf. the output]')
        self.logger.info('           [of the display command) of a given tagging algorithm    ]')
        self.logger.info('        ** remove_tagger <ID_reco> <ID_reco>')
        self.logger.info('           [remove a given tagging algorithm]')
        self.logger.info('        ** save_tagger <name>')
        self.logger.info('           [Saves the current tagger')
        self.logger.info('        ** load_tagger <name>')
        self.logger.info('           [Laod a saved tagger')
        self.logger.info("")
        self.logger.info("   **********************************************************")
        self.logger.info("")
        self.logger.info('     - Examples: ')
        self.logger.info("        ** define_tagger 5 21 0.707+5.6*1E-3*PT [30<pt<150]")
        self.logger.info("          # [Mistagging rate of a b-jet as a light jet]")
        self.logger.info("        ** define_tagger 5 5 6.27*1E-5*PT+3.1*1e-7*pt^2 [pt>150]")
        self.logger.info("          # [Tagging efficiency of a b-jet]")
        self.logger.info("        ** display tagger")
        self.logger.info("          # [Display the previously defined rule (n_rules)]")
        self.logger.info("        ** update_tagger 5 21 1 [2.5<eta]")
        self.logger.info("          # [Changes the bounds to which one of the taggers applies]")
        self.logger.info("        ** save_tagger blablabla")
        self.logger.info("          # [Saves the current tagger properties]")
        self.logger.info("        ** remove_tagger 5 21 1")
        self.logger.info("          # [Removes the mistagging rate rule #1]")
        self.logger.info("        ** remove_tagger 5 5")
        self.logger.info("          # [Removes the entire b-tagging module]")
        self.logger.info("        ** load_tagger blablabla")
        self.logger.info("          # [Loads a predefined tagger]")
        self.logger.info("")
        self.logger.info("   **********************************************************")

