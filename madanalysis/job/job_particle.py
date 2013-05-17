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


from madanalysis.enumeration.argument_type    import ArgumentType
from madanalysis.selection.instance_name      import InstanceName
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.observable.observable_base   import ObservableBase
import copy
import logging
import sys

def GetParticles(main):

    # Getting list of particle/multiparticle
    part_list   = []
    option_list = []
    for item in main.selection.table:

        # Histogram case
        if item.__class__.__name__=='Histogram':
            ExtractPart(item.arguments,\
                        item.observable,\
                        part_list,\
                        option_list,\
                        [item.rank,item.statuscode] )

        # Cut case
        if item.__class__.__name__=='Cut':

            partType=None

            # Loop over candidates to reject
            for combination in item.part:
                partType=ArgumentType.COMBINATION

                # Loop over the particles
                for part in combination:
                    part_list.append(part)
                    option_list.append([item.rank,item.statuscode])

            # Loop over canidates in condition
            LoopOverConditions(partType,\
                               item.conditions,\
                               part_list,option_list,\
                               [item.rank, item.statuscode])

    # For particle selected by a PT rank
    # adding particle without PTrank
    newpart_list = []
    newoption_list = []
    for i in range(len(part_list)):
        if part_list[i].PTrank!=0:
            newpart = copy.copy(part_list[i])
            newpart.PTrank=0
            newpart_list.append(newpart)
            newoption_list.append(["PTordering",option_list[i][1]])
    part_list.extend(newpart_list)
    option_list.extend(option_list)
                
            
    # Removing double counted
    final_list=[]
    for i in range(len(part_list)):
        doublon=False
        for j in range(i+1,len(part_list)):
            if part_list[i]==part_list[j] and \
               part_list[i].PTrank==part_list[j].PTrank and \
               part_list[i].mumPart==part_list[j].mumPart and \
               option_list[i]==option_list[j] :
                doublon=True
        if not doublon:
            final_list.append([part_list[i], option_list[i][0], option_list[i][1]])

    # End
    return final_list


def ExtractPart(args,obs,part_list,option_list,option):

    # Loop over arguments of the observable
    for iarg in range(len(args)):

        # Checking argument = particle or combination
        if obs.args[iarg] not in [ArgumentType.PARTICLE,\
                                  ArgumentType.COMBINATION]:
            continue

        # Loop over the vector of combination
        for combination in args[iarg]:

            # Loop over the particles
            for part in combination:
                part_list.append(part)
                option_list.append(option)
                
    
    
def LoopOverConditions(partType,current,part_list,option_list,option):

    i=0
    while i<len(current.sequence):
        if current.sequence[i].__class__.__name__=="ConditionType":
            if partType==None:
                obs=current.sequence[i].observable
            else:
                obs=ObservableBase.Clone(current.sequence[i].observable,\
                                    args=current.sequence[i].observable.args[1:])
            ExtractPart(current.sequence[i].parts,\
                        obs,\
                        part_list, option_list, option)
        elif current.sequence[i].__class__.__name__=="ConditionSequence":
            LoopOverConditions(partType, current.sequence[i],\
                               part_list,option_list,\
                               option)
        i+=1
    

