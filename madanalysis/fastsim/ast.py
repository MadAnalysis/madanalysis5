################################################################################
#  
#  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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
import re
from madanalysis.fastsim.ast_leaf import Leaf

class AST:

    # Initialization
    def __init__(self, id_, vars_):
        self.logger    = logging.getLogger('MA5')
        self.id        = id_
        self.leaves    = []
        self.variables = vars_
        self.unary_ops = ['acos', 'acosh', 'asin', 'asinh', 'atan', 'atanh',
           'cos', 'cosh', 'erf', 'erfc', 'exp', 'fabs', 'gamma', 'lgamma', 'log',
           'log10', 'sin', 'sinh', 'sqrt', 'tan', 'tanh']

    def size(self):
        return len(self.leaves)

    # printing all the info on an ast
    def info(self):
        self.logger.info("ast nr. "+str(self.id))
        self.logger.info("leaves = " + str(self.size()))
        for leaf in self.leaves:
            leaf.info()

    # Main method: creating an ast from a formula
    def feed(self, formula_string):
        frml = formula_string.split()
        frml = self.ToBasicLeaves(frml)
        print 'init formula = ', frml
        while ')' in frml:
            id_end   = frml.index(')')
            id_start = [i for i,x in enumerate(frml[:id_end]) if x=='('][-1]
            print "ooooh", id_start, id_end
            print frml[id_start+1:id_end-1]
            sub_tree = self.MakeConnections(frml[id_start+1:id_end])
            del frml[id_start:id_end+1]
            frml.insert(id_start, sub_tree)
        print "cleaned; finalizing", frml
        frml = self.MakeConnections(frml)
        print "finalized", frml
        bla


    # Allow to make connections between leaves
    # There is no parentheses so that we can proceed straightforwardly
    def MakeConnections(self, sub_formula):
        frml = sub_formula
        print 'To connect', frml
        while len(frml)>1:
            for i in range (0,len(frml)):
                print i, frml[i]
                print " --> ", frml[i].type
                if frml[i].type == 'un_op':
                    if frml[i].name in self.unary_ops:
                        print frml[i], frml[i+1]
                        blablabla
        return frml


    # replacing all constants from the formulas by leaves
    def ToBasicLeaves(self, formula):
        new_formula = []
        for elem in formula:
            # variables
            if elem.upper() in self.variables:
                new_leaf = Leaf(self.size(), 'var', elem.upper(), [], [])
                new_formula.append(new_leaf)
                self.leaves.append(new_leaf)
            # unary operators
            elif elem.lower() in self.unary_ops:
                new_leaf = Leaf(self.size(), 'un_op', elem.lower(), [], [])
                new_formula.append(new_leaf)
                self.leaves.append(new_leaf)
            # real numbers and integers
            elif re.match("^\d+?\.\d+?$", elem):
                new_leaf = Leaf(self.size(), 'cst', elem, [], [])
                new_formula.append(new_leaf)
                self.leaves.append(new_leaf)
            elif elem.isdigit() or (elem[0]=='-' and elem[1:].isdigit()):
                new_leaf = Leaf(self.size(), 'cst', elem, [], [])
                new_formula.append(new_leaf)
                self.leaves.append(new_leaf)
            # only parentheses
            else:
                new_formula.append(elem)
        return new_formula

    # transforming an op into an ast
    def ToAST(self, leaf1, leaf2, bin_op):
        for leaf in [leaf1, leaf2, bin_op]:
            if not isinstance(leaf,Leaf):
                self.logger.error('AST build: The element '+str(leaf)+' is not a Leaf')
                return
        if bin_op.daughters!=[]:
            self.logger.error('AST build: The operator '+str(op)+' is connected to leaves')
            return
        for leaf in [leaf1, leaf2]:
            if leaf.mother!=[]:
                self.logger.error('AST build: The leaf '+str(leaf)+' is connected to an operator')
                return
        bin_op.daughters = [leaf1.id, leaf2.id]
        leaf1.mother = [bin_op.id]
        leaf2.mother = [bin_op.id]
        for leaf in [leaf1, leaf2, bin_op]:
            self.leaves.append(leaf)

