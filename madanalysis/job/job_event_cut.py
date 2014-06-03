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


def GetConditions(current,table):

    i=0
    while i<len(current.sequence):
        if current.sequence[i].__class__.__name__=="ConditionType":
            table.append(current.sequence[i])
        elif current.sequence[i].__class__.__name__=="ConditionSequence":
            GetConditions(current.sequence[i],table)
        i+=1


def GetFinalCondition(current,index,tagName):

    msg='('
    i=0
    while i<len(current.sequence):
        if current.sequence[i].__class__.__name__=="ConditionType":
            msg+=tagName+'['+str(index)+']'
            index+=1
        elif current.sequence[i].__class__.__name__=="ConditionConnector":
            msg+=' '+current.sequence[i].GetStringCode()+' '
        elif current.sequence[i].__class__.__name__=="ConditionSequence":
            msg+=GetFinalCondition(current.sequence[i],index,tagName)
        i+=1
    msg+=')'
    return msg
    

def WriteEventCut(file,main,iabs,icut):

    # Opening bracket for the current histo
    file.write('  {\n')

    # Created condition table
    conditions = []
    GetConditions(main.selection[iabs].conditions,conditions)

    # Initializing tag
    tagName='filter'
    file.write('  std::vector<Bool_t> '+tagName+'('+str(len(conditions))+',false);\n')

    # Loop over conditions
    for ind in range(len(conditions)):
        file.write('  {\n')
        WriteConditions(file,main,iabs,icut,tagName,tagIndex=ind,condition=conditions[ind])
        file.write('  }\n')

    # Writing final tag
    file.write('  Bool_t ' + tagName + '_global = ' +\
               GetFinalCondition(main.selection[iabs].conditions,0,tagName)+';\n')

    # Event Cut ?
    if len(main.selection[iabs].part)==0:
        file.write('  if (')
        if main.selection[iabs].cut_type==CutType.SELECT:
            file.write('!')
        file.write(tagName+'_global) return true;\n')

    # Counter
    file.write('  cuts_['+str(icut)+'].Increment(__event_weight__);\n')

    # Closing bracket for the current histo
    file.write('  }\n')

    return


def WriteConditions(file,main,iabs,icut,tagName,tagIndex,condition):

    if len(condition.parts)==0:
        WriteCutWith0Arg(file,main,iabs,icut,tagName,tagIndex,condition)
    elif len(condition.parts)==1:
        WriteCutWith1Arg(file,main,iabs,icut,tagName,tagIndex,condition)
    elif len(condition.parts)==2:
        WriteCutWith2Args(file,main,iabs,icut,tagName,tagIndex,condition)
    else:
        logging.error("observable with more than 2 arguments are " +\
                      "not managed by MadAnalysis 5")


def WriteCutWith0Arg(file,main,iabs,icut,tagName,tagIndex,condition):
    file.write('  '+tagName+'['+str(tagIndex)+'] = (')
    file.write(condition.observable.code(main.mode)+' ')
    file.write(OperatorType.convert2cpp(condition.operator)+' ')
    file.write(str(condition.threshold))
    file.write(' );\n')


def WriteCutWith2Args(file,main,iabs,icut,tagName,tagIndex,condition):

    # Loop over combination
    for combi1 in condition.parts[0]:
        for combi2 in condition.parts[1]:
            file.write('  {\n')
            WriteJobExecute2Nbody(file,iabs,icut,combi1,combi2,main,\
                                  tagName,tagIndex,condition)
            file.write('  }\n')


def WriteCutWith1Arg(file,main,iabs,icut,tagName,tagIndex,condition):

    # Skip observable with INT of FLOAT argument
    # Temporary
    if condition.parts[0] in [ArgumentType.FLOAT,\
                              ArgumentType.INTEGER]:
        return

    # Loop over combination (keyword AND)
    for item in condition.parts[0]:
        file.write('  {\n')
        WriteJobExecuteNbody(file,iabs,icut,item,main,tagName,tagIndex,condition)
        file.write('  }\n')


