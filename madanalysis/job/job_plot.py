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


"""Writer of the C++ code filling the histograms of the selection.

For each combination of the arguments of the observable, nested loops over the
particle containers are written; combinations containing the same particle twice,
or already considered in another order, are skipped. With the ``all`` keyword, the
particles are summed before filling the histogram once per event.
"""

from __future__ import absolute_import
from __future__ import annotations
from typing import TYPE_CHECKING, Any, TextIO

if TYPE_CHECKING:
    from madanalysis.core.main import Main
    from madanalysis.multiparticle.particle_combination import ParticleCombination
from madanalysis.selection.histogram          import Histogram
from madanalysis.selection.instance_name      import InstanceName
from madanalysis.enumeration.observable_type  import ObservableType
from madanalysis.enumeration.argument_type    import ArgumentType
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.enumeration.combination_type import CombinationType
from madanalysis.interpreter.cmd_cut          import CmdCut
import logging
from six.moves import range


def WritePlot(file: TextIO,main: Main,iabs: int,ihisto: int) -> None:
    """Write the filling of a histogram according to the number of arguments of its observable.

    Args:
        file (``TextIO``): output C++ file.
        main (``Main``): session state.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
    """

    # Opening bracket for the current histo
    file.write('  {\n')

    if len(main.selection[iabs].arguments)==0:
        WritePlotWith0Arg(file,main,iabs,ihisto)
    elif len(main.selection[iabs].arguments)==1:
        WritePlotWith1Arg(file,main,iabs,ihisto)
    elif len(main.selection[iabs].arguments)==2:
        WritePlotWith2Args(file,main,iabs,ihisto) 
    else:
        logging.getLogger('MA5').error("observable with more than 2 arguments are " +\
                       "not managed by MadAnalysis 5")

    # Closing bracket for the current histo
    file.write('  }\n')


def WritePlotWith0Arg(file: TextIO,main: Main,iabs: int,ihisto: int) -> None:
    """Write the filling of a histogram of an event-level observable (or ``NPID``/``NAPID``).

    Args:
        file (``TextIO``): output C++ file.
        main (``Main``): session state.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
    """
    logging.getLogger('MA5').debug("  Writing histogram with 0 argument...")

    if main.selection[iabs].observable.name in ['NPID','NAPID']:
        WriteJobNPID(file,main,iabs,ihisto)
    else:
        file.write('    Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", ' +\
           main.selection[iabs].observable.code(main.mode)+');\n')

def WriteJobNPID(file: TextIO,main: Main,iabs: int,ihisto: int) -> None:
    """Write the filling of a ``NPID``/``NAPID`` histogram.

    At parton/hadron level the PDG codes of the Monte Carlo particles (with the
    requested status) are used; at reco level, the codes associated with the
    reconstructed objects (photons, leptons with charge, taus, jets and b/non-b jets).


    Args:
        file (``TextIO``): output C++ file.
        main (``Main``): session state.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
    """

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
            file.write('    Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", ' +\
                   'event.mc()->particles()[i].pdgid());\n')
        else:
            file.write('    Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", '  +\
                   'std::abs(event.mc()->particles()[i].pdgid()));\n')
        file.write('  }\n')

    # RECO mode
    else:

        # photons
        file.write('  for (unsigned int i=0;i<event.rec()->photons().size();i++)\n')
        file.write('  {\n')
        file.write('        Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\" , 22);\n')
        file.write('  }\n')

        # electrons
        file.write('  for (unsigned int i=0;i<event.rec()->electrons().size();i++)\n')
        file.write('  {\n')
        if npid:
            file.write('    if (event.rec()->electrons()[i].charge()<0) \n' +\
                       '          Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", +11);\n' +\
                       '    else\n' +\
                       '          Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", -11);\n')
        else:
            file.write('        Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", 11);\n')
        file.write('  }\n')

        # muons
        file.write('  for (unsigned int i=0;i<event.rec()->muons().size();i++)\n')
        file.write('  {\n')
        if npid:
            file.write('    if (event.rec()->muons()[i].charge()<0) \n' +\
                       '          Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", +13);\n' +\
                       '    else\n' +\
                       '          Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", -13);\n')
        else:
            file.write('        Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", 13);\n')
        file.write('  }\n')

        # taus
        file.write('  for (unsigned int i=0;i<event.rec()->taus().size();i++)\n')
        file.write('  {\n')
        if npid:
            file.write('    if (event.rec()->taus()[i].charge()<0) \n' +\
                       '          Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", +15);\n' +\
                       '    else\n' +\
                       '          Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", -15);\n')
        else:
            file.write('        Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", 15);\n')
        file.write('  }\n')

        # jets
        file.write('  for (unsigned int i=0;i<event.rec()->jets().size();i++)\n')
        file.write('  {\n')
        file.write('        Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", +21);\n')
        file.write('    if (event.rec()->jets()[i].btag())\n'+
                   '            Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", +5);\n')
        file.write('    else\n'+
                   '            Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", +1);\n')
        file.write('  }\n')

