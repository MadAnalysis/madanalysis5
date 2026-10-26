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


"""Writer of the ``user::Execute`` method of the generated analysis.

:class:`~madanalysis.selection.instance_name.InstanceName` is used to give unique C++
names to the containers and to avoid duplicated code; it is cleared between the
different code blocks.
"""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING, Any, TextIO

if TYPE_CHECKING:
    from madanalysis.core.main import Main
    from madanalysis.multiparticle.extraparticle import ExtraParticle
from madanalysis.selection.histogram          import Histogram
from madanalysis.selection.instance_name      import InstanceName
from madanalysis.enumeration.observable_type  import ObservableType
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.interpreter.cmd_cut          import CmdCut
import logging
import copy
from six.moves import range

def WriteExecute(file: TextIO,main: Main,part_list: list[list[Any]]) -> None:
    """Write ``user::Execute``.

    The method initialises the event weight, clears and fills the particle containers,
    then applies the plots and cuts of the selection in order.

    Args:
        file (``TextIO``): output C++ file.
        main (``Main``): session state.
        part_list (``list[list[Any]]``): particle containers (see
            :func:`~madanalysis.job.job_particle.GetParticles`).
    """

    # Function header
    file.write('MAbool user::Execute(SampleFormat& sample, ' +\
               'const EventFormat& event)\n{\n')

    # Getting the event weight
    file.write('  MAfloat32 __event_weight__ = 1.0;\n')
    file.write('  if (weighted_events_ && event.mc()!=0) ' +\
               '__event_weight__ = event.mc()->weight();\n\n')  
    file.write('  if (sample.mc()!=0) sample.mc()->addWeightedEvents(__event_weight__);\n')
    file.write('  Manager()->InitializeForNewEvent(__event_weight__);\n')
    file.write('\n')

    # Reseting instance name
    InstanceName.Clear()

    # Clearing and filling containers
    WriteContainer(file,main,part_list)

    # Writing each step of the selection
    WriteSelection(file,main,part_list)

    # End
    file.write('  return true;\n')
    file.write('}\n\n')

def WriteJobRank(part: ExtraParticle,file: TextIO,rank: str,status: str,regions: list[str]) -> None:
    """Write the extraction of the PT-ranked particle from the ordered container.

    Nothing is written for unranked particles.

    Args:
        part (``ExtraParticle``): particle of the container.
        file (``TextIO``): output C++ file.
        rank (``str``): ranking option (e.g. ``'PTordering'``).
        status (``str``): status code.
        regions (``list[str]``): regions of the container.
    """

    if part.PTrank==0:
        return

    # Skipping if already defined
    # NOTE: the 'PTRANK_' key is never registered with InstanceName.Get: this test never skips.
    if InstanceName.Find("PTRANK_"+part.name+rank+status):
        return
    container=InstanceName.Get('P_'+part.name+rank+status+'_REG_'+'_'.join(regions))
    refpart = copy.copy(part)
    refpart.PTrank=0
    newcontainer=InstanceName.Get('P_'+refpart.name+'PTordering'+status+'_REG_'+'_'.join(regions));

    file.write('  // Sorting particle collection according to '+rank+'\n')
    file.write('  // for getting '+str(part.PTrank)+'th particle\n')
    file.write('  '+container+'=SORTER->rankFilter('+\
               newcontainer+','+str(part.PTrank)+','+rank+');\n\n')


def WriteCleanContainer(part: ExtraParticle,file: TextIO,rank: str,status: str,regions: list[str]) -> None:
    """Write the clearing of a particle container (once per container).

    Args:
        part (``ExtraParticle``): particle of the container.
        file (``TextIO``): output C++ file.
        rank (``str``): ranking option (e.g. ``'PTordering'``).
        status (``str``): status code.
        regions (``list[str]``): regions of the container.
    """
    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status+'_REG_'+'_'.join(regions)):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status+'_REG_'+'_'.join(regions))

    # Getting id name
    # NOTE: 'id' is unused.
    id='isP_'+InstanceName.Get(part.name+rank+status+'_REG_'+'_'.join(regions))

    file.write('      ' + container + '.clear();\n')


