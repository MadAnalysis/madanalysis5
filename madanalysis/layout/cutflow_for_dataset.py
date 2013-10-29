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
from madanalysis.layout.measure               import Measure
from math                                     import sqrt


class CutFlowForDataset:

    def __init__(self,main,dataset):

        self.cuts = []
        self.initial = CutInfo()

        self.Ntotal_posweight     = 0
        self.Ntotal_negweight     = 0
        self.Nselected_posweight  = []
        self.Nrejected_posweight  = []
        self.Nselected_negweight  = []
        self.Nrejected_negweight  = []

        self.Ntotal_sumw2_posweight     = 0
        self.Ntotal_sumw2_negweight     = 0
        self.Nselected_sumw2_posweight  = []
        self.Nrejected_sumw2_posweight  = []
        self.Nselected_sumw2_negweight  = []
        self.Nrejected_sumw2_negweight  = []

        self.warnings=[]
        self.Ntotal=0
        self.Nselected=[]
        self.Nrejected=[]
        self.Ntotal_sumw2=0
        self.Nselected_sumw2=[]
        self.Nrejected_sumw2=[]
        self.eff=[]
        self.effcumu=[]

        self.main = main
        self.dataset = dataset


    def Initialize(self):

        # Preparing architecture for vectors
        self.Ntotal=Measure()
        self.Ntotal_posweight=Measure()
        self.Ntotal_negweight=Measure()
        self.Ntotal_sumw2=Measure()
        self.Ntotal_sumw2_posweight=Measure()
        self.Ntotal_sumw2_negweight=Measure()
        for iabscut in range(0,len(self.main.selection)):
            if self.main.selection[iabscut].__class__.__name__!="Cut":
                continue
            self.Nselected.append(Measure())
            self.Nrejected.append(Measure())
            self.Nselected_sumw2.append(Measure())
            self.Nrejected_sumw2.append(Measure())
            self.Nselected_posweight.append(Measure())
            self.Nrejected_posweight.append(Measure())
            self.Nselected_negweight.append(Measure())
            self.Nrejected_negweight.append(Measure())
            self.Nselected_sumw2_posweight.append(Measure())
            self.Nrejected_sumw2_posweight.append(Measure())
            self.Nselected_sumw2_negweight.append(Measure())
            self.Nrejected_sumw2_negweight.append(Measure())
            self.eff.append(Measure())
            self.effcumu.append(Measure())
            self.warnings.append([])

        # Extracting Nselected information
        for icut in range(0,len(self.Nselected)):
            self.Nselected_posweight[icut].mean=self.cuts[icut].sumw_pos
            self.Nselected_negweight[icut].mean=self.cuts[icut].sumw_neg
            self.Nselected_sumw2_posweight[icut].mean=self.cuts[icut].sumw2_pos
            self.Nselected_sumw2_negweight[icut].mean=self.cuts[icut].sumw2_neg

        # Extracting Ntotal
        self.Ntotal_posweight.mean       = self.initial.sumw_pos
        self.Ntotal_negweight.mean       = self.initial.sumw_neg
        self.Ntotal_sumw2_posweight.mean = self.initial.sumw2_pos
        self.Ntotal_sumw2_negweight.mean = self.initial.sumw2_neg

        return True


    def Calculate(self):

        # Calculating Nrejected for positive and negative weighted events
        for icut in range(0,len(self.Nselected)):
            if icut==0:
                self.Nrejected_posweight[icut].mean = self.Ntotal_posweight.mean - self.Nselected_posweight[icut].mean
                self.Nrejected_negweight[icut].mean = self.Ntotal_negweight.mean - self.Nselected_negweight[icut].mean
            else:
                self.Nrejected_posweight[icut].mean = self.Nselected_posweight[icut-1].mean - self.Nselected_posweight[icut].mean
                self.Nrejected_negweight[icut].mean = self.Nselected_negweight[icut-1].mean - self.Nselected_negweight[icut].mean

        # Combining negative & positive weight events for computing Nselected, Nrejected and Ntotal
        self.Ntotal.mean = self.Ntotal_posweight.mean - self.Ntotal_negweight.mean
        for icut in range(0,len(self.Nselected)):
            self.Nselected[icut].mean = self.Nselected_posweight[icut].mean - self.Nselected_negweight[icut].mean 
            self.Nrejected[icut].mean = self.Nrejected_posweight[icut].mean - self.Nrejected_negweight[icut].mean 

        # Checking that all numbers are positive : Nselected and Ntotal
        for icut in range(0,len(self.Nselected)):
            if self.Nselected[icut].mean<0:
                self.warnings[icut].append('The number of selected events is negative: '+\
                                           str(self.Nselected[icut].mean)+'. Set to 0.')
                self.Nselected[icut].mean=0
            if self.Nrejected[icut].mean<0:
                self.warnings[icut].append('The number of rejected events is negative: '+\
                                           str(self.Nrejected[icut].mean)+'. Set to 0.')
                self.Nrejected[icut].mean=0

        # Checking that a N cut i > N cut i+1 : Nselected and Nrejected
        for icut in range(0,len(self.Nselected)):
            if icut==0:
                if self.Nselected[icut].mean > self.Ntotal.mean:
                    self.warnings[icut].append('The number of selected events > the initial number of events : '+str(self.Nselected[icut].mean)+' > '+str(self.Ntotal.mean)+'. Set the number of selected events to 0.')
                    self.Nselected[icut].mean=0
            else:
                if self.Nselected[icut].mean > self.Nselected[icut-1].mean:
                    self.warnings[icut].append('The number of selected events > the initial number of events : '+str(self.Nselected[icut].mean)+' > '+str(self.Nselected[icut-1].mean)+'. Set the number of selected events to 0.')
                    self.Nselected[icut].mean=0
 
        # Calculating errors on Naccepted and Nrejected
        for icut in range(0,len(self.Nselected)):
            self.Nselected[icut].error = Measure.binomialNEventError(self.Nselected[icut].mean,self.Ntotal.mean)
            self.Nrejected[icut].error = Measure.binomialNEventError(self.Nrejected[icut].mean,self.Ntotal.mean)

        # efficiency calculation and its error
        for icut in range(0,len(self.Nselected)):
            
            if icut==0:
                if self.Ntotal.mean==0:
                    self.eff[icut].mean = 0
                else:                    
                    self.eff[icut].mean = float(self.Nselected[icut].mean) / \
                                                float(self.Ntotal.mean)
                self.eff[icut].error = Measure.binomialError(self.Nselected[icut].mean,self.Ntotal.mean)
            else:
                if self.Nselected[icut-1].mean==0:
                    self.eff[icut].mean = 0
                else:                    
                    self.eff[icut].mean = float(self.Nselected[icut].mean) / \
                                                float(self.Nselected[icut-1].mean)
                self.eff[icut].error = Measure.binomialError(self.Nselected[icut].mean,self.Nselected[icut-1].mean)
 
            if self.Ntotal.mean==0:
                self.effcumu[icut].mean=0
            else:
                self.effcumu[icut].mean = float(self.Nselected[icut].mean) / \
                                                float(self.Ntotal.mean)
            self.effcumu[icut].error = Measure.binomialError(self.Nselected[icut].mean,self.Ntotal.mean)

        # Getting xsection
        xsection = self.dataset.measured_global.xsection
        xerror  = self.dataset.measured_global.xerror
        if self.dataset.xsection!=0.:
            xsection=self.dataset.xsection
            xerror=0

        # Saving ntotal
        ntot = 0.+self.Ntotal.mean

        # Scaling Ntotal
        if self.main.normalize == NormalizeType.LUMI:
            self.Ntotal.error = xerror * self.main.lumi * 1000
            self.Ntotal.mean = xsection * self.main.lumi * 1000
        elif self.main.normalize == NormalizeType.LUMI_WEIGHT:
            self.Ntotal.error = xerror * self.main.lumi * 1000 * \
                                      self.dataset.weight
            self.Ntotal.mean = xsection * self.main.lumi * 1000 * \
                                      self.dataset.weight

        # Scaling Nselected
        for icut in range(0,len(self.Nselected)):
            if self.main.normalize == NormalizeType.LUMI:

                # error due to xsec 
                if ntot!=0:
                    errXsec = xerror * self.main.lumi * 1000 * self.Nselected[icut].mean / ntot
                else:
                    errXsec=0
                
                # scale factor
                if ntot!=0:
                    factor = xsection * self.main.lumi * 1000 / ntot
                else:
                    factor = 0 
                self.Nselected[icut].mean  *= factor
                self.Nselected[icut].error = Measure.binomialNEventError(self.Nselected[icut].mean,self.Ntotal.mean)

                # compute final error
                self.Nselected[icut].error = sqrt(\
                    self.Nselected[icut].error**2 + errXsec**2)
                
            elif self.main.normalize == NormalizeType.LUMI_WEIGHT:

                # error due to xsec 
                if ntot!=0:
                    errXsec = xerror * self.main.lumi * 1000 * self.dataset.weight * self.Nselected[icut].mean / ntot
                else:
                    errXsec=0
                
                # scale factor
                if ntot!=0:
                    factor = xsection * self.main.lumi * self.dataset.weight * 1000 / ntot
                else:
                    factor = 0 
                self.Nselected[icut].mean  *= factor
                self.Nselected[icut].error = Measure.binomialNEventError(self.Nselected[icut].mean,self.Ntotal.mean)
                
                # compute final error
                self.Nselected[icut].error = sqrt(\
                    self.Nselected[icut].error**2 + errXsec**2)

        # Scaling Nrejected
        for icut in range(0,len(self.Nrejected)):

            if self.main.normalize == NormalizeType.LUMI:

                # error due to xsec 
                if ntot!=0:
                    errXsec = xerror * self.main.lumi * 1000 * self.Nrejected[icut].mean / ntot
                else:
                    errXsec=0
                
                # scale factor
                if ntot!=0:
                    factor = xsection * self.main.lumi * 1000 / ntot
                else:
                    factor = 0 
                self.Nrejected[icut].mean  *= factor
                self.Nrejected[icut].error = Measure.binomialNEventError(self.Nrejected[icut].mean,self.Ntotal.mean)

                # compute final error
                self.Nrejected[icut].error = sqrt(\
                    self.Nrejected[icut].error**2 + errXsec**2)
                
            elif self.main.normalize == NormalizeType.LUMI_WEIGHT:

                # error due to xsec 
                if ntot!=0:
                    errXsec = xerror * self.main.lumi * 1000 * self.dataset.weight * self.Nrejected[icut].mean / ntot
                else:
                    errXsec=0
                
                # scale factor
                if ntot!=0:
                    factor = xsection * self.main.lumi * self.dataset.weight * 1000 / ntot
                else:
                    factor = 0 
                self.Nrejected[icut].mean  *= factor
                self.Nrejected[icut].error = Measure.binomialNEventError(self.Nrejected[icut].mean,self.Ntotal.mean)

                # compute final error
                self.Nrejected[icut].error = sqrt(\
                    self.Nrejected[icut].error**2 + errXsec**2)

        # recompute error to efficiency
        for icut in range(0,len(self.Nselected)):
            
            if icut==0:
                self.eff[icut].error = Measure.binomialError(self.Nselected[icut].mean,self.Ntotal.mean)
            else:
                self.eff[icut].error = Measure.binomialError(self.Nselected[icut].mean,self.Nselected[icut-1].mean)
            self.effcumu[icut].error = Measure.binomialError(self.Nselected[icut].mean,self.Ntotal.mean)
