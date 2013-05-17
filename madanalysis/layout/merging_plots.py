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


from madanalysis.selection.instance_name           import InstanceName
from madanalysis.enumeration.uncertainty_type      import UncertaintyType
from madanalysis.enumeration.normalize_type        import NormalizeType
from madanalysis.layout.root_config                import RootConfig
from madanalysis.enumeration.report_format_type    import ReportFormatType
from madanalysis.enumeration.observable_type       import ObservableType
from madanalysis.enumeration.color_type            import ColorType
from madanalysis.enumeration.linestyle_type        import LineStyleType
from madanalysis.enumeration.backstyle_type        import BackStyleType
from madanalysis.enumeration.stacking_method_type  import StackingMethodType
from madanalysis.layout.root_config                import RootConfig
from madanalysis.layout.merging_plots_for_dataset import MergingPlotsForDataset 
from math import sqrt


class MergingPlots:

    def __init__(self,main):
        self.main         = main
        self.detail       = []
        for i in range(0,len(main.datasets)):
            self.detail.append(MergingPlotsForDataset(main,main.datasets[i]))
        #self.datasetnames = []
        #self.filenames    = []


    def Initialize(self):

        # Creating plots
        for i in range(0,len(self.detail)):
            self.detail[i].FinalizeReading() 
            self.detail[i].CreateHistogram()


    def DrawAll(self,mode,output_path):

        # Reset Configuration
        RootConfig.Init()

        # Loop on each dataset
        for i in range(0,len(self.main.datasets)):
            self.DrawDatasetPlots(self.detail[i],\
                                  self.main.datasets[i],\
                                  mode,output_path)


    def DrawDatasetPlots(self,histos,dataset,mode,output_path):

        # Loop over DJR
        for i in range(0,100):
            
            DJRplots = []

            # Looking for global plot
            name = "DJR"+str(i+1)+"_total"
            test=False
            for h in range(len(histos)):
                if histos[h].name == name:
                    DJRplots.append(histos[h])
                    test=True
                    break
                
            # Global plot not found ?
            if not test:
                break

            # Loop over njets
            for j in range(0,100):

               # Looking for njet plot
                name = "DJR"+str(i+1)+"_"+str(j)+"jet"
                test=False
                for h in range(len(histos)):
                    if histos[h].name == name:
                        DJRplots.append(histos[h])
                        test=True
                        break
                
                # njet plot not found ?
                if not test:
                    break
                
            # Drawing
            self.DrawPlot(DJRplots,dataset,mode,output_path,i+1)
            
            
    def DrawPlot(self,DJRplots,dataset,mode,output_path,index):

        from ROOT import TH1
        from ROOT import TH1F
        from ROOT import THStack
        from ROOT import TLegend
        from ROOT import TCanvas

        # Creating a canvas
        canvas = TCanvas("tempo","")

        # Getting xsection
        xsection=dataset.measured_global.xsection
        if dataset.xsection!=0.:
            xsection=dataset.xsection

        # Setting color to the total plot
        DJRplots[0].myhisto.SetLineColor(1)
        DJRplots[0].myhisto.SetLineStyle(1)

        # Scaling the total plot
        if DJRplots[0].myhisto.GetEntries()!=0:
            DJRplots[0].myhisto.Scale( float(xsection) / \
                                       float(DJRplots[0].myhisto.GetEntries()) )

        # Loop over other DJR plots    
        for ind in range(1,len(DJRplots)):
            DJRplots[ind].myhisto.SetLineStyle(2)
            if DJRplots[ind].myhisto.GetEntries()!=0:
                DJRplots[ind].myhisto.Scale( float(xsection) / \
                                             float(DJRplots[0].myhisto.GetEntries()) )

        # Setting color an other settings to jet plots    
        if len(DJRplots)==2:
            DJRplots[1].myhisto.SetLineColor(9)
        elif len(DJRplots)==3:
            DJRplots[1].myhisto.SetLineColor(9)
            DJRplots[2].myhisto.SetLineColor(46)
        elif len(DJRplots)==4:
            DJRplots[1].myhisto.SetLineColor(9)
            DJRplots[2].myhisto.SetLineColor(46)
            DJRplots[3].myhisto.SetLineColor(8)
        elif len(DJRplots)==5:
            DJRplots[1].myhisto.SetLineColor(9)
            DJRplots[2].myhisto.SetLineColor(46)
            DJRplots[3].myhisto.SetLineColor(8)
            DJRplots[4].myhisto.SetLineColor(4)
        elif len(DJRplots)==6:
            DJRplots[1].myhisto.SetLineColor(9)
            DJRplots[2].myhisto.SetLineColor(46)
            DJRplots[3].myhisto.SetLineColor(8)
            DJRplots[4].myhisto.SetLineColor(4)
            DJRplots[5].myhisto.SetLineColor(6)
        elif len(DJRplots)==7:
            DJRplots[1].myhisto.SetLineColor(9)
            DJRplots[2].myhisto.SetLineColor(46)
            DJRplots[3].myhisto.SetLineColor(8)
            DJRplots[4].myhisto.SetLineColor(4)
            DJRplots[5].myhisto.SetLineColor(6)
            DJRplots[6].myhisto.SetLineColor(2)
        elif len(DJRplots)==8:
            DJRplots[1].myhisto.SetLineColor(9)
            DJRplots[2].myhisto.SetLineColor(46)
            DJRplots[3].myhisto.SetLineColor(8)
            DJRplots[4].myhisto.SetLineColor(4)
            DJRplots[5].myhisto.SetLineColor(6)
            DJRplots[6].myhisto.SetLineColor(2)
            DJRplots[7].myhisto.SetLineColor(7)
        elif len(DJRplots)==9:
            DJRplots[1].myhisto.SetLineColor(9)
            DJRplots[2].myhisto.SetLineColor(46)
            DJRplots[3].myhisto.SetLineColor(8)
            DJRplots[4].myhisto.SetLineColor(4)
            DJRplots[5].myhisto.SetLineColor(6)
            DJRplots[6].myhisto.SetLineColor(2)
            DJRplots[7].myhisto.SetLineColor(7)
            DJRplots[8].myhisto.SetLineColor(3)
        elif len(DJRplots)==10:
            DJRplots[1].myhisto.SetLineColor(9)
            DJRplots[2].myhisto.SetLineColor(46)
            DJRplots[3].myhisto.SetLineColor(8)
            DJRplots[4].myhisto.SetLineColor(4)
            DJRplots[5].myhisto.SetLineColor(6)
            DJRplots[6].myhisto.SetLineColor(2)
            DJRplots[7].myhisto.SetLineColor(7)
            DJRplots[8].myhisto.SetLineColor(3)
            DJRplots[9].myhisto.SetLineColor(42)
        elif len(DJRplots)==11:
            DJRplots[1].myhisto.SetLineColor(9)
            DJRplots[2].myhisto.SetLineColor(46)
            DJRplots[3].myhisto.SetLineColor(8)
            DJRplots[4].myhisto.SetLineColor(4)
            DJRplots[5].myhisto.SetLineColor(6)
            DJRplots[6].myhisto.SetLineColor(2)
            DJRplots[7].myhisto.SetLineColor(7)
            DJRplots[8].myhisto.SetLineColor(3)
            DJRplots[9].myhisto.SetLineColor(42)
            DJRplots[10].myhisto.SetLineColor(48)
        else:
            color=1
            for ind in range(1,len(DJRplots)):
                DJRplots[ind].myhisto.SetLineColor(color)
                color += 1

        # Creating and filling the stack; computing the total number of events
        stack = THStack("mystack","")
        ntot = DJRplots[0].myhisto.GetEntries()
        stack.Add(DJRplots[0].myhisto)
        for ind in range(1,len(DJRplots)):
            ntot+=DJRplots[ind].myhisto.GetEntries()
            stack.Add(DJRplots[ind].myhisto)

        # Drawing plots
        stack.Draw("nostack")
        
        # Setting Y axis label
        axis_titleY = "Cross section (pb/bin)"
        stack.GetYaxis().SetTitle(axis_titleY)
        if(len(axis_titleY) > 35): 
           stack.GetYaxis().SetTitleSize(0.04)
        else:
           stack.GetYaxis().SetTitleSize(0.06)
        stack.GetYaxis().SetTitleFont(22)
        stack.GetYaxis().SetLabelSize(0.04)

        # Setting X axis label
        stack.GetXaxis().SetTitle("log10(DJR"+str(index)+")")
        stack.GetXaxis().SetTitleSize(0.06)
        stack.GetXaxis().SetTitleFont(22)
        stack.GetXaxis().SetLabelSize(0.04)

        # Setting Log scale
        if ntot != 0:
            canvas.SetLogy()
        
        # Displaying a legend
        ymin_legend = .9-.055*(len(DJRplots)-1)
        if ymin_legend<0.1:
            ymin_legend = 0.1
        legend = TLegend(.65,ymin_legend,.9,.9)
        legend.SetTextSize(0.04); 
        legend.SetTextFont(22);
        legend.AddEntry(DJRplots[0].myhisto,"Sum")
        for ind in range(1,len(DJRplots)):
            legend.AddEntry(DJRplots[ind].myhisto,str(ind-1)+"-jet sample")
        legend.SetFillColor(0)    
        legend.Draw()

        # Save the canvas in the report format
        datasetname = InstanceName.Get(dataset.name)

        canvas.SaveAs(output_path+"/merging_" +\
                      datasetname+"_"+str(index)+"." +\
                      ReportFormatType.convert2filetype(mode))

        # Save the canvas in the C format
        canvas.SaveAs(output_path+"/merging_" +\
                      datasetname+"_"+str(index)+".C")

            
    def GetPlotNames(self,mode,output_path):

        allnames = []
        
        # Loop on each dataset
        for mydataset in range(0,len(self.main.datasets)):

            datasetname = InstanceName.Get(self.main.datasets[mydataset].name)
            names = []

            # Loop over DJR
            for i in range(0,100):
            
                name = "DJR"+str(i+1)+"_total"
                test=False
                for h in range(len(self.detail[mydataset])):
                    if self.detail[mydataset][h].name == name:
                        test=True
                        break

                if test:
                    names.append(output_path+"/merging_" +\
                      datasetname+"_"+str(i+1))#+"." +\
#                      ReportFormatType.convert2filetype(mode))

        allnames.append(names)
        return allnames
    
        
