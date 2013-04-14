################################################################################
#  
#  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <http://madanalysis.irmp.ucl.ac.be>
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


from madanalysis.multiparticle.particle_object import ParticleObject
from madanalysis.multiparticle.extraparticle   import ExtraParticle
from madanalysis.enumeration.operator_type     import OperatorType
from madanalysis.enumeration.argument_type     import ArgumentType
from madanalysis.enumeration.combination_type  import CombinationType
from madanalysis.enumeration.observable_type   import ObservableType
from madanalysis.selection.condition_type      import ConditionType
from madanalysis.selection.condition_sequence  import ConditionSequence
from madanalysis.selection.condition_connector import ConditionConnector
import logging

class CmdSelectionBase():

    def DisplayObservableError(self,word):
        logging.error("'"+word+\
                      "' is an unknown observable and cannot be used in a plot/cut definition.")

    def extract_observable(self,word,display=True):

        # Getting observable
        if self.main.observables.findPlotObservable(word):
            return word
        else:
            if display:
                logging.error("'"+word+\
                              "' is an unwknown observable and cannot be used int a plot definition.")
            return None



    def extract_operator(self,words):

        if len(words)==1:
            if words[0]=="=":
                return OperatorType.EQUAL
            elif words[0]=="<":
                return OperatorType.LESS
            elif words[0]==">":
                return OperatorType.GREATER
            else:
                return OperatorType.UNKNOWN
            
        elif len(words)==2:
            if words[0]=="=" and words[1]=="=":
                return OperatorType.EQUAL
            elif words[0]=="<" and words[1]=="=":
                return OperatorType.LESS_EQUAL
            elif words[0]==">" and words[1]=="=":
                return OperatorType.GREATER_EQUAL
            elif words[0]=="!" and words[1]=="=":
                return OperatorType.NOT_EQUAL
            else:
                return OperatorType.UNKNOWN

        else:
            return OperatorType.UNKNOWN
            

    def extract_arguments(self,words,obsName,obsRef):
        tmp=[]
        arguments=[]

        # Split arguments with 'comma' separation
        for item in words:
            if item!=",":
                tmp.append(item)
            else:
                arguments.append(tmp)
                tmp=[]
        if len(tmp)!=0:
            arguments.append(tmp)

        # Checking consistency between Observable and Number of arguments
        if len(arguments)!=len(obsRef.args):
            logging.error("the observable '"+obsName+"' accepts "+
                          str(len(obsRef.args))+" arguments whereas "+
                          str(len(arguments))+" arguments have been specified.")
            return None

        # Extracting according type
        results=[]
        for iarg in range(0,len(obsRef.args)):

            # one particle
            if obsRef.args[iarg]==ArgumentType.PARTICLE:
                result=self.extract_particle(arguments[iarg])
                if result==None:
                    return None
                for parts in result:
                    if len(parts)!=1:
                        logging.error("Argument "+str(iarg)+" of the observable "+\
                                      obsName+" must be a particle/multiparticle "+\
                                      "and not a combination of "+\
                                      "particles/multiparticles.")

            # particle combination
            elif obsRef.args[iarg]==ArgumentType.COMBINATION:
                result=self.extract_particle(arguments[iarg])
                if result==None:
                    return None
                if obsRef.combination==CombinationType.RATIO or \
                   obsRef.combination==CombinationType.DIFFSCALAR or \
                   obsRef.combination==CombinationType.DIFFVECTOR:
                    for parts in result:
                        if len(parts)!=2:
                            logging.error("the observable '"+obsName+ \
                                          "' is a property of a "+\
                                          "(multi)particle *pair*.")
                            return None

            # integer value
            elif obsRef.args[iarg]==ArgumentType.INTEGER:
                result=self.extract_integer(arguments[iarg])

            # float value
            elif obsRef.args[iarg]==ArgumentType.FLOAT:
                result=self.extract_float(arguments[iarg])

            # checking result
            if result==None:
                return None
            else:
                results.append(result)

        # returning the final arguments
        return results


    def extract_integer(self,words):
        theString = "".join(words)
        try:
            value=int(theString)
        except:
            value=None
            logging.error("Argument '"+theString+"' must be an integer value.") 
        return value


    def extract_float(self,words):
        theString = "".join(words)
        try:
            value=float(theString)
        except:
            value=None
            logging.error("Argument '"+theString+"' must be an float value.") 
        return value


    def extract_particle(self,words):

        # Checking first and end position
        if words[0]=="and" or words[-1]=="and":
            logging.error("the reserved word 'and' is incorrectly used.")
            return
        elif words[0]=="[" or words[-1]=="[":
            logging.error("incorrect use of the opening bracket '['.")
            return
        elif words[0]=="]":
            logging.error("incorrect use of the closing bracket ']'.")
            return
        elif words[0]=="<" or words[-1]=="<":
            logging.error("incorrect use of the '<' character.")
            return

        # Creating ParticleObject
        ALLmode = False
        parts   = []
        mothers = []
        object = ParticleObject()
        PTrankMode = 0
        motherMode = 0
        nBrackets = 0
        for item in words:
            
            # Common part
            if item=="(":
                nBrackets+=1
            elif item==")":
                nBrackets-=1
                if nBrackets<0:
                    logging.error("problem with brackets () : too much " +\
                                  "more closing-brackets.")
                    return
                
            # PT rank part
            elif PTrankMode>0:
                if PTrankMode==1:
                    if len(parts)==0:
                        logging.error("PT rank applied to no particle or " +\
                                      "multiparticle.")
                        return
                    if item=="0":
                        logging.error("PT rank cannot be equal to 0. " +\
                                      "The first PT rank is 1.")
                        return
                    try:
                        thePTrank = int(item)
                    except:
                        logging.error("PT rank '" + item + "' is not valid")
                        return
                    if len(mothers)==0:
                        if parts[-1].PTrank!=0:
                            logging.error("You cannot specify several PT " +\
                                          "ranks to '"+parts[-1].particle.name+\
                                          "'")
                            return
                        parts[-1].PTrank = thePTrank
                    else :
                        if mothers[-1].PTrank!=0:
                            logging.error("You cannot specify several PT " +\
                                          "ranks to '"+mothers[-1].particle.name+\
                                          "'")
                            return
                        mothers[-1].PTrank = thePTrank    
                    PTrankMode=2
                elif PTrankMode==2:
                    if item!="]":
                        logging.error("closing-bracket ']' is expected "+\
                                      "instead of '" + item + "'")
                        return
                    PTrankMode=0

            # Mother part
            elif motherMode>0:
                if item=="<":
                    if motherMode!=1:
                        logging.error("too much number of '<' character")
                        return
                    motherMode=2
                elif self.main.multiparticles.Find(item):
                    theMother = ExtraParticle(\
                                    self.main.multiparticles.Get(item))
                    if len(mothers)==0:
                        parts[-1].mumPart = theMother
                        if motherMode==1:
                            parts[-1].mumType = "<"
                        else:
                            parts[-1].mumType = "<<"
                    else:
                        mothers[-1].mumPart = theMother
                        if motherMode==1:
                            mothers[-1].mumType = "<"
                        else:
                            mothers[-1].mumType = "<<"
                            
                    mothers.append(theMother)
                    motherMode=0
                else:
                    logging.error("'"+item+"' is not a defined "+\
                                  "(multi)particle.")
                    return

            # Normal mode
            elif item=="and":
                if ALLmode and len(parts)>1:
                    logging.error("Reversed word 'all' must be applied in front of only (multi)particle")
                    return
                object.Add(parts,ALLmode)
                parts=[]
                mothers=[]
                ALLmode=False
            elif item=="all":
                if len(parts)!=0:
                    logging.error("Reserved word 'all' must be applied in front of a (multi)particle")
                    return 
                ALLmode=True
            elif item=="[":
                if PTrankMode!=0:
                    logging.error("You cannot specify several PT rank '[]'")
                    return
                PTrankMode=1
            elif item=="<":
                # Should not occur
                if motherMode!=0:
                    logging.error("problem with character '<'")
                    return
                motherMode=1
            elif self.main.multiparticles.Find(item):
                parts.append(ExtraParticle(self.main.multiparticles.Get(item)))
                mothers=[]
            else:
                logging.error("'"+item+"' is not a defined "+"(multi)particle.")
                return

        # End
        if nBrackets>0:
            logging.error("problem with brackets () : too much " +\
                                  "more opening-brackets")
            return
        
        if len(parts)!=0:
            if ALLmode and len(parts)>1:
                logging.error("Reversed word 'all' must be applied in front of only (multi)particle")
                return
            object.Add(parts,ALLmode)
        return object
        
        #object.Display()    
        #self.main.objects.Add(object)
        #self.main.objects.Display()
        #instance = self.main.objects.Get(object)
        #return instance


    def extract_options(self,histo,words):
        for item in words:
            test=histo.user_SetShortcuts(item)
            if not test:
                return False
        return True    

    

