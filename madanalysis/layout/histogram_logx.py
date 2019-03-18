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


from madanalysis.layout.histogram_core import HistogramCore
import logging
from math import sqrt, log10, pow
import array

class HistogramLogX:

    def __init__(self):
        self.Reset()


    def Print(self):

        # General info
        inform = self.name + ' ' + str(self.nbins) + str(self.xmin) + ' ' + str(self.xmax)
        if self.ymin!=[] or self.ymax!=[]:
           inform = inform + ' ' + str(self.ymin) + ' ' + str(self.ymax)
        logging.getLogger('MA5').info(inform)

        # Data
        self.positive.Print()
        self.negative.Print()
        self.summary.Print()


    def Reset(self):

        # General info
        self.name  = ""
        self.nbins = 100
        self.xmin  = 0.
        self.xmax  = 100.
        self.ymin  = []
        self.ymax  = []
        self.scale = 0.

        # Data
        self.positive = HistogramCore()
        self.negative = HistogramCore()
        self.summary  = HistogramCore()

        # Warnings
        self.warnings = []

        # regions
        self.regions = []

    def GetRegions(self):
        return self.regions

    def FinalizeReading(self,main,dataset):

        # Statistics
        self.summary.nevents   = self.positive.nevents   + self.negative.nevents
        self.summary.nentries   = self.positive.nentries   + self.negative.nentries
        self.summary.sumw      = self.positive.sumw      - self.negative.sumw
        if self.summary.sumw<0:
            self.summary.sumw=0
        self.summary.sumw2     = self.positive.sumw2     - self.negative.sumw2
        if self.summary.sumw2<0:
            self.summary.sumw2=0
        self.summary.sumwx     = self.positive.sumwx     - self.negative.sumwx
        if self.summary.sumwx<0:
            self.summary.sumwx=0
        self.summary.sumw2x    = self.positive.sumw2x    - self.negative.sumw2x
        if self.summary.sumw2x<0:
            self.summary.sumw2x=0
        self.summary.underflow = self.positive.underflow - self.negative.underflow
        if self.summary.underflow<0:
            self.summary.underflow=0
        self.summary.overflow  = self.positive.overflow  - self.negative.overflow
        if self.summary.overflow<0:
            self.summary.overflow=0
            
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
            

    def CreateHistogram(self):

        # Logarithm binning
        step = (log10(self.xmax) - log10(self.xmin) ) / \
               float (self.nbins)
        binnings=[]
        for i in range(0,self.nbins):
            binnings.append( pow(10., log10(self.xmin)+i*step) )
        binnings.append(self.xmax)



    def GetBinLowEdge(self,bin):

        # Special case
        if bin<=0:
            return self.xmin

        if bin>=self.nbins:
            return self.xmax
        
        # Computing steps
        step = (log10(self.xmax) - log10(self.xmin) ) / \
               float (self.nbins)
        
        # value
        return pow(10., log10(self.xmin)+bin*step)


    def GetBinUpperEdge(self,bin):

        # Special case
        if bin<=0:
            return self.xmin

        if bin>=self.nbins:
            return self.xmax
        
        # Computing steps
        step = (log10(self.xmax) - log10(self.xmin) ) / \
               float (self.nbins)
        
        # value
        return pow(10., log10(self.xmin)+(bin+1)*step)


    def GetBinMean(self,bin):

        # Special case
        if bin<=0:
            return self.xmin

        if bin>=self.nbins:
            return self.xmax
        
        # Computing steps
        step = (log10(self.xmax) - log10(self.xmin) ) / \
               float (self.nbins)
        
        # value
        return pow(10., log10(self.xmin)+(bin+0.5)*step)
