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
from madanalysis.enumeration.argument_type    import ArgumentType
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.enumeration.combination_type import CombinationType
from madanalysis.interpreter.cmd_cut          import CmdCut
import logging


def WritePlot(file,main,iabs,ihisto):

    # Opening bracket for the current histo
    file.write('  {\n')

    # Counting number of times this block is read
    file.write('  H'+str(ihisto)+'_->IncrementNEvents(__event_weight__);\n')
    if len(main.selection[iabs].arguments)==0:
        WritePlotWith0Arg(file,main,iabs,ihisto)
    elif len(main.selection[iabs].arguments)==1:
        WritePlotWith1Arg(file,main,iabs,ihisto)
    elif len(main.selection[iabs].arguments)==2:
        WritePlotWith2Args(file,main,iabs,ihisto)
    else:
        logging.error("observable with more than 2 arguments are " +\
                      "not managed by MadAnalysis 5")

    # Closing bracket for the current histo
    file.write('  }\n')


def WritePlotWith0Arg(file,main,iabs,ihisto):
    if main.selection[iabs].observable.name in ['NPID','NAPID']:
        WriteJobNPID(file,main,iabs,ihisto)
    else:
        file.write('    H'+str(ihisto)+'_->Fill(' +\
                   main.selection[iabs].observable.code(main.mode)+\
                   ',__event_weight__);\n')


def WriteJobNPID(file,main,iabs,ihisto):

    # NPID or NAPID ?
    npid = ( main.selection[iabs].observable.name == 'NPID' )

    # PARTON or HADRON mode
    if main.mode!=MA5RunningType.RECO:
        file.write('  for (unsigned int i=0;i<event.mc()->particles().size();i++)\n')
        file.write('  {\n')
        if main.selection[iabs].statuscode=="finalstate":
            file.write('    if (!PHYSICS->Id->IsFinalState(event.mc()->particles()[i])) continue;\n')
        elif main.selection[iabs].statuscode=="initialstate":
            file.write('    if (!PHYSICS->Id->IsInitialState(event.mc()->particles()[i])) continue;\n')
        elif main.selection[iabs].statuscode=="interstate":
            file.write('    if (!PHYSICS->Id->IsInterState(event.mc()->particles()[i])) continue;\n')
        if npid:
            file.write('    H'+str(ihisto)+'_' +\
                   '->Fill(event.mc()->particles()[i].pdgid(),__event_weight__);\n')
        else:
            file.write('    H'+str(ihisto)+'_' +\
                   '->Fill(std::abs(event.mc()->particles()[i].pdgid()),__event_weight__);\n')
        file.write('  }\n')

    # RECO mode
    else:

        # photons
        file.write('  for (unsigned int i=0;i<event.rec()->photons().size();i++)\n')
        file.write('  {\n')
        file.write('    H'+str(ihisto)+'_->Fill(22,__event_weight__);\n')
        file.write('  }\n')

        # electrons
        file.write('  for (unsigned int i=0;i<event.rec()->electrons().size();i++)\n')
        file.write('  {\n')
        if npid:
            file.write('    if (event.rec()->electrons()[i].charge()<0) H' +\
                       str(ihisto)+'_->Fill(+11,__event_weight__); else H' +\
                       str(ihisto)+'_->Fill(-11,__event_weight__);\n')
        else:
            file.write('    H'+str(ihisto)+'_->Fill(+11,__event_weight__);\n')
        file.write('  }\n')

        # muons
        file.write('  for (unsigned int i=0;i<event.rec()->muons().size();i++)\n')
        file.write('  {\n')
        if npid:
            file.write('    if (event.rec()->muons()[i].charge()<0) H' +\
                       str(ihisto)+'_->Fill(+13,__event_weight__); else H' +\
                       str(ihisto)+'_->Fill(-13,__event_weight__);\n')
        else:
            file.write('    H'+str(ihisto)+'_->Fill(+13,__event_weight__);\n')
        file.write('  }\n')

        # taus
        file.write('  for (unsigned int i=0;i<event.rec()->taus().size();i++)\n')
        file.write('  {\n')
        if npid:
            file.write('    if (event.rec()->taus()[i].charge()<0) H' +\
                       str(ihisto)+'_->Fill(+15,__event_weight__); else H' +\
                       str(ihisto)+'_->Fill(-15,__event_weight__);\n')
        else:
            file.write('    H'+str(ihisto)+'_->Fill(+15,__event_weight__);\n')
        file.write('  }\n')

        # jets
        file.write('  for (unsigned int i=0;i<event.rec()->jets().size();i++)\n')
        file.write('  {\n')
        file.write('    H'+str(ihisto)+'_->Fill(+21,__event_weight__);\n')
        file.write('    if (event.rec()->jets()[i].btag()) H'+str(ihisto)+'_->Fill(+5);\n') 
        file.write('    else H'+str(ihisto)+'_->Fill(+1);\n') 
        file.write('  }\n')



