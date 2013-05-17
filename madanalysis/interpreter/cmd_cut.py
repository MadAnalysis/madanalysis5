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


from madanalysis.interpreter.cmd_base           import CmdBase
from madanalysis.interpreter.cmd_selection_base import CmdSelectionBase
from madanalysis.multiparticle.particle_object  import ParticleObject
from madanalysis.multiparticle.extraparticle    import ExtraParticle
from madanalysis.enumeration.combination_type   import CombinationType
from madanalysis.enumeration.observable_type    import ObservableType
from madanalysis.enumeration.connector_type     import ConnectorType
from madanalysis.enumeration.operator_type      import OperatorType
from madanalysis.enumeration.cut_type           import CutType
from madanalysis.selection.cut                  import Cut
from madanalysis.selection.condition_type       import ConditionType
from madanalysis.selection.condition_sequence   import ConditionSequence
from madanalysis.selection.condition_connector  import ConditionConnector
from madanalysis.enumeration.argument_type      import ArgumentType
from madanalysis.observable.observable_base     import ObservableBase
import logging


class CmdCut(CmdBase,CmdSelectionBase):
    """Command CUT"""

    def __init__(self,main,cut_type):
        self.cut_type=cut_type
        CmdBase.__init__(self,main,\
                         CutType.convert2cmdname(self.cut_type))


    def do(self,args):

        # Skipping empty args
        if len(args)==0:
            logging.error("wrong syntax")
            self.help()
            return

        foundArguments  = False
        foundOptions    = False
        foundConditions = False 

        # 1. First check : counting number of braces ( ) and [ ]
        # 2. Detecting if there is argument or option
        endArguments=-1
        beginOptions=-1
        Nbracket1=0
        Nbracket2=0
        for i in range(0,len(args)):
            if args[i]=='(':
                Nbracket1+=1
                if i==0:
                    foundArguments=True
            elif args[i]==')':
                Nbracket1-=1
                endArguments=i
            elif args[i]=='[':
                Nbracket2+=1
                beginOptions=i
            elif args[i]==']':
                Nbracket2-=1
                if i==(len(args)-1):
                    foundOptions=True
                
        if Nbracket1!=0:
            logging.error("number of opening-bracket '(' and number of " +\
                          "closing-braket ')' does not match.")
            return
        if Nbracket2!=0:
            logging.error("number of opening-bracket '[' and number of " +\
                          "closing-braket ']' does not match.")
            return

        if not foundOptions:
            beginOptions=len(args)

        # Is there candidate ?
        # Candidate : ( A B C ... ) with A, B, C, ... are not observables 
        foundCandidate=False
        endCandidate=-1
        if args[0]=='(':
            Nbracket1=1
            nObservables=0
            for i in range(1,len(args)):
                if args[i]=='(':
                    Nbracket1+=1
                elif args[i]==')':
                    Nbracket1-=1
                elif args[i] in self.main.observables.full_list:
                    nObservables+=1
                if Nbracket1==0:
                    if nObservables==0:
                        foundCandidate=True
                        endCandidate=i
                    break

        # Extracting the possible candidate
        if foundCandidate:
            parts = self.extract_particle(args[1:endCandidate])
            if parts==None:
                return
        else:
            parts = ParticleObject()
            
        # Extracting the conditions
        condition = ConditionSequence(mother=True)
        if foundCandidate:
            argType=ArgumentType.PARTICLE
            for part in parts:
                if len(part)>1:
                    argType=ArgumentType.COMBINATION

            result = self.extract_sequence(condition,\
                                           args[endCandidate+1:beginOptions],\
                                           partType=argType)
        else:
            result = self.extract_sequence(condition,\
                                           args[:beginOptions],\
                                           partType=None)
        if result==None:
            return
            
        # Creating cut
        cut = Cut(parts,condition,self.cut_type)

        # Setting options
        if foundOptions:
            if not self.extract_options(cut,args[beginOptions+1:len(args)-1]):
                return

        # Adding the cut to selection
        self.main.selection.Add(cut)


    def clean_sequence(self,sequence):
        if len(sequence)<2:
            return sequence
        if sequence[0]=='(' and sequence[-1]==')':
            return self.clean_sequence(sequence[1:-1])
        else:
            return sequence

    
    def extract_sequence(self,current,sequence,partType):

        # Remove extra braces
        words=self.clean_sequence(sequence)

        # Loop over the words
        iword=0
        while iword < len(words):

            # opening brace
            if words[iword]=='(':

                # opening-brace is allowed ?
                if len(current.sequence)!=0 and \
                   current.sequence[-1].__class__.__name__!='ConditionConnector':
                    logging.error('opening-brace can be used at first position or after a logical connector')
                    return None

                # looking for the correspondig closing-brace
                nbrace=1
                endBrace=-1
                for j in range(iword+1,len(words)):
                    if words[j]=='(':
                        nbrace+=1
                    elif words[j]==')':
                        nbrace-=1
                    if nbrace==0:
                        endBrace=j
                        break

                # closing-brace found ?
                if endBrace==-1:
                    logging.error('no closing-brace for all opening-braces')
                    return None

                # add a new sequence
                current.sequence.append(ConditionSequence())
                res=self.extract_sequence(current.sequence[-1],\
                                          words[iword+1:endBrace],\
                                          partType)
                if res==None:
                    return None
                iword=endBrace
            
            # connector
            elif words[iword] in ['or','and']:
                if len(current.sequence)==0 or \
                       current.sequence[-1].__class__.__name__=="ConditionConnector":
                    logging.error("connector '"+words[iword]+\
                                  "' must be used only after a condition block")
                else:
                    current.sequence.append(ConditionConnector(words[iword]))

            # other
            else:
                
                # cut condition is allowed ?
                if len(current.sequence)!=0 and \
                   current.sequence[-1].__class__.__name__!='ConditionConnector':
                    logging.error("cut condition beginning by '"+words[iword]+\
                                  "' must be used at first position or " +\
                                  "after a logical connector")
                    return None

                # looking for the end of the block condition
                endCondition=-1
                for j in range(iword+1,len(words)):
                    if words[j] in ['and','or']:
                        endCondition=j
                        break
                    
                if endCondition!=-1:
                    res=self.extract_condition(current,\
                                               words[iword:endCondition],\
                                               partType)
                    if res==None:
                        return None
                    iword=endCondition-1
                else:
                    res=self.extract_condition(current,\
                                               words[iword:],\
                                               partType)
                    if res==None:
                        return None
                    break # quit while loop
                
            # incrementing
            iword+=1

        # Last check
        if current.sequence[-1].__class__.__name__=="ConditionConnector":
            logging.error("a condition cannot be finished with a connector '" +\
                          current.sequence[-1].GetStringDisplay()+"'.")
            return None

        return True 


    def layout_condition(self,words):

        # Empty case
        if len(words)==0:
            return words

        # Glue parts
        args=[]
        args.append(words[0])
        for i in range(1,len(words)):
            if words[i]=='=':
                if words[i-1] in ['=','<','>','!']:
                    args[-1]+='='
                    continue
            args.append(words[i])

        # Return results    
        return args
        

    def extract_condition(self,current,words,partType):

        # layout condition
        words = self.layout_condition(words)

        # checking number of arguments
        if len(words)<3:
            logging.error("condition '"+str(words)+"' is not correct.")
            return None

        # looking for observable
        # determining if doubleCondition case
        doubleCondition=False
        if words[0] not in self.main.observables.full_list:
            if words[2] not in self.main.observables.full_list:
                logging.error("no observable found in condition '"+str(words)+ "'")
                return None
            else:
                doubleCondition=True
                
        # double condition case
        if doubleCondition:

            # checking number of arguments
            if len(words)<5:
                logging.error("condition '"+str(words)+"' is not correct.")
                return None
            
            # extracting threshold
            try:
                threshold1=float(words[0])
            except:
                logging.error("'"+words[0]+"' must be a float value.")
                return None

            # extracting operator
            operator1=self.extract_operator(words[1])
            if operator1==OperatorType.UNKNOWN:
                logging.error("operator '"+words[1]+"' is unknown.")
                return None

        # extracting threshold
        try:
            threshold=float(words[-1])
        except:
            logging.error("'"+words[-1]+"' must be a float value.")
            return None

        # extracting operator
        operator=self.extract_operator(words[-2])
        if operator==OperatorType.UNKNOWN:
            logging.error("operator '"+words[-2]+"' is unknown.")
            return None

        # extracting observable
        obsName = words[0]
        if doubleCondition:
            obsName = words[2]

        if partType==None and obsName not in self.main.observables.cut_event_list:
            logging.error("observable '"+obsName+"' cannot be used for "+\
                          "rejecting an event.")
            return None
        if partType!=None and obsName not in self.main.observables.cut_candidate_list:
            logging.error("observable '"+obsName+"' cannot be used for "+\
                          "rejecting a candidate.")
            return None
        
        obsRef=self.main.observables.get(obsName)
        if partType==ArgumentType.COMBINATION and (len(obsRef.args)==0 or obsRef.args[0]==ArgumentType.PARTICLE):
            logging.error("observable '"+obsName+"' can be used on a particle but not a combination of particles")
            return None

        # Case with arguments
        arguments=[]
        if (partType!=None and len(obsRef.args)>1) or \
           (partType==None and len(obsRef.args)>0) :

            # Checking opening-brace
            if (doubleCondition and words[3]!='(') or \
               (not doubleCondition and words[1]!='(')    :
                logging.error("wrong syntax for the condition '" +\
                              str(words)+"'")
                return None

            # Checking closing-brace
            if words[-3]!=")":
                logging.error("wrong syntax for the condition '" +\
                              str(words)+"'")
                return None

            # Creating new obsRef
            if partType==None:
                obsRef2=obsRef
            else:
                obsRef2 = ObservableBase.Clone(obsRef,\
                                               args=obsRef.args[1:])

            # Extracting arguments
            if doubleCondition:
                arguments=self.extract_arguments(words[4:-3],\
                                                 obsName,\
                                                 obsRef2)
            else:
                arguments=self.extract_arguments(words[2:-3],\
                                                 obsName,\
                                                 obsRef2)

            # Checking arguments
            if arguments==None:
                return None
                
        # Case with no arguments
        else:
            if (doubleCondition and len(words)!=5) or \
               (not doubleCondition and len(words)!=3) :
                logging.error("wrong number of arguments in the condition '"+\
                              str(words)+"'")
                return None

        # Checking operator consistency with double condition
        if doubleCondition:
            if not (  ( operator1 in [OperatorType.GREATER,OperatorType.GREATER_EQUAL] ) \
                      and \
                      ( operator  in [OperatorType.GREATER,OperatorType.GREATER_EQUAL] ) ) \
               and \
               not (  ( operator1 in [OperatorType.LESS,OperatorType.LESS_EQUAL] ) \
                      and \
                      ( operator  in [OperatorType.LESS,OperatorType.LESS_EQUAL] ) ):
                logging.error('double conditions allowed are : < obs < , > obs >')
                return None
            
        # Storing condition
        if doubleCondition:
            current.sequence.append(ConditionSequence())
            if operator1==OperatorType.LESS:
                newOperator1=OperatorType.GREATER
            elif operator1==OperatorType.GREATER:
                newOperator1=OperatorType.LESS
            elif operator1==OperatorType.LESS_EQUAL:
                newOperator1=OperatorType.GREATER_EQUAL
            elif operator1==OperatorType.GREATER_EQUAL:
                newOperator1=OperatorType.LESS_EQUAL
            condition1 = ConditionType(obsRef, arguments, newOperator1, threshold1)
            current.sequence[-1].sequence.append(condition1)
            current.sequence[-1].sequence.append(ConditionConnector('and'))
            condition2 = ConditionType(obsRef, arguments, operator, threshold)
            current.sequence[-1].sequence.append(condition2)
        else:
            condition = ConditionType(obsRef, arguments, operator, threshold)
            current.sequence.append(condition)

        return True    
            
        
    def decodeConditions(self,args2):
            
        conditions=ConditionBlock()
        current=conditions
        nparameter=0
        observable1=0
        observable2=0
        operator=0
        value=0.
        for item in args:
            
            # Opening bracket