def WriteFillContainer(part: ExtraParticle,file: TextIO,rank: str,status: str,regions: list[str]) -> None:
    """Write the filling of a container with the Monte Carlo particles passing ``isP_<name>``.

    Nothing is written for PT-ranked particles (filled by :func:`WriteJobRank`).

    Args:
        part (``ExtraParticle``): particle of the container.
        file (``TextIO``): output C++ file.
        rank (``str``): ranking option (e.g. ``'PTordering'``).
        status (``str``): status code.
        regions (``list[str]``): regions of the container.
    """

    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status+'_REG_'+'_'.join(regions)):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status+'_REG_'+'_'.join(regions))

    # Getting id name
    id='isP_'+InstanceName.Get(part.name+rank+status)

    file.write('      if ('+id+'((&(event.mc()->particles()[i])))) ' +\
               container + '.push_back(&(event.mc()->particles()[i]));\n')


def WriteFillWithJetContainer(part: ExtraParticle,file: TextIO,rank: str,status: str,regions: list[str]) -> None:
    """Write the filling of the container with the jets of the event (unranked particles only).

    PDG 21: all jets, 5: b-tagged jets, 1: non-b-tagged jets.

    Args:
        part (``ExtraParticle``): particle of the container.
        file (``TextIO``): output C++ file.
        rank (``str``): ranking option (e.g. ``'PTordering'``).
        status (``str``): status code.
        regions (``list[str]``): regions of the container.
    """

    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status+'_REG_'+'_'.join(regions)):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status+'_REG_'+'_'.join(regions))

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


def WriteFillWithElectronContainer(part: ExtraParticle,file: TextIO,rank: str,status: str,regions: list[str]) -> None:
    """Write the filling of the container with the electrons of the event (unranked particles only).

    The sign of the PDG identifier selects the charge.

    Args:
        part (``ExtraParticle``): particle of the container.
        file (``TextIO``): output C++ file.
        rank (``str``): ranking option (e.g. ``'PTordering'``).
        status (``str``): status code.
        regions (``list[str]``): regions of the container.
    """

    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status+'_REG_'+'_'.join(regions)):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status+'_REG_'+'_'.join(regions))

    # Put negative electron
    if part.particle.Find(11):
        file.write('      if (event.rec()->electrons()[i].charge()<0) '+\
                   container+'.push_back(&(event.rec()->electrons()[i]));\n')

    # Put positive electron
    if part.particle.Find(-11):
        file.write('      if (event.rec()->electrons()[i].charge()>0) '+\
                   container+'.push_back(&(event.rec()->electrons()[i]));\n')


def WriteFillWithPhotonContainer(part: ExtraParticle,file: TextIO,rank: str,status: str,regions: list[str]) -> None:
    """Write the filling of the container with the photons of the event (unranked particles only).

    Args:
        part (``ExtraParticle``): particle of the container.
        file (``TextIO``): output C++ file.
        rank (``str``): ranking option (e.g. ``'PTordering'``).
        status (``str``): status code.
        regions (``list[str]``): regions of the container.
    """

    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status+'_REG_'+'_'.join(regions)):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status+'_REG_'+'_'.join(regions))

    # Put photon
    if part.particle.Find(22):
        file.write('      '+container+'.push_back(&(event.rec()->photons()[i]));\n')


def WriteFillWithMuonContainer(part: ExtraParticle,file: TextIO,rank: str,status: str,regions: list[str]) -> None:
    """Write the filling of the container with the muons of the event (unranked particles only).

    The sign selects the charge; PDG 130 stands for isolated muons.

    Args:
        part (``ExtraParticle``): particle of the container.
        file (``TextIO``): output C++ file.
        rank (``str``): ranking option (e.g. ``'PTordering'``).
        status (``str``): status code.
        regions (``list[str]``): regions of the container.
    """

    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status+'_REG_'+'_'.join(regions)):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status+'_REG_'+'_'.join(regions))

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


def WriteFillWithTauContainer(part: ExtraParticle,file: TextIO,rank: str,status: str,regions: list[str]) -> None:
    """Write the filling of the container with the hadronic taus of the event (unranked particles only).

    The sign of the PDG identifier selects the charge.

    Args:
        part (``ExtraParticle``): particle of the container.
        file (``TextIO``): output C++ file.
        rank (``str``): ranking option (e.g. ``'PTordering'``).
        status (``str``): status code.
        regions (``list[str]``): regions of the container.
    """

    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status+'_REG_'+'_'.join(regions)):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status+'_REG_'+'_'.join(regions))

    # Put negative tau
    if part.particle.Find(15):
        file.write('      if (event.rec()->taus()[i].charge()<0) '+\
                   container+'.push_back(&(event.rec()->taus()[i]));\n')

    # Put positive tau
    if part.particle.Find(-15):
        file.write('      if (event.rec()->taus()[i].charge()>0) '+\
                   container+'.push_back(&(event.rec()->taus()[i]));\n')

