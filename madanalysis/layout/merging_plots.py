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
            
        self.datasetnames = []
        self.filenames    = []


    def Initialize(self):

        # Creating plots
        for i in range(0,len(self.detail)):
            self.detail[i].FinalizeReading() 
            self.detail[i].CreateHistogram()


    def DrawAll(self,mode,output_path):

        # Reset Configuration
        RootConfig.Init()
        for i in range(0,len(self.main.datasets)):
            self.DrawDatasetPlots(self.files[i],self.main.datasets[i].name,self.main.datasets[i],mode,output_path)


    def DrawDatasetPlots(self,file,datasetname,dataset,mode,output_path):

        test = False
        outputs = []
        
        # Loop over DJR plot
        for i in range(0,100):

            # Getting 'total' plot
            total = file.Get("merging/DJR"+str(i+1)+"_total_pos","TH1F",False)
            if total is None:
                break

            # Getting list of 'jet contribution' plot
            jetplots = []
            for j in range(0,100):

                # Getting j-th plot
                jetplot = file.Get("merging/DJR"+str(i+1)+"_"+str(j)+"jet_pos","TH1F",False)
                if jetplot is None:
                    break
                else:
                    test=True
                    jetplots.append(jetplot)
                
            # Drawing plots
            self.DrawPlot(datasetname,i+1,total,jetplots,dataset,mode,output_path)

            outputs.append(self.output_path+"/merging_"+datasetname+"_"+str(i+1))            

        # Adding file
        if test:
            self.datasetnames.append(datasetname)
            self.filenames.append(outputs)


            
    def DrawPlot(self,datasetname,index,total,jetplots,dataset,mode,output_path):

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

        # Setting color and other settings to total
        total.SetLineColor(1)
        total.SetLineStyle(1)
        if total.GetEntries()!=0:
            total.Scale( float(xsection) / float(total.GetEntries()) )
            
        for ind in range(0,len(jetplots)):
            jetplots[ind].SetLineStyle(2)
            if jetplots[ind].GetEntries()!=0:
                jetplots[ind].Scale( float(xsection) / float(total.GetEntries()) )

        # Setting color an other settings to jet plots    
        color=1
        if len(jetplots)==1:
            jetplots[0].SetLineColor(9)
        elif len(jetplots)==2:
            jetplots[0].SetLineColor(9)
            jetplots[1].SetLineColor(46)
        elif len(jetplots)==3:
            jetplots[0].SetLineColor(9)
            jetplots[1].SetLineColor(46)
            jetplots[2].SetLineColor(8)
        elif len(jetplots)==4:
            jetplots[0].SetLineColor(9)
            jetplots[1].SetLineColor(46)
            jetplots[2].SetLineColor(8)
            jetplots[3].SetLineColor(4)
        elif len(jetplots)==5:
            jetplots[0].SetLineColor(9)
            jetplots[1].SetLineColor(46)
            jetplots[2].SetLineColor(8)
            jetplots[3].SetLineColor(4)
            jetplots[4].SetLineColor(6)
        elif len(jetplots)==6:
            jetplots[0].SetLineColor(9)
            jetplots[1].SetLineColor(46)
            jetplots[2].SetLineColor(8)
            jetplots[3].SetLineColor(4)
            jetplots[4].SetLineColor(6)
            jetplots[5].SetLineColor(2)
        elif len(jetplots)==7:
            jetplots[0].SetLineColor(9)
            jetplots[1].SetLineColor(46)
            jetplots[2].SetLineColor(8)
            jetplots[3].SetLineColor(4)
            jetplots[4].SetLineColor(6)
            jetplots[5].SetLineColor(2)
            jetplots[6].SetLineColor(7)
        elif len(jetplots)==8:
            jetplots[0].SetLineColor(9)
            jetplots[1].SetLineColor(46)
            jetplots[2].SetLineColor(8)
            jetplots[3].SetLineColor(4)
            jetplots[4].SetLineColor(6)
            jetplots[5].SetLineColor(2)
            jetplots[6].SetLineColor(7)
            jetplots[7].SetLineColor(3)
        elif len(jetplots)==9:
            jetplots[0].SetLineColor(9)
            jetplots[1].SetLineColor(46)
            jetplots[2].SetLineColor(8)
            jetplots[3].SetLineColor(4)
            jetplots[4].SetLineColor(6)
            jetplots[5].SetLineColor(2)
            jetplots[6].SetLineColor(7)
            jetplots[7].SetLineColor(3)
            jetplots[8].SetLineColor(42)
        elif len(jetplots)==10:
            jetplots[0].SetLineColor(9)
            jetplots[1].SetLineColor(46)
            jetplots[2].SetLineColor(8)
            jetplots[3].SetLineColor(4)
            jetplots[4].SetLineColor(6)
            jetplots[5].SetLineColor(2)
            jetplots[6].SetLineColor(7)
            jetplots[7].SetLineColor(3)
            jetplots[8].SetLineColor(42)
            jetplots[9].SetLineColor(48)
        else:
            jetplots[ind].SetLineColor(color)
            color += 1

        # Creating and filling the stack; computing the total number of events
        stack = THStack("mystack","")
        ntot = total.GetEntries()
        stack.Add(total)
        for item in jetplots:
            ntot+=item.GetEntries()
            stack.Add(item)

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
        ymin_legend = .9-.055*len(jetplots)
        if ymin_legend<0.1:
            ymin_legend = 0.1
        legend = TLegend(.65,ymin_legend,.9,.9)
        legend.SetTextSize(0.04); 
        legend.SetTextFont(22);
        legend.AddEntry(total,"Sum")
        for ind in range(0,len(jetplots)):
            legend.AddEntry(jetplots[ind],str(ind)+"-jet sample")
        legend.SetFillColor(0)    
        legend.Draw()

        # Save the canvas in the report format
        canvas.SaveAs(output_path+"/merging_"+datasetname+"_"+str(index)+"."+\
                      ReportFormatType.convert2filetype(mode))

        # Save the canvas in the C format
        canvas.SaveAs(output_path+"/merging_"+datasetname+"_"+str(index)+".C")

            
                   
