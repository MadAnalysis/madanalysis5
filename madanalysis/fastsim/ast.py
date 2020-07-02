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
import madanalysis.observable.observable_list as obs_list


class AST:

    # Initialization
    def __init__(self, id_, vars_):
        self.logger     = logging.getLogger('MA5')
        self.id         = id_
        self.leaves     = []
        self.variables  = vars_
        self.boolean     = ['true','false']
        self.unary_ops  = ['acos', 'acosh', 'asin', 'asinh', 'atan', 'atanh',
           'cos', 'cosh', 'erf', 'erfc', 'exp', 'fabs', 'gamma', 'lgamma', 'log',
           'log10', 'sin', 'sinh', 'sqrt', 'tan', 'tanh']
        self.binary2_ops = [ 'atan2', 'fmod', 'hypot',  'pow']
        self.binary1_ops = { '^':1, '+':3, '-':3, '*':2, '/':2, '<=':4 ,'>=':4,
          '<':4, '>':4, '==':4, 'and':5, 'or':5}


    # Number of leaves
    def size(self):
        return len(self.leaves)


    # Number of leaves
    def reset(self):
        self.leaves=[]

    # printing all the info on an ast
    def info(self):
        self.logger.info("ast nr. "+str(self.id))
        self.logger.info("leaves = " + str(self.size()))
        for leaf in self.leaves:
            leaf.info()

    # Main method: creating an ast from a formula
    def feed(self, formula_string):
        frml = formula_string.replace('**', ' ^ ')
        for op in self.binary1_ops.keys():
            frml = frml.replace(op, ' ' + op + ' ')
        frml = frml.replace('-', '- ')
        frml = frml.replace('  ', ' ')
        frml = frml.replace('e + ', 'e+')
        frml = frml.replace('E - ', 'e-')
        frml = frml.replace('E + ', 'e+')
        frml = frml.replace('e - ', 'e-')
