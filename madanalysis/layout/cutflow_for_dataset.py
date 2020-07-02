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
        nregions = len(main.regions.GetNames())
        if nregions == 0:
            nregions=1

        self.Ntotal_posweight     = 0
        self.Ntotal_negweight     = 0
        self.Nselected_posweight  = [[] for i in range(nregions) ]
        self.Nrejected_posweight  = [[] for i in range(nregions) ]
        self.Nselected_negweight  = [[] for i in range(nregions) ]
        self.Nrejected_negweight  = [[] for i in range(nregions) ]

        self.Ntotal_sumw2_posweight     = 0
        self.Ntotal_sumw2_negweight     = 0
        self.Nselected_sumw2_posweight  = [[] for i in range(nregions) ]
        self.Nrejected_sumw2_posweight  = [[] for i in range(nregions) ]
        self.Nselected_sumw2_negweight  = [[] for i in range(nregions) ]
        self.Nrejected_sumw2_negweight  = [[] for i in range(nregions) ]

        self.warnings        = [[] for i in range(nregions) ]
        self.Ntotal          = 0
        self.Nselected       = [[] for i in range(nregions) ]
        self.Nrejected       = [[] for i in range(nregions) ]
        self.Ntotal_sumw2    = 0
        self.Nselected_sumw2 = [[] for i in range(nregions) ]
        self.Nrejected_sumw2 = [[] for i in range(nregions) ]
        self.eff             = [[] for i in range(nregions) ]
        self.effcumu         = [[] for i in range(nregions) ]

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
        myregs = self.main.regions.GetNames()
        myregs.sort()
        if myregs == []:
            myregs = ['myregion']
        for iabscut in range(0,len(self.main.selection)):
            if self.main.selection[iabscut].__class__.__name__!="Cut":
                continue
            if len(self.main.selection[iabscut].part)!=0:
                continue
            ireg=0
            for reg in myregs:
                if (reg in self.main.selection[iabscut].regions) or (self.main.regions.GetNames()==[] and reg=="myregion"):
                    self.Nselected[ireg].append(Measure())
                    self.Nrejected[ireg].append(Measure())
                    self.Nselected_sumw2[ireg].append(Measure())
                    self.Nrejected_sumw2[ireg].append(Measure())
                    self.Nselected_posweight[ireg].append(Measure())
                    self.Nrejected_posweight[ireg].append(Measure())
                    self.Nselected_negweight[ireg].append(Measure())
                    self.Nrejected_negweight[ireg].append(Measure())
                    self.Nselected_sumw2_posweight[ireg].append(Measure())
                    self.Nrejected_sumw2_posweight[ireg].append(Measure())
                    self.Nselected_sumw2_negweight[ireg].append(Measure())
                    self.Nrejected_sumw2_negweight[ireg].append(Measure())
                    self.eff[ireg].append(Measure())
                    self.effcumu[ireg].append(Measure())
                    self.warnings[ireg].append([])
                ireg+=1

        # Extracting Nselected information
        for reg in range(len(myregs)):
            for icut in range(0,len(self.Nselected[reg])):
                self.Nselected_posweight[reg][icut].mean=self.cuts[reg][icut].sumw_pos
                self.Nselected_negweight[reg][icut].mean=self.cuts[reg][icut].sumw_neg
                self.Nselected_sumw2_posweight[reg][icut].mean=self.cuts[reg][icut].sumw2_pos
                self.Nselected_sumw2_negweight[reg][icut].mean=self.cuts[reg][icut].sumw2_neg

        # Extracting Ntotal
        self.Ntotal_posweight.mean       = self.initial.sumw_pos
        self.Ntotal_negweight.mean       = self.initial.sumw_neg
        self.Ntotal_sumw2_posweight.mean = self.initial.sumw2_pos
        self.Ntotal_sumw2_negweight.mean = self.initial.sumw2_neg

        return True


    def Calculate(self):
        myregs = self.main.regions.GetNames()
        if myregs == []:
            myregs = ['myregion']

        # Calculating Nrejected for positive and negative weighted events
        for reg in range(len(myregs)):
            for icut in range(0,len(self.Nselected[reg])):
                if icut==0:
                    self.Nrejected_posweight[reg][icut].mean = self.Ntotal_posweight.mean - self.Nselected_posweight[reg][icut].mean
                    self.Nrejected_negweight[reg][icut].mean = self.Ntotal_negweight.mean - self.Nselected_negweight[reg][icut].mean
                else:
                    self.Nrejected_posweight[reg][icut].mean = self.Nselected_posweight[reg][icut-1].mean - self.Nselected_posweight[reg][icut].mean
                    self.Nrejected_negweight[reg][icut].mean = self.Nselected_negweight[reg][icut-1].mean - self.Nselected_negweight[reg][icut].mean

        # Combining negative & positive weight events for computing Nselected, Nrejected and Ntotal
        self.Ntotal.mean = self.Ntotal_posweight.mean - self.Ntotal_negweight.mean
        for reg in range(len(myregs)):
            for icut in range(0,len(self.Nselected[reg])):
                self.Nselected[reg][icut].mean = self.Nselected_posweight[reg][icut].mean - self.Nselected_negweight[reg][icut].mean
                self.Nrejected[reg][icut].mean = self.Nrejected_posweight[reg][icut].mean - self.Nrejected_negweight[reg][icut].mean

        # Checking that all numbers are positive : Nselected and Ntotal
        for reg in range(len(myregs)):
            for icut in range(0,len(self.Nselected[reg])):
                if self.Nselected[reg][icut].mean<0:
                    self.warnings[reg][icut].append('The number of selected events is negative: '+\
                                               str(self.Nselected[reg][icut].mean)+'. Set to 0.')
                    self.Nselected[reg][icut].mean=0
                if self.Nrejected[reg][icut].mean<0:
                    self.warnings[reg][icut].append('The number of rejected events is negative: '+\
                                               str(self.Nrejected[reg][icut].mean)+'. Set to 0.')
                    self.Nrejected[reg][icut].mean=0

        # Checking that a N cut i > N cut i+1 : Nselected and Nrejected
        for reg in range(len(myregs)):
            for icut in range(0,len(self.Nselected[reg])):
                if icut==0:
                    if self.Nselected[reg][icut].mean > self.Ntotal.mean:
                        self.warnings[reg][icut].append('The number of selected events > the initial number of events: '+\
                          str(self.Nselected[reg][icut].mean)+' > '+str(self.Ntotal.mean)+'. Set the number of selected events to 0.')
                        self.Nselected[reg][icut].mean=0
                else:
                    if self.Nselected[reg][icut].mean > self.Nselected[reg][icut-1].mean:
                        self.warnings[reg][icut].append('The number of selected events > the initial number of events: '+\
                          str(self.Nselected[reg][icut].mean)+' > '+str(self.Nselected[reg][icut-1].mean)+'. Set the number of selected events to 0.')
                        self.Nselected[reg][icut].mean=0

        # Calculating errors on Naccepted and Nrejected
        for reg in range(len(myregs)):
            for icut in range(0,len(self.Nselected[reg])):
                self.Nselected[reg][icut].error = Measure.binomialNEventError(self.Nselected[reg][icut].mean,self.Ntotal.mean)
                self.Nrejected[reg][icut].error = Measure.binomialNEventError(self.Nrejected[reg][icut].mean,self.Ntotal.mean)

        # efficiency calculation and its error
        for reg in range(len(myregs)):
            for icut in range(0,len(self.Nselected[reg])):
                if icut==0:
                    if self.Ntotal.mean==0:
                        self.eff[reg][icut].mean = 0
                    else:
                        self.eff[reg][icut].mean = float(self.Nselected[reg][icut].mean) / \
                                                    float(self.Ntotal.mean)
                    self.eff[reg][icut].error = Measure.binomialError(self.Nselected[reg][icut].mean,self.Ntotal.mean)
                else:
                    if self.Nselected[reg][icut-1].mean==0:
                        self.eff[reg][icut].mean = 0
                    else:
                        self.eff[reg][icut].mean = float(self.Nselected[reg][icut].mean) / \
                                                    float(self.Nselected[reg][icut-1].mean)
                    self.eff[reg][icut].error = Measure.binomialError(self.Nselected[reg][icut].mean,self.Nselected[reg][icut-1].mean)

                if self.Ntotal.mean==0:
                    self.effcumu[reg][icut].mean=0
                else:
                    self.effcumu[reg][icut].mean = float(self.Nselected[reg][icut].mean) / \
                                                    float(self.Ntotal.mean)
                self.effcumu[reg][icut].error = Measure.binomialError(self.Nselected[reg][icut].mean,self.Ntotal.mean)

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
        for reg in range(len(myregs)):
            for icut in range(0,len(self.Nselected[reg])):
                if self.main.normalize == NormalizeType.LUMI:
                    # error due to xsec 
                    if ntot!=0:
                        errXsec = xerror * self.main.lumi * 1000 * self.Nselected[reg][icut].mean / ntot
                    else:
                        errXsec=0
                    # scale factor
                    if ntot!=0:
                        factor = xsection * self.main.lumi * 1000 / ntot
                    else:
                        factor = 0
                    self.Nselected[reg][icut].mean  *= factor
                    self.Nselected[reg][icut].error = Measure.binomialNEventError(self.Nselected[reg][icut].mean,self.Ntotal.mean)
                    # compute final error
                    self.Nselected[reg][icut].error = sqrt(self.Nselected[reg][icut].error**2 + errXsec**2)
                elif self.main.normalize == NormalizeType.LUMI_WEIGHT:
                    # error due to xsec 
                    if ntot!=0:
                        errXsec = xerror * self.main.lumi * 1000 * self.dataset.weight * self.Nselected[reg][icut].mean / ntot
                    else:
                        errXsec=0
                    # scale factor
                    if ntot!=0:
                        factor = xsection * self.main.lumi * self.dataset.weight * 1000 / ntot
                    else:
                        factor = 0
                    self.Nselected[reg][icut].mean  *= factor
                    self.Nselected[reg][icut].error = Measure.binomialNEventError(self.Nselected[reg][icut].mean,self.Ntotal.mean)
                    # compute final error
                    self.Nselected[reg][icut].error = sqrt(self.Nselected[reg][icut].error**2 + errXsec**2)

        # Scaling Nrejected
        for reg in range(len(myregs)):
            for icut in range(0,len(self.Nrejected[reg])):
                if self.main.normalize == NormalizeType.LUMI:
                    # error due to xsec 
                    if ntot!=0:
                        errXsec = xerror * self.main.lumi * 1000 * self.Nrejected[reg][icut].mean / ntot
                    else:
                        errXsec=0
                    # scale factor
                    if ntot!=0:
                        factor = xsection * self.main.lumi * 1000 / ntot
                    else:
                        factor = 0 
                    self.Nrejected[reg][icut].mean  *= factor
                    self.Nrejected[reg][icut].error = Measure.binomialNEventError(self.Nrejected[reg][icut].mean,self.Ntotal.mean)
                    # compute final error
                    self.Nrejected[reg][icut].error = sqrt(self.Nrejected[reg][icut].error**2 + errXsec**2)
                elif self.main.normalize == NormalizeType.LUMI_WEIGHT:
                    # error due to xsec 
                    if ntot!=0:
                        errXsec = xerror * self.main.lumi * 1000 * self.dataset.weight * self.Nrejected[reg][icut].mean / ntot
                    else:
                        errXsec=0
                    # scale factor
                    if ntot!=0:
                        factor = xsection * self.main.lumi * self.dataset.weight * 1000 / ntot
                    else:
                        factor = 0 
                    self.Nrejected[reg][icut].mean  *= factor
                    self.Nrejected[reg][icut].error = Measure.binomialNEventError(self.Nrejected[reg][icut].mean,self.Ntotal.mean)
                    # compute final error
                    self.Nrejected[reg][icut].error = sqrt(self.Nrejected[reg][icut].error**2 + errXsec**2)

        # recompute error to efficiency
        for reg in range(len(myregs)):
            for icut in range(0,len(self.Nselected[reg])):
                if icut==0:
                    self.eff[reg][icut].error = Measure.binomialError(self.Nselected[reg][icut].mean,self.Ntotal.mean)
                else:
                    self.eff[reg][icut].error = Measure.binomialError(self.Nselected[reg][icut].mean,self.Nselected[reg][icut-1].mean)
                self.effcumu[reg][icut].error = Measure.binomialError(self.Nselected[reg][icut].mean,self.Ntotal.mean)
