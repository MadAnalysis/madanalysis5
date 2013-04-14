################################################################################
#  
#  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <http://madanalysis.irmp.ucl.ac.be>
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


from madanalysis.enumeration.ma5_running_type import MA5RunningType
import madanalysis.observable.observable_list

class ObservableManager():

    def __init__(self,mode):

        list = madanalysis.observable.observable_list.__dict__.keys()

        # extract native list
        self.full_list      = []
        self.plot_list      = []
        self.cut_event_list = []
        self.cut_candidate_list = []

        for item in list:
            if item.startswith('__'):
                continue
            if item=="ObservableBase":
                continue

            ref = self.get(item)
            if ref.__class__.__name__!="ObservableBase":
                continue

            self.full_list.append(item)

            if mode==MA5RunningType.PARTON and ref.code_parton=="":
                continue
            elif mode==MA5RunningType.HADRON and ref.code_hadron=="":
                continue
            elif mode==MA5RunningType.RECO and ref.code_reco=="":
                continue
            self.plot_list.append(item)

            if ref.cut_event:
                self.cut_event_list.append(item)
            if ref.cut_candidate:
                self.cut_candidate_list.append(item)

        
    def get(self,name):
        if name not in \
               madanalysis.observable.observable_list.__dict__.keys():
            return None
        return madanalysis.observable.observable_list.__dict__[name]

    
    def findPlotObservable(self,obs):
        if obs in self.plot_list:
            return True
        else:
            return False

    def findCutObservable(self,obs):
        if obs in self.cut_list:
            return True
        else:
            return False
        
    def __getattr__(self, name):
        return self.get(name)
        