#        frml = frml.replace('^   - ', '^ -')
#        frml = frml.replace('^   + ', '^ +')
        for op in [ '> =', '< =' ]:
            frml = frml.replace(op, op.replace(' ',''))
        frml = frml.replace('(  - ', '( -')
        frml = frml.split()
        frml = self.ToBasicLeaves(frml)
        while ')' in frml:
            id_end   = frml.index(')')
            id_start = [i for i,x in enumerate(frml[:id_end]) if x=='('][-1]
            if ',' in frml[id_start+1:id_end-1]:
                comma_pos   = frml.index(',')
                sub_tree1 = self.MakeConnections(frml[id_start+1:comma_pos])
                sub_tree2 = self.MakeConnections(frml[comma_pos+1:id_end])
                if sub_tree1==False or sub_tree2==False:
                    self.reset()
                    return
                del frml[id_start:id_end+1]
                frml[id_start:id_start] = sub_tree1
                frml[id_start+1:id_start+1] = sub_tree2
                sub_tree = self.MakeConnections(frml[id_start-1:id_start+2])
                if sub_tree==False:
                    self.reset()
                    return
                del frml[id_start-1:id_start+2]
                frml[id_start-1:id_start-1] = sub_tree
            else:
                sub_tree = self.MakeConnections(frml[id_start+1:id_end])
                if sub_tree==False:
                    self.reset()
                    return
                del frml[id_start:id_end+1]
                frml[id_start:id_start] = sub_tree
        frml = self.MakeConnections(frml)
        if frml==False:
            self.reset()


    # Allow to make connections between leaves
    # There is no parentheses so that we can proceed straightforwardly
    def MakeConnections(self, sub_formula):
        frml = sub_formula
        iterator_limit = 0
        while len(frml)>1:
            iterator_limit += 1
            if iterator_limit > 100:
                self.logger.error('Incorrect formula (ignoring the line): ' + str(sub_formula))
                self.info()
                return False
            reset = False
            for i in range (0,len(frml)):
                if not isinstance(frml[i], Leaf):
                    self.logger.error('Incorrect formula: '+ str(sub_formula) + \
                       ' -> Ignored')
                    return False
                if frml[i].daughters!=[]:
                    continue
                if frml[i].type in ['var', 'cst', 'bin1_op']:
                    continue
                elif frml[i].type == 'un_op':
                    if frml[i].name in self.unary_ops:
                        sub_formula[i].connect([frml[i+1]])
                        del frml[i+1]
                        reset=True
                        break
                elif frml[i].type == 'bin2_op':
                    if frml[i].name in self.binary2_ops:
                        sub_formula[i].connect([frml[i+1],frml[i+2]])
                        del frml[i+1:i+3]
                        reset=True
                        break
                else:
                   print frml[i]
                   aieaieaaie
            if reset:
                continue
            for prior in range(1,6):
                replacement_done = False
                bin_ignore = [x for x in self.binary1_ops.keys() if self.binary1_ops[x]!=prior]
                for i in range (0,len(frml)):
                    if frml[i].daughters!=[]:
                        continue
                    if frml[i].type in ['var', 'cst']:
                        continue
                    if frml[i].name in bin_ignore:
                        continue
                    if frml[i].type == 'bin1_op':
                        if frml[i].name in self.binary1_ops:
                            if i==0:
                                sub_formula[i].connect([frml[i+1]])
                                frml[i].type='un_op'
                                del frml[i+1]
                            else:
                                sub_formula[i].connect([frml[i-1],frml[i+1]])
                                del frml[i+1]
                                del frml[i-1]
                            replacement_done = True
                            break
                    else:
                       print(frml, i, '->', frml[i])
                       aieaieaaie2
                if replacement_done:
                    break
        return frml


    # replacing all constants from the formulas by leaves
    def ToBasicLeaves(self, formula):
        new_formula = []
        for elem in formula:
            # constants
            if re.match("""(?x) ^ [+-]?\ *  (  \d+ ( \.\d* )? |\.\d+ ) ([eE][+-]?\d+)? $""", elem):
                new_leaf = Leaf(self.size(), 'cst', elem, [], [])
                new_formula.append(new_leaf)
                self.leaves.append(new_leaf)
            # variables
            elif elem.upper() in self.variables and elem!='gamma':
                new_leaf = Leaf(self.size(), 'var', elem.upper(), [], [])
                new_formula.append(new_leaf)
                self.leaves.append(new_leaf)
            # unary operators
            elif elem.lower() in self.unary_ops:
                new_leaf = Leaf(self.size(), 'un_op', elem.lower(), [], [])
                new_formula.append(new_leaf)
                self.leaves.append(new_leaf)
            # unary operators
            elif elem.lower() in self.binary2_ops:
                new_leaf = Leaf(self.size(), 'bin2_op', elem.lower(), [], [])
                new_formula.append(new_leaf)
                self.leaves.append(new_leaf)
            # booleans
            elif elem.lower() in self.boolean:
                new_leaf = Leaf(self.size(), 'bool', elem.lower(), [], [])
                new_formula.append(new_leaf)
                self.leaves.append(new_leaf)
            # unary operators
            elif elem.lower() in self.binary1_ops.keys():
                new_leaf = Leaf(self.size(), 'bin1_op', elem.lower(), [], [])
                new_formula.append(new_leaf)
                self.leaves.append(new_leaf)
            # only parentheses
            else:
                new_formula.append(elem)
        return new_formula


    # getting a given leaf
    def get(self, nr):
        result = [x for x in self.leaves if x.id==nr]
        if len(result)!=1:
            self.logger('trying to access an unexisting leaf')
        return result[0]


    # Writing a string out of the AST
    def tostring(self):
        main_mother = [x for x in self.leaves if x.mother==[]]
        if len(main_mother)!=1:
            self.logger.error('Undefined AST without any identified main mother')
        return main_mother[0].write(self)


    # Writing a c++ string out of the AST
    def tocpp(self,cpp_type,name):
        main_mother = [x for x in self.leaves if x.mother==[]]
        obs = list(set([x.name for x in self.leaves if x.type=='var']))
        if len(main_mother)!=1:
            self.logger.error('Undefined AST without any identified main mother')
        if obs!=[]:
            result =  cpp_type + ' fct_'+name+'(MAdouble64 '+ ', MAdouble64 '.join(obs)+')\n'+'{\n'
            result +=  '   return ' + main_mother[0].write_cpp(self) + ';'
        else: 
            result =  cpp_type + ' fct_'+name+'()\n'+'{\n'
            if len(main_mother)!=1:
                result +=  '   return 1;'
            else:
                result +=  '   return ' + main_mother[0].write_cpp(self) + ';'
        result+='\n'+'}\n'
        return result


    # Setting the c++ in text initialization
    def tocpp_call(self,obj,name):
        obs = list(set([x.name for x in self.leaves if x.type=='var']))
        result  = ' fct_' + name + '('
        result += ', '.join([obj+'->'+obs_list.__dict__[x].code_reco for x in obs])
        result += ') '
        return result