def WritePlotWith1Arg(file: TextIO,main: Main,iabs: int,ihisto: int) -> None:
    """Write the filling of a histogram of a one-argument observable (loop over the alternatives).

    Args:
        file (``TextIO``): output C++ file.
        main (``Main``): session state.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
    """


    logging.getLogger('MA5').debug("  Writing histogram with 1 argument...")

    # Skip observable with INT of FLOAT argument
    # Temporary
    # NOTE: arguments[0] is a ParticleObject, never an ArgumentType value: always False.
    if main.selection[iabs].arguments[0] in [ArgumentType.FLOAT,\
                                             ArgumentType.INTEGER]:
        return

    # Loop over combination
    for item in main.selection[iabs].arguments[0]:
        file.write('  {\n')
        WriteJobExecuteNbody(file,iabs,ihisto,item,main)
        file.write('  }\n')


def WritePlotWith2Args(file: TextIO,main: Main,iabs: int,ihisto: int) -> None:
    """Write the filling of a histogram of a two-argument observable (e.g. ``DELTAR``).

    Args:
        file (``TextIO``): output C++ file.
        main (``Main``): session state.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
    """
    
    logging.getLogger('MA5').debug("  Writing histogram with 2 arguments...")

    # Loop over combination
    for combi1 in main.selection[iabs].arguments[0]:
        for combi2 in main.selection[iabs].arguments[1]:
            file.write('  {\n')
            WriteJobExecute2Nbody(file,iabs,ihisto,combi1,combi2,main)
            file.write('  }\n')


