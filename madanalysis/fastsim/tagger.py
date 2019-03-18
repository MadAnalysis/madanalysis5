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


class Tagger:

    # Initialization
    def __init__(self):
        self.rules = {}


    # Adding a rule to the tagger
    # The bounds are rewritten under the form [variable, MIN, MAX]
    # By default, all values are None and optional
    # The operators * + - / ^ log sin are supported
    def add_rule(self,id_true, id_reco, function, bounds):
        ## Checking whether the reco/true pair already exists
        key_number=len(self.rules.keys())+1
        for key, value in self.rules.items():
            if value['id_true']==id_true and value['id_reco']==id_reco:
                key_number = key
        if not key_number in self.rules.keys():
            self.rules[key_number] = { 'id_true':id_true, 'id_reco':id_reco, 'efficiencies':{} }

        ## Defining a new rule ID for an existing tagger
        efficiency_key = len(self.rules[key_number]['efficiencies'])+1

        ## Bounds under the form of { var : [min, max] }
        bounds_code, interpreted_bounds = self.format_bounds(bounds)

        if None in [bounds_code, interpreted_bounds]:
            # error message is given in boundinterpreter
            return
        # bounds_code is a string of interpreted results
        # interpreted_bounds are a dictionary {'B_NAME':[MIN,MAX],
        #                                      'B2_NAME':[MIN,MAX]}

        ## Checking the overlap between the new bounds and the existing ones
        if bounds!=['default']:
            for i_bound in interpreted_bounds.keys():
                if i_bound in self.F.observables.keys():
                    continue
                else:
                    self.logger.error('Undefined observable in the bound definition: '+i_bound)
                    self.logger.error('Currently supported observables are: '+\
                    str(self.F.variables).replace('[','').replace(']','').replace("'",""))
                    return

            intersec = True
            for ID, val in self.rules[mod].items():
                for myvar in self.F.variables:
                    if myvar not in interpreted_bounds.keys():
                        continue
                    if myvar not in val['bounds'].keys():
                        continue
                    if not (
                       (interpreted_bounds[myvar][0] > val['bounds'][myvar][1]) or \
                       (interpreted_bounds[myvar][1] < val['bounds'][myvar][0]) ):
                        continue
                    intersec = False
                    break
                if intersec:
                    break
            if intersec and key_number > 1:
                self.logger.warning('The bounds intersect with an existing rule. Ignoring...')
                return False
        else:
            if key_number > 1:
                for key in self.rules[mod]:
                    if self.rules[mod][key]['bounds'].keys() == ['default']:
                        self.logger.warning('Multiple default bound setting detected.')
                        self.logger.warning('Updating the existing one...')
                        break

        ## Checking the function
        Fx = self.F.convert(function)
        if Fx == None:
            self.logger.warning('The efficiency function is invalid. Ignoring...')
            return False
        if not self.F.tag_val_tester(interpreted_bounds, Fx):
            # no need for warning function interpreter does whatever necessary
            #self.logger.warning('The efficiency function may be above one. Ignoring...')
            return False

        # Creating the rule
        self.rules[mod][key_number]                   = {}
        self.rules[mod][key_number]['bounds'        ] = interpreted_bounds
        self.rules[mod][key_number]['bound_code'    ] = bounds_code
        self.rules[mod][key_number]['function'      ] = function
        self.rules[mod][key_number]['function_code' ] = Fx


    # Format the bounds into an appropriate format: {NAME_STR = [MIN_FLOAT, MAX_FLOAT]}
    # The method also returns a c++ code for the bounds
    def format_bounds(self, bounds):
        ## Default behavior
        if bounds == ['default']:
            return  { 'default' : [None, None] }, 'default'

        #initialize the dictionaries
        output = {}
        output['bound']      = {} # {'name':[MIN,MAX]}
        output['bound_code'] = ''

        bound_dict = {}
        for bound in bounds:  # bounds is a list of bounds
            formatted_bound = bound
            for symb in ['<','>','=']:
                formatted_bound = formatted_bound.replace(symb, ' ' + symb +' ' )
            for symb in ['<  =','>  =','=  =']:
                formatted_bound = formatted_bound.replace(symb, ''.join(symb.split())  )

            print bound
            print formatted_bound.split()
            kcontinue
            begin = -1
            end   = -1
            i = 0
            while i < len(bound):
                if i == 0 :
                    begin = i
                    i+=1
                for elem in ['<','>']:
                    if bound[i] == elem:
                        end = i
                        if len(bound_dict.keys()) == 0:
                            bound_dict[1] = bound[begin:end]
                        else:
                            bound_dict[max(bound_dict.keys())+1] = bound[begin:end]
                        if bound[i+1] == '=':
                            bound_dict[max(bound_dict.keys())+1] = bound[i:i+2]
                            begin = i+2
                        else: 
                            bound_dict[max(bound_dict.keys())+1] = bound[i]
                            begin = i+1
                        if begin == len(bound)-1:
                            bound_dict[max(bound_dict.keys())+1] = bound[begin]
                if i == len(bound)-1 and begin < i:
                    bound_dict[max(bound_dict.keys())+1] = bound[begin:len(bound)]
                i+=1


            var, opt, val = [], [], []
            for i in bound_dict.keys():
                if bound_dict[i] in self.bound_operators:
                    # opt has to be even so check if it is
                    if i not in [2,4]:
                        self.logger.error('Invalid syntax for the bound. Ignoring...')
                        return None, None
                    opt += [i]
                else:
                    try:
                        float(bound_dict[i])
                        val += [i]
                    except:
                        var += [i]
    
            ## If variable is in the beggining change it
            if var[0] == 1:
                temp_dict = {}
                if len(opt) == 1:
                    temp_dict[1] = bound_dict[val[0]]
                    if bound_dict[opt[0]] == '<':
                        temp_dict[2] = '>'
                    elif bound_dict[opt[0]] == '>':
                        temp_dict[2] = '<'
                    elif bound_dict[opt[0]] == '>=':
                        temp_dict[2] = '<='
                    elif bound_dict[opt[0]] == '<=':
                        temp_dict[2] = '>='
                    temp_dict[3] = bound_dict[var[0]]
                    for i in temp_dict.keys():
                        bound_dict[i] =  temp_dict[i]
                    temp_dict.clear()
                elif len(opt) > 1:
                    temp_dict[1] = bound_dict[val[0]]
                    if bound_dict[opt[0]] == '<':
                        temp_dict[2] = '>'
                    elif bound_dict[opt[0]] == '>':
                        temp_dict[2] = '<'
                    elif bound_dict[opt[0]] == '>=':
                        temp_dict[2] = '<='
                    elif bound_dict[opt[0]] == '<=':
                        temp_dict[2] = '>='
                    temp_dict[3] = bound_dict[var[0]]
                    if bound_dict[opt[1]] == '<':
                        temp_dict[4] = '>'
                    elif bound_dict[opt[1]] == '>':
                        temp_dict[4] = '<'
                    elif bound_dict[opt[1]] == '>=':
                        temp_dict[4] = '<='
                    elif bound_dict[opt[1]] == '<=':
                        temp_dict[4] = '>='
                    temp_dict[5] = bound_dict[val[1]]
                    for i in temp_dict.keys():
                        bound_dict[i] =  temp_dict[i]
                    temp_dict.clear()

            ##write the min and max values as floats
            for i in bound_dict.keys():
                try:
                    bound_dict[i] = float(bound_dict[i])
                except: pass
            
            if output['bound_code'] != '':
                output['bound_code'] += ' && '

            # check if same bound variable is given via 'and'
            double_count = False
            if bound_dict[3] in output['bound'].keys():
                double_count = True

            ## write the output dictionary both for code and bound check
            if len(bound_dict.keys()) == 3:
                if bound_dict[2] == '<' or bound_dict[2] == '<=':
                    output['bound_code'] += '('+str(bound_dict[1])+bound_dict[2]+\
                                    bound_dict[3]+')'
                    if not double_count:
                        output['bound'][bound_dict[3]] = [bound_dict[1],None]
                    else:
                        output['bound'][bound_dict[3]] = [bound_dict[1],\
                                           output['bound'][bound_dict[3]][1]]
                        output['bound_code']=\
                        output['bound_code'].replace(') && (',' && ')
                            
                elif bound_dict[2] == '>' or bound_dict[2] == '>=':
                    output['bound_code'] += '('+str(bound_dict[1])+bound_dict[2]+\
                                    bound_dict[3]+')'
                    if not double_count:
                        output['bound'][bound_dict[3]] = [None,bound_dict[1]]
                    else:
                        output['bound'][bound_dict[3]] = [output['bound'][bound_dict[3]][0],
                                                          bound_dict[1]]
                        output['bound_code']=\
                        output['bound_code'].replace(') && (',' && ')

            elif len(bound_dict.keys()) == 5:
                if (bound_dict[2] == '<' or bound_dict[2] == '<=') and \
                (bound_dict[4] == '<' or bound_dict[4] == '<='):
                    output['bound'][bound_dict[3]] = [bound_dict[1],bound_dict[5]]
                    output['bound_code'] += '('+str(bound_dict[1])+bound_dict[2]+\
                    bound_dict[3]+' && '+bound_dict[3]+bound_dict[4]+\
                    str(bound_dict[5])+')'
                elif (bound_dict[2] == '>' or bound_dict[2] == '>=') and \
                (bound_dict[4] == '>' or bound_dict[4] == '>='):
                    output['bound'][bound_dict[3]] = [bound_dict[5],bound_dict[1]]
                    output['bound_code'] += '('+str(bound_dict[1])+bound_dict[2]+\
                    bound_dict[3]+' && '+bound_dict[3]+bound_dict[4]+\
                    str(bound_dict[5])+')'
                elif (bound_dict[2] == '<' or bound_dict[2] == '<=') and \
                (bound_dict[4] == '>' or bound_dict[4] == '>='):
                    output['bound'][bound_dict[3]] = [bound_dict[1],bound_dict[5]]
                    output['bound_code'] += '('+str(bound_dict[1])+bound_dict[2]+\
                    bound_dict[3]+' || '+bound_dict[3]+bound_dict[4]+\
                    str(bound_dict[5])+')'
                elif (bound_dict[2] == '>' or bound_dict[2] == '>=') and \
                (bound_dict[4] == '<' or bound_dict[4] == '<='):
                    output['bound'][bound_dict[3]] = [bound_dict[1],bound_dict[5]]
                    output['bound_code'] += '('+str(bound_dict[1])+bound_dict[2]+\
                    bound_dict[3]+' || '+bound_dict[3]+bound_dict[4]+\
                    str(bound_dict[5])+')'
            bound_dict.clear()

        bound_check = False
        for bound in output['bound'].keys():
            if bound in self.F.variables or bound == 'default':
                bound_check = True
            else:
                break
        if bound_check:
            return output['bound_code'], output['bound']
        else:
            self.logger.error('Invalid observable for the bound. Ignoring...')
            return None, None