#            if item=="(":
#                if nparameter!=0:
#                    logging.error("problem with an opening bracket") 
#                    return None
#                block=ConditionBlock()
#                block.mother=current
#                current.Add(block,0)
#                current=block
#            elif item==")":
#                if nparameter==3:
#                    logging.error("problem with a closing bracket")
#                    return None
#                current=current.mother

            # Observable 
            if nparameter==0:
                obs=self.extract_observable(item)
                if obs==None:
                    return None
                if not ObservableType.isCuttable(obs[1]):
                    logging.error("a cut applied to the observable '"+\
                                  item+"' is not possible")
                    return None
                observable1=obs[0]
                observable2=obs[1]
                nparameter=1

            # Operator    
            elif nparameter==1:
                operator=self.extract_operator(item)
                if operator==OperatorType.UNKNOWN:
                    return None
                nparameter=2

            # Threshold value
            elif nparameter==2:
                try:
                    value=float(item)
                except:
                    logging.error("the threshold '"+item+\
                                  "' is not a float value.")
                    return None
                nparameter=3

            # Connector     
            elif nparameter==3:
                if item=="or":
                    connector=ConnectorType.OR
                elif item=="and":
                    connector=ConnectorType.AND
                else:    
                    logging.error("'"+item+"' is not a valid connector")
                    return None
                nparameter=0
                block=ConditionType(observable1,observable2,operator,value)
                current.Add(block,connector)
                
        if nparameter==3:
            block=ConditionType(observable1,observable2,operator,value)
            current.Add(block,ConnectorType.UNKNOWN)

        return conditions    
            


    def help(self):
        logging.info("   Syntax: " + CutType.convert2cmdname(self.cut_type) +\
                     " observable_name ( multiparticle1 multiparticle2 ... ) operator threshold [ option1 option 2 ]")
        logging.info("   Declares a cut: ")
        logging.info("    - related to the distribution of a given observable, associated to one or a combination of (multi)particles,")
        logging.info("    - supported logical operators: <= , < , >= , > , == , != ,")
        logging.info("    - threshold being a value.")


    def complete(self,text,args,begidx,endidx):

        # cut ( part ... ) > = 100 and ... [ ]
        # 0   1 2    3      

        # Getting back arguments
        nargs=len(args)
        if not text:
            nargs += 1

        # Safe : impossible case
        if nargs<=1:
            return []

        # first agument
        elif nargs==2:
            output=['(']
            output.extend(ObservableType.get_cutlist1(self.main.mode))
            return self.finalize_complete(text,output)
        elif nargs>2:
            if args[1]!='(' and not (args[1] in ObservableType.get_cutlist1(self.main.mode)):
                return []
                    
        # counting number of () and []
        nbracket1=0
        nbracket2=0
        endArguments=-1
        for i in range(len(args)):
            if args[i]=="(":
                nbracket1+=1
            if args[i]==")":
                nbracket1-=1
                if nbracket1==0:
                    endArguments=i
            if args[i]=="[":
                nbracket2+=1
            if args[i]=="]":
                nbracket2-=1

        # User is writting particle combination
        if nbracket1>0 and endArguments==-1:

            # PTrank
            if nbracket2>0:
                return []

            # List of multiparticles
            output=(self.main.multiparticles.GetNames())
            if nargs>=4:
                output.append(')')
                output.append('or')
            return self.finalize_complete(text,output)

        # determine position of conditions
        if args[1]!='(':
            endArguments=1
        else:
            endArguments+=1

        # observable with particle
        if nargs==endArguments+1:
            output=ObservableType.get_cutlist2(self.main.mode)
            return self.finalize_complete(text,output)
            
        # observable with particle
        if nargs==endArguments+2:
            output=['<=','<','>','>=','=','!=']
            return self.finalize_complete(text,output)
        if nargs==endArguments+3:
            return []


        # observable with particle
        if nargs==endArguments+4:
            output=['and','or','[']
            return self.finalize_complete(text,output)

        # options mode
        if nbracket1==0 and nbracket2==1:
            output=Cut.userShortcuts.keys()
            output.append("]")
            return self.finalize_complete(text,output)

        # loop over arguments
        case=1
        for i in range(endArguments,nargs-1):
            if case==1:
                output=ObservableType.get_cutlist2(self.main.mode)
                if not (args[i] in output):
                    return []
            elif case==2:
                output=['<=','<','>','>=','=','!=']
                if not (args[i] in output):
                    return []
            elif case==3:
                try:
                    tmp=float(args[i])
                except:
                    return []
            elif case==4:
                output=['and','or']
                if not (args[i] in output):
                    return []
            # increment index
            if case==4:
                case=1
            else:
                case+=1

        # suggest
        if case==1:
            output=ObservableType.get_cutlist2(self.main.mode)
            return self.finalize_complete(text,output)
        elif case==2:
            output=['<=','<','>','>=','=','!=']
            return self.finalize_complete(text,output)
        elif case==3:
            return []
        elif case==4:
            output=['and','or','[']
            return self.finalize_complete(text,output)
        
        return []