def WritePlotWith1Arg(file,main,iabs,ihisto):

    # Skip observable with INT of FLOAT argument
    # Temporary
    if main.selection[iabs].arguments[0] in [ArgumentType.FLOAT,\
                                             ArgumentType.INTEGER]:
        return

    # Loop over combination
    for item in main.selection[iabs].arguments[0]:
        file.write('  {\n')
        WriteJobExecuteNbody(file,iabs,ihisto,item,main)
        file.write('  }\n')


def WritePlotWith2Args(file,main,iabs,ihisto):

    # Loop over combination
    for combi1 in main.selection[iabs].arguments[0]:
        for combi2 in main.selection[iabs].arguments[1]:
            file.write('  {\n')
            WriteJobExecute2Nbody(file,iabs,ihisto,combi1,combi2,main)
            file.write('  }\n')


def WriteJobExecute2Nbody(file,iabs,ihisto,combi1,combi2,main):

    obs      = main.selection[iabs].observable
    histo    = main.selection[iabs]
    allmode1 = ( len(combi1)==1 and combi1.ALL )
    allmode2 = ( len(combi2)==1 and combi2.ALL )

    # Determine if possible double counting for each argument
    redundancies1 = HasDoubleCounting(combi1)
    redundancies2 = HasDoubleCounting(combi2)

    # Determine if possible double counting between arguments
    common = []
    for combi in combi1:
        common.append(combi)
    for combi in combi2:
        common.append(combi)
    redundancies0 = HasDoubleCounting(common)
    
    # ----------------------------------------------------------
    #                  No ALL in the observable
    # ----------------------------------------------------------
    if not allmode1 and not allmode2:
        
        # FOR loop for first combi
        WriteJobLoop(file,iabs,ihisto,combi1,redundancies1,main,'a')

        # Checking redundancies for first combi
        WriteJobSameCombi(file,iabs,ihisto,combi1,redundancies1,main,'a')

        # FOR loop for second combi
        WriteJobLoop(file,iabs,ihisto,combi2,redundancies2,main,'b')

        # Checking redundancies for second combi
        WriteJobSameCombi(file,iabs,ihisto,combi2,redundancies2,main,'b')

        # Managing redundancies between arguments
        if redundancies0:
            WriteAvoidRedundancies(file,iabs,ihisto,combi1,combi2,main,'a','b')
        
        # Write body
        WriteBody2(file,iabs,ihisto,combi1,combi2,main,'a','b')
    
        # End Loop
        WriteEndLoop(file,iabs,ihisto,combi1,main)
        WriteEndLoop(file,iabs,ihisto,combi2,main)

    # ----------------------------------------------------------
    #              First argument = ALL observable
    # ----------------------------------------------------------
    elif allmode1 and not allmode2:

        # Before loop block
        WriteBeforeLoop(file,iabs,ihisto,combi1,main,value='value1',q='q1')
        
        # FOR loop for first combi
        WriteJobLoop(file,iabs,ihisto,combi1,redundancies1,main,'a')

        # Checking redundancies for first combi
        WriteJobSameCombi(file,iabs,ihisto,combi1,redundancies1,main,'a')

        # Write body
        WriteBody(file,iabs,ihisto,combi1,main,iterator='a',value='value1',q='q1')
        
        # End Loop
        WriteEndLoop(file,iabs,ihisto,combi1,main)

        # FOR loop for second combi
        WriteJobLoop(file,iabs,ihisto,combi2,redundancies2,main,'b')

        # Checking redundancies for second combi
        WriteJobSameCombi(file,iabs,ihisto,combi2,redundancies2,main,'b')

        # Getting container name
        containers2=[]
        for item in combi2:
            containers2.append(InstanceName.Get('P_'+\
                                                item.name+\
                                                histo.rank+\
                                                histo.statuscode))

        # Case of one particle/multiparticle
        if len(combi2)==1:
            if main.mode == MA5RunningType.PARTON:
              TheObs=obs.code_parton[:-2]
            elif main.mode == MA5RunningType.HADRON:
              TheObs=obs.code_hadron[:-2]
            else:
              TheObs=obs.code_reco[:-2]
            file.write('      H'+str(ihisto)+'_->Fill('\
                       'q1.'+TheObs+'('+containers2[0]+\
                       '[b[0]]),__event_weight__);\n')

        # Operation : sum or diff
        else :

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

                # Second part
                file.write('    ParticleBaseFormat q2;\n')
                for ind in range(0,len(combi2)):
                    TheOper='+'
                    if ind!=0:
                      TheOper=oper_string
                    file.write('    q2'+TheOper+'='+\
                               containers2[ind]+'['+iterator2+'['+str(ind)+']]->'+\
                               'momentum();\n')

                # Result    
                if main.mode == MA5RunningType.PARTON:
                  TheObs=obs.code_parton[:-2]
                elif main.mode == MA5RunningType.HADRON:
                  TheObs=obs.code_hadron[:-2]
                else:
                  TheObs=obs.code_reco[:-2]
                file.write('    H'+str(ihisto)+'_->Fill('+\
                               'q1.'+TheObs+'(q2),__event_weight__);\n')

        # End Loop
        WriteEndLoop(file,iabs,ihisto,combi2,main)

    # ----------------------------------------------------------
    #              Second argument = ALL observable
    # ----------------------------------------------------------
    elif not allmode1 and allmode2:

        # Before loop block
        WriteBeforeLoop(file,iabs,ihisto,combi2,main,value='value2',q='q2')
        
        # FOR loop for second combi
        WriteJobLoop(file,iabs,ihisto,combi2,redundancies2,main,'b')

        # Checking redundancies for second combi
        WriteJobSameCombi(file,iabs,ihisto,combi2,redundancies2,main,'b')

        # Write body
        WriteBody(file,iabs,ihisto,combi2,main,iterator='b',value='value2',q='q2')
        
        # End Loop
        WriteEndLoop(file,iabs,ihisto,combi2,main)

        # FOR loop for second combi
        WriteJobLoop(file,iabs,ihisto,combi1,redundancies1,main,'a')

        # Checking redundancies for second combi
        WriteJobSameCombi(file,iabs,ihisto,combi1,redundancies1,main,'a')

        # Getting container name
        containers1=[]
        for item in combi1:
            containers1.append(InstanceName.Get('P_'+\
                                                item.name+\
                                                histo.rank+\
                                                histo.statuscode))

        # Case of one particle/multiparticle
        if len(combi1)==1:
            if main.mode == MA5RunningType.PARTON:
              TheObs=obs.code_parton[:-2]
            elif main.mode == MA5RunningType.HADRON:
              TheObs=obs.code_hadron[:-2]
            else:
              TheObs=obs.code_reco[:-2]
            file.write('      H'+str(ihisto)+'_->Fill('\
                       'q2.'+TheObs+'('+containers1[0]+\
                       '[a[0]]),__event_weight__);\n')

        # Operation : sum or diff
        else:

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

                # Second part
                file.write('    ParticleBaseFormat q1;\n')
                for ind in range(0,len(combi1)):
                    TheOper='+'
                    if ind!=0:
                      TheOper=oper_string
                    file.write('    q1'+TheOper+'='+\
                               containers1[ind]+'['+iterator1+'['+str(ind)+']]->'+\
                               'momentum();\n')

                # Result    
                if main.mode == MA5RunningType.PARTON:
                  TheObs=obs.code_parton[:-2]
                elif main.mode == MA5RunningType.HADRON:
                  TheObs=obs.code_hadron[:-2]
                else:
                  TheObs=obs.code_reco[:-2]
                file.write('    H'+str(ihisto)+'_->Fill('+\
                               'q1.'+TheObs+'(q2),__event_weight__);\n')

        # End Loop
        WriteEndLoop(file,iabs,ihisto,combi1,main)

    # ----------------------------------------------------------
    #                   ALL in the observable
    # ----------------------------------------------------------
    elif allmode1 and allmode2:

        # Before loop block
        WriteBeforeLoop(file,iabs,ihisto,combi1,main,value='value1',q='q1')
        
        # FOR loop for first combi
        WriteJobLoop(file,iabs,ihisto,combi1,redundancies1,main,'a')

        # Checking redundancies for first combi
        WriteJobSameCombi(file,iabs,ihisto,combi1,redundancies1,main,'a')

        # Write body
        WriteBody(file,iabs,ihisto,combi1,main,iterator='a',value='value1',q='q1')
        
        # End Loop
        WriteEndLoop(file,iabs,ihisto,combi1,main)

        # Before loop block
        WriteBeforeLoop(file,iabs,ihisto,combi1,main,value='value2',q='q2')

        # FOR loop for second combi
        WriteJobLoop(file,iabs,ihisto,combi2,redundancies2,main,'b')

        # Checking redundancies for second combi
        WriteJobSameCombi(file,iabs,ihisto,combi2,redundancies2,main,'b')

        # Write body
        WriteBody(file,iabs,ihisto,combi2,main,iterator='b',value='value2',q='q2')
    
        # End Loop
        WriteEndLoop(file,ihisto,iabs,combi2,main)

        # After the two loops 
        if main.mode == MA5RunningType.PARTON:
          TheObs=obs.code_parton[:-2]
        elif main.mode == MA5RunningType.HADRON:
          TheObs=obs.code_hadron[:-2]
        else:
          TheObs=obs.code_reco[:-2]
        file.write('    H'+str(ihisto)+'_->Fill(q1.'+TheObs+'(q2),__event_weight__);\n')
        

