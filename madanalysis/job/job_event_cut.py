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


"""Writer of the C++ code of the event cuts (``select``/``reject`` without candidate).

Each condition of the cut is evaluated into ``filter[i]``; the logical expression
of the cut is then built from these results and passed to
``Manager()->ApplyCut``.
"""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING, TextIO

if TYPE_CHECKING:
    from madanalysis.core.main import Main
    from madanalysis.multiparticle.particle_combination import ParticleCombination
    from madanalysis.selection.condition_sequence import ConditionSequence
    from madanalysis.selection.condition_type import ConditionType
from madanalysis.selection.histogram          import Histogram
from madanalysis.selection.instance_name      import InstanceName
from madanalysis.enumeration.observable_type  import ObservableType
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.enumeration.cut_type         import CutType
from madanalysis.enumeration.operator_type    import OperatorType
from madanalysis.enumeration.argument_type    import ArgumentType
from madanalysis.interpreter.cmd_cut          import CmdCut
from madanalysis.enumeration.combination_type import CombinationType
import logging
from six.moves import range


def GetConditions(current: ConditionSequence,table: list[ConditionType]) -> None:
    """Flatten (recursively) the conditions of a sequence.

    Args:
        current (``ConditionSequence``): sequence of conditions.
        table (``list[ConditionType]``): list to extend (in place).
    """

    i=0
    while i<len(current.sequence):
        if current.sequence[i].__class__.__name__=="ConditionType":
            table.append(current.sequence[i])
        elif current.sequence[i].__class__.__name__=="ConditionSequence":
            GetConditions(current.sequence[i],table)
        i+=1


def GetFinalCondition(current: ConditionSequence,index: int,tagName: str) -> tuple[str, int]:
    """Build the C++ logical expression of a sequence of conditions.

    Args:
        current (``ConditionSequence``): sequence of conditions.
        index (``int``): index of the first condition of the sequence in the flattened
            list.
        tagName (``str``): name of the C++ vector of condition results.

    Returns:
        ``tuple[str, int]``:
        The expression (e.g. ``(filter[0] && (filter[1] || filter[2]))``) and the index
        following the last condition.
    """
    msg='('
    i=0
    while i<len(current.sequence):
        if current.sequence[i].__class__.__name__=="ConditionType":
            msg+=tagName+'['+str(index)+']'
            index+=1
        elif current.sequence[i].__class__.__name__=="ConditionConnector":
            msg+=' '+current.sequence[i].GetStringCode()+' '
        elif current.sequence[i].__class__.__name__=="ConditionSequence":
            msg2,index=GetFinalCondition(current.sequence[i],index,tagName)
            msg+=msg2
        i+=1
    msg+=')'
    return msg,index

def WriteEventCut(file: TextIO,main: Main,iabs: int,icut: int) -> None:
    """Write the code of an event cut.

    Args:
        file (``TextIO``): output C++ file.
        main (``Main``): session state.
        iabs (``int``): index of the cut in the selection.
        icut (``int``): 1-based number of the event cut.
    """

    # Opening bracket for the current histo
    file.write('  {\n')

    # Created condition table
    conditions = []
    GetConditions(main.selection[iabs].conditions,conditions)

    # Initializing tag
    tagName='filter'
    file.write('    std::vector<MAbool> '+tagName+'('+str(len(conditions))+',false);\n')

    # Loop over conditions
    for ind in range(len(conditions)):
        file.write('    {\n')
        WriteConditions(file,main,iabs,icut,tagName,tagIndex=ind,condition=conditions[ind])
        file.write('    }\n')

    # Writing final tag
    file.write('    MAbool ' + tagName + '_global = ' +\
               GetFinalCondition(main.selection[iabs].conditions,0,tagName)[0]+';\n')

    # Event Cut ?
    if len(main.selection[iabs].part)==0:
        if main.selection[iabs].cut_type==CutType.SELECT:
            file.write('    if(!Manager()->ApplyCut('+tagName+'_global, \"' +\
              str(icut)+'_'+main.selection[iabs].conditions.GetStringDisplay()+ '\")) return true;\n')
        else:
            file.write('    if(!Manager()->ApplyCut(!'+tagName+'_global, \"' +\
              str(icut)+'_'+main.selection[iabs].conditions.GetStringDisplay()+ '\")) return true;\n')

    # Closing bracket for the current histo
    file.write('  }\n')

    return


