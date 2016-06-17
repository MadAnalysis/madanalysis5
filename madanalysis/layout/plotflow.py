################################################################################
#  
#  Copyright (C) 2012-2016 Eric Conte, Benjamin Fuks
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
            # PS: [:] -> clone the arrays
            histo[ihisto].positive.array = array_positive[:]
            histo[ihisto].negative.array = array_negative[:]
            histo[ihisto].labels         = newlabels[:]


    @staticmethod
    def NiceTitle(text):
        newtext=text 
        for i,j in PlotFlow.diconicetitle.iteritems():
           newtext = newtext.replace(i,j)
        return newtext


    def DrawAll(self,histo_path,modes,output_paths):

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

            # Name of output files
            filenameC = histo_path+"/selection_"+str(irelhisto)+".C"
            logging.debug('Producing file '+filenameC+' ...')
            output_files=[]
            for iout in range(0,len(output_paths)):
                output_files.append(output_paths[iout]+\
                                    "/selection_"+str(irelhisto)+"."+\
                                    ReportFormatType.convert2filetype(modes[iout]))

            # normal mode
            if not self.main.developer_mode:

                for iset in range(0,len(self.detail)):
 
                    # Appending histo
                    histos.append(self.detail[iset][irelhisto].myhisto)
#                    if mode==2:
                    scales.append(self.detail[iset][irelhisto].scale)
#                    else:
#                        scales.append(1)

                self.Draw(histos,scales,self.main.selection[iabshisto],irelhisto,\
                          filenameC,output_files)
                
                irelhisto+=1

            # developer mode
            elif self.main.developer_mode:

                for iset in range(0,len(self.detail)):
 
                    # Appending histo
                    histos.append(self.detail[iset][irelhisto])
#                    if mode==2:
                    scales.append(self.detail[iset][irelhisto].scale)
#                    else:
#                        scales.append(1)

                self.DrawROOT(histos,scales,self.main.selection[iabshisto],\
                              irelhisto,filenameC,output_files)
                  
                irelhisto+=1


        # Launching ROOT
        if self.main.developer_mode:
            print "LAUNCHING ROOT"
            commands=['root','-l','-q','-b']
            for ind in range(0,irelhisto):
                commands.append(histo_path+'/selection_'+str(ind)+'.C')
            import os
            os.system(' '.join(commands))
            
        return True

    def Draw(self,histos,scales,ref,irelhisto,filenameC,output_files):

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

        # Save the canvas
        canvas.SaveAs(filenameC)
        for output_file in output_files:
            canvas.SaveAs(output_file)



    def DrawROOT(self,histos,scales,ref,irelhisto,filenameC,outputnames):

        # Is there any legend?
        legendmode = False
        if len(self.main.datasets)>1:
            legendmode = True

        # Type of histogram
        frequencyhisto = True
        for histo in histos:
            if histo.__class__.__name__!='HistogramFrequency':
                frequencyhisto = False
                break
        logxhisto = True
        for histo in histos:
            if histo.__class__.__name__!='HistogramLogX':
                logxhisto = False
                break
            
        # Stacking or superimposing histos ?
        stackmode = False
        if ref.stack==StackingMethodType.STACK or \
           ( ref.stack==StackingMethodType.AUTO and \
             self.main.stack==StackingMethodType.STACK ):
            stackmode=True


        # Open the file in write-mode
        try:
            outputC = file(filenameC,'w')
        except:
            logging.error('Impossible to write the file: '+filenameC)
            return False

        # File header
        function_name = filenameC[:-2]
        function_name = function_name.split('/')[-1]
        outputC.write('void '+function_name+'()\n')
        outputC.write('{\n\n')

        # ROOT version
        outputC.write('  // ROOT version\n')
        outputC.write('  Int_t root_version = gROOT->GetVersionInt();\n')
        outputC.write('\n')

        # Creating the TCanvas
        PlotFlow.counter=PlotFlow.counter+1
        canvas_name='tempo'+str(PlotFlow.counter)
        outputC.write('  // Creating a new TCanvas\n')
        widthx=700
        if legendmode:
            widthx=1000
        outputC.write('  TCanvas* canvas = new TCanvas("'+canvas_name+'","'+canvas_name+'",0,0,'+str(widthx)+',500);\n')
        outputC.write('  gStyle->SetOptStat(0);\n')
        outputC.write('  gStyle->SetOptTitle(0);\n')
        outputC.write('  canvas->SetHighLightColor(2);\n')
