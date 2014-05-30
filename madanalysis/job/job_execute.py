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
from madanalysis.interpreter.cmd_cut          import CmdCut
import logging
import copy

def WriteExecute(file,main,part_list):

    # Function header
    file.write('bool user::Execute(SampleFormat& sample, ' +\
               'const EventFormat& event)\n{\n')

    # Getting the event weight
    file.write('  Float_t __event_weight__ = 1.0;\n')
    file.write('  if (weighted_events_ && event.mc()!=0) ' +\
               '__event_weight__ = event.mc()->weight();\n\n')  
    file.write('  if (sample.mc()!=0) sample.mc()->addWeightedEvents(__event_weight__);\n')

    # Reseting instance name
    InstanceName.Clear()

    # Clearing and filling containers
    WriteContainer(file,main,part_list)

    # Writing each step of the selection
    WriteSelection(file,main,part_list)

    # End
    file.write('  return true;\n')
    file.write('}\n\n')

def WriteJobRank(part,file,rank,status):

    if part.PTrank==0:
        return

    # Skipping if already defined
    if InstanceName.Find("PTRANK_"+part.name+rank+status):
        return
    container=InstanceName.Get('P_'+part.name+rank+status)
    refpart = copy.copy(part)
    refpart.PTrank=0
    newcontainer=InstanceName.Get('P_'+refpart.name+rank+status);

    file.write('  // Sorting particle collection according to '+rank+'\n')
    file.write('  // for getting '+str(part.PTrank)+'th particle\n')
    file.write('  '+container+'=SORTER->rankFilter('+\
               newcontainer+','+str(part.PTrank)+','+rank+');\n\n')


def WriteCleanContainer(part,file,rank,status):

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status)

    # Getting id name
    id='isP_'+InstanceName.Get(part.name+rank+status)

    file.write('      ' + container + '.clear();\n')


def WriteFillContainer(part,file,rank,status):

    # If PTrank, no fill
    if part.PTrank!=0:
        return
    
    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status)

    # Getting id name
    id='isP_'+InstanceName.Get(part.name+rank+status)

    file.write('      if ('+id+'((&(event.mc()->particles()[i])))) ' +\
               container + '.push_back(&(event.mc()->particles()[i]));\n')
        

def WriteFillWithJetContainer(part,file,rank,status):
    
    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status)

    # Put jet
    if part.particle.Find(21):
        file.write('      '+container+\
                   '.push_back(&(event.rec()->jets()[i]));\n')
        return

    # Put b jet
    if part.particle.Find(5):
        file.write('      if (event.rec()->jets()[i].btag()) '+\
                   container+'.push_back(&(event.rec()->jets()[i]));\n')

    # Put nb jet
    if part.particle.Find(1):
        file.write('      if (!event.rec()->jets()[i].btag()) '+\
                   container+'.push_back(&(event.rec()->jets()[i]));\n')


def WriteFillWithElectronContainer(part,file,rank,status):
    
    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status)

    # Put negative electron
    if part.particle.Find(11):
        file.write('      if (event.rec()->electrons()[i].charge()<0) '+\
                   container+'.push_back(&(event.rec()->electrons()[i]));\n')

    # Put positive electron
    if part.particle.Find(-11):
        file.write('      if (event.rec()->electrons()[i].charge()>0) '+\
                   container+'.push_back(&(event.rec()->electrons()[i]));\n')


def WriteFillWithPhotonContainer(part,file,rank,status):
    
    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status)

    # Put photon
    if part.particle.Find(22):
        file.write('      '+container+'.push_back(&(event.rec()->photons()[i]));\n')


def WriteFillWithMuonContainer(part,file,rank,status):
    
    # If PTrank, no fill
    if part.PTrank!=0:
        return
    
    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status)

    # Put negative muon
    if part.particle.Find(13):
        file.write('      if (event.rec()->muons()[i].charge()<0) '+\
                   container+'.push_back(&(event.rec()->muons()[i]));\n')

    # Put positive muon
    if part.particle.Find(-13):
        file.write('      if (event.rec()->muons()[i].charge()>0) '+\
                   container+'.push_back(&(event.rec()->muons()[i]));\n')

    # Put isolated negative muon
    if part.particle.Find(130):
        file.write('      if ( (event.rec()->muons()[i].charge()<0) &&'+\
               ' PHYSICS->Id->IsIsolatedMuon(event.rec()->muons()[i],event.rec()) ) '+\
               container+'.push_back(&(event.rec()->muons()[i]));\n')

    # Put isolated positive muon
    if part.particle.Find(-130):
        file.write('      if ( (event.rec()->muons()[i].charge()>0) &&'+\
               ' PHYSICS->Id->IsIsolatedMuon(event.rec()->muons()[i],event.rec()) ) '+\
               container+'.push_back(&(event.rec()->muons()[i]));\n')


def WriteFillWithTauContainer(part,file,rank,status):
    
    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status)

    # Put negative tau
    if part.particle.Find(15):
        file.write('      if (event.rec()->taus()[i].charge()<0) '+\
                   container+'.push_back(&(event.rec()->taus()[i]));\n')

    # Put positive tau
    if part.particle.Find(-15):
        file.write('      if (event.rec()->taus()[i].charge()>0) '+\
                   container+'.push_back(&(event.rec()->taus()[i]));\n')
    

def WriteFillWithMETContainer(part,file,rank,status):
    
    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status)

    # Put MET
    if part.particle.Find(100):
        file.write('       '+container+\
                   '.push_back(&(event.rec()->MET()));\n')