def WriteBeforeLoop(file,iabs,ihisto,combination,main,value='value',q='q'):
    
    # shortcut
    obs = main.selection[iabs].observable
    allmode = ( len(combination)==1 and combination.ALL )

    # Case of N
    if not allmode:
        if obs.name in ['N','vN','sN','sdN','dsN','dvN','vdN','dN','rN']:
            file.write('    unsigned int Ncounter=0;\n')

    # ALL reserved word
    if allmode:
        if obs.combination in [CombinationType.SUMSCALAR,\
                               CombinationType.DIFFSCALAR]:
            file.write('    Double_t '+value+'=0;\n')
        else:
            file.write('    ParticleBaseFormat '+q+';\n')


    
def WriteEndLoop(file,iabs,ihisto,combination,main):
    for combi in range(len(combination)):
        file.write('    }\n')

def WriteAfterLoop(file,iabs,ihisto,combination,main):

    # shortcut
    obs     = main.selection[iabs].observable
    histo   = main.selection[iabs]
    allmode = ( len(combination)==1 and combination.ALL )

    # Case of ALL
    if allmode:
    
        # Case of observable N
        if obs.name in ['N','vN','sN','sdN','dsN','dvN','vdN','dN','rN']:
            file.write('    H'+str(ihisto)+'_->Fill(1,__event_weight__);\n')

        # Case of other observable
        else:
            if obs.combination in [CombinationType.SUMSCALAR,\
                                   CombinationType.DIFFSCALAR]:
                file.write('    H'+str(ihisto)+'_->Fill(value,__event_weight__);\n')
            else:
                file.write('    H'+str(ihisto)+'_->Fill(q.'+\
                           obs.code(main.mode)+\
                           ',__event_weight__);\n')

    # Case of observable N but not ALL
    elif obs.name in ['N','vN','sN','sdN','dsN','dvN','vdN','dN','rN']:
        file.write('    H'+str(ihisto)+'_->Fill(Ncounter,__event_weight__);\n')


