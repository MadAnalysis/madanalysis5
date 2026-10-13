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


"""Parsing helpers shared by the ``plot`` and ``select``/``reject`` commands."""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING, Any

if TYPE_CHECKING:
    from madanalysis.observable.observable_base import ObservableBase
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
from six.moves import range

class CmdSelectionBase():
    """Mixin parsing observables, operators, arguments and particle expressions.

    .. note::
        The class relies on the attribute ``main`` defined by
        :class:`~madanalysis.interpreter.cmd_base.CmdBase`, which the concrete commands
        (:class:`~madanalysis.interpreter.cmd_plot.CmdPlot`,
        :class:`~madanalysis.interpreter.cmd_cut.CmdCut`) also inherit.
    """

    def __init__(self) -> None:
        """Initialise the logger."""
        self.logger       = logging.getLogger('MA5')

    def DisplayObservableError(self,word: str) -> None:
        """Log an error for an unknown observable.

        Args:
            word (``str``): name of the observable.
        """
        self.logger.error("'"+word+\
                      "' is an unknown observable and cannot be used in a plot/cut definition.")

    def extract_observable(self,word: str,display: bool = True) -> str | None:
        """Check whether a word is an observable that can be plotted.

        Args:
            word (``str``): candidate observable name.
            display (``bool``, default ``True``): log an error if the observable is unknown.

        Returns:
            ``str | None``:
            The observable name, or ``None`` if unknown.
        """

        # Getting observable
        if self.main.observables.findPlotObservable(word):
            return word
        else:
            if display:
                # FIXME: typos in the message ('unwknown', 'int a plot').
                self.logger.error("'"+word+\
                              "' is an unwknown observable and cannot be used int a plot definition.")
            return None

    def extract_operator(self,words: list[str]) -> int:
        """Decode a comparison operator split into one or two characters.

        Args:
            words (``list[str]``): characters of the operator (e.g. ``['<', '=']``).

        Returns:
            ``int``:
            Value of :class:`~madanalysis.enumeration.operator_type.OperatorType`
            (``UNKNOWN`` if not recognised).
        """

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

    def extract_arguments(self,words: list[str],obsName: str,obsRef: ObservableBase) -> list[Any] | None:
        """Decode the comma-separated arguments of an observable.

        Each argument is decoded according to the expected
        :class:`~madanalysis.enumeration.argument_type.ArgumentType` (particle, particle
        combination, integer or float).

        Args:
            words (``list[str]``): words between the parentheses of the observable.
            obsName (``str``): name of the observable (for error messages).
            obsRef (``ObservableBase``): description of the observable.

        Returns:
            ``list[Any] | None``:
            Decoded arguments (``ParticleObject``, ``int`` or ``float``), or ``None`` on
            error.
        """
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
            self.logger.error("the observable '"+obsName+"' accepts "+
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
                        # FIXME: the error is logged but None is not returned: the invalid argument is accepted.
                        self.logger.error("Argument "+str(iarg)+" of the observable "+\
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
                            self.logger.error("the observable '"+obsName+ \
                                          "' is a property of a "+\
                                          "(multi)particle *pair*.")
                            return None

            # integer value
            elif obsRef.args[iarg]==ArgumentType.INTEGER:
                result=self.extract_integer(arguments[iarg])

            # float value
            elif obsRef.args[iarg]==ArgumentType.FLOAT:
                result=self.extract_float(arguments[iarg])
            else:
                result=None

            # checking result
            if result==None:
                return None
            else:
                results.append(result)

        # returning the final arguments
        return results


    def extract_integer(self,words: list[str]) -> int | None:
        """Decode an integer argument.

        Args:
            words (``list[str]``): words of the argument (joined without separator).

        Returns:
            ``int | None``:
            The value, or ``None`` on error.
        """
        theString = "".join(words)
        try:
            value=int(theString)
        except:
            value=None
            self.logger.error("Argument '"+theString+"' must be an integer value.") 
        return value


    def extract_float(self,words: list[str]) -> float | None:
        """Decode a floating-point argument.

        Args:
            words (``list[str]``): words of the argument (joined without separator).

        Returns:
            ``float | None``:
            The value, or ``None`` on error.
        """
        theString = "".join(words)
        try:
            value=float(theString)
        except:
            value=None
            self.logger.error("Argument '"+theString+"' must be an float value.") 
        return value


    def extract_particle(self,words: list[str]) -> ParticleObject | None:
        """Decode a particle expression.

        The grammar supports combinations (``a b``), alternatives (``and``), the ``all``
        keyword, PT ranks (``a[1]``), mother requirements (``a < b`` for a direct mother,
        ``a << b`` for any ancestor) and parentheses.

        Args:
            words (``list[str]``): words of the expression.

        Returns:
            ``ParticleObject | None``:
            The decoded expression, or ``None`` on syntax error.
        """

        # Checking first and end position
        if words[0]=="and" or words[-1]=="and":
            self.logger.error("the reserved word 'and' is incorrectly used.")
            return
        elif words[0]=="[" or words[-1]=="[":
            self.logger.error("incorrect use of the opening bracket '['.")
            return
        elif words[0]=="]":
            self.logger.error("incorrect use of the closing bracket ']'.")
            return
        elif words[0]=="<" or words[-1]=="<":
            self.logger.error("incorrect use of the '<' character.")
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
                    self.logger.error("problem with brackets () : too much " +\
                                  "more closing-brackets.")
                    return
                
            # PT rank part
            elif PTrankMode>0:
                if PTrankMode==1:
                    if len(parts)==0:
                        self.logger.error("PT rank applied to no particle or " +\
                                      "multiparticle.")
                        return
                    if item=="0":
                        self.logger.error("PT rank cannot be equal to 0. " +\
                                      "The first PT rank is 1.")
                        return
                    try:
                        thePTrank = int(item)
                    except:
                        self.logger.error("PT rank '" + item + "' is not valid")
                        return
                    if len(mothers)==0:
                        if parts[-1].PTrank!=0:
                            self.logger.error("You cannot specify several PT " +\
                                          "ranks to '"+parts[-1].particle.name+\
                                          "'")
                            return
                        parts[-1].PTrank = thePTrank
                    else :
                        if mothers[-1].PTrank!=0:
                            self.logger.error("You cannot specify several PT " +\
                                          "ranks to '"+mothers[-1].particle.name+\
                                          "'")
                            return
                        mothers[-1].PTrank = thePTrank    
                    PTrankMode=2
                elif PTrankMode==2:
                    if item!="]":
                        self.logger.error("closing-bracket ']' is expected "+\
                                      "instead of '" + item + "'")
                        return
                    PTrankMode=0

            # Mother part
            elif motherMode>0:
                if item=="<":
                    if motherMode!=1:
                        self.logger.error("too much number of '<' character")
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
                    self.logger.error("'"+item+"' is not a defined "+\
                                  "(multi)particle.")
                    return

            # Normal mode
            elif item=="and":
                if ALLmode and len(parts)>1:
                    # FIXME: typo 'Reversed' (instead of 'Reserved') in the message below.
                    self.logger.error("Reversed word 'all' must be applied in front of only (multi)particle")
                    return
                object.Add(parts,ALLmode)
                parts=[]
                mothers=[]
                ALLmode=False
            elif item=="all":
                if len(parts)!=0:
                    self.logger.error("Reserved word 'all' must be applied in front of a (multi)particle")
                    return 
                ALLmode=True
            elif item=="[":
                if PTrankMode!=0:
                    self.logger.error("You cannot specify several PT rank '[]'")
                    return
                PTrankMode=1
            elif item=="<":
                # Should not occur
                if motherMode!=0:
                    self.logger.error("problem with character '<'")
                    return
                motherMode=1
            elif self.main.multiparticles.Find(item):
                parts.append(ExtraParticle(self.main.multiparticles.Get(item)))
                mothers=[]
            else:
                self.logger.error("'"+item+"' is not a defined "+"(multi)particle.")
                return

        # End
        if nBrackets>0:
            self.logger.error("problem with brackets () : too much " +\
                                  "more opening-brackets")
            return

        if len(parts)!=0:
            if ALLmode and len(parts)>1:
                # FIXME: typo 'Reversed' (instead of 'Reserved') in the message below.
                self.logger.error("Reversed word 'all' must be applied in front of only (multi)particle")
                return
            object.Add(parts,ALLmode)
        return object

        #object.Display()    
        #self.main.objects.Add(object)
        #self.main.objects.Display()
        #instance = self.main.objects.Get(object)
        #return instance


    def extract_options(self,histo: Any,words: list[str]) -> bool:
        """Apply shortcut options (e.g. ``logY``, ``normalize2one``) to a histogram.

        Args:
            histo (``Any``): histogram (``Histogram``, ``HistogramFrequency`` or
                ``HistogramLogX``).
            words (``list[str]``): options.

        Returns:
            ``bool``:
            ``False`` at the first invalid option, ``True`` otherwise.
        """
        for item in words:
            test=histo.user_SetShortcuts(item)
            if not test:
                return False
        return True    

    

