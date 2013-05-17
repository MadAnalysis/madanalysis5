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


import logging
import madanalysis.dataset.dataset as Dataset

class DatasetCollection:

    def __init__(self):
        self.table = []

    def __len__(self):
        return len(self.table)

    def __getitem__(self,i):
        return self.table[i][1]

    def Display(self):
        logging.info(" ********* List of defined datasets *********" )
        for value in self.table:
            logging.info(" "+value[0]+" ("+value[1].GetStringTag()+")")
        logging.info(" ********************************************" )

    def Find(self,name):
        name.lower()
        for item in self.table:
            if name == item[0]:
                return True
        return False

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