def WriteFillWithMETContainer(part: ExtraParticle,file: TextIO,rank: str,status: str,regions: list[str]) -> None:
    """Write the filling of the container with the reconstructed MET (PDG 100).

    Args:
        part (``ExtraParticle``): particle of the container.
        file (``TextIO``): output C++ file.
        rank (``str``): ranking option (e.g. ``'PTordering'``).
        status (``str``): status code.
        regions (``list[str]``): regions of the container.
    """

    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    # NOTE: the key tested here (without regions) differs from the one registered below.
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status+'_REG_'+'_'.join(regions))

    # Put MET
    if part.particle.Find(100):
        file.write('       '+container+\
                   '.push_back(&(event.rec()->MET()));\n')


def WriteFillWithMHTContainer(part: ExtraParticle,file: TextIO,rank: str,status: str,regions: list[str]) -> None:
    """Write the filling of the container with the reconstructed MHT (PDG 99).

    Args:
        part (``ExtraParticle``): particle of the container.
        file (``TextIO``): output C++ file.
        rank (``str``): ranking option (e.g. ``'PTordering'``).
        status (``str``): status code.
        regions (``list[str]``): regions of the container.
    """

    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status+'_REG_'+'_'.join(regions))

    # Put MHT
    if part.particle.Find(99):
        file.write('       '+container+\
                   '.push_back(&(event.rec()->MHT()));\n')


def WriteFillWithMETContainerMC(part: ExtraParticle,file: TextIO,rank: str,status: str,regions: list[str]) -> None:
    """Write the filling of the container with the Monte Carlo MET (PDG 100).

    Args:
        part (``ExtraParticle``): particle of the container.
        file (``TextIO``): output C++ file.
        rank (``str``): ranking option (e.g. ``'PTordering'``).
        status (``str``): status code.
        regions (``list[str]``): regions of the container.
    """

    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status+'_REG_'+'_'.join(regions))

    # Put MET
    if part.particle.Find(100):
        file.write('       '+container+\
                   '.push_back(&(event.mc()->MET()));\n')


def WriteFillWithMHTContainerMC(part: ExtraParticle,file: TextIO,rank: str,status: str,regions: list[str]) -> None:
    """Write the filling of the container with the Monte Carlo MHT (PDG 99).

    Args:
        part (``ExtraParticle``): particle of the container.
        file (``TextIO``): output C++ file.
        rank (``str``): ranking option (e.g. ``'PTordering'``).
        status (``str``): status code.
        regions (``list[str]``): regions of the container.
    """
    
    # If PTrank, no fill
    if part.PTrank!=0:
        return

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting container name
    container=InstanceName.Get('P_'+part.name+rank+status+'_REG_'+'_'.join(regions))

    # Put MHT
    if part.particle.Find(99):
        file.write('       '+container+\
                   '.push_back(&(event.mc()->MHT()));\n')