def WriteBody(file,iabs,ihisto,combination,main,iterator='ind',value='value',q='q'):

    # Shortcut
    histo   = main.selection[iabs]
    obs     = main.selection[iabs].observable
    allmode = ( len(combination)==1 and combination.ALL )

    # Case of observable N
    if obs.name in ['N','vN','sN','sdN','dsN','dvN','vdN','dN','rN']:

        # Case with ALL
        if allmode:
            pass
        # Other case
        else:
            file.write('      Ncounter++;\n')
        return    

    # Getting container name
    containers=[]
    for item in combination:
        containers.append(InstanceName.Get('P_'+\
                                           item.name+\
                                           histo.rank+\
                                           histo.statuscode))

    # Only one particle (but not ALL)
    if len(combination)==1 and not allmode:
        file.write('      H'+str(ihisto)+'_->Fill('\
                   +containers[0]+'['+iterator+'[0]]->'+\
                   obs.code(main.mode)+\
                   ',__event_weight__);\n')
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

        if not allmode:
            file.write('    Double_t '+value+'=0;\n')
        for ind in range(len(combination)):
            TheOper='+'
            if ind!=0:
              TheOper=oper_string
            file.write('    '+value+TheOper+'='+\
                       containers[ind]+'['+iterator+'['+str(ind)+']]->'+\
                       obs.code(main.mode)+';\n')

        if not allmode:
            file.write('    H'+str(ihisto)+'_->Fill(' +\
                       value+',__event_weight__);\n')

    # Vector sum/diff
    elif obs.combination in [CombinationType.DEFAULT,\
                             CombinationType.SUMVECTOR,\
                             CombinationType.DIFFVECTOR]:
        if not allmode:
            file.write('    ParticleBaseFormat '+q+';\n')
            
        for ind in range(len(combination)):
            TheOper='+'
            if ind!=0:
              TheOper=oper_string
            file.write('    '+q+TheOper+'='+\
                       containers[ind]+'['+iterator+'['+str(ind)+']]->'+\
                       'momentum();\n')
        if not allmode:
            file.write('    H'+str(ihisto)+'_->Fill('+\
                       q+'.')
            file.write(obs.code(main.mode)+\
                       ',__event_weight__);\n');
            
    # ratio       
    elif obs.combination==CombinationType.RATIO and \
        len(combination)==2:
        file.write('    H'+str(ihisto)+'_->Fill((' +\
                   containers[0]+'['+iterator+'[0]]->'+\
                   obs.code(main.mode)+\
                   '-'+\
                   containers[1]+'['+iterator+'[1]]->'+\
                   obs.code(main.mode)+\
                   ') / '+\
                   containers[0]+'['+iterator+'[0]]->'+\
                   obs.code(main.mode)+\
                   ',__event_weight__);\n');


