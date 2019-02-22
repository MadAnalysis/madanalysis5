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


import logging
import os

class DelphesModule():
    
    def __init__(self):
        self.name=''
        self.type=''
        self.inputs=[]
        self.outputs=[]
        self.ModuleExecutionPaths=[]
        self.Modules=[]
        self.PileUps=[]


class DelphesCardChecker():

    def __init__(self,dirname,main):
        self.dirname = dirname
        self.main    = main


    def getNameCard(self):
        if self.main.fastsim.package=="delphes":
            cardname = self.main.fastsim.delphes.card
        elif self.main.fastsim.package=="delphesMA5tune":
            cardname = self.main.fastsim.delphesMA5tune.card
        return self.dirname+"/Input/"+cardname        
        

    def checkPresenceCard(self):
        logging.getLogger('MA5').debug("Check the presence of the Delphes card: "+self.getNameCard()+' ...')
        if not os.path.isfile(self.getNameCard()):
            logging.getLogger('MA5').error('DelphesCard is not found: '+self.getNameCard())
            return False
        return True
                          

    def editCard(self):
        logging.getLogger('MA5').debug("Invite the user to edit the Delphes card: "+self.getNameCard()+' ...')
        if self.main.forced or self.main.script:
            return True

        logging.getLogger('MA5').info("Would you like to edit the Delphes Card? (Y/N)")
        allowed_answers=['n','no','y','yes']
        answer=""
        while answer not in  allowed_answers:
            answer=raw_input("Answer: ")
            answer=answer.lower()
        if answer=="no" or answer=="n":
            return True
        else:
            # TO BE CODED PROPERLY AND CHECK IF THE COMMAND HAS WORKED
            os.system(self.main.session_info.editor+" "+self.getNameCard())
            return True


    def checkContentCard(self):
        logging.getLogger('MA5').debug("Check the content of the Delphes card: "+self.getNameCard()+' ...')

        self.extractContentCard()
        test = self.decodeContentCard()
        if test or self.main.forced or self.main.script:
            return test
        else:
            logging.getLogger('MA5').info("Are you sure to go on with this Card? (Y/N)")
            allowed_answers=['n','no','y','yes']
            answer=""
            while answer not in  allowed_answers:
                answer=raw_input("Answer: ")
                answer=answer.lower()
            if answer=="no" or answer=="n":
                return False # no, give up
            else:
                return True # go on       
    

    def extractContentCard(self):
        ExecutionMode=False
        self.ModuleExecutionPaths=[]
        self.Modules=[]
        self.PileUps=[]
        module=""
        input = open(self.getNameCard())
        counter=0
        for line in input:
            counter = counter+1

            #isolating '=' character 
            line = line.replace('=',' = ')
            line = line.replace('{',' { ')
            line = line.replace('}',' } ')
            
            #cleaning the line
            line = line.lstrip()

            #rejecting comment line
            if line.startswith('#'):
                continue

            #rejecting comment part of a line
            if '#' in line:
                line = line.split('#')[0]
            line = line.rstrip()

            #splitting line
            split = line.split()
            if len(split)==0:
                continue
            if ExecutionMode:
                if split[0]=='}':
                    ExecutionMode=False
                elif len(split)==1:
                    self.ModuleExecutionPaths.append(split[0])
                else:
                    logging.getLogger("MA5").warning('Problem with Delphes card: incorrect syntax @ line '+str(counter))
                continue
            if len(split)>=3 and split[0]=='module':
                MyModule=DelphesModule()
                MyModule.name = split[2]
                MyModule.type = split[1]
                module=MyModule.type
                self.Modules.append(MyModule)
            if len(split)>=3 and split[0] in ['set','add'] and split[1].endswith('InputArray'):
                if len(self.Modules)==0:
                    logging.getLogger("MA5").warning('Problem with Delphes card: InputArray before module definition @ line '+str(counter))
                else:          
                    self.Modules[-1].inputs.append(split[2])
                    if len(split)==4:
                        self.Modules[-1].outputs.append(split[3])
                    
            if len(split)>=3 and split[0]=='set' and split[1].endswith('OutputArray'):
                if len(self.Modules)==0:
                    logging.getLogger("MA5").warning('Problem with Delphes card OutputArray before module definition @ line '+str(counter))
                else:          
                    self.Modules[-1].outputs.append(split[2])
            if len(split)>=3 and split[0]=='set' and split[1]=='PileUpFile' and module=='PileUpMerger':
                self.PileUps.append(split[2])
            if len(split)>=2 and split[0]=='set' and split[1]=='ExecutionPath':
                ExecutionMode=True
            
        input.close()
 
    def decodeContentCard(self):
        # check that modules are declared
        logging.getLogger("MA5").debug("- Check that the modules to execute are declared (#modules="+str(len(self.ModuleExecutionPaths))+")...")
        test = True
        for i in self.ModuleExecutionPaths:
            ok=False
            for j in self.Modules: 
                if i==j.name:
                    ok=True
                    break
            if not ok:
                logging.getLogger("MA5").warning("Problem with Delphes card: module "+item+" is not declared.")
                test=False

        # check pileup path
        logging.getLogger("MA5").debug("- Check the pile-up files (#files="+str(len(self.PileUps))+")...")
        for item in self.PileUps:
            if not os.path.isfile(item):
                logging.getLogger("MA5").warning("Problem with Delphes card: the pile-up file "+item+" is not found.")
                test=False

        # check inputs and output
        logging.getLogger("MA5").debug("- Check the input of modules...")
        for module in self.Modules:
            for input in module.inputs:
                words=input.split('/')
                if len(words)!=2:
                    logging.getLogger("MA5").warning("Problem with Delphes card: the module "+module.name+" has a bad syntax for the input collection: "+input)
                    test=False
                else:
                    theModule=words[0]
                    theCollection=words[1]

                    ok=False
                    outputModule=0
                    for i in self.Modules:
                       if theModule==i.name:
                           outputModule=i
                           ok=True
                           break
                    if not ok and theModule!='Delphes':
                        logging.getLogger("MA5").warning("Problem with Delphes card: the module "+module.name+" has unknown InputArray module called: "+theModule)
                        test=False
                    elif theModule!='Delphes':
                        ok=False
                        for j in outputModule.outputs:
                            if j==theCollection:
                                ok=True
                                break
                        if not ok:
                            logging.getLogger("MA5").warning("Problem with Delphes card: the module "+module.name+" has unknown InputArray label called: "+theCollection)
                            test=False


        return test 

        
