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
import copy

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
    


def WriteCandidateCut(file,main,iabs,icut,part_list):

    # Opening bracket for the current histo
    file.write('  {\n')

    # Created condition table
    conditions = []
    GetConditions(main.selection[iabs].conditions,conditions)

    # PATCH : TEMPORARY : TO REMOVE
    for combination in main.selection[iabs].part:
        if len(combination)>1:
            logging.warning("sorry but the possibility to apply a cut on a combination of " +\
                            "particles is not still implemented in MadAnalysis 5.")
            logging.warning("this cut will be disabled.")
            file.write('  cuts_['+str(icut)+'].Increment(__event_weight__);\n')
            file.write('  }\n')
            return

    # Loop over the possible candidate to cut (keyword AND)
    for combination in main.selection[iabs].part[0]:

        # Opening-brace for possible candidate
        file.write('  {\n')

        # container
        container=InstanceName.Get('P_'+combination.name+\
                                   main.selection[iabs].rank+\
                                   main.selection[iabs].statuscode)

        # create new container
        file.write('    std::vector<const ');
        if main.mode==MA5RunningType.RECO:
            file.write('RecParticleFormat*')
        else:
            file.write('MCParticleFormat*')
        file.write('> toRemove;\n')    

        # loop over particles
        file.write('    for (UInt_t muf=0;muf<'+container+'.size();muf++)\n')
        file.write('    {\n')

        # Initializing tag
        tagName='filter'
        file.write('      std::vector<Bool_t> '+tagName+'('+str(len(conditions))+',false);\n')

        # Loop over conditions
        for ind in range(len(conditions)):
            file.write('      {\n')
            WriteFactorizedConditions(file,main,iabs,icut,container,\
                                      tagName,tagIndex=ind,condition=conditions[ind])
            file.write('      }\n')

        # Writing final tag
        file.write('      Bool_t ' + tagName + '_global = ' +\
                   GetFinalCondition(main.selection[iabs].conditions,0,tagName)+';\n')
        
        # Add candidate ?
        file.write('      if (')
        if main.selection[iabs].cut_type==CutType.SELECT:
            file.write('!')
        file.write(tagName+'_global) toRemove.push_back('+container+'[muf]);\n')

        # End of for loop  
        file.write('    }\n')

        # Remove candidate from all containers    
        file.write('    // Removing rejected candidates from all containers\n')

        # Loop over containers
        for other_part in part_list:

            # Skip container with particle with a PTrank
            if other_part[0].PTrank!=0:
                continue
            
            # Get next container
            container2 = InstanceName.Get('P_'+other_part[0].name+\
                                          main.selection[iabs].rank+\
                                          main.selection[iabs].statuscode)

            # Is this container concerned by the cut ?
            concerned=False
            for other in other_part:
                if other_part[0].particle.IsThereCommonPart(combination.particle):
                    concerned=True
                    break
            if not concerned:
                continue

            # Bracket for begin 
            file.write('    {\n')

            # create new container
            file.write('    std::vector<const ');
            if main.mode==MA5RunningType.RECO:
                file.write('RecParticleFormat*')
            else:
                file.write('MCParticleFormat*')
            file.write('> tmp;\n')    

            # algo for removing rejected candidate
            file.write('    for (unsigned int i=0;i<'+container2+\
                       '.size();i++)\n')
            file.write('    {\n')
            file.write('      Bool_t reject=false;\n')
            file.write('      for (unsigned int j=0;j<toRemove.size();j++)\n')
            file.write('      {\n')
            file.write('        if (toRemove[j]=='+container2+'[i]) {reject=true;break;}\n')
            file.write('      }\n')
            file.write('      if (!reject) tmp.push_back('+container2+'[i]);\n')
            file.write('    }\n') 
            file.write('    '+container2+'=tmp;\n')
            
            # Bracket for end
            file.write('    }\n')

        # Remove candidate from all containers    
        file.write('    // Sorting particles according PTrank\n')

        # Loop over containers
        for other_part in part_list:

            # Skip container with particle with a PTrank
            if other_part[0].PTrank==0:
                continue

            # Get next container
            container2 = InstanceName.Get('P_'+other_part[0].name+\
                                          main.selection[iabs].rank+\
                                          main.selection[iabs].statuscode)
            refpart = copy.copy(other_part[0])
            refpart.PTrank=0
            newcontainer2 = InstanceName.Get('P_'+refpart.name+\
                                             main.selection[iabs].rank+\
                                             main.selection[iabs].statuscode)

            # Is this container concerned by the cut ?
            concerned=False
            for other in other_part:
                if other_part[0].particle.IsThereCommonPart(combination.particle):
                    concerned=True
                    break
            if not concerned:
                continue

            # Bracket for begin 
            file.write('    {\n')

            file.write('      '+container2+'=SORTER->rankFilter('+\
                        newcontainer2+','+str(other_part[0].PTrank)+','+\
                        'PTordering'+');\n\n')
            

            # Bracket for end
            file.write('    }\n')


#        file.write('    }\n')
        
        # Opening-brace for possible candidate
        file.write('  }\n')

    # Counter
    file.write('  cuts_['+str(icut)+'].Increment(__event_weight__);\n')

    # Closing bracket for the current cut
    file.write('  }\n')

    return