#       outputC.write('  canvas->Range(-2.419355,-0.005372711,16.93548,0.03939988);\n')
        outputC.write('  canvas->SetFillColor(0);\n')
        outputC.write('  canvas->SetBorderMode(0);\n')
        outputC.write('  canvas->SetBorderSize(3);\n')
        outputC.write('  canvas->SetFrameBorderMode(0);\n')
        outputC.write('  canvas->SetFrameBorderSize(0);\n')
        outputC.write('  canvas->SetTickx(1);\n')
        outputC.write('  canvas->SetTicky(1);\n')
        outputC.write('  canvas->SetLeftMargin(0.14);\n')
        margin=0.05
        if legendmode:
            margin=0.3
        outputC.write('  canvas->SetRightMargin('+str(margin)+');\n')
        outputC.write('  canvas->SetBottomMargin(0.15);\n')
        outputC.write('  canvas->SetTopMargin(0.05);\n')
        outputC.write('\n')

        # Binning
        xnbin=histos[0].nbins
        if logxhisto:
            outputC.write('  // Histo binning\n')
            outputC.write('  Double_t xBinning['+str(xnbin+1)+'] = {')
            for bin in range(1,xnbin+2):
                if bin!=1:
                    outputC.write(',')
                outputC.write(str(histos[0].GetBinLowEdge(bin)))
            outputC.write('};\n')
            outputC.write('\n')

        # Loop over datasets and histos
        for ind in range(0,len(histos)):

            # Creating TH1F
            outputC.write('  // Creating a new TH1F\n')
            histoname=histos[ind].name+'_'+str(ind)
            xmin=histos[ind].xmin
            xmax=histos[ind].xmax
            if logxhisto:
                 outputC.write('  TH1F* '+histoname+' = new TH1F("'+histoname+'","'+\
                               histoname+'",'+str(xnbin)+',xBinning);\n')
            else:
                 outputC.write('  TH1F* '+histoname+' = new TH1F("'+histoname+'","'+\
                               histoname+'",'+str(xnbin)+','+\
                               str(xmin)+','+str(xmax)+');\n')

            # TH1F content
            outputC.write('  // Content\n')
            outputC.write('  '+histoname+'->SetBinContent(0'+\
                          ','+str(histos[ind].summary.underflow*scales[ind])+');\n')
            for bin in range(1,xnbin+1):
                outputC.write('  '+histoname+'->SetBinContent('+str(bin)+\
                              ','+str(histos[ind].summary.array[bin-1]*scales[ind])+'); // underflow\n')
            nentries=histos[ind].summary.nentries
            outputC.write('  '+histoname+'->SetEntries('+str(nentries)+');\n')
            outputC.write('  '+histoname+'->SetBinContent('+str(xnbin+1)+\
                          ','+str(histos[ind].summary.overflow*scales[ind])+'); // overflow\n')

            # linecolor
            if self.main.datasets[ind].linecolor!=ColorType.AUTO:
                colorline=ColorType.convert2root( \
                          self.main.datasets[ind].linecolor,\
                          self.main.datasets[ind].lineshade)

            # Setting AUTO settings
            if len(histos)==1:
                linecolor1 = [9]
                linecolor  = linecolor1[ind]
                if stackmode:
                    backstyle1 = [3004]
                    backstyle  = backstyle1[ind]
                    backcolor  = linecolor1[ind]
            elif len(histos)==2:
                linecolor2 = [9,46]
                linecolor  = linecolor2[ind]
                if stackmode:
                    backstyle2 = [3004,3005]
                    backstyle  = backstyle2[ind]
                    backcolor  = linecolor2[ind]
            elif len(histos)==3:
                linecolor3 = [9,46,8]
                linecolor  = linecolor3[ind]
                if stackmode:
                    backstyle3 = [3004,3005,3006]
                    backstyle  = backstyle3[ind]
                    backcolor  = linecolor3[ind]                    
            elif len(histos)==4:
                linecolor4 = [9,46,8,4]
                linecolor  = linecolor4[ind]
                if stackmode:
                    backstyle4 = [3004,3005,3006,3007]
                    backstyle  = backstyle4[ind]
                    backcolor  = linecolor4[ind]
            elif len(histos)==5:
                linecolor5 = [9,46,8,4,6]
                linecolor  = linecolor5[ind]
                if stackmode:
                    backstyle5 = [3004,3005,3006,3007,3013]
                    backstyle  = backstyle5[ind]
                    backcolor  = linecolor5[ind]
            elif len(histos)==6:
                linecolor6 = [9,46,8,4,6,2]
                linecolor  = linecolor6[ind]
                if stackmode:
                    backstyle6 = [3004,3005,3006,3007,3013,3017]
                    backstyle  = backstyle6[ind]
                    backcolor  = linecolor6[ind]
            elif len(histos)==7:
                linecolor7 = [9,46,8,4,6,2,7]
                linecolor  = linecolor7[ind]
                if stackmode:
                    backstyle7 = [3004,3005,3006,3007,3013,3017,3022]
                    backstyle  = backstyle7[ind]
                    backcolor  = linecolor7[ind]
            elif len(histos)==8:
                linecolor8 = [9,46,8,4,6,2,7,3]
                linecolor  = linecolor8[ind]
                if stackmode:
                    backstyle8 = [3004,3005,3006,3007,3013,3017,3022,3315]
                    backstyle  = backstyle8[ind]
                    backcolor  = linecolor8[ind]
            elif len(histos)==9:
                linecolor9 = [9,46,8,4,6,2,7,3,42]
                linecolor  = linecolor9[ind]
                if stackmode:
                    backstyle9 = [3004,3005,3006,3007,3013,3017,3022,3315,3351]
                    backstyle  = backstyle9[ind]
                    backcolor  = linecolor9[ind]
            elif len(histos)==10:
                linecolor10 = [9,46,8,4,6,2,7,3,42,48]
                linecolor   = linecolor10[ind]
                if stackmode:
                    backstyle10 = [3004,3005,3006,3007,3013,3017,3022,3315,3481]
                    backstyle   = backstyle10[ind]
                    backcolor   = linecolor10[ind]
            else:
                linecolor=self.color
                self.color += 1

            # lineStyle
            linestyle=LineStyleType.convert2code(self.main.datasets[ind].linestyle)

            # linewidth
            linewidth=self.main.datasets[ind].linewidth

            # background color
            if self.main.datasets[ind].backcolor!=ColorType.AUTO:
                backcolor=ColorType.convert2root( \
                          self.main.datasets[ind].backcolor,\
                          self.main.datasets[ind].backshade)

            # background color  
            if self.main.datasets[ind].backstyle!=BackStyleType.AUTO:
                backstyle=BackStyleType.convert2code( \
                          self.main.datasets[ind].backstyle)

            # style
            outputC.write('  // Style\n')
            outputC.write('  '+histoname+'->SetLineColor('+str(linecolor)+');\n')
            outputC.write('  '+histoname+'->SetLineStyle('+str(linestyle)+');\n')
            outputC.write('  '+histoname+'->SetLineWidth('+str(linewidth)+');\n')
            outputC.write('  '+histoname+'->SetFillColor('+str(backcolor)+');\n')
            outputC.write('  '+histoname+'->SetFillStyle('+str(backstyle)+');\n')
            if frequencyhisto:
                outputC.write('  '+histoname+'->SetBarWidth(0.8);\n')
                outputC.write('  '+histoname+'->SetBarOffset(0.1);\n')
            outputC.write('\n')
        
        # Creating the THStack
        outputC.write('  // Creating a new THStack\n')
        PlotFlow.counter+=1
        outputC.write('  THStack* stack = new THStack("mystack_'+str(PlotFlow.counter)+'","mystack");\n')
        # Loop over datasets and histos
        ntot = 0
        for ind in range(0,len(histos)):
            histoname=histos[ind].name+'_'+str(ind)
            ntot+=histos[ind].summary.integral
            outputC.write('  stack->Add('+histoname+');\n')

        drawoptions=[]
        if not stackmode:
            drawoptions.append('nostack')
        if frequencyhisto:
            drawoptions.append('bar1')
        outputC.write('  stack->Draw("'+''.join(drawoptions)+'");\n')
        outputC.write('\n')
        
        # Setting Y axis label
        outputC.write('  // Y axis\n')
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

        if(len(axis_titleY) > 35): 
           titlesize=0.04
        else:
           titlesize=0.06
        outputC.write('  stack->GetYaxis()->SetLabelSize(0.04);\n')
        outputC.write('  stack->GetYaxis()->SetLabelOffset(0.005);\n')
        outputC.write('  stack->GetYaxis()->SetTitleSize('+str(titlesize)+');\n')
        outputC.write('  stack->GetYaxis()->SetTitleFont(22);\n')
        outputC.write('  stack->GetYaxis()->SetTitleOffset(1);\n')
        outputC.write('  stack->GetYaxis()->SetTitle("'+axis_titleY+'");\n')

        outputC.write('\n')
        outputC.write('  // X axis\n')

        # Setting X axis label
        if ref.titleX=="": 
            axis_titleX = ref.GetXaxis()
        else:
            axis_titleX = PlotFlow.NiceTitle(ref.titleX)
        
        # Setting X axis label
        outputC.write('  stack->GetXaxis()->SetLabelSize(0.04);\n')
        outputC.write('  stack->GetXaxis()->SetLabelOffset(0.005);\n')
        outputC.write('  stack->GetXaxis()->SetTitleSize(0.06);\n')
        outputC.write('  stack->GetXaxis()->SetTitleFont(22);\n')
        outputC.write('  stack->GetXaxis()->SetTitleOffset(1);\n')
        outputC.write('  stack->GetXaxis()->SetTitle("'+axis_titleX+'");\n')
        if frequencyhisto:
            for bin in range(1,xnbin+1):
                 outputC.write('  stack->GetXaxis()->SetBinLabel('+str(bin)+','\
                               '"'+str(histos[ind].stringlabels[bin-1])+'");\n')
        outputC.write('\n')

        # Setting Log scale
        outputC.write('  // Finalizing the TCanvas\n')
        logx=0
        if ref.logX and ntot != 0:
            logx=1
        logy=0
        if ref.logY and ntot != 0:
            logy=1
        outputC.write('  canvas->SetLogx('+str(logx)+');\n')
        outputC.write('  canvas->SetLogy('+str(logy)+');\n')
        outputC.write('\n')
        
        # Displaying a legend
        if legendmode:
            outputC.write('  // Creating a TLegend\n')
            outputC.write('  TLegend* legend = new TLegend(.73,.5,.97,.95);\n')
            for ind in range(0,len(histos)):
                histoname=histos[ind].name+'_'+str(ind)
                nicetitle=PlotFlow.NiceTitle(self.main.datasets[ind].title)
                outputC.write('  legend->AddEntry('+histoname+',"'+nicetitle+'");\n')
            outputC.write('  legend->SetFillColor(0);\n')
            outputC.write('  legend->SetTextSize(0.05);\n')
            outputC.write('  legend->SetTextFont(22);\n')
            outputC.write('  legend->SetY1(TMath::Max(0.15,0.97-0.10*legend->GetListOfPrimitives()->GetSize()));\n')
            outputC.write('  legend->Draw();\n')
            outputC.write('\n')
        
        # Producing the image
        outputC.write('  // Saving the image\n')
        for outputname in outputnames:
            outputC.write('  canvas->SaveAs("'+outputname+'");\n')
        outputC.write('\n')

        # File foot
        outputC.write('}\n')

        # Close the file
        try:
            outputC.close()
        except:
            logging.error('Impossible to close the file: '+outputC)
            return False

        # Ok
        return True