def WriteBody2(file,iabs,ihisto,combi1,combi2,main,iterator1,iterator2):

    # Shortcut
    histo    = main.selection[iabs]
    obs      = main.selection[iabs].observable
    allmode1 = ( len(combi1)==1 and combi1.ALL )
    allmode2 = ( len(combi2)==1 and combi2.ALL )

    # Getting container name
    containers1=[]
    for item in combi1:
        containers1.append(InstanceName.Get('P_'+\
                                            item.name+\
                                            histo.rank+\
                                            histo.statuscode))

    # Getting container name
    containers2=[]
    for item in combi2:
        containers2.append(InstanceName.Get('P_'+\
                                            item.name+\
                                            histo.rank+\
                                            histo.statuscode))

    # Case of one particle/multiparticle
    if len(combi1)==1 and len(combi2)==1:
        if main.mode == MA5RunningType.PARTON:
          TheObs=obs.code_parton[:-2]
        elif main.mode == MA5RunningType.HADRON:
          TheObs=obs.code_hadron[:-2]
        else:
          TheObs=obs.code_reco[:-2]
        file.write('      H'+str(ihisto)+'_->Fill('+\
                   containers1[0]+'['+iterator1+'[0]]->'+\
                   TheObs+'('+containers2[0]+'['+iterator2+'[0]]),__event_weight__);\n')
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
            TheOper='+'
            if ind!=0:
              TheOper=oper_string
            file.write('    q1'+TheOper+'='+\
                       containers1[ind]+'[+'+iterator1+'['+str(ind)+']]->'+\
                       'momentum();\n')

        # Second part
        file.write('    ParticleBaseFormat q2;\n')
        for ind in range(0,len(combi2)):
            TheOper='+'
            if ind!=0:
              TheOper=oper_string
            file.write('    q2'+TheOper+'='+\
                       containers2[ind]+'['+iterator2+'['+str(ind)+']]->'+\
                       'momentum();\n')

        # Result    
        if main.mode == MA5RunningType.PARTON:
          TheObs=obs.code_parton[:-2]
        elif main.mode == MA5RunningType.HADRON:
          TheObs=obs.code_hadron[:-2]
        else:
          TheObs=obs.code_reco[:-2]
        file.write('    H'+str(ihisto)+'_->Fill('+\
                       'q1.'+TheObs+'(q2),__event_weight__);\n')


