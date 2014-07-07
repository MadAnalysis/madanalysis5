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


from madanalysis.layout.histogram_frequency_core import HistogramFrequencyCore
import logging

class HistogramFrequency:

    stamp = 0

    def __init__(self):
        self.Reset()


    def Print(self):
       # Data
       self.positive.Print()
       self.negative.Print()
       self.summary.Print()


    def FinalizeReading(self,main,dataset):

        import numpy

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
        self.summary.array = numpy.array(data)

        # Integral
        self.positive.ComputeIntegral()
        self.negative.ComputeIntegral()
        self.summary.ComputeIntegral()


    def CreateHistogram(self,NPID,main):

        # New stamp
        HistogramFrequency.stamp+=1

        # Creating a new histo
        from ROOT import TH1F
        self.myhisto = TH1F(\
            self.name+"_"+str(HistogramFrequency.stamp),\
            self.name+"_"+str(HistogramFrequency.stamp),\
            len(self.summary.array),\
            0,\
            len(self.summary.array))

        # Filling bins
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
            self.myhisto.GetXaxis().SetBinLabel(bin+1,spid)

            # Setting the bin content
            self.myhisto.SetBinContent(bin+1, self.summary.array[bin])


    def Reset(self):

        import numpy

        # General info
        self.name     = ""
        self.labels   = numpy.array([])
        self.scale    = 0.

        # Data
        self.positive = HistogramFrequencyCore()
        self.negative = HistogramFrequencyCore()
        self.summary  = HistogramFrequencyCore()

        # Histogram
        self.myhisto = 0

        # warnings
        self.warnings = []




