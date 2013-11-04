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


from madanalysis.enumeration.uncertainty_type import UncertaintyType
from madanalysis.enumeration.normalize_type   import NormalizeType
from madanalysis.IOinterface.job_reader       import JobReader
from madanalysis.layout.cut_info              import CutInfo
from madanalysis.layout.cutflow_for_dataset   import CutFlowForDataset
from madanalysis.layout.measure               import Measure
from math                                     import sqrt

                           
class CutFlow:

    def __init__(self,main):
        self.main         = main
        self.detail       = []
        self.isSignal     = False
        self.isBackground = False
        for i in range(0,len(main.datasets)):
            self.detail.append(CutFlowForDataset(main,main.datasets[i]))
            if main.datasets[i].background:
                self.isBackground=True
            else:
                self.isSignal=True
        self.background = CutFlowForDataset(main,0)
        self.signal     = CutFlowForDataset(main,0)
        self.formulaSBratio()

    def Initialize(self):

        # Initialize cut list for each dataset
        for item in self.detail:
            item.Initialize()
            item.Calculate()

        # Initialize B/S
        self.CalculateSummary(self.signal,background=False)
        self.CalculateSummary(self.background,background=True)
            

    def calculateBSratio(self,B,eB,S,eS):
        value = Measure()
        value.mean  = self.Mformula.Eval(S,B)
        value.error = self.Eformula.Eval(S,B,eS,eB) 
        return value


    def formulaSBratio(self):
        from ROOT import TFormula
        text = self.main.SBratio
        text = text.replace("S","x")
        text = text.replace("B","y")
        self.Mformula = TFormula("SBratio",text)
        text = self.main.SBerror
        text = text.replace("ES","z")
        text = text.replace("EB","t")
        text = text.replace("S","x")
        text = text.replace("B","y")
        self.Eformula = TFormula("SBerror",text)
    
    def CalculateSummary(self,summary,background=True):

        # Ntotal
        summary.Ntotal=Measure()
        for i in range(0,len(self.detail)):
            if background!=self.main.datasets[i].background:
                continue
            summary.Ntotal.mean  += self.detail[i].Ntotal.mean
            summary.Ntotal.error += self.detail[i].Ntotal.error**2
        summary.Ntotal.error = sqrt(summary.Ntotal.error)

        # Prepare vectors
        for i in range(0,len(self.detail[0].Nselected)):
            summary.Nselected.append(Measure())
            summary.Nrejected.append(Measure())
            summary.eff.append(Measure())
            summary.effcumu.append(Measure())

        # Fill selected and rejected
        for iset in range (0,len(self.detail)):

            if background!=self.main.datasets[iset].background:
                continue
            
            for icut in range (0,len(self.detail[iset].Nselected)):
                summary.Nselected[icut].mean  += self.detail[iset].Nselected[icut].mean
                summary.Nrejected[icut].mean  += self.detail[iset].Nrejected[icut].mean
                summary.Nselected[icut].error += self.detail[iset].Nselected[icut].error**2
                summary.Nrejected[icut].error += self.detail[iset].Nrejected[icut].error**2
        for icut in range (0,len(self.detail[0].Nselected)):
            summary.Nselected[icut].error = sqrt(summary.Nselected[icut].error)
            summary.Nrejected[icut].error = sqrt(summary.Nrejected[icut].error)

        # Compute efficiencies
        for i in range(0,len(summary.eff)):
            if summary.Ntotal.mean!=0:
                summary.effcumu[i].mean=float(summary.Nselected[i].mean)/float(summary.Ntotal.mean)
                summary.effcumu[i].error=Measure.binomialError(summary.Nselected[i].mean,summary.Ntotal.mean)
            if i==0:
                if summary.Ntotal.mean!=0:
                    summary.eff[i].mean=float(summary.Nselected[i].mean)/float(summary.Ntotal.mean)
                    summary.eff[i].error=Measure.binomialError(summary.Nselected[i].mean,summary.Ntotal.mean)
            else:
                if summary.Nselected[i-1].mean!=0:
                    summary.eff[i].mean=float(summary.Nselected[i].mean)/float(summary.Nselected[i-1].mean)
                    summary.eff[i].error=Measure.binomialError(summary.Nselected[i].mean,summary.Nselected[i-1].mean)
