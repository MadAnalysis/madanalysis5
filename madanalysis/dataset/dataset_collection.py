################################################################################
#  
#  Copyright (C) 2012-2020 Jack Araz, Eric Conte & Benjamin Fuks
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


from __future__ import absolute_import
from __future__ import print_function
import logging
import madanalysis.dataset.dataset as Dataset
import six

class DatasetCollection:

    def __init__(self):
        self.table = []

    def __len__(self):
        return len(self.table)

    def __getitem__(self,i):
        return self.table[i][1]

    def Display(self):
        logging.getLogger('MA5').info(" ********* List of defined datasets *********" )
        for value in self.table:
            logging.getLogger('MA5').info(" "+value[0]+" ("+value[1].GetStringTag()+")")
        logging.getLogger('MA5').info(" ********************************************" )

    def Find(self,name):
        name.lower()
        for item in self.table:
            if name == item[0]:
                return True
        return False

    def Reset(self):
        self.table = []
    
    def Add(self,name):
        name.lower()
        if not self.Find(name):
            self.table.append([name,Dataset.Dataset(name)])

    def Get(self,name):
        name.lower()
        for item in self.table:
            if name == item[0]:
                return item[1]
        return None

    def Remove(self,name):
        name.lower()
        if self.Find(name):
            newtable = []
            for item in self.table:
                if name != item[0]:
                    newtable.append(item)
            self.table = newtable        

    def Reset(self):
        self.table = []

    def GetNames(self):
        names=[]
        for item in self.table:
            names.append(item[0])
        return names


    def LoadWithSAF(self,ast):
        # Reseting the multiparticle collection
        self.Reset()
        
        # Getting datasets branches
        datasets = ast.GetBranch("datasets",1)
        if datasets==None:
            return

        # Looping over the branches of the tree
        for key, value in six.iteritems(datasets.GetBranches()):

            # Keeping only 'dataset' branches
            if key[0]!='dataset':
                continue

            # Getting the name of the dataset (if it exists)
            name = value.GetParameterToStringWithoutQuotes('name')
            if name==None:
                logging.getLogger('MA5').error('dataset name is not found in the tree')
                continue

            # Creating a new dataset
            self.Add(name)
            dataset = self.Get(name)

            # Getting physics parameters
            physics = value.GetBranch('physics',1)
            if physics==None:
                print("ERROR: no physics branch")
                continue
            else:
                background = physics.GetParameterToBool("background")
                dataset.background = dataset.background if background==None else background
                
                weight = physics.GetParameterToFloat("weight")
                dataset.weight = dataset.weight if weight==None else weight
                
                xsection = physics.GetParameterToFloat("xsection")
                dataset.xsection = dataset.xsection if xsection==None else xsection

            # Getting layout parameters
            layout = value.GetBranch('layout',1)
            if layout==None:
                print("ERROR: no layout branch")
                continue
            else:
                title = layout.GetParameterToStringWithoutQuotes("title")
                dataset.title = dataset.title if title==None else title

                linecolor = layout.GetParameterToUInt("linecolor")
                dataset.linecolor = dataset.linecolor if linecolor==None else linecolor
                
                linestyle = layout.GetParameterToUInt("linestyle")
                dataset.linestyle = dataset.linestyle if linestyle==None else linestyle

                lineshade = layout.GetParameterToUInt("lineshade")
                dataset.lineshade = dataset.lineshade if lineshade==None else lineshade

                linewidth = layout.GetParameterToUInt("linewidth")
                dataset.linewidth = dataset.linewidth if linewidth==None else linewidth

                backcolor = layout.GetParameterToUInt("backcolor")
                dataset.backcolor = dataset.backcolor if backcolor==None else backcolor
                
                backstyle = layout.GetParameterToUInt("backstyle")
                dataset.backstyle = dataset.backstyle if backstyle==None else backstyle

                backshade = layout.GetParameterToUInt("backshade")
                dataset.backshade = dataset.backshade if backshade==None else backshade
                
                
                
                
            
        
    