def WriteJobExecute2Nbody(file: TextIO,iabs: int,ihisto: int,combi1: ParticleCombination,combi2: ParticleCombination,main: Main) -> None:
    """Write the loops and the filling of a two-argument observable.

    Four cases are handled depending on the ``all`` keyword in each argument.

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
        combi1 (``ParticleCombination``): combination of the first argument.
        combi2 (``ParticleCombination``): combination of the second argument.
        main (``Main``): session state.
    """

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
                                                histo.statuscode+'_REG_'+'_'.join(histo.regions)))

        # Case of one particle/multiparticle
        if len(combi2)==1:
            if main.mode == MA5RunningType.PARTON:
              TheObs=obs.code_parton[:-2]
            elif main.mode == MA5RunningType.HADRON:
              TheObs=obs.code_hadron[:-2]
            else:
              TheObs=obs.code_reco[:-2]
            file.write('          Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", ' +\
                       'q1.'+TheObs+'('+containers2[0]+\
                       '[b[0]]));\n')

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
                    # FIXME: 'iterator2' is undefined (NameError); the iterator of the second combination is 'b'.
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
                file.write('        Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", ' +\
                               'q1.'+TheObs+'(q2));\n')

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
                                                histo.statuscode+'_REG_'+'_'.join(histo.regions)))

        # Case of one particle/multiparticle
        if len(combi1)==1:
            if main.mode == MA5RunningType.PARTON:
              TheObs=obs.code_parton[:-2]
            elif main.mode == MA5RunningType.HADRON:
              TheObs=obs.code_hadron[:-2]
            else:
              TheObs=obs.code_reco[:-2]
            file.write('          Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", ' +\
                       'q2.'+TheObs+'('+containers1[0]+'[a[0]]));\n')

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
                    # FIXME: 'iterator1' is undefined (NameError); the iterator of the first combination is 'a'.
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
                file.write('        Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", ' +
                               'q1.'+TheObs+'(q2));\n')

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
        # NOTE: iabs and ihisto are swapped (unused by WriteEndLoop).
        WriteEndLoop(file,ihisto,iabs,combi2,main)

        # After the two loops 
        if main.mode == MA5RunningType.PARTON:
          TheObs=obs.code_parton[:-2]
        elif main.mode == MA5RunningType.HADRON:
          TheObs=obs.code_hadron[:-2]
        else:
          TheObs=obs.code_reco[:-2]
        file.write('        Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", ' +\
            'q1.'+TheObs+'(q2));\n')

def WriteBeforeLoop(file: TextIO,iabs: int,ihisto: int,combination: ParticleCombination,main: Main,value: str = 'value',q: str = 'q') -> None:
    """Declare the accumulators needed before the loops (counter or ``all`` sums).

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
        combination (``ParticleCombination``): combination of particles.
        main (``Main``): session state.
        value (``str``, default ``'value'``): C++ name of the scalar accumulator.
        q (``str``, default ``'q'``): C++ name of the four-vector accumulator.
    """

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
            file.write('    MAdouble64 '+value+'=0;\n')
        else:
            file.write('    ParticleBaseFormat '+q+';\n')

def WriteEndLoop(file: TextIO,iabs: int,ihisto: int,combination: ParticleCombination,main: Main) -> None:
    """Close the loops opened by :func:`WriteJobLoop`.

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
        combination (``ParticleCombination``): combination of particles.
        main (``Main``): session state.
    """
    for combi in range(len(combination)):
        file.write('    }\n')

def WriteAfterLoop(file: TextIO,iabs: int,ihisto: int,combination: ParticleCombination,main: Main) -> None:
    """Write the filling done after the loops (``all`` keyword and multiplicity observables).

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
        combination (``ParticleCombination``): combination of particles.
        main (``Main``): session state.
    """

    # shortcut
    obs     = main.selection[iabs].observable
    histo   = main.selection[iabs]
    allmode = ( len(combination)==1 and combination.ALL )

    # Case of ALL
    if allmode:

        # Case of observable N
        if obs.name in ['N','vN','sN','sdN','dsN','dvN','vdN','dN','rN']:
            file.write('        Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", 1);\n')

        # Case of other observable
        else:
            if obs.combination in [CombinationType.SUMSCALAR,\
                                   CombinationType.DIFFSCALAR]:
                file.write('        Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", value);\n')
            else:
                file.write('        Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", q.'+\
                           obs.code(main.mode)+');\n')

    # Case of observable N but not ALL
    elif obs.name in ['N','vN','sN','sdN','dsN','dvN','vdN','dN','rN']:
        file.write('        Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", Ncounter);\n')