def WriteConditions(file: TextIO,main: Main,iabs: int,icut: int,tagName: str,tagIndex: int,condition: ConditionType) -> None:
    """Write the evaluation of a condition according to the number of arguments of its observable.

    Args:
        file (``TextIO``): output C++ file.
        main (``Main``): session state.
        iabs (``int``): index of the cut in the selection.
        icut (``int``): 1-based number of the event cut.
        tagName (``str``): name of the C++ vector of condition results.
        tagIndex (``int``): index of the condition in this vector.
        condition (``ConditionType``): condition.
    """

    if len(condition.parts)==0:
        WriteCutWith0Arg(file,main,iabs,icut,tagName,tagIndex,condition)
    elif len(condition.parts)==1:
        WriteCutWith1Arg(file,main,iabs,icut,tagName,tagIndex,condition)
    elif len(condition.parts)==2:
        WriteCutWith2Args(file,main,iabs,icut,tagName,tagIndex,condition)
    else:
        logging.getLogger('MA5').error("observable with more than 2 arguments are " +\
                      "not managed by MadAnalysis 5")


def WriteCutWith0Arg(file: TextIO,main: Main,iabs: int,icut: int,tagName: str,tagIndex: int,condition: ConditionType) -> None:
    """Write the evaluation of a condition on an event-level observable.

    Args:
        file (``TextIO``): output C++ file.
        main (``Main``): session state.
        iabs (``int``): index of the cut in the selection.
        icut (``int``): 1-based number of the event cut.
        tagName (``str``): name of the C++ vector of condition results.
        tagIndex (``int``): index of the condition in this vector.
        condition (``ConditionType``): condition.
    """
    file.write('      '+tagName+'['+str(tagIndex)+'] = (')
    file.write(condition.observable.code(main.mode)+' ')
    file.write(OperatorType.convert2cpp(condition.operator)+' ')
    file.write(str(condition.threshold))
    file.write(' );\n')


def WriteCutWith2Args(file: TextIO,main: Main,iabs: int,icut: int,tagName: str,tagIndex: int,condition: ConditionType) -> None:
    """Write the evaluation of a condition on a two-argument observable (e.g. ``DELTAR``).

    Args:
        file (``TextIO``): output C++ file.
        main (``Main``): session state.
        iabs (``int``): index of the cut in the selection.
        icut (``int``): 1-based number of the event cut.
        tagName (``str``): name of the C++ vector of condition results.
        tagIndex (``int``): index of the condition in this vector.
        condition (``ConditionType``): condition.
    """

    # Loop over combination
    for combi1 in condition.parts[0]:
        for combi2 in condition.parts[1]:
            file.write('    {\n')
            WriteJobExecute2Nbody(file,iabs,icut,combi1,combi2,main,\
                                  tagName,tagIndex,condition)
            file.write('    }\n')


def WriteCutWith1Arg(file: TextIO,main: Main,iabs: int,icut: int,tagName: str,tagIndex: int,condition: ConditionType) -> None:
    """Write the evaluation of a condition on a one-argument observable (loop over the alternatives).

    Args:
        file (``TextIO``): output C++ file.
        main (``Main``): session state.
        iabs (``int``): index of the cut in the selection.
        icut (``int``): 1-based number of the event cut.
        tagName (``str``): name of the C++ vector of condition results.
        tagIndex (``int``): index of the condition in this vector.
        condition (``ConditionType``): condition.
    """

    # Skip observable with INT of FLOAT argument
    # Temporary
    # NOTE: parts[0] is a ParticleObject, never an ArgumentType value: this test is always False.
    if condition.parts[0] in [ArgumentType.FLOAT,\
                              ArgumentType.INTEGER]:
        return

    # Loop over combination (keyword AND)
    for item in condition.parts[0]:
        file.write('    {\n')
        WriteJobExecuteNbody(file,iabs,icut,item,main,tagName,tagIndex,condition)
        file.write('    }\n')


