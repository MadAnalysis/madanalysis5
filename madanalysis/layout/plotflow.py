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
from madanalysis.layout.plotflow_for_dataset      import PlotFlowForDataset
from math  import sqrt
import time
import copy
import logging
class PlotFlow:

    diconicetitle = {' ^ {':'^{', ' _ {':'_{', '\\\\':'#'}

    counter=0

    def __init__(self,main):
        self.main               = main
        self.detail             = []
        for i in range(0,len(main.datasets)):
            self.detail.append(PlotFlowForDataset(main,main.datasets[i]))


    def Initialize(self):

        # Initializing NPID
        for ihisto in range(0,len(self.detail[0])):
            if self.detail[0].histos[ihisto].__class__.__name__ == "HistogramFrequency":
                self.InitializeHistoFrequency(ihisto)

        # Creating plots
        for i in range(0,len(self.detail)):
            self.detail[i].FinalizeReading() 
            self.detail[i].ComputeScale()
            self.detail[i].CreateHistogram()


    def InitializeHistoFrequency(self,ihisto):

        import numpy
        
        # New collection of labels
        newlabels=[]

        # Loop over datasets
        for histo in self.detail:

            # Loop over the label
            for label in histo[ihisto].labels:

                # Add in the collection 
                if label not in newlabels:
                    newlabels.append(label)

        # Sorting labels (alphabetical order)
        newlabels = sorted(newlabels)

        # Loop over datasets
        for histo in self.detail:

            # New array for data
            array_positive=[]
            array_negative=[]
            
            # Loop over the new labels
            for newlabel in newlabels:

                # Loop over the old labels
                found = False
                value_positive = 0
                value_negative = 0
                for i in range(len(histo[ihisto].labels)):

                    if newlabel==histo[ihisto].labels[i]:
                        value_positive = histo[ihisto].positive.array[i]
                        value_negative = histo[ihisto].negative.array[i]
                        found = True
                        break

                # Fill
                if found:
                    array_positive.append(value_positive)
                    array_negative.append(value_negative)
                else:
                    array_positive.append(0.)
                    array_negative.append(0.)

            # save result
            histo[ihisto].positive.array = numpy.array(array_positive)
            histo[ihisto].negative.array = numpy.array(array_negative)
            histo[ihisto].labels   = numpy.array(newlabels)


    @staticmethod
    def NiceTitle(text):
        newtext=text 
        for i,j in PlotFlow.diconicetitle.iteritems():
           newtext = newtext.replace(i,j)
        return newtext


    def DrawAll(self,mode,output_path):

        # Reset Configuration
        RootConfig.Init()

        # Loop on each histo type
        irelhisto=0
        for iabshisto in range(0,len(self.main.selection)):
            if self.main.selection[iabshisto].__class__.__name__!="Histogram":
                continue
            self.color=1
            histos=[]
            scales=[]
            for iset in range(0,len(self.detail)):

                # Appending histo
                histos.append(self.detail[iset][irelhisto].myhisto)
                if mode==2:
                 scales.append(self.detail[iset][irelhisto].scale)
                else:
                 scales.append(1)

            # Draw
            self.Draw(histos,scales,self.main.selection[iabshisto],irelhisto,mode,output_path,preview=False)
                
            irelhisto+=1


    def Draw(self,histos,scales,ref,irelhisto,mode,output_path,preview=False):

        from ROOT import TH1
        from ROOT import TH1F
        from ROOT import THStack
        from ROOT import TLegend
        from ROOT import TCanvas
        from ROOT import TASImage
        from ROOT import TAttImage
        from ROOT import TPad

        # Creating a canvas
        PlotFlow.counter=PlotFlow.counter+1
        canvas = TCanvas("tempo"+str(PlotFlow.counter),"")

        # Loop over datasets and histos
        for ind in range(0,len(histos)):
            # Scaling 
            histos[ind].Scale(scales[ind])
            
        # Stacking or superimposing histos ?
        stackmode = False
        if ref.stack==StackingMethodType.STACK or \
           ( ref.stack==StackingMethodType.AUTO and \
             self.main.stack==StackingMethodType.STACK ):
            stackmode=True

        # Setting AUTO settings
        if len(histos)==1:
            histos[0].SetLineColor(9)
            if stackmode:
                histos[0].SetFillColor(9)
                histos[0].SetFillStyle(3004)
        elif len(histos)==2:
            histos[0].SetLineColor(9)
            histos[1].SetLineColor(46)
            if stackmode:
                histos[0].SetFillColor(9)
                histos[0].SetFillStyle(3004)
                histos[1].SetFillColor(46)
                histos[1].SetFillStyle(3005)
        elif len(histos)==3:
            histos[0].SetLineColor(9)
            histos[1].SetLineColor(46)
            histos[2].SetLineColor(8)
            if stackmode:
                histos[0].SetFillColor(9)
                histos[0].SetFillStyle(3004)
                histos[1].SetFillColor(46)
                histos[1].SetFillStyle(3005)
                histos[2].SetFillColor(8)
                histos[2].SetFillStyle(3006)
        elif len(histos)==4:
            histos[0].SetLineColor(9)
            histos[1].SetLineColor(46)
            histos[2].SetLineColor(8)
            histos[3].SetLineColor(4)
            if stackmode:
                histos[0].SetFillColor(9)
                histos[0].SetFillStyle(3004)
                histos[1].SetFillColor(46)
                histos[1].SetFillStyle(3005)
                histos[2].SetFillColor(8)
                histos[2].SetFillStyle(3006)
                histos[3].SetFillColor(4)
                histos[3].SetFillStyle(3007)
        elif len(histos)==5:
            histos[0].SetLineColor(9)
            histos[1].SetLineColor(46)
            histos[2].SetLineColor(8)
            histos[3].SetLineColor(4)
            histos[4].SetLineColor(6)
            if stackmode:
                histos[0].SetFillColor(9)
                histos[0].SetFillStyle(3004)
                histos[1].SetFillColor(46)
                histos[1].SetFillStyle(3005)
                histos[2].SetFillColor(8)
                histos[2].SetFillStyle(3006)
                histos[3].SetFillColor(4)
                histos[3].SetFillStyle(3007)
                histos[4].SetFillColor(6)
                histos[4].SetFillStyle(3013)
        elif len(histos)==6:
            histos[0].SetLineColor(9)
            histos[1].SetLineColor(46)
            histos[2].SetLineColor(8)
            histos[3].SetLineColor(4)
            histos[4].SetLineColor(6)
            histos[5].SetLineColor(2)
            if stackmode:
                histos[0].SetFillColor(9)
                histos[0].SetFillStyle(3004)
                histos[1].SetFillColor(46)
                histos[1].SetFillStyle(3005)
                histos[2].SetFillColor(8)
                histos[2].SetFillStyle(3006)
                histos[3].SetFillColor(4)
                histos[3].SetFillStyle(3007)
                histos[4].SetFillColor(6)
                histos[4].SetFillStyle(3013)
                histos[5].SetFillColor(2)
                histos[5].SetFillStyle(3017)
        elif len(histos)==7:
            histos[0].SetLineColor(9)
            histos[1].SetLineColor(46)
            histos[2].SetLineColor(8)
            histos[3].SetLineColor(4)
            histos[4].SetLineColor(6)
            histos[5].SetLineColor(2)
            histos[6].SetLineColor(7)
            if stackmode:
                histos[0].SetFillColor(9)
                histos[0].SetFillStyle(3004)
                histos[1].SetFillColor(46)
                histos[1].SetFillStyle(3005)
                histos[2].SetFillColor(8)
                histos[2].SetFillStyle(3006)
                histos[3].SetFillColor(4)
                histos[3].SetFillStyle(3007)
                histos[4].SetFillColor(6)
                histos[4].SetFillStyle(3013)
                histos[5].SetFillColor(2)
                histos[5].SetFillStyle(3017)
                histos[6].SetFillColor(7)
                histos[6].SetFillStyle(3022)
        elif len(histos)==8:
            histos[0].SetLineColor(9)
            histos[1].SetLineColor(46)
            histos[2].SetLineColor(8)
            histos[3].SetLineColor(4)
            histos[4].SetLineColor(6)
            histos[5].SetLineColor(2)
            histos[6].SetLineColor(7)
            histos[7].SetLineColor(3)
            if stackmode:
                histos[0].SetFillColor(9)
                histos[0].SetFillStyle(3004)
                histos[1].SetFillColor(46)
                histos[1].SetFillStyle(3005)
                histos[2].SetFillColor(8)
                histos[2].SetFillStyle(3006)
                histos[3].SetFillColor(4)
                histos[3].SetFillStyle(3007)
                histos[4].SetFillColor(6)
                histos[4].SetFillStyle(3013)
                histos[5].SetFillColor(2)
                histos[5].SetFillStyle(3017)
                histos[6].SetFillColor(7)
                histos[6].SetFillStyle(3022)
                histos[7].SetFillColor(3)
                histos[7].SetFillStyle(3315)
        elif len(histos)==9:
            histos[0].SetLineColor(9)
            histos[1].SetLineColor(46)
            histos[2].SetLineColor(8)
            histos[3].SetLineColor(4)
            histos[4].SetLineColor(6)
            histos[5].SetLineColor(2)
            histos[6].SetLineColor(7)
            histos[7].SetLineColor(3)
            histos[8].SetLineColor(42)
            if stackmode:
                histos[0].SetFillColor(9)
                histos[0].SetFillStyle(3004)
                histos[1].SetFillColor(46)
                histos[1].SetFillStyle(3005)
                histos[2].SetFillColor(8)
                histos[2].SetFillStyle(3006)
                histos[3].SetFillColor(4)
                histos[3].SetFillStyle(3007)
                histos[4].SetFillColor(6)
                histos[4].SetFillStyle(3013)
                histos[5].SetFillColor(2)
                histos[5].SetFillStyle(3017)
                histos[6].SetFillColor(7)
                histos[6].SetFillStyle(3022)
                histos[7].SetFillColor(3)
                histos[7].SetFillStyle(3315)
                histos[8].SetFillColor(42)
                histos[8].SetFillStyle(3351)
        elif len(histos)==10:
            histos[0].SetLineColor(9)
            histos[1].SetLineColor(46)
            histos[2].SetLineColor(8)
            histos[3].SetLineColor(4)
            histos[4].SetLineColor(6)
            histos[5].SetLineColor(2)
            histos[6].SetLineColor(7)
            histos[7].SetLineColor(3)
            histos[8].SetLineColor(42)
            histos[9].SetLineColor(48)
            if stackmode:
                histos[0].SetFillColor(9)
                histos[0].SetFillStyle(3004)
                histos[1].SetFillColor(46)
                histos[1].SetFillStyle(3005)
                histos[2].SetFillColor(8)
                histos[2].SetFillStyle(3006)
                histos[3].SetFillColor(4)
                histos[3].SetFillStyle(3007)
                histos[4].SetFillColor(6)
                histos[4].SetFillStyle(3013)
                histos[5].SetFillColor(2)
                histos[5].SetFillStyle(3017)
                histos[6].SetFillColor(7)
                histos[6].SetFillStyle(3022)
                histos[7].SetFillColor(3)
                histos[7].SetFillStyle(3315)
                histos[8].SetFillColor(42)
                histos[8].SetFillStyle(3351)
                histos[9].SetFillColor(48)
                histos[9].SetFillStyle(3481)
        else:
            histos[ind].SetLineColor(self.color)
            self.color += 1

        # Setting USER color
        for ind in range(0,len(histos)):

            # linecolor
            if self.main.datasets[ind].linecolor!=ColorType.AUTO:
                histos[ind].SetLineColor(ColorType.convert2root( \
                self.main.datasets[ind].linecolor,\
                self.main.datasets[ind].lineshade))

            # lineStyle
            histos[ind].SetLineStyle(LineStyleType.convert2code( \
                self.main.datasets[ind].linestyle))

            # linewidth
            histos[ind].SetLineWidth(self.main.datasets[ind].linewidth)

            # background color  
            if self.main.datasets[ind].backcolor!=ColorType.AUTO:
                histos[ind].SetFillColor(ColorType.convert2root( \
                self.main.datasets[ind].backcolor,\
                self.main.datasets[ind].backshade))

            # background color  
            if self.main.datasets[ind].backstyle!=BackStyleType.AUTO:
                histos[ind].SetFillStyle(BackStyleType.convert2code( \
                self.main.datasets[ind].backstyle))

        # Creating and filling the stack; computing the total number of events
        stack = THStack("mystack","")
        ntot = 0
        for item in histos:
            ntot+=item.Integral()
            stack.Add(item)

        # Drawing
        if stackmode:
            stack.Draw()
        else:
            stack.Draw("nostack")

        # Setting Y axis label
        axis_titleY = ref.GetYaxis()

        # Scale to one ?
        scale2one = False
        if ref.stack==StackingMethodType.NORMALIZE2ONE or \
           (self.main.stack==StackingMethodType.NORMALIZE2ONE and \
           ref.stack==StackingMethodType.AUTO):
            scale2one = True

        if scale2one:
            axis_titleY += " ( scaled to one )"
        elif self.main.normalize == NormalizeType.LUMI or \
           self.main.normalize == NormalizeType.LUMI_WEIGHT:
            axis_titleY += " ( L_{int} = " + str(self.main.lumi)+ " fb^{-1} )"
        elif self.main.normalize == NormalizeType.NONE:
            axis_titleY += " (not normalized)"

        if ref.titleY!="": 
            axis_titleY = PlotFlow.NiceTitle(ref.titleY)

        stack.GetYaxis().SetTitle(axis_titleY)
        if(len(axis_titleY) > 35): 
           stack.GetYaxis().SetTitleSize(0.04)
        else:
           stack.GetYaxis().SetTitleSize(0.06)
        stack.GetYaxis().SetTitleFont(22)
        stack.GetYaxis().SetLabelSize(0.04)

        # Setting X axis label
        if ref.titleX=="": 
            axis_titleX = ref.GetXaxis()
        else:
            axis_titleX = PlotFlow.NiceTitle(ref.titleX)
        
        # Setting X axis label
        stack.GetXaxis().SetTitle(axis_titleX)
        stack.GetXaxis().SetTitleSize(0.06)
        stack.GetXaxis().SetTitleFont(22)
        stack.GetXaxis().SetLabelSize(0.04)

        # Setting Log scale
        if ref.logX and ntot != 0:
            canvas.SetLogx()
        if ref.logY and ntot != 0:
            canvas.SetLogy()
        
        # Displaying a legend
        if len(self.main.datasets)>1:
            ymin_legend = .9-.055*len(histos)
            if ymin_legend<0.1:
                ymin_legend = 0.1
            legend = TLegend(.65,ymin_legend,.9,.9)
            legend.SetTextSize(0.05); 
            legend.SetTextFont(22); 
            for ind in range(0,len(histos)):
                legend.AddEntry(histos[ind],PlotFlow.NiceTitle(self.main.datasets[ind].title))
            legend.SetFillColor(0)    
            legend.Draw()

        if not preview:

            # Put the MA5 logo
