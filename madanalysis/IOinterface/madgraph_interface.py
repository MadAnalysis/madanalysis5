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


"""Automatic generation of MadAnalysis 5 cards for MadGraph5_aMC@NLO.

When MadAnalysis 5 is used from MG5_aMC, default analysis cards are written from the
generated processes: plots of the kinematics of the final-state (and intermediate)
particles at parton level, and a reconstruction (FastJet, Delphes) plus plots and
recasting commands at hadron level. The MG5_aMC objects (processes, model, legs) are
accessed through their ``get`` method.
"""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any
from madanalysis.configuration.recast_configuration     import RecastConfiguration
import itertools
import logging
import os
import six
from six.moves import range

class MadGraphInterface():
    """Generator of the MadAnalysis 5 cards used by MG5_aMC.

    Attributes:
        model: UFO model of MG5_aMC (``particle_dict`` gives the particle properties).
        multiparticles (``dict[str, list[int]]``): multiparticles defined in MG5_aMC.
        card (``list[str]``): lines of the card being generated.
        invisible_particles (``list[str]``): names of the invisible particles.
        invisible_pdgs (``list[str]``): PDG codes of the invisible particles.
        recastinfo (``RecastConfiguration``): used to list the PAD analyses.
        has_root / has_matplotlib / has_delphes / has_delphesMA5tune (``bool``): available
            packages (set by the MG5_aMC interface).
    """

    def __init__(self) -> None:
        """Create the generator (all packages assumed available)."""
        self.logger = logging.getLogger('MA5')
        self.model = ''
        self.multiparticles={}
        self.card=[]
        self.invisible_particles = []
        self.invisible_pdgs = []
        self.recastinfo = RecastConfiguration()
        self.has_root           = True
        self.has_matplotlib     = True
        self.has_delphes        = True
        self.has_delphesMA5tune = True

    class InvalidCard(Exception):
        """Raised for an unknown card type."""
        pass
    class MultiParts(Exception):
        """Raised when a PDG-code list matches no multiparticle."""
        pass


    def generate_card(self, MG5history: list[str], ProcessesDefinitions: list, ProcessesLists: list, card_type: str = 'parton') -> str:
        """Generate a MadAnalysis 5 card.

        Args:
            MG5history (``list[str]``): MG5_aMC command history (used for ``define`` commands).
            ProcessesDefinitions (``list``): one process definition per ``generate``/``add
                process`` command.
            ProcessesLists (``list``): detailed list of the processes (gives the model).
            card_type (``str``, default ``'parton'``): ``'parton'`` or ``'hadron'``.

        Raises:
            ``MadGraphInterface.InvalidCard``: for an unknown card type.

        Returns:
            ``str``:
            The content of the card.
        """

        ## Initialization
        self.logger.info('Creating an MA5 card for the mode: ' + card_type)
        self.card=[]

        ## card header
        if card_type=='parton':
            self.card.append('# Uncomment the line below to skip this analysis altogether')
            self.card.append('# @MG5aMC skip_analysis\n')
            self.card.append('@MG5aMC stdout_lvl=INFO\n')
            self.card.append('@MG5aMC inputs = *.lhe')
            self.card.append('@MG5aMC analysis_name = analysis1\n')
        elif card_type=='hadron':
            self.card.append('# Uncomment the line below to skip this analysis altogether')
            self.card.append('# @MG5aMC skip_analysis\n')
            self.card.append('@MG5aMC stdout_lvl=INFO\n')
            if self.has_root and not self.has_delphes:
                self.card.append('# Recasting functionalities based on Delphes turned off. Please type')
                self.card.append('#       install MadAnalysis5 --update --with_delphes')
                self.card.append('# in the MG5 interpereter to turn them on.\n')
            if self.has_root and not self.has_delphesMA5tune:
                self.card.append('# Recasting functionalities based on DelphesMA5tune turned off. Please type')
                self.card.append('#       install MadAnalysis5 --update --with_delphesMA5tune')
                self.card.append('# in the MG5 interpereter to turn them on.\n')

            self.card.append('@MG5aMC inputs = *.hepmc, *.hep, *.stdhep, *.lhco, *.fifo\n')
            self.card.append('# Reconstruction using FastJet')
            self.card.append('@MG5aMC reconstruction_name = BasicReco')
            self.card.append('@MG5aMC reco_output = lhe')

        self.logger.info('Getting the UFO model:')
        self.model = ProcessesLists[0][0].get('model')
        self.logger.debug('  >> ' + self.model.get('name'))
        self.get_invisible(card_type)

        self.logger.info('Getting the multiparticle definitions')
        for line in MG5history:
            if 'define' in line:
                myline = line.split('#')[0].split()
                self.logger.debug('pdgs = '+str(myline[3:]))
                mypdgs= [self.get_pdg_code(prt) for prt in myline[3:]]
                self.multiparticles[myline[1]]=sorted(sum([e if isinstance(e,list) else [e] for e in mypdgs],[]))
        # FIXME: deleting entries while iterating over the dictionary raises a RuntimeError when an
        # empty multiparticle is found.
        for key, value in self.multiparticles.items():
            if len([x for x in value if x != '']) == 0:
                del self.multiparticles[key]
        self.logger.debug('  >> ' + str(self.multiparticles))
        self.logger.debug('  >> invisible: ' + str(self.invisible_particles))
        if card_type=='parton':
            self.write_multiparticles()

        if card_type=='hadron':
            return self.generate_hadron_card(ProcessesDefinitions, ProcessesLists)
        elif card_type=='parton':
            return self.generate_parton_card(ProcessesDefinitions, ProcessesLists)
        else:
            self.logger.error('  ** Unknown card type')
            raise self.InvalidCard('Unknown card type')


    def generate_parton_card(self, ProcessesDefinitions: list, ProcessesLists: list) -> str:
        """Complete the parton-level card (global plots and plots of each process).

        Args:
            ProcessesDefinitions (``list``): process definitions.
            ProcessesLists (``list``): detailed process lists (unused).

        Returns:
            ``str``:
            The content of the card.
        """
        self.card.append('# Histogram drawer (options: matplotlib or root)')
        if self.has_root:
            self.card.append('set main.graphic_render = root\n')
        elif self.has_matplotlib:
            self.card.append('set main.graphic_render = matplotlib\n')
        else:
            self.logger.warning('plots cannot be generated (neither root nor matplotlib can be found')
            self.card.append('set main.graphic_render = none\n')

        # global observables
        self.card.append('# Global event variables')
        self.card.append('plot THT   40 0 500 [logY]')
        self.card.append('plot MET   40 0 500 [logY]')
        self.card.append('plot SQRTS 40 0 500 [logY]')

        # processes is a list of ProcessDefinitions
        self.logger.info('Decoding the considered process')
        for myprocdef in ProcessesDefinitions:
            self.generate_parton_card_for_procdef(myprocdef, interstate=[], finalstate=[])

        # output
        return '\n'.join(self.card)


    def generate_hadron_card(self, ProcessesDefinitions: list, ProcessesLists: list) -> str:
        """Complete the hadron-level card.

        The card defines a FastJet reconstruction (and a Delphes one if available), basic
        object selections, plots of the leading objects expected from the final states, and
        the recasting commands with all PAD analyses commented out.

        Args:
            ProcessesDefinitions (``list``): process definitions.
            ProcessesLists (``list``): detailed process lists (unused).

        Returns:
            ``str``:
            The content of the card.
        """
        self.card.append('set main.fastsim.package = fastjet')
        self.card.append('set main.fastsim.algorithm = antikt')
        self.card.append('set main.fastsim.radius = 0.4')
        self.card.append('set main.fastsim.ptmin = 5.0')
        self.card.append('# b-tagging')
        self.card.append('set main.fastsim.bjet_id.matching_dr = 0.4')
        # FIXME: the b/tau efficiency options below are deprecated (they now only print an error).
        self.card.append('set main.fastsim.bjet_id.efficiency = 1.0')
        self.card.append('set main.fastsim.bjet_id.misid_cjet = 0.0')
        self.card.append('set main.fastsim.bjet_id.misid_ljet = 0.0')
        self.card.append('# tau-tagging')
        self.card.append('set main.fastsim.tau_id.efficiency = 1.0')
        self.card.append('set main.fastsim.tau_id.misid_ljet = 0.0')

        if self.has_root and self.has_delphes:
            self.card.append('\n# Reconstruction using Delphes')
            self.card.append('@MG5aMC reconstruction_name = CMSReco')
            self.card.append('@MG5aMC reco_output = root')
            self.card.append('set main.fastsim.package  = delphes')
            self.card.append('set main.fastsim.detector = cms-ma5tune')
        elif self.has_root and self.has_delphesMA5tune:
            self.card.append('\n# Reconstruction using Delphes')
            self.card.append('@MG5aMC reconstruction_name = CMSReco')
            self.card.append('@MG5aMC reco_output = root')
            self.card.append('set main.fastsim.package  = delphesMA5tune')
            self.card.append('set main.fastsim.detector = cms')


        if self.has_root and (self.has_delphes or self.has_delphesMA5tune):
            self.card.append('\n# Analysis using both reco')
            self.card.append('@MG5aMC analysis_name = analysis2')
            self.card.append('# Uncomment the next line to bypass this analysis')
            self.card.append('# @MG5aMC skip_analysis')
            self.card.append('@MG5aMC set_reconstructions = [\'BasicReco\', \'CMSReco\']')
        else:
            self.card.append('\n# Analysis using the fastjet reco')
            self.card.append('@MG5aMC analysis_name = analysis2')
            self.card.append('# Uncomment the next line to bypass this analysis')
            self.card.append('# @MG5aMC skip_analysis')
            self.card.append('@MG5aMC set_reconstructions = [\'BasicReco\']')
        self.card.append('\n# plot tunning: dsigma/sigma is plotted.')
        self.card.append('set main.stacking_method = normalize2one')
        self.card.append('\n# object definition')
        self.card.append('define e = e+ e-')
        self.card.append('define mu = mu+ mu-')
        self.card.append('select (j)  PT > 20')
        self.card.append('select (b)  PT > 20')
        self.card.append('select (e)  PT > 10')
        self.card.append('select (mu) PT > 10')
        self.card.append('select (j)  ABSETA < 2.5')
        self.card.append('select (b)  ABSETA < 2.5')
        self.card.append('select (e)  ABSETA < 2.5')
        self.card.append('select (mu) ABSETA < 2.5')

        self.card.append('# Basic plots')
        self.card.append('plot MET 40 0 500')
        self.card.append('plot THT 40 0 500')

        ## Getting the number of expected jets, electrons, etc...
        nj, nb, ntau, nmu, ne, na = 0,0,0,0,0,0
        for myprocdef in ProcessesDefinitions:
            mynj, mynb, myntau, mynmu, myne, myna = 0,0,0,0,0,0
            myfinal = self.get_finalstate_particles(myprocdef)
            for x in myfinal:
                mynj  += self.get_Npart(x, [4,3,2,1,-1,-2,-3,-4,21])
                mynb  += self.get_Npart(x, [5,-5])
                myna  += self.get_Npart(x, [22])
                myne  += self.get_Npart(x, [11,-11])
                mynmu += self.get_Npart(x, [13,-13])
                myntau+= self.get_Npart(x, [15,-15])
            if nj < mynj:
                nj=mynj
            if nb < mynb:
                nb=mynb
            if ne < myne:
                ne=myne
            if nmu< mynmu:
                nmu=mynmu
            if ntau < myntau:
                ntau=myntau
            if na < myna:
                na=myna

        # plots
        all_particles=[]
        self.card.append('# basic properties of the non-b-tagged jets')
        for i in range(1,max(2,nj)+1):
            self.card.append('plot PT(j['+str(i)+']) 40 0 500 [logY]')
            self.card.append('plot ETA(j['+str(i)+']) 40 -10 10 [logY]')
            self.card.append('plot MT_MET(j[' + str(i)+ ']) 40 0 500 [logY]')
            all_particles.append('j['+str(i)+']')
        if nb!=0:
            self.card.append('# basic properties of the b-tagged jets')
            for i in range(1,nb+1):
                self.card.append('plot PT(b['+str(i)+']) 40 0 500 [logY]')
                self.card.append('plot ETA(b['+str(i)+']) 40 -10 10 [logY]')
                self.card.append('plot MT_MET(b[' + str(i)+ ']) 40 0 500 [logY]')
                all_particles.append('b['+str(i)+']')

        if (ne+nmu+ntau)>0:
            self.card.append('# basic properties of the leptons')
        for i in range(1,ne+1):
            self.card.append('plot PT(e['+str(i)+']) 40 0 500 [logY]')
            self.card.append('plot ETA(e['+str(i)+']) 40 -10 10 [logY]')
            self.card.append('plot MT_MET(e[' + str(i)+ ']) 40 0 500 [logY]')
            all_particles.append('e['+str(i)+']')
        for i in range(1,nmu+1):
            self.card.append('plot PT(mu['+str(i)+']) 40 0 500 [logY]')
            self.card.append('plot ETA(mu['+str(i)+']) 40 -10 10 [logY]')
            self.card.append('plot MT_MET(mu[' + str(i)+ ']) 40 0 500 [logY]')
            all_particles.append('mu['+str(i)+']')
        for i in range(1,ntau+1):
            self.card.append('plot PT(ta['+str(i)+']) 40 0 500 [logY]')
            self.card.append('plot ETA(ta['+str(i)+']) 40 -10 10 [logY]')
            self.card.append('plot MT_MET(ta[' + str(i)+ ']) 40 0 500 [logY]')
            all_particles.append('ta['+str(i)+']')

        if na>0:
            self.card.append('# basic properties of the photons')
        for i in range(1,na+1):
            self.card.append('plot PT(a['+str(i)+']) 40 0 500 [logY]')
            self.card.append('plot ETA(a['+str(i)+']) 40 -10 10 [logY]')
            self.card.append('plot MT_MET(a[' + str(i)+ ']) 40 0 500 [logY]')
            all_particles.append('a['+str(i)+']')

        if len(all_particles)>15:
            all_particles = [x for x in all_particles if ("1" in x) or ("2" in x) ]
        permlist = [c for i in range(1,len(all_particles)) for c in itertools.combinations(all_particles, i+1)]
        permlist.sort()
        permlist=list(permlist for permlist,_ in itertools.groupby(permlist))
        if len(permlist)>0:
            self.card.append('# Invariant-mass distributions')
            for perm in permlist:
                if len(perm)==2:
                    self.card.append('plot M('+' '.join(perm)+') 40 0  500 [logY]')
            self.card.append('# Angular distance distributions')
            for perm in permlist:
                if len(perm)==2:
                    self.card.append('plot DELTAR('+','.join(perm)+') 40 0 10 [logY]')

        # recasting
        if self.has_root and (self.has_delphes or self.has_delphesMA5tune):
            self.card.append('\n# Recasting')
            self.card.append('@MG5aMC recasting_commands')
            self.card.append('set main.recast = on')
            self.card.append('set main.recast.store_root = False')
            self.card.append('@MG5aMC recasting_card')
            self.card.append('# Uncomment the analyses to run')
            self.card.append('# Delphes cards must be located in the PAD(ForMA5tune) directory')
            self.card.append('# Switches must be on or off')
            self.card.append('# AnalysisName               PADType    Switch     DelphesCard')
            ma5dir = \
              os.path.abspath(os.path.join(os.path.dirname(os.path.realpath( __file__ )),os.pardir,os.pardir))
            if self.has_delphes:
                cpath = os.path.normpath(os.path.join(ma5dir,'PAD'))
                tmp = self.recastinfo.CreateMyCard(cpath,"PAD",False)
                tmp = ['# '+x for x in tmp]
                self.card+= tmp
            if self.has_delphesMA5tune:
                cpath = os.path.normpath(os.path.join(ma5dir,'PADForMA5tune'))
                tmp = self.recastinfo.CreateMyCard(cpath,"PADForMA5tune",False)
                tmp = ['# '+x for x in tmp]
                self.card+= tmp

        # output
        return '\n'.join(self.card)


    def get_Npart(self, prt: int | list[int], pdglist: list[int]) -> int:
        """Check whether a particle (or one of a multiparticle) belongs to a PDG-code list.

        Args:
            prt (``int | list[int]``): PDG code(s).
            pdglist (``list[int]``): PDG codes to match.

        Returns:
            ``int``:
            ``1`` if matched, ``0`` otherwise.
        """
        if isinstance(prt, list):
            for x in prt:
                if x in pdglist:
                    return 1
            return 0
        else:
            return int(prt in pdglist)


    def generate_parton_card_for_procdef(self, process: Any, interstate: list[str] = [], finalstate: list[str] = [], invisstate: list[str] = []) -> None:
        """Add the plots of one process (before and after the decays of the intermediate particles).

        Args:
            process (``Any``): MG5_aMC process definition (strings are ignored).
            interstate (``list[str]``, default ``[]``): intermediate particles.
            finalstate (``list[str]``, default ``[]``): visible final-state particles.
            invisstate (``list[str]``, default ``[]``): invisible final-state particles.
        """

        # init
        if interstate==[] and finalstate==[]:
            self.logger.debug('  >> new process')

        # checking if process is not a string
        if isinstance(process,str):
            return

        # getting the list of particles and creating the plots
        if interstate==[] and finalstate==[]:
            dummy, interstate,finalstate,invisstate = self.particles_in_process(process)
        self.logger.debug('    >> visible inter state particles: ' + str(interstate))
        self.logger.debug('    >> visible final state particles: ' + str(finalstate))
        self.logger.debug('    >> invisible final state particles: ' + str(invisstate))
        # Hard process before decay
        self.generate_plots(interstate,finalstate,invisstate)
        # Hard process after decay
        if interstate !=[]:
            interstate, finalstate, invisstate = \
               self.decay(process.get('decay_chains'),interstate,finalstate,invisstate)
            self.logger.debug('    >> visible final state particles after decay: ' + str(finalstate))
            self.logger.debug('    >> invisible final state particles after decay: ' + str(invisstate))
            self.generate_plots(interstate,finalstate,invisstate)

    def decay(self,chains: list,old_int: list[str],old_fin: list[str],old_inv: list[str]) -> tuple[list[str], list[str], list[str]]:
        """Replace the decaying particles by their decay products (recursively).

        Args:
            chains (``list``): decay chains.
            old_int (``list[str]``): intermediate particles.
            old_fin (``list[str]``): visible final-state particles.
            old_inv (``list[str]``): invisible final-state particles.

        Returns:
            ``tuple[list[str], list[str], list[str]]``:
            The updated intermediate, visible and invisible particles (the input lists are
            modified in place).
        """
        new_int, new_fin, new_inv = old_int, old_fin, old_inv
        for mydecay in chains:
            dec_init,dec_inter,dec_final,dec_inv = self.particles_in_process(mydecay)
            for x in dec_init:
                if x in new_int:
                    new_int.remove(x)
                new_inv+=dec_inv
                new_fin+=dec_final
                new_int+=dec_inter
            if new_int!=[]:
                # NOTE: typo 'newfin' (new_fin is however modified in place).
                new_int, newfin, new_inv = self.decay(mydecay.get('decay_chains'),new_int,new_fin,new_inv)
        return new_int,new_fin,new_inv

    def particles_in_process(self,process: Any) -> tuple[list[str], list[str], list[str], list[str]]:
        """Classify the legs of a process.

        Args:
            process (``Any``): MG5_aMC process definition.

        Returns:
            ``tuple[list[str], list[str], list[str], list[str]]``:
            Initial-state, intermediate (decaying), visible final-state and invisible
            final-state particle names.
        """
         # init
        initstate = []
        intstate = []
        finstate = []
        decaying_particles=[]

        # decay properties
        for mydecay in process.get('decay_chains'):
            decaying_particles.append(mydecay.get('legs')[0].get('ids'))

        for myleg in process.get('legs'):
            prts = sorted(myleg.get('ids'))
            if not myleg.get('state'):
                initstate.append(self.get_name(prts))
            elif prts in decaying_particles:
                intstate.append(self.get_name(prts))
            else:
                finstate.append(self.get_name(prts))
        invstate   = [ x for x in finstate if x in self.invisible_particles ]
        finstate = [ x for x in finstate if not x in invstate ]
        return initstate,intstate,finstate,invstate



    def generate_plots(self,interstate: list[str],finalstate: list[str],invisible: list[str]) -> None:
        """Add PT/ETA, invariant-mass, DELTAR and (with invisible particles) MT_MET plots.

        Args:
            interstate (``list[str]``): intermediate particles.
            finalstate (``list[str]``): visible final-state particles.
            invisible (``list[str]``): invisible particles.
        """
        # Formatting the inputs (tally)
        new_inter = []
        new_final = []
        for x,num in [[x,interstate.count(x)] for x in set(interstate)]:
            for i in range(num):
                new_inter.append(x+'['+str(i+1)+']')
        for x,num in [[x,finalstate.count(x)] for x in set(finalstate)]:
            for i in range(num):
                new_final.append(x+'['+str(i+1)+']')

        # properties of the final state particles
        self.card.append('# PT and ETA distributions of all particles')
        for part in new_inter:
            self.card.append('plot  PT(' + part + ') 40 0  500 [logY interstate]')
            self.card.append('plot ETA(' + part + ') 40 -10 10 [logY interstate]')
        for part in new_final:
            self.card.append('plot  PT(' + part + ') 40 0  500 [logY]')
            self.card.append('plot ETA(' + part + ') 40 -10 10 [logY]')

        # invariant mass ditributions
        tagstate = 'allstate'
        if len(interstate)==0:
            tagstate=''
        allstate = new_inter+new_final
        permlist = [c for i in range(1,len(allstate)) for c in itertools.combinations(allstate, i+1)]
        permlist.sort()
        permlist=list(permlist for permlist,_ in itertools.groupby(permlist))
        if len(permlist)>75:
            permlist = []
        if len(permlist)>0:
            self.card.append('# Invariant-mass distributions')
        for perm in permlist:
            self.card.append('plot M('+' '.join(perm)+') 40 0  500 [logY '+tagstate+']')

        # delta R of between two particles
        if len(permlist)>0:
            self.card.append('# Angular distance distributions')
        for perm in permlist:
            if len(perm)==2:
                self.card.append('plot DELTAR('+','.join(perm)+') 40 0 10 [logY '+tagstate+']')

        # MET
        if len(invisible)>0:
            self.card.append('# Invisible')
            for part in new_inter:
                self.card.append('plot MT_MET(' + part + ') 40 0  500 [logY interstate]')
            for part in new_final:
                self.card.append('plot MT_MET(' + part + ') 40 0  500 [logY]')

    # from pdf list to name
    def get_name(self,pdg: list[int]) -> str:
        """Get the name of a particle or multiparticle from its PDG code(s).

        Args:
            pdg (``list[int]``): PDG code(s).

        Raises:
            ``MadGraphInterface.MultiParts``: if no multiparticle matches the codes.

        Returns:
            ``str``:
            The (multi)particle name.
        """
        if len(pdg)==1:
            myprt =self.model.get('particle_dict')[pdg[0]]
            if myprt['is_part']:
                return myprt['name']
            else:
                return myprt['antiname']
        else:
            for key, value in six.iteritems(self.multiparticles):
                self.logger.debug('new multiparticle ' + key + ' = ' + str(value))
                if sorted(value)==sorted(pdg):
                    return key
        self.logger.error('  ** Cannot find the name associated with the pdg code list' + str(pdg))
        raise self.MultiParts("  ** Problem with the multiparticle definitions")


    # from pdg code to name
    def get_pdg_code(self,prt: str) -> int | list[int] | str:
        """Get the PDG code(s) of a particle or multiparticle name.

        Args:
            prt (``str``): name or PDG code.

        Returns:
            ``int | list[int] | str``:
            The PDG code, the list of codes of a multiparticle, or ``''`` if unknown.
        """
        try:
            # NOTE: isinstance(int(prt), int) is always True when int() succeeds.
            if isinstance( int(prt), int ):
               return int(prt)
        except:
            for key, value in six.iteritems(self.model.get('particle_dict')):
                if value['antiname']==prt and not value['is_part']:
                    return key
                elif value['name']==prt and value['is_part']:
                    return key
            if prt in list(self.multiparticles.keys()):
                return self.multiparticles[prt]
            else:
                return ''

    # adding the particle definitions
    def get_invisible(self, card_type: str = 'parton') -> None:
        """Find the invisible particles of the model (massless-width, colourless, neutral, not
        the photon) and define the ``invisible`` multiparticle (hadron cards).

        Args:
            card_type (``str``, default ``'parton'``): card type.
        """
        do_parton = card_type=='parton'
        # Do we have MET?
        for key, value in six.iteritems(self.model.get('particle_dict')):
            if value['width'] == 'ZERO' and value['color']==1 and value['charge']==0 and not value['name']=='a':
                self.invisible_particles.append(value['name'])
                self.invisible_particles.append(value['antiname'])
                self.invisible_pdgs.append(str(value['pdg_code']))
                self.invisible_pdgs.append(str(-value['pdg_code']))
        self.invisible_particles=list(set(self.invisible_particles))
        if len(self.invisible_particles)>0:
            self.card.append('# Multiparticle definition')
            if not do_parton:
                self.card.append('define invisible = ' + ' '.join(list(set(self.invisible_pdgs))))

    def write_multiparticles(self) -> None:
        """Write the invisible multiparticles and the ``invisible`` definition in the card.
        """
        for key, value in six.iteritems(self.multiparticles):
            if len([ x for x in value if x in [self.get_pdg_code(y) for y in self.invisible_particles] ])==len(value):
                self.invisible_particles.append(key)
                self.card.append('define ' + key + ' = ' + ' '.join([str(x) for x in value]))
        self.card.append('define invisible = ' + ' '.join(self.invisible_particles)+'\n')

    def get_finalstate_particles(self, process: Any) -> list:
        """Get the PDG codes of the visible final-state particles (after all decays).

        Args:
            process (``Any``): MG5_aMC process definition.

        Returns:
            ``list``:
            PDG codes (or lists of codes for multiparticles).
        """
        dummy, interstate,finalstate,invisstate = self.particles_in_process(process)
        if interstate !=[]:
            interstate, finalstate, invisstate = \
               self.decay(process.get('decay_chains'),interstate,finalstate,invisstate)
        return [self.get_pdg_code(x) for x in finalstate]