def WriteJobExecute2Nbody(file: TextIO,iabs: int,icut: int,combi1: ParticleCombination,combi2: ParticleCombination,main: Main,tagName: str,tagIndex: int,condition: ConditionType) -> None:
    """Write the loops and the test of a two-argument observable.

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the cut in the selection.
        icut (``int``): 1-based number of the event cut.
        combi1 (``ParticleCombination``): combination of the first argument.
        combi2 (``ParticleCombination``): combination of the second argument.
        main (``Main``): session state.
        tagName (``str``): name of the C++ vector of condition results.
        tagIndex (``int``): index of the condition in this vector.
        condition (``ConditionType``): condition.
    """

    obs = condition.observable

    # ALL reserved word for the first argument
    if len(combi1)==1 and combi1.ALL:
        if obs.combination in [CombinationType.SUMSCALAR,\
                               CombinationType.DIFFSCALAR]:
            file.write('      MAdouble64 value1=0;\n')
        else:
            file.write('      ParticleBaseFormat q1;\n')

    # ALL reserved word for the second argument
    if len(combi2)==1 and combi2.ALL:
        if obs.combination in [CombinationType.SUMSCALAR,\
                               CombinationType.DIFFSCALAR]:
            file.write('      MAdouble64 value2=0;\n')
        else:
            file.write('      ParticleBaseFormat q2;\n')

    # Determine if same particle in first combi
    redundancies1 = False
    if len(combi1)>1:
        for i in range(len(combi1)):
            for j in range(len(combi1)):
                if i==j:
                    continue
                if combi1[i].particle.IsThereCommonPart(combi1[j].particle):
                    # FIXME: sets 'redundancies' instead of 'redundancies1': the redundancy treatment is never
                    # activated for the first argument.
                    redundancies = True

    # FOR loop for first combi
    WriteJobLoop(file,iabs,icut,combi1,redundancies1,main,'a')

    # Checking redundancies for second combi
    WriteJobSameCombi(file,iabs,icut,combi1,redundancies1,main,'a')

    # Determine if same particle in second combi
    redundancies2 = False
    if len(combi2)>1:
        for i in range(len(combi2)):
            for j in range(len(combi2)):
                if i==j:
                    continue
                if combi2[i].particle.IsThereCommonPart(combi2[j].particle):
                    # FIXME: sets 'redundancies' instead of 'redundancies2': the redundancy treatment is never
                    # activated for the second argument.
                    redundancies = True

    # FOR loop for second combi
    WriteJobLoop(file,iabs,icut,combi2,redundancies2,main,'b')

    # Checking redundancies for second combi
    WriteJobSameCombi(file,iabs,icut,combi2,redundancies2,main,'b')

    # ALL reserved word
    if len(combi1)==1 and combi1.ALL:
        pass

    # Getting number of combinations
    # FIXME: dead code (obs is an ObservableBase, never ObservableType.N); it would fail anyway:
    # 'combination' is undefined, open() is used instead of file.write and tagIndex is not a str.
    if obs is ObservableType.N:
        file.write('        Ncounter++;\n')
        for combi in range(len(combination)):
            file.write('      }\n')
            file.write('      }\n')
        open('      if ( Ncounter ')
        file.write(OperatorType.convert2cpp(condition.operator) + \
                   str(condition.threshold) +   \
                   ') '+tagName+'['+tagIndex+']=true;\n')

    # Normal case
    else :

        WriteJobSum2N(file,iabs,icut,combi1,combi2,main,tagName,tagIndex,condition,'a','b')
        for ind in range(len(combi1)):
            file.write('      }\n')
        for ind in range(len(combi2)):
            file.write('      }\n')