def WriteJobExecute2Nbody(file,iabs,icut,combi1,combi2,main,tagName,tagIndex,condition):

    obs = condition.observable

    # ALL reserved word for the first argument
    if len(combi1)==1 and combi1.ALL:
        if obs.combination in [CombinationType.SUMSCALAR,\
                               CombinationType.DIFFSCALAR]:
            file.write('    Double_t value1=0;\n')
        else:
            file.write('    ParticleBaseFormat q1;\n')
    
    # ALL reserved word for the second argument
    if len(combi2)==1 and combi2.ALL:
        if obs.combination in [CombinationType.SUMSCALAR,\
                               CombinationType.DIFFSCALAR]:
            file.write('    Double_t value2=0;\n')
        else:
            file.write('    ParticleBaseFormat q2;\n')

    # Determine if same particle in first combi
    redundancies1 = False
    if len(combi1)>1:
        for i in range(len(combi1)):
            for j in range(len(combi1)):
                if i==j:
                    continue
                if combi1[i].particle.IsThereCommonPart(combi1[j].particle):
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
                    redundancies = True

    # FOR loop for second combi
    WriteJobLoop(file,iabs,icut,combi2,redundancies2,main,'b')

    # Checking redundancies for second combi
    WriteJobSameCombi(file,iabs,icut,combi2,redundancies2,main,'b')

    # ALL reserved word
    if len(combi1)==1 and combi1.ALL:
        pass

    # Getting number of combinations
    if obs is ObservableType.N:
        file.write('      Ncounter++;\n')
        for combi in range(len(combination)):
            file.write('    }\n')
            file.write('    }\n')
        file('    if ( Ncounter ')
        file.write(OperatorType.convert2cpp(condition.operator) + \
                   str(condition.threshold) +   \
                   ') '+tagName+'['+tagIndex+']=true;\n')

    # Normal case
    else :

        WriteJobSum2N(file,iabs,icut,combi1,combi2,main,tagName,tagIndex,condition,'a','b')
        for ind in range(len(combi1)):
            file.write('    }\n')
        for ind in range(len(combi2)):
            file.write('    }\n')


def WriteJobSum2N(file,iabs,icut,combi1,combi2,main,tagName,tagIndex,condition,iterator1,iterator2):

    cut = main.selection[iabs]
    obs = condition.observable

    # Getting container name
    containers1=[]
    for item in combi1:
        containers1.append(InstanceName.Get('P_'+\
                                           item.name+cut.rank+cut.statuscode))

    containers2=[]
    for item in combi2:
        containers2.append(InstanceName.Get('P_'+\
                                           item.name+cut.rank+cut.statuscode))

    # Case of one particle/multiparticle
    if len(combi1)==1 and len(combi2)==1:
        if main.mode == MA5RunningType.PARTON:
          TheObs=obs.code_parton[:-2]
        elif main.mode == MA5RunningType.HADRON:
          TheObs=obs.code_hadron[:-2]
        else:
          TheObs=obs.code_reco[:-2]
        file.write('    if (')
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
        file.write('    ParticleBaseFormat q1;\n')
        for ind in range(0,len(combi1)):
            file.write('    q1'+oper_string+'='+\
                       containers1[ind]+'[+'+iterator1+'['+str(ind)+']]->'+\
                       'momentum();\n')

        # Second part
        file.write('    ParticleBaseFormat q2;\n')
        for ind in range(0,len(combi2)):
            file.write('    q2'+oper_string+'='+\
                       containers2[ind]+'['+iterator2+'['+str(ind)+']]->'+\
                       'momentum();\n')

        # Result    
        if main.mode == MA5RunningType.PARTON:
          TheObs=obs.code_parton[:-2]
        elif main.mode == MA5RunningType.HADRON:
          TheObs=obs.code_hadron[:-2]
        else:
          TheObs=obs.code_reco[:-2]
        file.write('    if (q1.'+TheObs+'(q2)'+\
                   ''+ OperatorType.convert2cpp(condition.operator) + \
                   str(condition.threshold) +   \
                   ') {'+tagName+'['+str(tagIndex)+']=true; break;}\n')