def WriteBody(file: TextIO,iabs: int,ihisto: int,combination: ParticleCombination,main: Main,iterator: str = 'ind',value: str = 'value',q: str = 'q') -> None:
    """Write the body of the loops of a one-argument observable.

    Depending on the observable, the counter is incremented, the observable of a single
    particle is filled, or the scalar/vector sum (difference) or ratio of the particles
    is computed (and filled unless the ``all`` keyword is used).

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
        combination (``ParticleCombination``): combination of particles.
        main (``Main``): session state.
        iterator (``str``, default ``'ind'``): name of the C++ index array.
        value (``str``, default ``'value'``): C++ name of the scalar accumulator.
        q (``str``, default ``'q'``): C++ name of the four-vector accumulator.
    """

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
                                           histo.statuscode+'_REG_'+'_'.join(histo.regions)))

    # Only one particle (but not ALL)
    if len(combination)==1 and not allmode:
        file.write('      Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", ' +\
           containers[0]+'['+iterator+'[0]]->'+obs.code(main.mode)+');\n')
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
            file.write('    MAdouble64 '+value+'=0;\n')
        for ind in range(len(combination)):
            TheOper='+'
            if ind!=0:
              TheOper=oper_string
            file.write('    '+value+TheOper+'='+\
                       containers[ind]+'['+iterator+'['+str(ind)+']]->'+\
                       obs.code(main.mode)+';\n')

        if not allmode:
            file.write('      Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", ' +\
               value+');\n')

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
            file.write('      Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", ' +\
                q+'.'+obs.code(main.mode)+');\n')

    # ratio
    elif obs.combination==CombinationType.RATIO and \
        len(combination)==2:
        file.write('      Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", (' +\
            containers[0]+'['+iterator+'[0]]->'+\
            obs.code(main.mode)+\
            '-'+\
            containers[1]+'['+iterator+'[1]]->'+\
            obs.code(main.mode)+\
            ') / '+\
            containers[0]+'['+iterator+'[0]]->'+\
            obs.code(main.mode)+');\n')

def WriteBody2(file: TextIO,iabs: int,ihisto: int,combi1: ParticleCombination,combi2: ParticleCombination,main: Main,iterator1: str,iterator2: str) -> None:
    """Write the filling of a two-argument observable for the current combinations.

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
        combi1 (``ParticleCombination``): combination of the first argument.
        combi2 (``ParticleCombination``): combination of the second argument.
        main (``Main``): session state.
        iterator1 (``str``): C++ index array of the first combination.
        iterator2 (``str``): C++ index array of the second combination.
    """

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
                                            histo.statuscode+'_REG_'+'_'.join(histo.regions)))

    # Getting container name
    containers2=[]
    for item in combi2:
        containers2.append(InstanceName.Get('P_'+\
                                            item.name+\
                                            histo.rank+\
                                            histo.statuscode+'_REG_'+'_'.join(histo.regions)))

    # Case of one particle/multiparticle
    if len(combi1)==1 and len(combi2)==1:
        if main.mode == MA5RunningType.PARTON:
          TheObs=obs.code_parton[:-2]
        elif main.mode == MA5RunningType.HADRON:
          TheObs=obs.code_hadron[:-2]
        else:
          TheObs=obs.code_reco[:-2]
        file.write('      Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", ' +\
                   containers1[0]+'['+iterator1+'[0]]->'+\
                   TheObs+'('+containers2[0]+'['+iterator2+'[0]]));\n')
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
        file.write('        Manager()->FillHisto(\"'+str(ihisto)+'_'+main.selection[iabs].observable.name+'\", ' +\
                       'q1.'+TheObs+'(q2));\n')

def HasDoubleCounting(combination: ParticleCombination | list[Any]) -> bool:
    """Check whether two particles of a combination may share the same object.

    Args:
        combination (``ParticleCombination | list[Any]``): combination (or list of
            particles).

    Returns:
        ``bool``:
        ``True`` if two (multi)particles have a common PDG code.
    """
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

def WriteJobExecuteNbody(file: TextIO,iabs: int,ihisto: int,combination: ParticleCombination,main: Main) -> None:
    """Write the loops and the filling of a one-argument observable.

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
        combination (``ParticleCombination``): combination of particles.
        main (``Main``): session state.
    """

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