def HasDoubleCounting(combination):
    if len(combination)<=1:
        return False
    
    for i in range(len(combination)):
        for j in range(len(combination)):
            if i==j:
                continue
            if combination[i].particle.IsThereCommonPart(\
                                                    combination[j].particle):
                return True

    return False            
                
    
def WriteJobExecuteNbody(file,iabs,ihisto,combination,main):

    # shortcut
    obs = main.selection[iabs].observable
    histo = main.selection[iabs]

    # Before Loop block 
    WriteBeforeLoop(file,iabs,ihisto,combination,main)

    # Determine if same particle in loop
    redundancies = HasDoubleCounting(combination)

    # BeginLoop
    WriteJobLoop(file,iabs,ihisto,combination,redundancies,main)

    # Reject Double Counting
    WriteJobSameCombi(file,iabs,ihisto,combination,redundancies,main)

    # Write body
    WriteBody(file,iabs,ihisto,combination,main)
    
    # End Loop
    WriteEndLoop(file,iabs,ihisto,combination,main)

    # After Loop
    WriteAfterLoop(file,iabs,ihisto,combination,main)
    

def WriteAvoidRedundancies(file,iabs,ihisto,combi1,combi2,main,iterator1,iterator2):

    # Shortcut
    histo    = main.selection[iabs]
    obs      = main.selection[iabs].observable
    allmode1 = ( len(combi1)==1 and combi1.ALL )
    allmode2 = ( len(combi2)==1 and combi2.ALL )

    # Getting container name
    containers1=[]
    for item in combi1:
        containers1.append(InstanceName.Get('P_'+\
                                            item.name+\
                                            histo.rank+\
                                            histo.statuscode))

    # Getting container name
    containers2=[]
    for item in combi2:
        containers2.append(InstanceName.Get('P_'+\
                                            item.name+\
                                            histo.rank+\
                                            histo.statuscode))

    # Case of one particle/multiparticle
    if len(combi1)==1 and len(combi2)==1:
        file.write('     if ( '+containers1[0]+'['+iterator1+'[0]] == '+\
                   containers2[0]+'['+iterator2+'[0]] ) continue;\n')


def WriteJobLoop(file,iabs,ihisto,combination,redundancies,main,iterator='ind'):

    histo = main.selection[iabs]

    # Getting container name
    containers=[]
    for item in combination:
        containers.append(InstanceName.Get('P_'+\
                                           item.name+histo.rank+histo.statuscode))

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


def WriteJobSameCombi(file,iabs,ihisto,combination,redundancies,main,iterator='ind'):
    
    if len(combination)==1 or not redundancies:
        return

    histo = main.selection[iabs]

    # Getting container name
    containers=[]
    for item in combination:
        containers.append(InstanceName.Get('P_'+\
                                           item.name+histo.rank+histo.statuscode))

    file.write('\n    // Checking if consistent combination\n')
    if main.mode in [MA5RunningType.PARTON,MA5RunningType.HADRON]:
        file.write('    std::set<const MCParticleFormat*> mycombi;\n')
    else:
        file.write('    std::set<const RecParticleFormat*> mycombi;\n')
    file.write('    for (UInt_t i=0;i<'+str(len(combination))+';i++)\n')
    file.write('    {\n')
    for i in range(len(combination)):
        file.write('      mycombi.insert('+containers[i]+'['+iterator+'[i]]);\n')
    file.write('    }\n')
    file.write('    Bool_t matched=false;\n')
    file.write('    for (UInt_t i=0;i<combis.size();i++)\n')
    file.write('      if (combis[i]==mycombi) {matched=true; break;}\n')
    file.write('    if (matched) continue;\n')
    file.write('    else combis.push_back(mycombi);\n\n')