def WriteFactorizedConditions(file,main,iabs,icut,container,\
                              tagName,tagIndex,condition):
    if len(condition.parts)==0:
        WriteFactorizedCutWith0Arg(file,main,iabs,icut,container,\
                                   tagName,tagIndex,condition)
    elif len(condition.parts)==1:
        WriteFactorizedCutWith1Arg(file,main,iabs,icut,container,\
                                   tagName,tagIndex,condition)
    else:
        logging.error("observable with more than 2 arguments are " +\
                      "not managed by MadAnalysis 5")


def WriteFactorizedCutWith0Arg(file,main,iabs,icut,container,tagName,tagIndex,condition):
    file.write('        if (')
    file.write(container+'[muf]->' +\
               condition.observable.code(main.mode) +\
               OperatorType.convert2cpp(condition.operator) +\
               str(condition.threshold) +\
               ') '+tagName+'['+str(tagIndex)+']=true;\n')


def WriteFactorizedCutWith1Arg(file,main,iabs,icut,container,tagName,tagIndex,condition):

    for item in condition.parts[0]:
        file.write('      {\n')
        WriteJobExecuteNbody(file,iabs,icut,item,main,container,\
                             tagName,tagIndex,condition) 
        file.write('      }\n')


def WriteJobExecuteNbody(file,iabs,icut,combi1,main,container,tagName,tagIndex,condition):

    obs = condition.observable
    cut = main.selection[iabs]

    # ALL reserved word for the first argument
    if len(combi1)==1 and combi1.ALL:
        if obs.combination in [CombinationType.SUMSCALAR,\
                               CombinationType.DIFFSCALAR]:
            file.write('    Double_t value1=0;\n')
        else:
            file.write('    ParticleBaseFormat q1;\n')

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


    # Getting container name
    containers1=[]
    for item in combi1:
        containers1.append(InstanceName.Get('P_'+\
                                           item.name+cut.rank+cut.statuscode))

    # Case of one particle/multiparticle
    if len(combi1)==1:
        file.write('    if (')
        if main.mode == MA5RunningType.PARTON:
          TheObs=obs.code_parton[:-2]
        elif main.mode == MA5RunningType.HADRON:
          TheObs=obs.code_hadron[:-2]
        else:
          TheObs=obs.code_reco[:-2]
        file.write(containers1[0]+'[a]->' +\
                   TheObs+'('+container+'[muf])' +\
                   OperatorType.convert2cpp(condition.operator) +\
                   str(condition.threshold) +\
                   ') {'+tagName+'['+str(tagIndex)+']=true; break;}\n')

        for ind in range(len(combi1)):
            file.write('    }\n')

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
                       containers1[ind]+'[a['+str(ind)+']]->'+\
                       'momentum();\n')

        # Result    
        file.write('    if (q1.')
        if obs.name in ['DELTAR','vDELTAR','DPHI_0_PI','DPHI_0_2PI']:
            if main.mode == MA5RunningType.PARTON:
              TheObs=obs.code_parton[:-2]
            elif main.mode == MA5RunningType.HADRON:
              TheObs=obs.code_hadron[:-2]
            else:
              TheObs=obs.code_reco[:-2]
            file.write(TheObs+'('+container+'[muf])'+ OperatorType.convert2cpp(condition.operator) + \
                   str(condition.threshold) +   \
                   ') {'+tagName+'['+str(tagIndex)+']=true; break;}\n')
        else:
            file.write(obs.code(main.mode)+\
                   '('+container+'[muf])'+ OperatorType.convert2cpp(condition.operator) + \
                   str(condition.threshold) +   \
                   ') {'+tagName+'['+str(tagIndex)+']=true; break;}\n')

    for ind in range(len(combi1)):
        file.write('    }\n')


def WriteJobLoop(file,iabs,icut,combination,redundancies,main,iterator='ind'):

    cut = main.selection[iabs]

    # Getting container name
    containers=[]
    for item in combination:
        containers.append(InstanceName.Get('P_'+\
                                           item.name+cut.rank+cut.statuscode))

    if len(combination)==1:
        file.write('    for (UInt_t '+iterator+'=0;'+iterator+'<' + containers[0] + '.size();'+iterator+'++)\n')
        file.write('    {\n')
    else:

        file.write('    UInt_t '+iterator+'['+str(len(combination))+'];\n')
        if redundancies:
            if main.mode in [MA5RunningType.PARTON,MA5RunningType.HADRON]:
                file.write('    std::vector<std::set<const MCParticleFormat*> > combis;\n')
            else:
                file.write('    std::vector<std::set<const RecParticleFormat*> > combis;\n')
        for i in range(len(combination)):
            file.write('    for ('+iterator+'['+str(i)+']=0;'\
                       +iterator+'['+str(i)+']<'+containers[i]+'.size();'\
                       +iterator+'['+str(i)+']++)\n')
            file.write('    {\n')

            # Managing same indices
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
        file.write('    std::set<const RecParticleFormat*> mycombi;\n')
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