def WriteJobExecuteNbody(file,iabs,icut,combination,main,tagName,tagIndex,condition):

    obs = condition.observable
    
    # Case of N
    if obs.name in ['N','vN','sN','sdN','dsN','dvN','vdN','dN','rN']:
        file.write('    unsigned int Ncounter=0;\n')

    # ALL reserved word
    if len(combination)==1 and combination.ALL:
        if obs.combination in [CombinationType.SUMSCALAR,\
                               CombinationType.DIFFSCALAR]:
            file.write('    Double_t value=0;\n')
        else:
            file.write('    ParticleBaseFormat q;\n')

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
        file.write('      Ncounter++;\n')
        for combi in range(len(combination)):
            file.write('    }\n')
        file.write('    if ( Ncounter ')
        file.write(OperatorType.convert2cpp(condition.operator) + \
                   str(condition.threshold) +   \
                   ') '+tagName+'['+str(tagIndex)+']=true;\n')

    # Adding values    
    else:
        WriteJobSum(file,iabs,icut,combination,main,tagName,tagIndex,condition)
        for combi in range(len(combination)):
            file.write('    }\n')


def WriteJobLoop(file,iabs,icut,combination,redundancies,main,iterator='ind'):

    cut = main.selection[iabs]

    # Getting container name
    containers=[]
    for item in combination:
        containers.append(InstanceName.Get('P_'+\
                                           item.name+cut.rank+cut.statuscode))

    # Declaring indicator
    file.write('    UInt_t '+iterator+'['+str(len(combination))+'];\n')

    # Rendundancies case
    if redundancies:
        if main.mode in [MA5RunningType.PARTON,MA5RunningType.HADRON]:
            file.write('    std::vector<std::set<const MCParticleFormat*> > combis;\n')
        else:
            file.write('    std::vector<std::set<const RecParticleFormat*> > combis;\n')

    # Writing Loop For
    for i in range(len(combination)):
        file.write('    for ('+iterator+'['+str(i)+']=0;'\
                   +iterator+'['+str(i)+']<'+containers[i]+'.size();'\
                   +iterator+'['+str(i)+']++)\n')
        file.write('    {\n')

        # Redundancies case : managing same indices
        if i!=0 and redundancies:
            file.write('    if (')
            for j in range (0,i):
                if j!=0:
                    file.write(' || ')
                file.write(containers[i]+'['+iterator+'['+str(i)+']]=='+\
                           containers[j]+'['+iterator+'['+str(j)+']]')
            file.write(') continue;\n')     



def WriteJobSameCombi(file,iabs,icut,combination,redundancies,main,iterator='ind'):
    
    if len(combination)==1 or not redundancies:
        return

    cut = main.selection[iabs]

    # Getting container name
    containers=[]
    for item in combination:
        containers.append(InstanceName.Get('P_'+\
                                           item.name+cut.rank+cut.statuscode))

    file.write('\n    // Checking if consistent combination\n')
    if main.mode in [MA5RunningType.PARTON,MA5RunningType.HADRON]:
        file.write('    std::set<const MCParticleFormat*> mycombi;\n')
    else:
        file.write('    std::set<const RecParticleFormat*> mycombi;\n');
    file.write('    for (UInt_t i=0;i<'+str(len(combination))+';i++)\n')
    file.write('    {\n')
    for i in range(0,len(combination)):
        file.write('      mycombi.insert('+containers[i]+'['+iterator+'[i]]);\n')
    file.write('    }\n')
    file.write('    Bool_t matched=false;\n')
    file.write('    for (UInt_t i=0;i<combis.size();i++)\n')
    file.write('      if (combis[i]==mycombi) {matched=true; break;}\n')
    file.write('    if (matched) continue;\n')
    file.write('    else combis.push_back(mycombi);\n\n')


def WriteJobSum(file,iabs,icut,combination,main,tagName,tagIndex,condition,iterator='ind'):

    cut = main.selection[iabs]
    obs = condition.observable
    
    # Getting container name
    containers=[]
    for item in combination:
        containers.append(InstanceName.Get('P_'+\
                                           item.name+cut.rank+cut.statuscode))
        
    # Case of one particle/multiparticle
    if len(combination)==1:
        file.write('    if (')
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
        file.write('    if ((')
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
        file.write('    ParticleBaseFormat q;\n')
        for ind in range(len(combination)):
            file.write('    q'+oper_string+'='+containers[ind]+'['+iterator+'['+str(ind)+']]->'+\
                             'momentum();\n')
        file.write('    if (q.')
        file.write(obs.code(main.mode)+\
                   ''+ OperatorType.convert2cpp(condition.operator) + \
                   str(condition.threshold) +   \
                   ') {'+tagName+'['+str(tagIndex)+']=true; break;}\n')
    # ratio       
    elif obs.combination==CombinationType.RATIO and \
        len(combination)==2:
        file.write('    if (((')
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
