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


from madanalysis.selection.instance_name           import InstanceName
from madanalysis.enumeration.uncertainty_type      import UncertaintyType
from madanalysis.enumeration.normalize_type        import NormalizeType
from madanalysis.enumeration.report_format_type    import ReportFormatType
from madanalysis.enumeration.observable_type       import ObservableType
from madanalysis.enumeration.color_type            import ColorType
from madanalysis.enumeration.linestyle_type        import LineStyleType
from madanalysis.enumeration.backstyle_type        import BackStyleType
from madanalysis.enumeration.stacking_method_type  import StackingMethodType
from madanalysis.layout.merging_plots_for_dataset  import MergingPlotsForDataset 
import madanalysis.enumeration.color_hex
from math import sqrt
import logging

class MergingPlots:

    counter = 0

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


    def DrawAll(self,histo_path,modes,output_paths,ListROOTplots):

        # Loop on each dataset
        rootfiles=[]
        for i in range(0,len(self.main.datasets)):
            self.DrawDatasetPlots(self.detail[i],\
                                  self.main.datasets[i],\
                                  histo_path,modes,output_paths,\
                                  rootfiles)

        # Saving files
        for item in rootfiles:
            ListROOTplots.append(item)

    def DrawDatasetPlots(self,histos,dataset,histo_path,modes,output_paths,rootfiles):

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

            # Save the canvas in the report format
            datasetname = InstanceName.Get(dataset.name)
            index=i+1

            filenameC = histo_path+"/merging_" +\
                        datasetname+"_"+str(index)
            rootfiles.append(filenameC)
            filenamePy = filenameC+'.py'
            filenameC  += '.C'
            output_files=[]
            for iout in range(0,len(output_paths)):
                output_files.append(output_paths[iout]+\
                                    "/merging_" +\
                                    datasetname+"_"+str(index)+"." +\
                                    ReportFormatType.convert2filetype(modes[iout]))
            # Drawing
            logging.getLogger('MA5').debug('Producing file '+filenameC+' ...')
            self.DrawROOT(DJRplots,dataset,filenameC,output_files,index)
            logging.getLogger('MA5').debug('Producing file '+filenamePy+' ...')
            self.DrawMATPLOTLIB(DJRplots,dataset,filenamePy,output_files,index)


            
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
    

    def DrawROOT(self,DJRplots,dataset,filenameC,output_files,index):

        # Open the file in write-mode
        try:
            outputC = file(filenameC,'w')
        except:
            logging.getLogger('MA5').error('Impossible to write the file: '+filenameC)
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
        MergingPlots.counter=MergingPlots.counter+1
        canvas_name='canvas_merging_tempo'+str(MergingPlots.counter)
        outputC.write('  // Creating a new TCanvas\n')
        outputC.write('  TCanvas* canvas = new TCanvas("'+canvas_name+'","'+canvas_name+'",0,0,1000,500);\n')
        outputC.write('  gStyle->SetOptStat(0);\n')
        outputC.write('  gStyle->SetOptTitle(0);\n')
        outputC.write('  canvas->SetHighLightColor(2);\n')
        outputC.write('  canvas->SetFillColor(0);\n')
        outputC.write('  canvas->SetBorderMode(0);\n')
        outputC.write('  canvas->SetBorderSize(3);\n')
        outputC.write('  canvas->SetFrameBorderMode(0);\n')
        outputC.write('  canvas->SetFrameBorderSize(0);\n')
        outputC.write('  canvas->SetTickx(1);\n')
        outputC.write('  canvas->SetTicky(1);\n')
        outputC.write('  canvas->SetLeftMargin(0.14);\n')
        outputC.write('  canvas->SetRightMargin(0.30);\n')
        outputC.write('  canvas->SetBottomMargin(0.15);\n')
        outputC.write('  canvas->SetTopMargin(0.05);\n')
        outputC.write('\n')

        # Getting xsection
        xsection=dataset.measured_global.xsection
        if dataset.xsection!=0.:
            xsection=dataset.xsection

        # Scaling the total plot
        scales=[]
        if DJRplots[0].summary.nentries!=0:
            scales.append( float(xsection) / \
                           float(DJRplots[0].summary.nentries) )
        else:
            scales.append(1.)

        # Loop over other DJR plots    
        for ind in range(1,len(DJRplots)):
            if DJRplots[ind].summary.nentries!=0:
                scales.append( float(xsection) / \
                               float(DJRplots[0].summary.nentries) )
            else:
                scales.append(1.)

        # Loop over datasets and histos
        for ind in range(0,len(DJRplots)):

            # Creating TH1F
            outputC.write('  // Creating a new TH1F\n')
            histoname=DJRplots[ind].name
            xmin=DJRplots[ind].xmin
            xmax=DJRplots[ind].xmax
            ymin=DJRplots[ind].ymin
            ymax=DJRplots[ind].ymax
            xnbin=DJRplots[ind].nbins
            outputC.write('  TH1F* '+histoname+' = new TH1F("'+histoname+\
                          '_'+str(MergingPlots.counter)+'","'+\
                          histoname+'",'+str(xnbin)+','+\
                          str(xmin)+','+str(xmax)+');\n')

            # TH1F content
            outputC.write('  // Content\n')
            outputC.write('  '+histoname+'->SetBinContent(0'+\
                          ','+str(DJRplots[ind].summary.underflow*scales[ind])+'); // underflow\n')
            for bin in range(1,xnbin+1):
                outputC.write('  '+histoname+'->SetBinContent('+str(bin)+\
                              ','+str(DJRplots[ind].summary.array[bin-1]*scales[ind])+');\n')
            nentries=DJRplots[ind].summary.nentries
            outputC.write('  '+histoname+'->SetBinContent('+str(xnbin+1)+\
                          ','+str(DJRplots[ind].summary.overflow*scales[ind])+'); // overflow\n')
            outputC.write('  '+histoname+'->SetEntries('+str(nentries)+');\n')

            # Setting color an other settings to jet plots    
            mycolors=[1,9,46,8,4,6,2,7,3,42,48]
            outputC.write('  '+histoname+'->SetLineColor('+str(mycolors[ind])+');\n')
            if ind==0:
                 outputC.write('  '+histoname+'->SetLineStyle(1);\n')
            else:
                 outputC.write('  '+histoname+'->SetLineStyle(2);\n')
            outputC.write('\n')
            

        # Creating the THStack
        outputC.write('  // Creating a new THStack\n')
        outputC.write('  THStack* stack = new THStack("mystack_'+str(MergingPlots.counter)+'","mystack");\n')

        # Loop over datasets and histos
        ntot=DJRplots[0].summary.integral
        for ind in range(0,len(DJRplots)):
            histoname=DJRplots[ind].name
            outputC.write('  stack->Add('+histoname+');\n')
        outputC.write('  stack->Draw("nostack");\n')
        outputC.write('\n')

        # Setting Y axis label
        outputC.write('  // Y axis\n')
        axis_titleY = "Cross section (pb/bin)"
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
        if DJRplots[ind].ymin!=[]:
            outputC.write('  stack->SetMinimum('+DJRplots[ind].ymin+');\n')
        if DJRplots[ind].ymax!=[]:
            outputC.write('  stack->SetMinimum('+DJRplots[ind].ymax+');\n')
        outputC.write('\n')

        # Setting X axis label
        outputC.write('  // X axis\n')
        axis_titleX = "log10(DJR"+str(index)+")"
        outputC.write('  stack->GetXaxis()->SetLabelSize(0.04);\n')
        outputC.write('  stack->GetXaxis()->SetLabelOffset(0.005);\n')
        outputC.write('  stack->GetXaxis()->SetTitleSize(0.06);\n')
        outputC.write('  stack->GetXaxis()->SetTitleFont(22);\n')
        outputC.write('  stack->GetXaxis()->SetTitleOffset(1);\n')
        outputC.write('  stack->GetXaxis()->SetTitle("'+axis_titleX+'");\n')
        outputC.write('\n')

        # Setting Log scale
        outputC.write('  // Finalizing the TCanvas\n')
        logy=0
        if ntot!=0:
            logy=1
        outputC.write('  canvas->SetLogx(0);\n')
        outputC.write('  canvas->SetLogy('+str(logy)+');\n')
        outputC.write('\n')

        # Displaying a legend
        outputC.write('  // Creating a TLegend\n')
        outputC.write('  TLegend* legend = new TLegend(.73,.5,.97,.95);\n')
        outputC.write('  legend->AddEntry('+DJRplots[0].name+',"Sum");\n')
        for ind in range(1,len(DJRplots)):
                histoname=DJRplots[ind].name
                nicetitle=str(ind-1)+"-jet sample"
                outputC.write('  legend->AddEntry('+histoname+',"'+nicetitle+'");\n')
        outputC.write('  legend->SetFillColor(0);\n')
        outputC.write('  legend->SetTextSize(0.04);\n')
        outputC.write('  legend->SetTextFont(22);\n')
        outputC.write('  legend->SetY1(TMath::Max(0.15,0.97-0.10*legend->GetListOfPrimitives()->GetSize()));\n')
        outputC.write('  legend->Draw();\n')
        outputC.write('\n')

        # Producing the image
        outputC.write('  // Saving the image\n')
        for outputname in output_files:
            outputC.write('  canvas->SaveAs("'+outputname+'");\n')
        outputC.write('\n')

        # File foot
        outputC.write('}\n')

        # Close the file
        try:
            outputC.close()
        except:
            logging.getLogger('MA5').error('Impossible to close the file: '+outputC)
            return False

        # Ok
        return True

    def DrawMATPLOTLIB(self,DJRplots,dataset,filenamePy,output_files,index):

        # Open the file in write-mode
        try:
            outputPy = file(filenamePy,'w')
        except:
            logging.getLogger('MA5').error('Impossible to write the file: '+filenamePy)
            return False

        # File header
        function_name = filenamePy[:-3]
        function_name = function_name.split('/')[-1]
        outputPy.write('def '+function_name+'():\n')
        outputPy.write('\n')

        # Import Libraries
        outputPy.write('    # Library import\n')
        outputPy.write('    import numpy\n')
        outputPy.write('    import matplotlib\n')
        outputPy.write('    import matplotlib.pyplot   as plt\n')
        outputPy.write('    import matplotlib.gridspec as gridspec\n')
        outputPy.write('\n')

        # Matplotlib & numpy version
        outputPy.write('    # Library version\n')
        outputPy.write('    matplotlib_version = matplotlib.__version__\n')
        outputPy.write('    numpy_version      = numpy.__version__\n')
        outputPy.write('\n')


        # Getting xsection
        xsection=dataset.measured_global.xsection
        if dataset.xsection!=0.:
            xsection=dataset.xsection

        # Scaling the total plot
        scales=[]
        if DJRplots[0].summary.nentries!=0:
            scales.append( float(xsection) / \
                           float(DJRplots[0].summary.nentries) )
        else:
            scales.append(1.)

        # Loop over other DJR plots    
        for ind in range(1,len(DJRplots)):
            if DJRplots[ind].summary.nentries!=0:
                scales.append( float(xsection) / \
                               float(DJRplots[0].summary.nentries) )
            else:
                scales.append(1.)

        # Binnning and x-axis
        xmin=DJRplots[0].xmin
        xmax=DJRplots[0].xmax
        ymin=DJRplots[0].ymin
        ymax=DJRplots[0].ymax
        xnbin=DJRplots[0].nbins
        outputPy.write('    # Histo binning\n')
        outputPy.write('    xBinning = numpy.linspace('+\
                       str(xmin)+','+str(xmax)+','+str(xnbin+1)+\
                       ',endpoint=True)\n')
        outputPy.write('\n')

        outputPy.write('    # Creating data sequence: middle of each bin\n')
        outputPy.write('    xData = numpy.array([')
        for bin in range(0,xnbin):
            if bin!=0:
                outputPy.write(',')
            outputPy.write(str(DJRplots[ind].GetBinMean(bin)))
        outputPy.write('])\n\n')

        # Loop over datasets and histos
        for ind in range(0,len(DJRplots)):
            # Creating Histogram data
            histoname=DJRplots[ind].name
            outputPy.write('    # Creating weights for histo: '+histoname+'\n')
            outputPy.write('    '+histoname+'_'+str(MergingPlots.counter)+'_weights = numpy.array([')
            ntot=0
            for bin in range(1,xnbin+1):
                ntot+=DJRplots[ind].summary.array[bin-1]*scales[ind]
                if bin!=1:
                    outputPy.write(',')
                outputPy.write(str(DJRplots[ind].summary.array[bin-1]*scales[ind]))
            outputPy.write('])\n')

        # Canvas
        outputPy.write('\n    # Creating a new Canvas\n')
        dpi=80
        height=500
        widthx=1000
        outputPy.write('    fig   = plt.figure(figsize=('+\
           str(widthx/dpi)+','+str(height/dpi)+\
           '),dpi='+str(dpi)+')\n')
        outputPy.write('    frame = gridspec.GridSpec(1,1,right=0.7)\n')
        outputPy.write('    pad   = fig.add_subplot(frame[0])\n\n')

        ## Curves
        colors=[1,9,46,8,4,6,2,7,3,42,48]
        for ind in range(0,len(DJRplots)):
            histoname=DJRplots[ind].name
            linecolor='"'+madanalysis.enumeration.color_hex.color_hex[colors[ind]]+'"'
            if ind==0:
                linestyle='\"solid\"'
            else:
                linestyle='\"dashed\"'

            outputPy.write('    pad.hist('+\
                'x=xData, '+\
                'bins=xBinning, '+\
                'weights='+histoname+'_'+str(MergingPlots.counter)+'_weights,\\\n')
            if ind==0:
                outputPy.write('             label=\'Sum\', ')
            else:
                outputPy.write('             label=\''+str(ind-1)+'-jet sample\', ')
            outputPy.write('rwidth=0.8,\\\n'+\
                '             color='+linecolor+', '+\
                'edgecolor='+linecolor+', '+\
                'linewidth=1, '+\
                'linestyle='+linestyle+',\\\n'+\
                '             bottom=None, '+\
                'cumulative=False, normed=False, ' +\
                'align="mid", orientation="vertical")\n\n')

        # Setting X axis label
        axis_titleX = "log10(DJR"+str(ind)+")"
        outputPy.write('    # Axes\n')
        outputPy.write("    plt.rc('text',usetex=False)\n")
        outputPy.write('    plt.xlabel(r"'+axis_titleX+'",\\\n')
        outputPy.write('               fontsize=16,color="black")\n')
        # Setting Y axis label
        axis_titleY = "Cross section (pb/bin)"
        outputPy.write('    plt.ylabel(r"'+axis_titleY+'",\\\n')
        outputPy.write('               fontsize=16,color="black")\n')
        outputPy.write('\n')
        wname = DJRplots[0].name+'_'+str(MergingPlots.counter)+'_weights'
        if DJRplots[ind].ymax==[]:
            outputPy.write('    ymax='+wname+'.max()*1.1\n')
        else:
            outputPy.write('    ymax='+str(DJRplots[ind].ymax)+'\n')
        if DJRplots[ind].ymin==[]:
            minweight='[x for x in ('+wname+') if x]'
            outputPy.write('    ymin=min('+minweight+')/100\n')
        else:
            outputPy.write('    ymin='+str(DJRplots[ind].ymin)+'\n')
        outputPy.write('    plt.gca().set_ylim(ymin,ymax)\n')
        outputPy.write('    plt.gca().set_yscale("log",nonposy="clip")\n')
        outputPy.write('\n\n')

        # Displaying a legend
        outputPy.write('    # Legend\n')
        outputPy.write('    plt.legend(bbox_to_anchor=(1.05,1), loc=2,'+\
                                ' borderaxespad=0.)\n\n')

        # Producing the image
        outputPy.write('    # Saving the image\n')
        for outputname in output_files:
            outputPy.write("    plt.savefig('"+outputname+"')\n")
        outputPy.write('\n')

        # Call the function
        outputPy.write('# Running!\n')
        outputPy.write("if __name__ == '__main__':\n")
        outputPy.write('    '+function_name+'()\n')

        # Close the file
        try:
            outputPy.close()
        except:
            logging.getLogger('MA5').error('Impossible to close the file: '+outputPy)
            return False

        # Ok
        return True

