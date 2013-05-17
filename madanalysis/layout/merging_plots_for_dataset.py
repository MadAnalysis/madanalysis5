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


from madanalysis.enumeration.uncertainty_type     import UncertaintyType
from madanalysis.enumeration.normalize_type       import NormalizeType
from madanalysis.layout.root_config               import RootConfig
from madanalysis.enumeration.report_format_type   import ReportFormatType
from madanalysis.enumeration.observable_type      import ObservableType
from madanalysis.enumeration.color_type           import ColorType
from madanalysis.enumeration.linestyle_type       import LineStyleType
from madanalysis.enumeration.backstyle_type       import BackStyleType
from madanalysis.enumeration.stacking_method_type import StackingMethodType
from madanalysis.layout.root_config               import RootConfig
from math import sqrt


class MergingPlotsForDataset:

    def __init__(self,main,dataset):
        self.dataset = dataset
        self.main    = main
        self.histos  = []

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

        for histo in self.histos:
            histo.CreateHistogram()



