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


from madanalysis.layout.histogram_core import HistogramCore
import logging
from math import sqrt, log10, pow
import array

class HistogramLogX:

    stamp=0

    def __init__(self):
        self.Reset()


    def Print(self):

        # General info
        logging.info(self.name + ' ' + str(self.nbins) + \
                     str(self.xmin) + ' ' + str(self.xmax))

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
        self.scale = 0.

        # Data
        self.positive = HistogramCore()
        self.negative = HistogramCore()
        self.summary  = HistogramCore()

        # Histogram
        self.myhisto = 0

        # Warnings
        self.warnings = []

        
    def FinalizeReading(self,main,dataset):

        import numpy

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
        self.summary.array = numpy.array(data)

        # Integral
        self.positive.ComputeIntegral()
        self.negative.ComputeIntegral()
        self.summary.ComputeIntegral()
            

    def CreateHistogram(self):

        # New stamp
        HistogramLogX.stamp+=1

        # Logarithm binning
        step = (log10(self.xmax) - log10(self.xmin) ) / \
               float (self.nbins)
        binnings=[]
        for i in range(0,self.nbins):
            binnings.append( pow(10., log10(self.xmin)+i*step) )
        binnings.append(self.xmax)

        # Creating a new histo
        from ROOT import TH1F
        self.myhisto = TH1F(\
            self.name+"_"+str(HistogramLogX.stamp),\
            self.name+"_"+str(HistogramLogX.stamp),\
            self.nbins,\
            self.xmin,\
            self.xmax)

        # Filling bins
        self.myhisto.SetBins(self.nbins,array.array('d',binnings))
        for bin in range(0,self.nbins):
            self.myhisto.SetBinContent(bin+1, self.summary.array[bin])