def WriteContainer(file: TextIO,main: Main,part_list: list[list[Any]]) -> None:
    """Write the clearing, filling and PT ranking of all particle containers.

    At parton/hadron level the Monte Carlo particles are looped over; at reco level,
    the jets, photons, electrons, muons, taus, MET and MHT.

    Args:
        file (``TextIO``): output C++ file.
        main (``Main``): session state.
        part_list (``list[list[Any]]``): particle containers (see
            :func:`~madanalysis.job.job_particle.GetParticles`).
    """

    # Skipping empty case
    if len(part_list)==0:
        return

    # Cleaning particle containers
    file.write('  // Clearing particle containers\n') 
    file.write('  {\n')
    for item in part_list:
        WriteCleanContainer(item[0],file,item[1],item[2], item[3])
    file.write('  }\n')
    InstanceName.Clear()
    file.write('\n')

    # Filling particle containers
    file.write('  // Filling particle containers\n') 
    file.write('  {\n')
    InstanceName.Clear()

    # Filling particle containers in PARTON and HADRON mode
    if main.mode in [MA5RunningType.PARTON, MA5RunningType.HADRON]:

        # Special particles : MET or MHT
        for item in part_list:
            if item[0].particle.Find(99) or item[0].particle.Find(100):
                WriteFillWithMETContainerMC(item[0],file,item[1],item[2],item[3])
                WriteFillWithMHTContainerMC(item[0],file,item[1],item[2],item[3])

        # Ordinary particles
        file.write('    for (MAuint32 i=0;i<event.mc()->particles().size();i++)\n')
        file.write('    {\n')
        for item in part_list:
            if item[0].particle.Find(99) or item[0].particle.Find(100):
                pass
            else:
                WriteFillContainer(item[0],file,item[1],item[2],item[3])
        file.write('    }\n')
        InstanceName.Clear()

    # Filling particle containers in RECO mode    
    else:

        # Filling with jets
        file.write('    for (MAuint32 i=0;i<event.rec()->jets().size();i++)\n')
        file.write('    {\n')
        for item in part_list:
            WriteFillWithJetContainer(item[0],file,item[1],item[2],item[3])
        file.write('    }\n')
        InstanceName.Clear()

        # Filling with photons
        file.write('    for (MAuint32 i=0;i<event.rec()->photons().size();i++)\n')
        file.write('    {\n')
        for item in part_list:
            WriteFillWithPhotonContainer(item[0],file,item[1],item[2],item[3])
        file.write('    }\n')
        InstanceName.Clear()

        # Filling with electrons
        file.write('    for (MAuint32 i=0;i<event.rec()->electrons().size();i++)\n')
        file.write('    {\n')
        for item in part_list:
            WriteFillWithElectronContainer(item[0],file,item[1],item[2],item[3])
        file.write('    }\n')
        InstanceName.Clear()

        # Filling with muons
        file.write('    for (MAuint32 i=0;i<event.rec()->muons().size();i++)\n')
        file.write('    {\n')
        for item in part_list:
            WriteFillWithMuonContainer(item[0],file,item[1],item[2],item[3])
        file.write('    }\n')
        InstanceName.Clear()

        # Filling with taus
        file.write('    for (MAuint32 i=0;i<event.rec()->taus().size();i++)\n')
        file.write('    {\n')
        for item in part_list:
            WriteFillWithTauContainer(item[0],file,item[1],item[2],item[3])
        file.write('    }\n')
        InstanceName.Clear()

        # Filling with MET
        file.write('    {\n')
        for item in part_list:
            WriteFillWithMETContainer(item[0],file,item[1],item[2],item[3])
        file.write('    }\n')
        InstanceName.Clear()

        # Filling with MHT
        file.write('    {\n')
        for item in part_list:
            WriteFillWithMHTContainer(item[0],file,item[1],item[2],item[3])
        file.write('    }\n')
        InstanceName.Clear()

    file.write('  }\n\n')

    # Managing PT rank
    file.write('  // Sorting particles\n') 
    for item in part_list:
        WriteJobRank(item[0],file,item[1],item[2],item[3])
    InstanceName.Clear()


def WriteSelection(file: TextIO,main: Main,part_list: list[list[Any]]) -> None:
    """Write the code of each plot, event cut and candidate cut of the selection.

    Args:
        file (``TextIO``): output C++ file.
        main (``Main``): session state.
        part_list (``list[list[Any]]``): particle containers (see
            :func:`~madanalysis.job.job_particle.GetParticles`).
    """

    import madanalysis.job.job_plot          as JobPlot
    import madanalysis.job.job_event_cut     as JobEventCut
    import madanalysis.job.job_candidate_cut as JobCandidateCut

    # Loop over histogram and cut
    ihisto  = 1
    icut    = 1
    iobject = 1
    for iabs in range(len(main.selection.table)):

        logging.getLogger('MA5').debug("--------------------------------------------")
        logging.getLogger('MA5').debug("SELECTION STEP "+str(iabs)+": "+main.selection[iabs].GetStringDisplay())

        if main.selection[iabs].__class__.__name__=="Histogram":
            logging.getLogger('MA5').debug("- selection step = histogram")
            file.write('  // Histogram number '+str(ihisto)+'\n')
            file.write('  // '+main.selection[iabs].GetStringDisplay()+'\n')
            JobPlot.WritePlot(file,main,iabs,ihisto)
            ihisto+=1

        elif main.selection[iabs].__class__.__name__=="Cut":
            # Event cut
            if len(main.selection[iabs].part)==0:
                logging.getLogger('MA5').debug("- selection step = cut on event")
                file.write('  // Event selection number '+str(icut)+'\n')
                file.write('  // '+main.selection[iabs].GetStringDisplay()+'\n')
                JobEventCut.WriteEventCut(file,main,iabs,icut)
                icut+=1

            # Candidate cut
            else:
                logging.getLogger('MA5').debug("- selection step = cut on candidate")
                file.write('  // Object selection number '+str(iobject)+'\n')
                file.write('  // '+main.selection[iabs].GetStringDisplay()+'\n')
                JobCandidateCut.WriteCandidateCut(file,main,iabs,part_list)
                iobject+=1

        file.write('\n')
    logging.getLogger('MA5').debug("--------------------------------------------")