#            logo = TASImage.Open(self.main.archi_info.ma5dir+\
#                              "/madanalysis/input/logo.eps")
#            if not logo.IsValid():
#                logging.warning("file called '"+self.main.archi_info.ma5dir+\
#                                "/madanalysis/input/logo.eps' " +\
#                                "is not found")
#            else:
#                logo.SetConstRatio(0)
#                logo.SetImageQuality(TAttImage.kImgBest)
#                logo.Vectorize(256)
#                w = logo.GetWidth()
#                h = logo.GetHeight()
#                logo.Scale(int(w*0.2),int(h*0.2))
#                mypad = TPad("i1", "i1", 0.75, 0.9, 0.85, 1)
#                mypad.Draw()
#                mypad.cd()
#                logo.Draw()
               
#            # Save the canvas in the report format
#            canvas.Update()
#            
#            thepicture = TASImage.Create()
#            thepicture.FromPad(canvas)
#            thepicture.SetConstRatio(0)
#            thepicture.SetImageQuality(TAttImage.kImgBest)
#            thepicture.WriteImage(output_path+"/selection_"+str(irelhisto)+\
#                                  "."+ReportFormatType.convert2filetype(mode))
            canvas.SaveAs(output_path+"/selection_"+str(irelhisto)+\
                          "."+ReportFormatType.convert2filetype(mode))

            # Save the canvas in the C format
            canvas.SaveAs(output_path+"/selection_"+str(irelhisto)+".C")
            
        else:
            # break 
            answer=raw_input("Press enter to continue : ")
  
            
                   