def WriteAvoidRedundancies(file: TextIO,iabs: int,ihisto: int,combi1: ParticleCombination,combi2: ParticleCombination,main: Main,iterator1: str,iterator2: str) -> None:
    """Skip the combinations where both arguments are the same object (single particles only).

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
        combi1 (``ParticleCombination``): combination of the first argument.
        combi2 (``ParticleCombination``): combination of the second argument.
        main (``Main``): session state.
        iterator1 (``str``): C++ index array of the first combination.
        iterator2 (``str``): C++ index array of the second combination.
    """

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
                                            histo.statuscode+'_REG_'+'_'.join(histo.regions)))

    # Getting container name
    containers2=[]
    for item in combi2:
        containers2.append(InstanceName.Get('P_'+\
                                            item.name+\
                                            histo.rank+\
                                            histo.statuscode+'_REG_'+'_'.join(histo.regions)))

    # Case of one particle/multiparticle
    if len(combi1)==1 and len(combi2)==1:
        file.write('     if ( '+containers1[0]+'['+iterator1+'[0]] == '+\
                   containers2[0]+'['+iterator2+'[0]] ) continue;\n')


def WriteJobLoop(file: TextIO,iabs: int,ihisto: int,combination: ParticleCombination,redundancies: bool,main: Main,iterator: str = 'ind') -> None:
    """Open the nested C++ loops over the containers of a combination.

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
        combination (``ParticleCombination``): combination of particles.
        redundancies (``bool``): some particles can appear in several containers.
        main (``Main``): session state.
        iterator (``str``, default ``'ind'``): name of the C++ index array.
    """

    histo = main.selection[iabs]

    # Getting container name
    containers=[]
    for item in combination:
        containers.append(InstanceName.Get('P_'+\
                                           item.name+histo.rank+histo.statuscode+'_REG_'+'_'.join(histo.regions)))

    # Declaring indicator
    file.write('    MAuint32 '+iterator+'['+str(len(combination))+'];\n')

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


def WriteJobSameCombi(file: TextIO,iabs: int,ihisto: int,combination: ParticleCombination,redundancies: bool,main: Main,iterator: str = 'ind') -> None:
    """Write the skipping of combinations already considered (in another order).

    Args:
        file (``TextIO``): output C++ file.
        iabs (``int``): index of the histogram in the selection.
        ihisto (``int``): 1-based number of the histogram.
        combination (``ParticleCombination``): combination of particles.
        redundancies (``bool``): some particles can appear in several containers.
        main (``Main``): session state.
        iterator (``str``, default ``'ind'``): name of the C++ index array.
    """

    if len(combination)==1 or not redundancies:
        return

    histo = main.selection[iabs]

    # Getting container name
    containers=[]
    for item in combination:
        containers.append(InstanceName.Get('P_'+\
                                           item.name+histo.rank+histo.statuscode+'_REG_'+'_'.join(histo.regions)))

    file.write('\n    // Checking if consistent combination\n')
    if main.mode in [MA5RunningType.PARTON,MA5RunningType.HADRON]:
        file.write('    std::set<const MCParticleFormat*> mycombi;\n')
    else:
        file.write('    std::set<const RecParticleFormat*> mycombi;\n')
    file.write('    for (MAuint32 i=0;i<'+str(len(combination))+';i++)\n')
    file.write('    {\n')
    for i in range(len(combination)):
        # FIXME: the generated C++ loop inserts containers[k][iterator[i]] for every i and every k
        # (the C++ index i is used for all containers): wrong combination set and possible out-of-range access.
        file.write('      mycombi.insert('+containers[i]+'['+iterator+'[i]]);\n')
    file.write('    }\n')
    file.write('    MAbool matched=false;\n')
    file.write('    for (MAuint32 i=0;i<combis.size();i++)\n')
    file.write('      if (combis[i]==mycombi) {matched=true; break;}\n')
    file.write('    if (matched) continue;\n')
    file.write('    else combis.push_back(mycombi);\n\n')