def WriteJobSum2N(file: TextIO,iabs: int,icut: int,combi1: ParticleCombination,combi2: ParticleCombination,main: Main,tagName: str,tagIndex: int,condition: ConditionType,iterator1: str,iterator2: str) -> None:
    """Write the test of a two-argument observable for the current combination.

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the cut in the selection.
        icut (``int``): 1-based number of the event cut.
        combi1 (``ParticleCombination``): combination of the first argument.
        combi2 (``ParticleCombination``): combination of the second argument.
        main (``Main``): session state.
        tagName (``str``): name of the C++ vector of condition results.
        tagIndex (``int``): index of the condition in this vector.
        condition (``ConditionType``): condition.
        iterator1 (``str``): name of the C++ index array of the first combination.
        iterator2 (``str``): name of the C++ index array of the second combination.
    """

    cut = main.selection[iabs]
    obs = condition.observable

    # Getting container name
    containers1=[]
    for item in combi1:
        containers1.append(InstanceName.Get('P_'+\
                                           item.name+cut.rank+cut.statuscode+'_REG_'+'_'.join(cut.regions)))

    containers2=[]
    for item in combi2:
        containers2.append(InstanceName.Get('P_'+\
                                           item.name+cut.rank+cut.statuscode+'_REG_'+'_'.join(cut.regions)))

    # Case of one particle/multiparticle
    if len(combi1)==1 and len(combi2)==1:
        if main.mode == MA5RunningType.PARTON:
          TheObs=obs.code_parton[:-2]
        elif main.mode == MA5RunningType.HADRON:
          TheObs=obs.code_hadron[:-2]
        else:
          TheObs=obs.code_reco[:-2]
        file.write('      if (')
        file.write(containers1[0]+'['+iterator1+'[0]]->' +\
                   TheObs+'('+containers2[0]+'['+iterator2+'[0]])' +\
                   OperatorType.convert2cpp(condition.operator) +\
                   str(condition.threshold) +\
                   ') {'+tagName+'['+str(tagIndex)+']=true; break;}\n')
        return

    # Operation : sum or diff
    if obs.combination in [CombinationType.SUMSCALAR,\
                           CombinationType.SUMVECTOR,\
                           CombinationType.DEFAULT]:
        oper_string = '+'
    else:
        oper_string = '-'

    # Vector sum/diff
    if obs.combination in [CombinationType.DEFAULT,\
                             CombinationType.SUMVECTOR,\
                             CombinationType.DIFFVECTOR]:

        # First part
        file.write('      ParticleBaseFormat q1;\n')
        for ind in range(0,len(combi1)):
            TheOper='+'
            if ind!=0:
              TheOper=oper_string
            # NOTE: '[+' generates a (harmless) unary plus in the C++ index.
            file.write('      q1'+TheOper+'='+\
                       containers1[ind]+'[+'+iterator1+'['+str(ind)+']]->'+\
                       'momentum();\n')

        # Second part
        file.write('      ParticleBaseFormat q2;\n')
        for ind in range(0,len(combi2)):
            TheOper='+'
            if ind!=0:
              TheOper=oper_string
            file.write('      q2'+TheOper+'='+\
                       containers2[ind]+'['+iterator2+'['+str(ind)+']]->'+\
                       'momentum();\n')

        # Result
        if main.mode == MA5RunningType.PARTON:
          TheObs=obs.code_parton[:-2]
        elif main.mode == MA5RunningType.HADRON:
          TheObs=obs.code_hadron[:-2]
        else:
          TheObs=obs.code_reco[:-2]
        file.write('      if (q1.'+TheObs+'(q2)'+\
                   ''+ OperatorType.convert2cpp(condition.operator) + \
                   str(condition.threshold) +   \
                   ') {'+tagName+'['+str(tagIndex)+']=true; break;}\n')


def WriteJobExecuteNbody(file: TextIO,iabs: int,icut: int,combination: ParticleCombination,main: Main,tagName: str,tagIndex: int,condition: ConditionType) -> None:
    """Write the loops and the test of a one-argument observable.

    For the multiplicity observables (``N``, ``sN``, ...), the combinations are counted
    and the count is compared to the threshold.

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the cut in the selection.
        icut (``int``): 1-based number of the event cut.
        combination (``ParticleCombination``): combination of particles.
        main (``Main``): session state.
        tagName (``str``): name of the C++ vector of condition results.
        tagIndex (``int``): index of the condition in this vector.
        condition (``ConditionType``): condition.
    """

    obs = condition.observable

    # Case of N
    if obs.name in ['N','vN','sN','sdN','dsN','dvN','vdN','dN','rN']:
        file.write('      unsigned int Ncounter=0;\n')

    # ALL reserved word
    if len(combination)==1 and combination.ALL:
        if obs.combination in [CombinationType.SUMSCALAR,\
                               CombinationType.DIFFSCALAR]:
            file.write('      MAdouble64 value=0;\n')
        else:
            file.write('      ParticleBaseFormat q;\n')

    # Determine if same particle in loop
    redundancies = False
    if len(combination)>1:
        for i in range(len(combination)):
            for j in range(len(combination)):
                if i==j:
                    continue
                if combination[i].particle.IsThereCommonPart(combination[j].particle):
                    redundancies = True

    # FOR loop
    WriteJobLoop(file,iabs,icut,combination,redundancies,main)

    # Checking redundancies
    WriteJobSameCombi(file,iabs,icut,combination,redundancies,main)

    # Getting number of combinations
    if obs.name in ['N','vN','sN','sdN','dsN','dvN','vdN','dN','rN']:
        file.write('        Ncounter++;\n')
        for combi in range(len(combination)):
            file.write('      }\n')
        file.write('      if ( Ncounter ')
        file.write(OperatorType.convert2cpp(condition.operator) + \
                   str(condition.threshold) +   \
                   ') '+tagName+'['+str(tagIndex)+']=true;\n')

    # Adding values
    else:
        WriteJobSum(file,iabs,icut,combination,main,tagName,tagIndex,condition)
        for combi in range(len(combination)):
            file.write('      }\n')


