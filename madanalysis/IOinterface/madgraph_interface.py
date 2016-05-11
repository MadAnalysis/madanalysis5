################################################################################
#
#  Copyright (C) 2012-2016 Eric Conte, Benjamin Fuks
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

import itertools
import logging
import os

class MadGraphInterface():

    def __init__(self):
        self.logger = logging.getLogger('MA5')
        self.model = ''
        self.multiparticles={}
        self.card=[]
        self.invisible_particles = []
        self.PID=0

    class InvalidCard(Exception):
        pass
    class MultiParts(Exception):
        pass


    def generate_card(self, MG5history, ProcessesDefinitions, ProcessesLists, card_type='parton'):
        """ Main routine allowing for the creation of ma5 cards.
            ProcessesDefinitions is a list of blocks, one block for each generate or add process command;
            ProcessesLists is a more detailed list"""

        self.logger.info('Creating an MA5 card for the mode: ' + card_type)

        self.logger.info('Getting the UFO model:')
        self.model = ProcessesLists[0][0].get('model')
        self.logger.debug('  >> ' + self.model.get('name'))
        self.get_invisible()

        self.logger.info('Getting the multiparticle definitions')
        for line in MG5history:
            if 'define' in line:
                myline = line.split()
                if myline[1]=='p':
                    continue
                mypdgs= [self.get_pdg_code(prt) for prt in myline[3:]]
                self.multiparticles[myline[1]]=sorted(sum([e if isinstance(e,list) else [e] for e in mypdgs],[]))
        self.logger.debug('  >> ' + str(self.multiparticles))
        self.logger.debug('  >> invisible: ' + str(self.invisible_particles))
        self.write_multiparticles()

        if card_type=='hadron':
            return self.generate_hadron_card(ProcessesDefinitions, ProcessesLists)
        elif card_type=='parton':
            return self.generate_parton_card(ProcessesDefinitions, ProcessesLists)
        else:
            self.logger.error('  ** Unknown card type')
            raise self.InvalidCard('Unknown card type')


    def generate_parton_card(self, ProcessesDefinitions, ProcessesLists):

        # global observables
        self.card.append('# Global event variables')
        self.card.append('plot THT   40 0 1000 [logY]')
        self.card.append('plot MET   40 0 1000 [logY]')
        self.card.append('plot SQRTS 40 0 1000 [logY]')

        # processes is a list of ProcessDefinitions
        self.logger.info('Decoding the considered process')
        for myprocdef in ProcessesDefinitions:
            self.generate_parton_card_for_procdef(myprocdef, interstate=[], finalstate=[])

        # output
        return '\n'.join(self.card)


    def generate_hadron_card(self, ProcessesDefinitions, ProcessesLists):
        return "Beware of the sluggy Valentin."


    def generate_parton_card_for_procdef(self, process, interstate=[], finalstate=[], invisiblestate=[]):
        """ Main routine for decoding the parton-level process"""

        # init
        if interstate==[] and finalstate==[]:
            self.PID+=1
            self.card.append('# Process # ' + str(self.PID))
            self.logger.debug('  >> new process')

        # getting the list of particles and creating the plots
        if interstate==[] and finalstate==[]:
            interstate,finalstate,invisiblestate = self.particles_in_process(process)
        self.logger.debug('    >> visible inter state particles: ' + str(interstate))
        self.logger.debug('    >> visible final state particles: ' + str(finalstate))
        self.logger.debug('    >> invisible final state particles: ' + str(invisiblestate))
        self.generate_plots(interstate,finalstate,invisiblestate)
