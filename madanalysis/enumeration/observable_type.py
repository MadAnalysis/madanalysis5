################################################################################
#  
#  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://github.com/MadAnalysis/madanalysis5>
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
from madanalysis.enumeration.ma5_running_type import MA5RunningType
import math
import six

class metaclass(type):
        def __getattr__(self, name):
            if name in list(self.values.keys()):
                return list(self.values.keys()).index(name)
            else:
                return list(self.values.keys()).index('UNKNOWN')

        def accept_particles(self, index):
            name = list(self.values.keys())[index]
            return self.values[name][0]

        def convert2string(self,index):
            return list(self.values.keys())[index]

        def convert2job_string(self,index,level):
            name = list(self.values.keys())[index]
            if level==MA5RunningType.PARTON:
                return self.values[name][1]
            elif level==MA5RunningType.HADRON:
                return self.values[name][2]
            elif level==MA5RunningType.RECO:
                return self.values[name][3]
            return ""

        def convert2unit(self,index):
            name = list(self.values.keys())[index]
            return self.values[name][4]

        def convert2nbins(self,index):
            name = list(self.values.keys())[index]
            return self.values[name][5]

        def convert2xmin(self,index):
            name = list(self.values.keys())[index]
            return self.values[name][6]

        def convert2xmax(self,index):
            name = list(self.values.keys())[index]
            return self.values[name][7]

        def isCuttable(self,index):
            name = list(self.values.keys())[index]
            return self.values[name][8]

        def prefix(self,index):
            name = list(self.values.keys())[index]
            return self.values[name][9]

        def get_list(self,level=None):
            if level == None:
                level = MA5RunningType.PARTON
            output = []
            for item in self.values.keys():
                x = ObservableType.convert2job_string(list(self.values.keys()).index(item),level)
                if x=="":
                    continue
                output.append(item)
                if self.values[item][0] and self.values[item][9]:
                    output.append('s'+item)
                    output.append('v'+item)
                    output.append('sd'+item)
                    output.append('ds'+item)
                    output.append('d'+item)
                    output.append('dv'+item)
                    output.append('vd'+item)
                    output.append('r'+item)
            return output

        def get_cutlist1(self,level=None):
            if level is None:
                level = MA5RunningType.PARTON
            output = []
            for item in self.values.keys():
                if item=="N":
                    output.append(item)
                    continue
                x = ObservableType.convert2job_string(list(self.values.keys()).index(item),level)
                if x=="":
                    continue
                if not self.values[item][8]:
                    continue
                if self.values[item][0]:
                    continue
                output.append(item)
            return output

        def get_cutlist2(self,level=None):
            if level is None:
                level = MA5RunningType.PARTON
            output = []
            for item in self.values.keys():
                x = ObservableType.convert2job_string(list(self.values.keys()).index(item),level)
                if item=="N":
                    continue
                if x=="":
                    continue
                if not self.values[item][8]:
                    continue
                if not self.values[item][0]:
                    continue
                output.append(item)
                if not self.values[item][9]:
                    continue
                output.append('s'+item)
                output.append('v'+item)
                output.append('sd'+item)
                output.append('ds'+item)
                output.append('d'+item)
                output.append('dv'+item)
                output.append('vd'+item)
                output.append('r'+item)

            return output






@six.add_metaclass(metaclass)
class ObservableType(object):

    # name : accept_particles 
    values = { 'UNKNOWN' : [False,'','','','',0,0,0,False,False],\
               'SQRTS' :   [False,'PHYSICS->SqrtS(event.mc())','PHYSICS->SqrtS(event.mc())','','GeV',100,0.,1000., True, False],\
               'TET' :     [False,'PHYSICS->Transverse->EventTET(event.mc())','PHYSICS->Transverse->EventTET(event.mc())',\
                            'PHYSICS->Transverse->EventTET(event.rec())','GeV',100,0.,1000., True,False],\
               'MET' :     [False,'PHYSICS->Transverse->EventMET(event.mc())','PHYSICS->Transverse->EventMET(event.mc())',\
                            'PHYSICS->Transverse->EventMET(event.rec())','GeV',100,0.,1000., True,False],\
               'THT' :     [False,'PHYSICS->Transverse->EventTHT(event.mc())','PHYSICS->Transverse->EventTHT(event.mc())',\
                            'PHYSICS->Transverse->EventTHT(event.rec())','GeV',100,0.,1000., True,False],\
               'MHT' :     [False,'PHYSICS->Transverse->EventMHT(event.mc())','PHYSICS->Transverse->EventMHT(event.mc())',\
                            'PHYSICS->Transverse->EventMHT(event.rec())','GeV',100,0.,1000.,True,False],\
               'WEIGHTS' : [False,'PHYSICS->weights(event.mc())','PHYSICS->weights(event.mc())','','',100,0.,1., True,False],\
               'NPID':     [False,'NPID','NPID','NPID','',100,0.,100.,False,False],\
               'NAPID':    [False,'NAPID','NAPID','NAPID','',100,0.,100.,False,False],\
               'E'   :     [True,'e()','e()','e()','GeV',100,0.,1000.,True,True],\
               'M'   :     [True,'m()','m()','m()','GeV/c^{2}',100,0.,1000.,True,True],\
               'P'   :     [True,'p()','p()','p()','GeV/c',100,0.,1000.,True,True],\
               'ET'  :     [True,'et()','et()','et()','GeV',100,0.,1000.,True,True],\
               'MT'  :     [True,'mt()','mt()','mt()','GeV/c^{2}',100,0.,1000.,True,True],\
               'PT'  :     [True,'pt()','pt()','pt()','GeV/c',100,0.,1000.,True,True],\
               'PX'  :     [True,'px()','px()','px()','GeV/c',100,-1000.,1000.,True,True],\
               'PY'  :     [True,'py()','py()','py()','GeV/c',100,-1000.,1000.,True,True],\
               'PZ'  :     [True,'pz()','pz()','pz()','GeV/c',100,-1000.,1000.,True,True],\
               'R'   :     [True,'r()','r()','r()','',100,0.,1000.,True,True],\
               'THETA' :   [True,'theta()','theta()','theta()','',100,0.,2*math.pi+0.01,True,True],\
               'ETA' :     [True,'eta()','eta()','eta()','',100,-8.0,+8.0,True,True],\
               'PHI' :     [True,'phi()','phi()','phi()','',100,0.,2*math.pi+0.01,True,True],\
               'Y'   :     [True,'y()','y()','y()','',100,-8.0,+8.0,True,True],\
               'BETA' :    [True,'beta()','beta()','beta()','',100,0.,1.,True,True],\
               'GAMMA':    [True,'gamma()','gamma()','gamma()','',100,1.,1000.,True,True],\
               'N'    :    [True,'N()','N()','N()','',20,0.,20.,True,True],\
               'ISOL' :    [True,'','','isolated()','',2,0,1,True,False],\
               'HE_EE':    [True,'','','HEoverEE()','',100,0,100,True,False],\
               'NTRACKS':  [True,'','','ntracks()','',100,0,100,True,False]  }