def WriteFillWithMHTContainer(part,file,rank,status):
    
    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status)

    # Put MHT
    if part.particle.Find(99):
        file.write('       '+container+\
                   '.push_back(&(event.rec()->MHT()));\n')


def WriteFillWithMETContainerMC(part,file,rank,status):
    
    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status)

    # Put MET
    if part.particle.Find(100):
        file.write('       '+container+\
                   '.push_back(&(event.mc()->MET()));\n')


def WriteFillWithMHTContainerMC(part,file,rank,status):
    
    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status)

    # Put MHT
    if part.particle.Find(99):
        file.write('       '+container+\
                   '.push_back(&(event.mc()->MHT()));\n')


def WriteContainer(file,main,part_list):

    # Skipping empty case
    if len(part_list)==0:
        return

    # Cleaning particle containers
    file.write('  // Clearing particle containers\n') 
    file.write('  {\n')
    for item in part_list:
        WriteCleanContainer(item[0],file,item[1],item[2])
    file.write('  }\n')
    InstanceName.Clear()

    # Filling particle containers
    file.write('  // Filling particle containers\n') 
    file.write('  {\n')
    InstanceName.Clear()

    # Filling particle containers in PARTON and HADRON mode
    if main.mode in [MA5RunningType.PARTON, MA5RunningType.HADRON]:

        # Special particles : MET or MHT
        for item in part_list:
            if item[0].particle.Find(99) or item[0].particle.Find(100):
                WriteFillWithMETContainerMC(item[0],file,item[1],item[2]) 
                WriteFillWithMHTContainerMC(item[0],file,item[1],item[2])

        # Ordinary particles
        file.write('    for (UInt_t i=0;i<event.mc()->particles().size();i++)\n')
        file.write('    {\n')
        for item in part_list:
            if item[0].particle.Find(99) or item[0].particle.Find(100):
                pass
            else:
                WriteFillContainer(item[0],file,item[1],item[2])
        file.write('    }\n')
        InstanceName.Clear()

    # Filling particle containers in RECO mode    
    else:

        # Filling with jets
        file.write('    for (UInt_t i=0;i<event.rec()->jets().size();i++)\n')
        file.write('    {\n')
        for item in part_list:
            WriteFillWithJetContainer(item[0],file,item[1],item[2])
        file.write('    }\n')
        InstanceName.Clear()

        # Filling with photons
        file.write('    for (UInt_t i=0;i<event.rec()->photons().size();i++)\n')
        file.write('    {\n')
        for item in part_list:
            WriteFillWithPhotonContainer(item[0],file,item[1],item[2])
        file.write('    }\n')
        InstanceName.Clear()

        # Filling with electrons
        file.write('    for (UInt_t i=0;i<event.rec()->electrons().size();i++)\n')
        file.write('    {\n')
        for item in part_list:
            WriteFillWithElectronContainer(item[0],file,item[1],item[2])
        file.write('    }\n')
        InstanceName.Clear()

        # Filling with muons
        file.write('    for (UInt_t i=0;i<event.rec()->muons().size();i++)\n')
        file.write('    {\n')
        for item in part_list:
            WriteFillWithMuonContainer(item[0],file,item[1],item[2])
        file.write('    }\n')
        InstanceName.Clear()

        # Filling with taus
        file.write('    for (UInt_t i=0;i<event.rec()->taus().size();i++)\n')
        file.write('    {\n')
        for item in part_list:
            WriteFillWithTauContainer(item[0],file,item[1],item[2])
        file.write('    }\n')
        InstanceName.Clear()

        # Filling with MET
        file.write('    {\n')
        for item in part_list:
            WriteFillWithMETContainer(item[0],file,item[1],item[2])
        file.write('    }\n')
        InstanceName.Clear()

        # Filling with MHT
        file.write('    {\n')
        for item in part_list:
            WriteFillWithMHTContainer(item[0],file,item[1],item[2])
        file.write('    }\n')
        InstanceName.Clear()

    file.write('  }\n\n')

    # Managing PT rank
    file.write('  // Sorting particles\n') 
    for item in part_list:
        WriteJobRank(item[0],file,item[1],item[2])
    InstanceName.Clear()


def WriteSelection(file,main,part_list):

    import madanalysis.job.job_plot          as JobPlot
    import madanalysis.job.job_event_cut     as JobEventCut
    import madanalysis.job.job_candidate_cut as JobCandidateCut

    # Is there cuts
    Ncuts   = 0
    for item in main.selection.table:
        if item.__class__.__name__=="Cut":
            Ncuts+=1

    # Initial number of events
    if Ncuts!=0:
        file.write('  // Filling initial number\n')
        file.write('  cuts_.IncrementNInitial(__event_weight__);\n\n')

    # Loop over histogram and cut
    ihisto = 0
    icut = 0
    for iabs in range(len(main.selection.table)):
        
        file.write('  // Histogram/Cut number '+str(iabs)+'\n')
        file.write('  // '+main.selection[iabs].GetStringDisplay()+'\n')
        
        if main.selection[iabs].__class__.__name__=="Histogram":
            JobPlot.WritePlot(file,main,iabs,ihisto)
            ihisto+=1
            
        elif main.selection[iabs].__class__.__name__=="Cut":

            # Event cut
            if len(main.selection[iabs].part)==0:
                JobEventCut.WriteEventCut(file,main,iabs,icut)

            # Candidate cut    
            else:
                JobCandidateCut.WriteCandidateCut(file,main,iabs,icut,part_list)

            icut+=1
            
        file.write('\n')