def WriteJobLoop(file: TextIO,iabs: int,icut: int,combination: ParticleCombination,redundancies: bool,main: Main,iterator: str = 'ind') -> None:
    """Open the nested C++ loops over the containers of a combination.

    With redundancies, identical particles in different containers are skipped and the
    vector ``combis`` of already-used combinations is declared.

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the cut in the selection.
        icut (``int``): 1-based number of the event cut.
        combination (``ParticleCombination``): combination of particles.
        redundancies (``bool``): some particles can appear in several containers.
        main (``Main``): session state.
        iterator (``str``, default ``'ind'``): name of the C++ index array.
    """

    cut = main.selection[iabs]

    # Getting container name
    containers=[]
    for item in combination:
        containers.append(InstanceName.Get('P_'+\
                                           item.name+cut.rank+cut.statuscode+'_REG_'+'_'.join(cut.regions)))

    # Declaring indicator
    file.write('      MAuint32 '+iterator+'['+str(len(combination))+'];\n')

    # Rendundancies case
    if redundancies:
        if main.mode in [MA5RunningType.PARTON,MA5RunningType.HADRON]:
            file.write('      std::vector<std::set<const MCParticleFormat*> > combis;\n')
        else:
            file.write('      std::vector<std::set<const RecParticleFormat*> > combis;\n')

    # Writing Loop For
    for i in range(len(combination)):
        file.write('      for ('+iterator+'['+str(i)+']=0;'\
                   +iterator+'['+str(i)+']<'+containers[i]+'.size();'\
                   +iterator+'['+str(i)+']++)\n')
        file.write('      {\n')

        # Redundancies case : managing same indices
        if i!=0 and redundancies:
            file.write('        if (')
            for j in range (0,i):
                if j!=0:
                    file.write(' || ')
                file.write(containers[i]+'['+iterator+'['+str(i)+']]=='+\
                           containers[j]+'['+iterator+'['+str(j)+']]')
            file.write(') continue;\n')     



def WriteJobSameCombi(file: TextIO,iabs: int,icut: int,combination: ParticleCombination,redundancies: bool,main: Main,iterator: str = 'ind') -> None:
    """Write the skipping of combinations already considered (in another order).

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the cut in the selection.
        icut (``int``): 1-based number of the event cut.
        combination (``ParticleCombination``): combination of particles.
        redundancies (``bool``): some particles can appear in several containers.
        main (``Main``): session state.
        iterator (``str``, default ``'ind'``): name of the C++ index array.
    """
    if len(combination)==1 or not redundancies:
        return

    cut = main.selection[iabs]

    # Getting container name
    containers=[]
    for item in combination:
        containers.append(InstanceName.Get('P_'+\
                                           item.name+cut.rank+cut.statuscode+'_REG_'+'_'.join(cut.regions)))

    file.write('\n    // Checking if consistent combination\n')
    if main.mode in [MA5RunningType.PARTON,MA5RunningType.HADRON]:
        file.write('    std::set<const MCParticleFormat*> mycombi;\n')
    else:
        file.write('    std::set<const RecParticleFormat*> mycombi;\n');
    file.write('    for (MAuint32 i=0;i<'+str(len(combination))+';i++)\n')
    file.write('    {\n')
    for i in range(0,len(combination)):
        # FIXME: the generated C++ loop inserts containers[k][iterator[i]] for every i and every k
        # (the C++ index i is used for all containers): wrong combination set and possible out-of-range access.
        file.write('      mycombi.insert('+containers[i]+'['+iterator+'[i]]);\n')
    file.write('    }\n')
    file.write('    MAbool matched=false;\n')
    file.write('    for (MAuint32 i=0;i<combis.size();i++)\n')
    file.write('      if (combis[i]==mycombi) {matched=true; break;}\n')
    file.write('    if (matched) continue;\n')
    file.write('    else combis.push_back(mycombi);\n\n')