##        for mydecay in process.get('decay_chains'):
##            dec_inter,dec_final,dec_inv = self.particles_in_process(mydecay)
##            interstate =dec_inter
##            finalstate+=dec_final
##            invisiblestate+=dec_inv
##            self.logger.debug('  >> decay branch')
##            self.generate_parton_card_for_procdef(mydecay,interstate=interstate,finalstate=finalstate,
##               invisiblestate=invisiblestate)

    def particles_in_process(self,process):
         # init
        instate = []
        finstate = []
        decaying_particles=[]

        # decay properties
        for mydecay in process.get('decay_chains'):
            decaying_particles.append(mydecay.get('legs')[0].get('ids'))

        for myleg in process.get('legs'):
            if not myleg.get('state'):
                continue
            prts = sorted(myleg.get('ids'))
            if prts in decaying_particles:
                instate.append(self.get_name(prts))
            else:
                finstate.append(self.get_name(prts))
        invstate   = [ x for x in finstate if x in self.invisible_particles ]
        finstate = [ x for x in finstate if not x in invstate ]
        return instate,finstate,invstate


    def generate_plots(self,interstate,finalstate,invisible):
        # properties of the final state particles
        self.card.append('# PT and ETA distributions of all particles')
        for part in interstate:
            self.card.append('plot  PT(' + part + ') 40 0 1000 [logY interstate]')
            self.card.append('plot ETA(' + part + ') 40 -10 10 [logY interstate]')
        for part in finalstate:
            self.card.append('plot  PT(' + part + ') 40 0 1000 [logY]')
            self.card.append('plot ETA(' + part + ') 40 -10 10 [logY]')

        # invariant mass ditributions
        tagstate = 'allstate'
        if len(interstate)==0:
            tagstate=''
        allstate = interstate+finalstate
        permlist = [c for i in range(1,len(allstate)) for c in itertools.combinations(allstate, i+1)]
        if len(permlist)>0:
            self.card.append('# Invariant-mass distributions')
        for perm in permlist:
            self.card.append('plot M('+' '.join(perm)+') 40 0 1000 [logY '+tagstate+']')

        # delta R of between two final state particles
        if len(permlist)>0:
            self.card.append('# Angular distance distributions')
        for perm in permlist:
            self.card.append('plot DELTAR('+','.join(perm)+') 40 0 10 [logY '+tagstate+']')

        # MET
        if len(invisible)>0:
            self.card.append('# Invisible')
            for part in interstate:
                self.card.append('plot MT_MET(' + part + ') 40 0 1000 [logY interstate]')
            for part in finalstate:
                self.card.append('plot MT_MET(' + part + ') 40 0 1000 [logY]')

    # from pdf list to name
    def get_name(self,pdg):
        if len(pdg)==1:
            myprt =self.model.get('particle_dict')[pdg[0]]
            if myprt['is_part']:
                return myprt['name']
            else:
                return myprt['antiname']
        else:
            for key, value in self.multiparticles.iteritems():
                if value==pdg:
                    return key
        self.logger.error('  ** Cannot find the name associated with the pdg code list' + str(pdg))
        raise self.MultiParts("  ** Problem with the multiparticle definitions")


    # from pdg code to name
    def get_pdg_code(self,prt):
        for key, value in self.model.get('particle_dict').iteritems():
            if value['antiname']==prt and not value['is_part']:
                return key
            elif value['name']==prt and value['is_part']:
                return key
        if prt in self.multiparticles.keys():
            return self.multiparticles[prt]
        else:
            self.logger.error("  ** Problem with the multiparticle definitions")
            raise self.MultiParts("  ** Problem with the multiparticle definitions")

    # adding the particle definitions
    def get_invisible(self):
        # Do we have MET?
        for key, value in self.model.get('particle_dict').iteritems():
            if value['width'] == 'ZERO' and value['color']==1 and value['charge']==0 and not value['name']=='a':
                self.invisible_particles.append(value['name'])
                self.invisible_particles.append(value['antiname'])
        self.invisible_particles=list(set(self.invisible_particles))
        if len(self.invisible_particles)>0:
            self.card.append('# Multiparticle definition')
            self.card.append('define invisible = ' + ' '.join(self.invisible_particles))

    def write_multiparticles(self):
        for key, value in self.multiparticles.iteritems():
            self.card.append('define ' + key + ' = ' + ' '.join([str(x) for x in value]))

