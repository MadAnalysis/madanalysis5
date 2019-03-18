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
from madanalysis.fastsim.tagger import Tagger

class SuperFastSim:

    # Initialization
    def __init__(self):
        self.logger      = logging.getLogger('MA5')
        self.tagger      = Tagger()
        self.smearer     = Tagger()
        self.observables = ''

    def InitObservables(self, obs_list):
        self.observables = obs_list

    # Definition of a new tagging/smearing rule
    def define(self,args,prts):
        def is_pdgcode(prt):
            return (prt[0] in ('-','+') and prt[1:].isdigit()) or prt.isdigit()

        ## Checking the length of the argument list
        if (args[0]=='tagger' and len(args)<5) or (args[0]=='smearer' and len(args)<3):
            self.logger.error('Not enough arguments for a tagger/smnearer')
            return

        ## Chercking the first argument
        true_id = args[1]
        if not (true_id in prts or is_pdgcode(true_id)):
            self.logger.error('the 1st argument must be a PDG code or (multi)particle label')
            return

        ## Chercking the second and third arguments of a tagger
        if args[0]=='tagger':
            if args[2]!='as':
                self.logger.error('the 2nd argument must be the keyword \'as\'')
                return

            reco_id = args[3]
            if not (reco_id in prts or is_pdgcode(reco_id)):
                self.logger.error('the 4th argument must be a PDG code or (multi)particle label')
                return
            to_decode=args[4:]
        else:
            reco_id = true_id
            to_decode=args[2:]

        ## Getting the bounds and the function
        function, bounds = self.decode_args(to_decode)
        if function == '':
            self.logger.error('cannot decode the function')
            return

        ## Adding a rule to a tagger/smearer
        if args[0]=='tagger':
            self.tagger.add_rule(true_id,reco_id,function,bounds)

        self.logger.warning('we need to implement what todo with a rulefor a tagger/smearer')
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

        ## Main function
        function = ''
        for elem in args[:beginOptions]:
            function += elem.upper()

        ## Putting the bounds in an AST
        ## To AST: creating an empty ast
        from madanalysis.fastsim.ast import AST
        ast_id     = 0
        ast_bounds = AST(ast_id, self.observables.full_list)

        ## feeding the AST with the bound formula
        bounds = ' '.join(args[beginOptions+1:endOptions])
        ast_bounds.feed(bounds)
        bla


        from madanalysis.fastsim.ast_leaf import Leaf
        import re
        binary_ops_list = ['atan2', 'copysign', 'fmod', 'hypot', 'ldexp', 'pow', '^', '+',
          '-', '*', '/', '<' ,'>', '<=', '>=', '==']
        formatted_bounds = []

        ## To AST: Observables and constants to leafs

        print (' new bounds =', formatted_bounds)

        ## Getting the operator structure
        ast_id+=1
        ast_bounds.info()

        for i in range(2,len(formatted_bounds)):
            if isinstance(formatted_bounds[i],Leaf) and isinstance(formatted_bounds[i-1],Leaf) and\
              isinstance(formatted_bounds[i-2],Leaf):
                ast_bounds.ToAST(formatted_bounds[i-2],formatted_bounds[i],formatted_bounds[i-1])

        ast_bounds.info()

        vla
        bounds   = []
        current_bound = ''
        for elem in args[beginOptions+1:endOptions]:
            if elem.upper() != 'AND':
                current_bound += elem.upper()
            else:
                bounds.append(current_bound)
                current_bound = ''
        if current_bound!='':
            bounds.append(current_bound)
        if bounds == []:
            bounds = ['default']

        ## Return
        return function, bounds


    # Display of a taggers/smearer
    def display(self,args):
        logging.getLogger('MA5').warning('we need to implement how to display a tagger/smearer')
        return





