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


from madanalysis.layout.histogram_frequency_core import HistogramFrequencyCore
import logging

class HistogramFrequency:

    def __init__(self):
        self.Reset()


    def Print(self):
        # General info
        if self.ymin!=[] or self.ymax!=[]:
            logging.getLogger('MA5').info(' ' + str(self.ymin) + ' ' + str(self.ymax))

        # Data
        self.positive.Print()
        self.negative.Print()
        self.summary.Print()


    def FinalizeReading(self,main,dataset):

        # Statistics
        self.summary.nevents = self.positive.nevents + self.negative.nevents
        self.summary.entries = self.positive.entries + self.negative.entries

        # Data
        data = []
        for i in range(0,len(self.positive.array)):
            data.append(self.positive.array[i]-self.negative.array[i])
            if data[-1]<0:
                self.warnings.append(\
                    'dataset='+dataset.name+\
                    ' -> bin '+str(i)+\
                    ' has a negative content : '+\
                    str(data[-1])+'. This value is set to zero')
                data[-1]=0
        self.summary.array = data[:] # [:] -> clone of data

        # Integral
        self.positive.ComputeIntegral()
        self.negative.ComputeIntegral()
        self.summary.ComputeIntegral()


    def CreateHistogram(self,NPID,main):

        # Filling bins
        self.stringlabels = []
        for bin in range(0,len(self.labels)):

            # Looking for the good label
            pid = int(self.labels[bin])
            if NPID:
                spid = main.multiparticles.GetName(pid)
            else:
                spid = main.multiparticles.GetAName(-pid,pid)
            if spid=='':
                spid=str(pid)

            # Set labels
            self.stringlabels.append(spid)

            # Put final settings
            self.nbins = len(self.labels)
            self.xmin  = 0.
            self.xmax  = self.nbins


    def Reset(self):

        # General info
        self.name     = ""
        self.scale    = 0.
        self.nbins    = 0
        self.xmin     = 0.
        self.xmax     = 1.
        self.ymin     = []
        self.ymax     = []

        # labels
        self.labels       = [] # int: PDG id 
        self.stringlabels = [] # string: label

        # Data
        self.positive = HistogramFrequencyCore()
        self.negative = HistogramFrequencyCore()
        self.summary  = HistogramFrequencyCore()

        # warnings
        self.warnings = []

        # regions
        self.regions = []

    def GetRegions(self):
        return self.regions


    def GetBinLowEdge(self,bin):

        # Special case
        if bin<=0:
            return self.xmin

        if bin>=self.nbins:
            return self.xmax
        
        # Computing steps
        step = (self.xmax - self.xmin) / float (self.nbins)
        
        # value
        return self.xmin+bin*step


    def GetBinUpperEdge(self,bin):

        # Special case
        if bin<=0:
            return self.xmin

        if bin>=self.nbins:
            return self.xmax
        
        # Computing steps
        step = (self.xmax - self.xmin) / float (self.nbins)
        
        # value
        return self.xmin+(bin+1)*step


    def GetBinMean(self,bin):

        # Special case
        if bin<0:
            return self.xmin

        if bin>=self.nbins:
            return self.xmax
        
        # Computing steps
        step = (self.xmax - self.xmin) / float (self.nbins)
        
        # value
        return self.xmin+(bin+0.5)*step
    

    