def WriteJobSum(file: TextIO,iabs: int,icut: int,combination: ParticleCombination,main: Main,tagName: str,tagIndex: int,condition: ConditionType,iterator: str = 'ind') -> None:
    """Write the test of a one-argument observable for the current combination.

    The observable is computed on a single particle, on the scalar or vector
    sum/difference of the particles, or as the ratio ``(a-b)/a``.

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the cut in the selection.
        icut (``int``): 1-based number of the event cut.
        combination (``ParticleCombination``): combination of particles.
        main (``Main``): session state.
        tagName (``str``): name of the C++ vector of condition results.
        tagIndex (``int``): index of the condition in this vector.
        condition (``ConditionType``): condition.
        iterator (``str``, default ``'ind'``): name of the C++ index array.
    """

    cut = main.selection[iabs]
    obs = condition.observable

    # Getting container name
    containers=[]
    for item in combination:
        containers.append(InstanceName.Get('P_'+\
                                           item.name+cut.rank+cut.statuscode+'_REG_'+'_'.join(cut.regions)))

    # Case of one particle/multiparticle
    if len(combination)==1:
        file.write('        if (')
        file.write(containers[0]+'['+iterator+'[0]]->' +\
                   obs.code(main.mode) +\
                   OperatorType.convert2cpp(condition.operator) +\
                   str(condition.threshold) +\
                   ') {'+tagName+'['+str(tagIndex)+']=true; break;}\n')
        return

    # Operation : sum or diff
    if obs.combination in [CombinationType.SUMSCALAR,\
                           CombinationType.SUMVECTOR,\
                           CombinationType.DEFAULT]:
        oper_string = '+'
    else:
        oper_string = '-'

    # Scalar sum/diff
    if obs.combination in [CombinationType.SUMSCALAR,\
                           CombinationType.DIFFSCALAR]:
        file.write('         if ((')
        variables=[]
        for ind in range(len(combination)):
            variables.append(containers[ind]+'['+iterator+'['+str(ind)+']]->'+\
                             ''+\
            obs.code(main.mode))
        file.write(oper_string.join(variables))
        file.write(')'+ OperatorType.convert2cpp(condition.operator) + \
                   str(condition.threshold) +   \
                   ') {'+tagName+'['+str(tagIndex)+']=true; break;}\n')

    # Vector sum/diff
    elif obs.combination in [CombinationType.DEFAULT,\
                             CombinationType.SUMVECTOR,\
                             CombinationType.DIFFVECTOR]:
        file.write('        ParticleBaseFormat q;\n')
        for ind in range(len(combination)):
            TheOper='+'
            if ind!=0:
              TheOper=oper_string
            file.write('        q'+TheOper+'='+containers[ind]+'['+iterator+'['+str(ind)+']]->'+\
                             'momentum();\n')
        file.write('        if (q.')
        file.write(obs.code(main.mode)+\
                   ''+ OperatorType.convert2cpp(condition.operator) + \
                   str(condition.threshold) +   \
                   ') {'+tagName+'['+str(tagIndex)+']=true; break;}\n')
    # ratio
    elif obs.combination==CombinationType.RATIO and \
        len(combination)==2:
        file.write('        if (((')
        file.write(containers[0]+'['+iterator+'[0]]->'+\
                   obs.code(main.mode)+\
                   '-'+\
                   containers[1]+'['+iterator+'[1]]->'+\
                   obs.code(main.mode)+\
                   ') / ('+\
                   containers[0]+'['+iterator+'[0]]->'+\
                   obs.code(main.mode)+\
                   ')')
        file.write(')'+ OperatorType.convert2cpp(condition.operator) + \
                   str(condition.threshold) +   \
                   ') {'+tagName+'['+str(tagIndex)+']=true; break;}\n')
