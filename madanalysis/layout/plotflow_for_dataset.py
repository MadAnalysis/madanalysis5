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


from madanalysis.enumeration.uncertainty_type     import UncertaintyType
from madanalysis.enumeration.normalize_type       import NormalizeType
from madanalysis.layout.root_config               import RootConfig
from madanalysis.enumeration.report_format_type   import ReportFormatType
from madanalysis.enumeration.observable_type      import ObservableType
from madanalysis.enumeration.color_type           import ColorType
from madanalysis.enumeration.linestyle_type       import LineStyleType
from madanalysis.enumeration.backstyle_type       import BackStyleType
from madanalysis.enumeration.stacking_method_type import StackingMethodType
import copy


class PlotFlowForDataset:

    def __init__(self,main,dataset):
        self.histos  = []
        self.main    = main
        self.dataset = dataset

        # Getting xsection
        self.xsection = self.dataset.measured_global.xsection
        if self.dataset.xsection!=0.:
            self.xsection = self.dataset.xsection


    def __len__(self):
        return len(self.histos)


    def __getitem__(self,i):
        return self.histos[i]


    # Computing integral
    def FinalizeReading(self):

        for histo in self.histos:
            histo.FinalizeReading(self.main,self.dataset)

            
    # Computing integral
    def CreateHistogram(self):

        iplot=0

        # Loop over plot
        for iabshisto in range(0,len(self.main.selection)):

            # Keep only histogram
            if self.main.selection[iabshisto].__class__.__name__!="Histogram":
                continue

            # Case of histogram frequency
            if self.histos[iplot].__class__.__name__=="HistogramFrequency":
                if self.main.selection[iabshisto].observable.name=="NPID":
                    NPID=True
                else:
                    NPID=False
                self.histos[iplot].CreateHistogram(NPID,self.main)
            else:
                self.histos[iplot].CreateHistogram()
            iplot+=1


    # Computing scales
    def ComputeScale(self):

        iplot=0

        # Loop over plot
        for iabshisto in range(0,len(self.main.selection)):

            # Keep only histogram
            if self.main.selection[iabshisto].__class__.__name__!="Histogram":
                continue

            # Reset scale
            scale=0.

            # Case 1: Normalization to ONE
            if self.main.selection[iabshisto].stack==StackingMethodType.NORMALIZE2ONE or \
              (self.main.stack==StackingMethodType.NORMALIZE2ONE and \
               self.main.selection[iabshisto].stack==StackingMethodType.AUTO):
                integral=self.histos[iplot].positive.integral -\
                         self.histos[iplot].negative.integral
                if integral>0.:
                    scale = 1./integral
                else:
                    scale = 0.

            # Case 2: No normalization
            elif self.main.normalize == NormalizeType.NONE:
                scale = 1.

            # Case 3: Normalization formula depends on LUMI
            elif self.main.normalize == NormalizeType.LUMI:
                if not self.dataset.weighted_events:
                    scale = self.xsection * self.main.lumi * 1000 / \
                            float(self.dataset.measured_global.nevents)
                else:
                    scale = self.main.lumi * 1000 / \
                            len(self.dataset.filenames)

            # Case 4: Normalization formula depends on WEIGHT + LUMI
            elif self.main.normalize == NormalizeType.LUMI_WEIGHT:
                if not self.dataset.weighted_events:
                    scale = self.xsection * self.main.lumi * 1000 * \
                            self.dataset.weight / \
                            float(self.dataset.measured_global.nevents)
                else:
                    scale = self.main.lumi * 1000 * \
                            self.dataset.weight / \
                            len(self.dataset.filenames)

            # Setting the computing scale
            self.histos[iplot].scale=copy.copy(scale)

            # Incrementing counter
            iplot+=1

