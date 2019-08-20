################################################################################
#  
#  Copyright (C) 2012-2018 Eric Conte, Benjamin Fuks
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


import logging
from madanalysis.fastsim.ast            import AST
from madanalysis.fastsim.tagger         import Tagger
from madanalysis.fastsim.smearer        import Smearer
from madanalysis.fastsim.recoefficiency import RecoEfficiency

class SuperFastSim:

    # Initialization
    def __init__(self):
        self.logger      = logging.getLogger('MA5')
        self.tagger      = Tagger()
        self.smearer     = Smearer()
        self.reco        = RecoEfficiency()
        self.jetrecomode = 'jets'
        self.observables = ''

    def InitObservables(self, obs_list):
        self.observables = obs_list

    # Definition of a new tagging/smearing rule
    def define(self,args,prts):
        prts.Add('c',[4])
        ## list of PDG codes associated with a a multiparticle
        def is_pdgcode(prt):
            return (prt[0] in ('-','+') and prt[1:].isdigit()) or prt.isdigit()

        ## Checking the length of the argument list
        if (args[0]=='tagger' and len(args)<5) or (args[0]=='smearer' and len(args)<3) \
           or  (args[0]=='reco_efficiency' and len(args)<3):
            self.logger.error('Not enough arguments for a tagger/smearer/reconstruction')
            prts.Remove('c', None)
            return

        ## Checking the first argument
        true_id = args[1]
        #### First, do we have either a multiparticle or a PDG code
        if not (true_id in prts.GetNames() or is_pdgcode(true_id)):
            self.logger.error('the 1st argument must be a PDG code or (multi)particle label')
            prts.Remove('c', None)
            return
        #### Second let's check if we have a multiparticle associated with a unique PDGID
        if true_id in prts.GetNames() and len(list(set([abs(x) for x in prts[true_id]])))==1:
            true_id = str(abs(prts[true_id][0]))
        #### Third, we have a number
        elif is_pdgcode(true_id):
            true_id=str(abs(int(true_id)))
        #### light jet protection
        if true_id in ['1','2','3', 'j']:
            true_id = '21'

        ## Checking the second and third arguments of a tagger
        if args[0]=='tagger':
            if args[2]!='as':
                self.logger.error('the 2nd argument must be the keyword \'as\'')
                prts.Remove('c', None)
                return
            reco_id = args[3]
            #### First, do we have either a multiparticle or a PDG code
            if not (reco_id in prts.GetNames() or is_pdgcode(reco_id)):
                self.logger.error('the 4th argument must be a PDG code or (multi)particle label')
                prts.Remove('c', None)
                return
            #### Second let's check if we have a multiparticle associated with a unique PDGID
            if reco_id in prts.GetNames() and len(list(set([abs(x) for x in prts[reco_id]])))==1:
                reco_id = str(abs(prts[reco_id][0]))
            #### Third, we have a number
            elif is_pdgcode(reco_id):
                reco_id=str(abs(int(reco_id)))
            #### light jet protection
            if reco_id in ['1','2','3', 'j']:
                reco_id = '21'
            to_decode=args[4:]

        ## Checking the second and third arguments of a smearer
        elif args[0] == 'smearer':
            if args[2]!='with':
                self.logger.error('the 2nd argument must be the keyword \'with\'')
                prts.Remove('c', None)
                return
            obs = args[3].upper()
            if not (obs in self.smearer.vars):
                self.logger.error('the 4th argument must be an observable in '+ ', '.join(self.smearer.vars))
                prts.Remove('c', None)
                return
            to_decode=args[4:]

        ## Checking the second and third arguments of a smearer
        elif args[0] == 'reco_efficiency':
            to_decode=args[2:]

        ## Getting the bounds and the function
        function, bounds = self.decode_args(to_decode)
        if function=='':
            self.logger.error('Cannot decode the function or the bounds - ' + args[0] + ' ignored.')
            prts.Remove('c', None)
            return

        ## Adding a rule to a tagger/smearer
        if args[0]=='tagger':
            self.tagger.add_rule(true_id,reco_id,function,bounds)
        elif args[0]=='smearer':
            self.smearer.add_rule(true_id,obs,function,bounds)
        elif args[0]=='reco_efficiency':
            self.reco.add_rule(true_id,function,bounds)
        prts.Remove('c', None)
        return


    # Transform the arguments passed in the interpreter in the right format
    def decode_args(self,myargs):
        # Special formating for the power operator
        args = ' '.join(myargs).replace('^',' ^ ')
        for symb in ['< =','> =','= =']:
            args = args.replace(symb, ''.join(symb.split()))
        args = args.split()

        ## To get the difference pieces of the command
        Nbracket1    = 0
        Nbracket2    = 0
        beginOptions = len(args)
        endOptions   = -1
        foundOptions = False

        ## Extraction of the arguments
        for i in range(0,len(args)):
            if args[i]=='(':
                Nbracket1+=1
            elif args[i]==')':
                Nbracket1-=1
            elif args[i] == '[':
                Nbracket2+=1
                if Nbracket1==0:
                    beginOptions = i
                    foundOptions = True
            elif args[i] == ']':
                Nbracket2-=1
                if Nbracket1==0:
                    endOptions = i

        ## Sanity
        if Nbracket1!=0:
            self.logger.error("number of opening '(' and closing ')' does not match.")
            return '', []
        if Nbracket2!=0:
            self.logger.error("number of opening '[' and closing ']' does not match.")
            return '', []

        ## Putting the bounds into an AST
        bounds = ' '.join(args[beginOptions+1:endOptions])
        if bounds in ['', ' ']:
            bounds = 'true'
        ast_bounds = AST(0, self.observables.full_list)
        ast_bounds.feed(bounds)

        ## Putting the efficiency into an AST
        efficiency = ' ' .join(args[:beginOptions])
        ast_eff = AST(1, self.observables.full_list)
        ast_eff.feed(efficiency)
        return ast_eff, ast_bounds

    # Display of a taggers/smearer
    def display(self,args):
        if args[0]=='tagger':
            self.tagger.display()
        elif args[0]=='smearer':
            self.smearer.display(self.jetrecomode)
        elif args[0]=='reco_efficiency':
            self.reco.display()
        return


