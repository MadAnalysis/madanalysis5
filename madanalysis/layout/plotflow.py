################################################################################
#
#  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#
#  This file is part of MadAnalysis 5.
#  Official website: <https://github.com/MadAnalysis/madanalysis5>
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


from __future__ import absolute_import

import logging

import numpy as np
import six
from six.moves import range

import madanalysis.enumeration.color_hex
from madanalysis.enumeration.backstyle_type import BackStyleType
from madanalysis.enumeration.color_type import ColorType
from madanalysis.enumeration.linestyle_type import LineStyleType
from madanalysis.enumeration.normalize_type import NormalizeType
from madanalysis.enumeration.report_format_type import ReportFormatType
from madanalysis.enumeration.stacking_method_type import StackingMethodType
from madanalysis.layout.plotflow_for_dataset import PlotFlowForDataset

# pylint: disable=C0200,C0103


class PlotFlow:

    diconicetitle = {" ^ {": "^{", " _ {": "_{", "\\\\": "#"}

    counter = 0

    def __init__(self, main):
        self.main = main
        self.detail = []
        for i in range(0, len(main.datasets)):
            self.detail.append(PlotFlowForDataset(main, main.datasets[i]))

    def Initialize(self):

        # Initializing NPID
        if len(self.detail) > 0:
            for ihisto in range(0, len(self.detail[0])):
                if (
                    self.detail[0].histos[ihisto].__class__.__name__
                    == "HistogramFrequency"
                ):
                    self.InitializeHistoFrequency(ihisto)

        # Creating plots
        for i in range(0, len(self.detail)):
            self.detail[i].FinalizeReading()
            self.detail[i].ComputeScale()
            self.detail[i].CreateHistogram()

    def InitializeHistoFrequency(self, ihisto):

        # New collection of labels
        newlabels = []

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
            array_positive = []
            array_negative = []

            # Loop over the new labels
            for newlabel in newlabels:

                # Loop over the old labels
                found = False
                value_positive = 0
                value_negative = 0
                for i in range(len(histo[ihisto].labels)):

                    if newlabel == histo[ihisto].labels[i]:
                        value_positive = histo[ihisto].positive.array[i]
                        value_negative = histo[ihisto].negative.array[i]
                        found = True
                        break

                # Fill
                if found:
                    array_positive.append(value_positive)
                    array_negative.append(value_negative)
                else:
                    array_positive.append(0.0)
                    array_negative.append(0.0)

            # save result
            # PS: [:] -> clone the arrays
            histo[ihisto].positive.array = array_positive[:]
            histo[ihisto].negative.array = array_negative[:]
            histo[ihisto].labels = newlabels[:]

    @staticmethod
    def NiceTitle(text):
        newtext = text
        for i, j in six.iteritems(PlotFlow.diconicetitle):
            newtext = newtext.replace(i, j)
        return newtext

    @staticmethod
    def NiceTitleMatplotlib(text):
        text = PlotFlow.NiceTitle(text)
        text = text.replace("#DeltaR", "#Delta R")
        text = "$" + text.replace("#", "\\\\") + "$"
        return text

    def DrawAll(self, histo_path, modes, output_paths, ListROOTplots):
        # Loop on each histo type
        irelhisto = 0
        for iabshisto in range(0, len(self.main.selection)):
            if self.main.selection[iabshisto].__class__.__name__ != "Histogram":
                continue
            self.color = 1
            histos = []
            scales = []
            multiweight_histos = []

            # Name of output files
            filenameC = histo_path + "/selection_" + str(irelhisto) + ".C"
            filenamePy = histo_path + "/selection_" + str(irelhisto) + ".py"

            output_files = []
            for iout in range(0, len(output_paths)):
                output_files.append(
                    "../../"
                    + output_paths[iout].split("/")[-2]
                    + "/"
                    + output_paths[iout].split("/")[-1]
                    + "/selection_"
                    + str(irelhisto)
                    + "."
                    + ReportFormatType.convert2filetype(modes[iout])
                )

            for iset in range(0, len(self.detail)):
                # Appending histo
                histos.append(self.detail[iset][irelhisto])
                #               if mode==2:
                scales.append(self.detail[iset][irelhisto].scale)
                multiweight_histos.append(self.detail[iset].multiweight_histos[irelhisto])
            #               else:
            #                   scales.append(1)

            logging.getLogger("MA5").debug("Producing file " + filenameC + " ...")
            self.DrawROOT(
                histos,
                scales,
                self.main.selection[iabshisto],
                irelhisto,
                filenameC,
                output_files,
            )

            logging.getLogger("MA5").debug("Producing file " + filenamePy + " ...")
            self.DrawMATPLOTLIB(
                histos,
                scales,
                self.main.selection[iabshisto],
                irelhisto,
                filenamePy,
                output_files,
            )

            logging.getLogger("MA5").debug("Producing file " + filenamePy + " ...")
            self.DrawMULTIWEIGHT(
                multiweight_histos,
                self.main.selection[iabshisto],
                filenamePy,
                output_files,
            )

            irelhisto += 1

        # Save ROOT files
        for ind in range(0, irelhisto):
            ListROOTplots.append(histo_path + "/selection_" + str(ind))

        return True

    def DrawROOT(self, histos, scales, ref, irelhisto, filenameC, outputnames):

        # Is there any legend?
        legendmode = False
        if len(self.main.datasets) > 1:
            legendmode = True

        # Type of histogram
        frequencyhisto = True
        for histo in histos:
            if histo.__class__.__name__ != "HistogramFrequency":
                frequencyhisto = False
                break
        logxhisto = True
        for histo in histos:
            if histo.__class__.__name__ != "HistogramLogX":
                logxhisto = False
                break

        # Stacking or superimposing histos ?
        stackmode = False
        if ref.stack == StackingMethodType.STACK or (
            ref.stack == StackingMethodType.AUTO
            and self.main.stack == StackingMethodType.STACK
        ):
            stackmode = True

        # Open the file in write-mode
        try:
            outputC = open(filenameC, "w")
        except:
            logging.getLogger("MA5").error("Impossible to write the file: " + filenameC)
            return False

        # File header
        function_name = filenameC[:-2]
        function_name = function_name.split("/")[-1]
        outputC.write("void " + function_name + "()\n")
        outputC.write("{\n\n")

        # ROOT version
        outputC.write("  // ROOT version\n")
        outputC.write("  Int_t root_version = gROOT->GetVersionInt();\n")
        outputC.write("\n")

        # Creating the TCanvas
        PlotFlow.counter = PlotFlow.counter + 1
        canvas_name = "canvas_plotflow_tempo" + str(PlotFlow.counter)
        outputC.write("  // Creating a new TCanvas\n")
        widthx = 700
        if legendmode:
            widthx = 1000
        outputC.write(
            '  TCanvas* canvas = new TCanvas("'
            + canvas_name
            + '","'
            + canvas_name
            + '",0,0,'
            + str(widthx)
            + ",500);\n"
        )
        outputC.write("  gStyle->SetOptStat(0);\n")
        outputC.write("  gStyle->SetOptTitle(0);\n")
        outputC.write("  canvas->SetHighLightColor(2);\n")
        #       outputC.write('  canvas->Range(-2.419355,-0.005372711,16.93548,0.03939988);\n')
        outputC.write("  canvas->SetFillColor(0);\n")
        outputC.write("  canvas->SetBorderMode(0);\n")
        outputC.write("  canvas->SetBorderSize(3);\n")
        outputC.write("  canvas->SetFrameBorderMode(0);\n")
        outputC.write("  canvas->SetFrameBorderSize(0);\n")
        outputC.write("  canvas->SetTickx(1);\n")
        outputC.write("  canvas->SetTicky(1);\n")
        outputC.write("  canvas->SetLeftMargin(0.14);\n")
        margin = 0.05
        if legendmode:
            margin = 0.3
        outputC.write("  canvas->SetRightMargin(" + str(margin) + ");\n")
        outputC.write("  canvas->SetBottomMargin(0.15);\n")
        outputC.write("  canvas->SetTopMargin(0.05);\n")
        outputC.write("\n")

        # Binning
        xnbin = histos[0].nbins
        if logxhisto:
            outputC.write("  // Histo binning\n")
            outputC.write("  Double_t xBinning[" + str(xnbin + 1) + "] = {")
            for bin in range(1, xnbin + 2):
                if bin != 1:
                    outputC.write(",")
                outputC.write(str(histos[0].GetBinLowEdge(bin)))
            outputC.write("};\n")
            outputC.write("\n")

        # Loop over datasets and histos
        ntot = 0
        for ind in range(0, len(histos)):

            # Creating TH1F
            outputC.write("  // Creating a new TH1F\n")
            histoname = "S" + histos[ind].name + "_" + str(ind)
            xmin = histos[ind].xmin
            xmax = histos[ind].xmax
            if logxhisto:
                outputC.write(
                    "  TH1F* "
                    + histoname
                    + ' = new TH1F("'
                    + histoname
                    + '","'
                    + histoname
                    + '",'
                    + str(xnbin)
                    + ",xBinning);\n"
                )
            else:
                outputC.write(
                    "  TH1F* "
                    + histoname
                    + ' = new TH1F("'
                    + histoname
                    + '","'
                    + histoname
                    + '",'
                    + str(xnbin)
                    + ","
                    + str(xmin)
                    + ","
                    + str(xmax)
                    + ");\n"
                )

            # TH1F content
            outputC.write("  // Content\n")
            outputC.write(
                "  "
                + histoname
                + "->SetBinContent(0"
                + ","
                + str(histos[ind].summary.underflow * scales[ind])
                + "); // underflow\n"
            )
            for bin in range(1, xnbin + 1):
                ntot += histos[ind].summary.array[bin - 1] * scales[ind]
                outputC.write(
                    "  "
                    + histoname
                    + "->SetBinContent("
                    + str(bin)
                    + ","
                    + str(histos[ind].summary.array[bin - 1] * scales[ind])
                    + ");\n"
                )
            nentries = histos[ind].summary.nentries
            outputC.write(
                "  "
                + histoname
                + "->SetBinContent("
                + str(xnbin + 1)
                + ","
                + str(histos[ind].summary.overflow * scales[ind])
                + "); // overflow\n"
            )
            outputC.write("  " + histoname + "->SetEntries(" + str(nentries) + ");\n")

            # reset
            linecolor = 0
            linestyle = 0
            backcolor = 0
            backstyle = 0
            linewidth = 1

            # Setting AUTO settings
            if len(histos) == 1:
                linecolor1 = [9]
                linecolor = linecolor1[ind]
                if stackmode:
                    backstyle1 = [3004]
                    backstyle = backstyle1[ind]
                    backcolor = linecolor1[ind]
            elif len(histos) == 2:
                linecolor2 = [9, 46]
                linecolor = linecolor2[ind]
                if stackmode:
                    backstyle2 = [3004, 3005]
                    backstyle = backstyle2[ind]
                    backcolor = linecolor2[ind]
            elif len(histos) == 3:
                linecolor3 = [9, 46, 8]
                linecolor = linecolor3[ind]
                if stackmode:
                    backstyle3 = [3004, 3005, 3006]
                    backstyle = backstyle3[ind]
                    backcolor = linecolor3[ind]
            elif len(histos) == 4:
                linecolor4 = [9, 46, 8, 4]
                linecolor = linecolor4[ind]
                if stackmode:
                    backstyle4 = [3004, 3005, 3006, 3007]
                    backstyle = backstyle4[ind]
                    backcolor = linecolor4[ind]
            elif len(histos) == 5:
                linecolor5 = [9, 46, 8, 4, 6]
                linecolor = linecolor5[ind]
                if stackmode:
                    backstyle5 = [3004, 3005, 3006, 3007, 3013]
                    backstyle = backstyle5[ind]
                    backcolor = linecolor5[ind]
            elif len(histos) == 6:
                linecolor6 = [9, 46, 8, 4, 6, 2]
                linecolor = linecolor6[ind]
                if stackmode:
                    backstyle6 = [3004, 3005, 3006, 3007, 3013, 3017]
                    backstyle = backstyle6[ind]
                    backcolor = linecolor6[ind]
            elif len(histos) == 7:
                linecolor7 = [9, 46, 8, 4, 6, 2, 7]
                linecolor = linecolor7[ind]
                if stackmode:
                    backstyle7 = [3004, 3005, 3006, 3007, 3013, 3017, 3022]
                    backstyle = backstyle7[ind]
                    backcolor = linecolor7[ind]
            elif len(histos) == 8:
                linecolor8 = [9, 46, 8, 4, 6, 2, 7, 3]
                linecolor = linecolor8[ind]
                if stackmode:
                    backstyle8 = [3004, 3005, 3006, 3007, 3013, 3017, 3022, 3315]
                    backstyle = backstyle8[ind]
                    backcolor = linecolor8[ind]
            elif len(histos) == 9:
                linecolor9 = [9, 46, 8, 4, 6, 2, 7, 3, 42]
                linecolor = linecolor9[ind]
                if stackmode:
                    backstyle9 = [3004, 3005, 3006, 3007, 3013, 3017, 3022, 3315, 3351]
                    backstyle = backstyle9[ind]
                    backcolor = linecolor9[ind]
            elif len(histos) == 10:
                linecolor10 = [9, 46, 8, 4, 6, 2, 7, 3, 42, 48]
                linecolor = linecolor10[ind]
                if stackmode:
                    backstyle10 = [
                        3004,
                        3005,
                        3006,
                        3007,
                        3013,
                        3017,
                        3022,
                        3315,
                        3351,
                        3481,
                    ]
                    backstyle = backstyle10[ind]
                    backcolor = linecolor10[ind]
            else:
                linecolor = self.color
                self.color += 1

            # linecolor
            if self.main.datasets[ind].linecolor != ColorType.AUTO:
                linecolor = ColorType.convert2root(
                    self.main.datasets[ind].linecolor, self.main.datasets[ind].lineshade
                )

            # lineStyle
            linestyle = LineStyleType.convert2code(self.main.datasets[ind].linestyle)

            # linewidth
            linewidth = self.main.datasets[ind].linewidth

            # background color
            if self.main.datasets[ind].backcolor != ColorType.AUTO:
                backcolor = ColorType.convert2root(
                    self.main.datasets[ind].backcolor, self.main.datasets[ind].backshade
                )

            # background color
            if self.main.datasets[ind].backstyle != BackStyleType.AUTO:
                backstyle = BackStyleType.convert2code(self.main.datasets[ind].backstyle)

            # style
            outputC.write("  // Style\n")
            outputC.write("  " + histoname + "->SetLineColor(" + str(linecolor) + ");\n")
            outputC.write("  " + histoname + "->SetLineStyle(" + str(linestyle) + ");\n")
            outputC.write("  " + histoname + "->SetLineWidth(" + str(linewidth) + ");\n")
            outputC.write("  " + histoname + "->SetFillColor(" + str(backcolor) + ");\n")
            outputC.write("  " + histoname + "->SetFillStyle(" + str(backstyle) + ");\n")
            if frequencyhisto:
                outputC.write("  " + histoname + "->SetBarWidth(0.8);\n")
                outputC.write("  " + histoname + "->SetBarOffset(0.1);\n")
            outputC.write("\n")

        # Creating the THStack
        outputC.write("  // Creating a new THStack\n")
        PlotFlow.counter += 1
        outputC.write(
            '  THStack* stack = new THStack("mystack_'
            + str(PlotFlow.counter)
            + '","mystack");\n'
        )
        # Loop over datasets and histos
        for ind in range(0, len(histos)):
            histoname = "S" + histos[ind].name + "_" + str(ind)
            outputC.write("  stack->Add(" + histoname + ");\n")

        drawoptions = []
        if not stackmode:
            drawoptions.append("nostack")
        if frequencyhisto:
            drawoptions.append("bar1")
        outputC.write('  stack->Draw("' + "".join(drawoptions) + '");\n')
        outputC.write("\n")

        # Setting Y axis label
        outputC.write("  // Y axis\n")
        axis_titleY = ref.GetYaxis()

        # Scale to one ?
        scale2one = False
        if ref.stack == StackingMethodType.NORMALIZE2ONE or (
            self.main.stack == StackingMethodType.NORMALIZE2ONE
            and ref.stack == StackingMethodType.AUTO
        ):
            scale2one = True

        if scale2one:
            axis_titleY += " ( scaled to one )"
        elif (
            self.main.normalize == NormalizeType.LUMI
            or self.main.normalize == NormalizeType.LUMI_WEIGHT
        ):
            axis_titleY += " ( L_{int} = " + str(self.main.lumi) + " fb^{-1} )"
        elif self.main.normalize == NormalizeType.NONE:
            axis_titleY += " (not normalized)"

        if ref.titleY != "":
            axis_titleY = PlotFlow.NiceTitle(ref.titleY)

        if len(axis_titleY) > 35:
            titlesize = 0.04
        else:
            titlesize = 0.06
        outputC.write("  stack->GetYaxis()->SetLabelSize(0.04);\n")
        outputC.write("  stack->GetYaxis()->SetLabelOffset(0.005);\n")
        outputC.write("  stack->GetYaxis()->SetTitleSize(" + str(titlesize) + ");\n")
        outputC.write("  stack->GetYaxis()->SetTitleFont(22);\n")
        outputC.write("  stack->GetYaxis()->SetTitleOffset(1);\n")
        outputC.write('  stack->GetYaxis()->SetTitle("' + axis_titleY + '");\n')
        if ref.ymin != []:
            outputC.write("  stack->SetMinimum(" + str(ref.ymin) + ");\n")
        if ref.ymax != []:
            outputC.write("  stack->SetMaximum(" + str(ref.ymax) + ");\n")

        outputC.write("\n")
        outputC.write("  // X axis\n")

        # Setting X axis label
        if ref.titleX == "":
            axis_titleX = ref.GetXaxis_Root()
        else:
            axis_titleX = PlotFlow.NiceTitle(ref.titleX)

        # Setting X axis label
        outputC.write("  stack->GetXaxis()->SetLabelSize(0.04);\n")
        outputC.write("  stack->GetXaxis()->SetLabelOffset(0.005);\n")
        outputC.write("  stack->GetXaxis()->SetTitleSize(0.06);\n")
        outputC.write("  stack->GetXaxis()->SetTitleFont(22);\n")
        outputC.write("  stack->GetXaxis()->SetTitleOffset(1);\n")
        outputC.write('  stack->GetXaxis()->SetTitle("' + axis_titleX + '");\n')
        if frequencyhisto:
            for bin in range(1, xnbin + 1):
                outputC.write(
                    "  stack->GetXaxis()->SetBinLabel(" + str(bin) + ","
                    '"' + str(histos[ind].stringlabels[bin - 1]) + '");\n'
                )
        outputC.write("\n")

        # Setting Log scale
        outputC.write("  // Finalizing the TCanvas\n")
        logx = 0
        if ref.logX and ntot != 0:
            logx = 1
        logy = 0
        if ref.logY and ntot != 0:
            logy = 1
        outputC.write("  canvas->SetLogx(" + str(logx) + ");\n")
        outputC.write("  canvas->SetLogy(" + str(logy) + ");\n")
        outputC.write("\n")

        # Displaying a legend
        if legendmode:
            outputC.write("  // Creating a TLegend\n")
            outputC.write("  TLegend* legend = new TLegend(.73,.5,.97,.95);\n")
            for ind in range(0, len(histos)):
                histoname = "S" + histos[ind].name + "_" + str(ind)
                nicetitle = PlotFlow.NiceTitle(self.main.datasets[ind].title)
                outputC.write(
                    "  legend->AddEntry(" + histoname + ',"' + nicetitle + '");\n'
                )
            outputC.write("  legend->SetFillColor(0);\n")
            outputC.write("  legend->SetTextSize(0.05);\n")
            outputC.write("  legend->SetTextFont(22);\n")
            outputC.write(
                "  legend->SetY1(TMath::Max(0.15,0.97-0.10*legend->GetListOfPrimitives()->GetSize()));\n"
            )
            outputC.write("  legend->Draw();\n")
            outputC.write("\n")

        # Producing the image
        outputC.write("  // Saving the image\n")
        for outputname in outputnames:
            outputC.write('  canvas->SaveAs("' + outputname + '");\n')
        outputC.write("\n")

        # File foot
        outputC.write("}\n")

        # Close the file
        try:
            outputC.close()
        except:
            logging.getLogger("MA5").error("Impossible to close the file: " + outputC)
            return False

        # Ok
        return True

    def DrawMATPLOTLIB(self, histos, scales, ref, irelhisto, filenamePy, outputnames):

        # Is there any legend?
        legendmode = False
        if len(self.main.datasets) > 1:
            legendmode = True

        # Type of histogram
        frequencyhisto = True
        for histo in histos:
            if histo.__class__.__name__ != "HistogramFrequency":
                frequencyhisto = False
                break
        logxhisto = True
        for histo in histos:
            if histo.__class__.__name__ != "HistogramLogX":
                logxhisto = False
                break

        # Stacking or superimposing histos ?
        stackmode = False
        if ref.stack == StackingMethodType.STACK or (
            ref.stack == StackingMethodType.AUTO
            and self.main.stack == StackingMethodType.STACK
        ):
            stackmode = True

        # Open the file in write-mode
        try:
            outputPy = open(filenamePy, "w")
        except Exception as err:
            logging.getLogger("MA5").error("Impossible to write the file: " + filenamePy)
            logging.getLogger("MA5").debug(err)
            return False

        # File header
        function_name = filenamePy[:-3]
        function_name = function_name.split("/")[-1]
        outputPy.write("def " + function_name + "():\n")
        outputPy.write("\n")

        # Import Libraries
        outputPy.write("    # Library import\n")
        outputPy.write("    import numpy\n")
        outputPy.write("    import matplotlib\n")
        outputPy.write("    import matplotlib.pyplot   as plt\n")
        outputPy.write("    import matplotlib.gridspec as gridspec\n")
        outputPy.write("\n")

        # Matplotlib & numpy version
        outputPy.write("    # Library version\n")
        outputPy.write("    matplotlib_version = matplotlib.__version__\n")
        outputPy.write("    numpy_version      = numpy.__version__\n")
        outputPy.write("\n")

        # Binning
        # Loop over datasets and histos
        xnbin = histos[0].nbins
        xmin = histos[0].xmin
        xmax = histos[0].xmax
        outputPy.write("    # Histo binning\n")
        if logxhisto:
            outputPy.write("    xBinning = [")
            for bin in range(1, xnbin + 2):
                if bin != 1:
                    outputPy.write(",")
                outputPy.write(str(histos[0].GetBinLowEdge(bin)))
            outputPy.write("]\n")
            outputPy.write("\n")
        else:
            outputPy.write(
                "    xBinning = numpy.linspace("
                + str(xmin)
                + ","
                + str(xmax)
                + ","
                + str(xnbin + 1)
                + ",endpoint=True)\n"
            )
        outputPy.write("\n")

        # Data
        outputPy.write("    # Creating data sequence: middle of each bin\n")
        outputPy.write("    xData = numpy.array([")
        for bin in range(0, xnbin):
            if bin != 0:
                outputPy.write(",")
            outputPy.write(str(histos[0].GetBinMean(bin)))
        outputPy.write("])\n\n")

        # Loop over datasets and histos
        ntot = 0
        for ind in range(0, len(histos)):

            # Creating a new histo
            histoname = "y" + histos[ind].name + "_" + str(ind)
            outputPy.write("    # Creating weights for histo: " + histoname + "\n")
            outputPy.write("    " + histoname + "_weights = numpy.array([")
            for bin in range(1, xnbin + 1):
                ntot += histos[ind].summary.array[bin - 1] * scales[ind]
                if bin != 1:
                    outputPy.write(",")
                outputPy.write(str(histos[ind].summary.array[bin - 1] * scales[ind]))
            outputPy.write("])\n\n")

        # Canvas
        outputPy.write("    # Creating a new Canvas\n")
        dpi = 80
        height = 500
        widthx = 700
        if legendmode:
            widthx = 1000
        outputPy.write(
            "    fig   = plt.figure(figsize=("
            + str(widthx / dpi)
            + ","
            + str(height / dpi)
            + "),dpi="
            + str(dpi)
            + ")\n"
        )
        if not legendmode:
            outputPy.write("    frame = gridspec.GridSpec(1,1)\n")
        else:
            outputPy.write("    frame = gridspec.GridSpec(1,1,right=0.7)\n")
        # subplot argument: nrows, ncols, plot_number
        # outputPy.write('    pad = fig.add_subplot(111)\n')
        outputPy.write("    pad   = fig.add_subplot(frame[0])\n")
        outputPy.write("\n")

        # Stack
        outputPy.write("    # Creating a new Stack\n")
        for ind in range(len(histos) - 1, -1, -1):
            myweight = "y" + histos[ind].name + "_" + str(ind) + "_weights"
            mytitle = (
                '"' + PlotFlow.NiceTitleMatplotlib(self.main.datasets[ind].title) + '"'
            )
            mytitle = mytitle.replace("_", "\_")

            if not stackmode:
                myweights = "y" + histos[ind].name + "_" + str(ind) + "_weights"
            else:
                myweights = ""
                for ind2 in range(0, ind + 1):
                    if ind2 >= 1:
                        myweights += "+"
                    myweights += "y" + histos[ind2].name + "_" + str(ind2) + "_weights"

            # reset
            linecolor = 0
            linestyle = 0
            backcolor = 0
            backstyle = 0
            linewidth = 1

            # Setting AUTO settings
            if len(histos) == 1:
                linecolor1 = [9]
                linecolor = linecolor1[ind]
                if stackmode:
                    backstyle1 = [3004]
                    backstyle = backstyle1[ind]
                    backcolor = linecolor1[ind]
            elif len(histos) == 2:
                linecolor2 = [9, 46]
                linecolor = linecolor2[ind]
                if stackmode:
                    backstyle2 = [3004, 3005]
                    backstyle = backstyle2[ind]
                    backcolor = linecolor2[ind]
            elif len(histos) == 3:
                linecolor3 = [9, 46, 8]
                linecolor = linecolor3[ind]
                if stackmode:
                    backstyle3 = [3004, 3005, 3006]
                    backstyle = backstyle3[ind]
                    backcolor = linecolor3[ind]
            elif len(histos) == 4:
                linecolor4 = [9, 46, 8, 4]
                linecolor = linecolor4[ind]
                if stackmode:
                    backstyle4 = [3004, 3005, 3006, 3007]
                    backstyle = backstyle4[ind]
                    backcolor = linecolor4[ind]
            elif len(histos) == 5:
                linecolor5 = [9, 46, 8, 4, 6]
                linecolor = linecolor5[ind]
                if stackmode:
                    backstyle5 = [3004, 3005, 3006, 3007, 3013]
                    backstyle = backstyle5[ind]
                    backcolor = linecolor5[ind]
            elif len(histos) == 6:
                linecolor6 = [9, 46, 8, 4, 6, 2]
                linecolor = linecolor6[ind]
                if stackmode:
                    backstyle6 = [3004, 3005, 3006, 3007, 3013, 3017]
                    backstyle = backstyle6[ind]
                    backcolor = linecolor6[ind]
            elif len(histos) == 7:
                linecolor7 = [9, 46, 8, 4, 6, 2, 7]
                linecolor = linecolor7[ind]
                if stackmode:
                    backstyle7 = [3004, 3005, 3006, 3007, 3013, 3017, 3022]
                    backstyle = backstyle7[ind]
                    backcolor = linecolor7[ind]
            elif len(histos) == 8:
                linecolor8 = [9, 46, 8, 4, 6, 2, 7, 3]
                linecolor = linecolor8[ind]
                if stackmode:
                    backstyle8 = [3004, 3005, 3006, 3007, 3013, 3017, 3022, 3315]
                    backstyle = backstyle8[ind]
                    backcolor = linecolor8[ind]
            elif len(histos) == 9:
                linecolor9 = [9, 46, 8, 4, 6, 2, 7, 3, 42]
                linecolor = linecolor9[ind]
                if stackmode:
                    backstyle9 = [3004, 3005, 3006, 3007, 3013, 3017, 3022, 3315, 3351]
                    backstyle = backstyle9[ind]
                    backcolor = linecolor9[ind]
            elif len(histos) == 10:
                linecolor10 = [9, 46, 8, 4, 6, 2, 7, 3, 42, 48]
                linecolor = linecolor10[ind]
                if stackmode:
                    backstyle10 = [
                        3004,
                        3005,
                        3006,
                        3007,
                        3013,
                        3017,
                        3022,
                        3315,
                        3351,
                        3481,
                    ]
                    backstyle = backstyle10[ind]
                    backcolor = linecolor10[ind]
            else:
                linecolor = self.color
                self.color += 1

            # linecolor
            if self.main.datasets[ind].linecolor != ColorType.AUTO:
                linecolor = ColorType.convert2root(
                    self.main.datasets[ind].linecolor, self.main.datasets[ind].lineshade
                )
            mylinecolor = ('"' + madanalysis.enumeration.color_hex.color_hex[linecolor] + '"')

            # lineStyle
            mylinestyle = LineStyleType.convert2matplotlib(self.main.datasets[ind].linestyle)

            # linewidth
            mylinewidth = self.main.datasets[ind].linewidth

            # background color
            if self.main.datasets[ind].backcolor != ColorType.AUTO:
                backcolor = ColorType.convert2root(
                    self.main.datasets[ind].backcolor, self.main.datasets[ind].backshade
                )
            mybackcolor = ('"' + madanalysis.enumeration.color_hex.color_hex[backcolor] + '"')

            # background style
            if self.main.datasets[ind].backstyle != BackStyleType.AUTO:
                backstyle = BackStyleType.convert2matplotlib(
                    self.main.datasets[ind].backstyle
                )

            # Filling mode
            filledmode = '"stepfilled"'
            rWidth = 1.0
            if backcolor == 0:  # invisible
                filledmode = '"step"'
                mybackcolor = "None"

            # Histo
            outputPy.write(
                "    pad.hist("
                + "x=xData, "
                + "bins=xBinning, "
                + "weights="
                + myweights
                + ",\\\n"
                + "             label="
                + mytitle
                + ", "
            )
            if ntot != 0:
                outputPy.write("histtype=" + filledmode + ", ")
            try:
                import matplotlib.pyplot as plt

                plt.hist([0], normed=True)
                outputPy.write(
                    "rwidth="
                    + str(rWidth)
                    + ",\\\n"
                    + "             color="
                    + mybackcolor
                    + ", "
                    + "edgecolor="
                    + mylinecolor
                    + ", "
                    + "linewidth="
                    + str(mylinewidth)
                    + ", "
                    + "linestyle="
                    + mylinestyle
                    + ",\\\n"
                    + "             bottom=None, "
                    + 'cumulative=False, normed=False, align="mid", orientation="vertical")\n\n'
                )
            except Exception:
                outputPy.write(
                    "rwidth="
                    + str(rWidth)
                    + ",\\\n"
                    + "             color="
                    + mybackcolor
                    + ", "
                    + "edgecolor="
                    + mylinecolor
                    + ", "
                    + "linewidth="
                    + str(mylinewidth)
                    + ", "
                    + "linestyle="
                    + mylinestyle
                    + ",\\\n"
                    + "             bottom=None, "
                    + 'cumulative=False, density=False, align="mid",'
                    + ' orientation="vertical")\n\n'
                )
        outputPy.write("\n")

        # Label
        outputPy.write("    # Axis\n")
        outputPy.write("    plt.rc('text',usetex=False)\n")

        # X-axis
        if ref.titleX == "":
            axis_titleX = ref.GetXaxis_Matplotlib()
        else:
            axis_titleX = ref.titleX
        axis_titleX = axis_titleX.replace("#DeltaR", "#Delta R")
        axis_titleX = axis_titleX.replace("#", "\\")
        outputPy.write('    plt.xlabel(r"' + axis_titleX + '",\\\n')
        outputPy.write('               fontsize=16,color="black")\n')

        # Y-axis
        axis_titleY = ref.GetYaxis_Matplotlib()

        # Scale to one ?
        scale2one = False
        if ref.stack == StackingMethodType.NORMALIZE2ONE or (
            self.main.stack == StackingMethodType.NORMALIZE2ONE
            and ref.stack == StackingMethodType.AUTO
        ):
            scale2one = True

        if scale2one:
            axis_titleY += " $(#mathrm{scaled}\ #mathrm{to}# #mathrm{one})$"
        elif (
            self.main.normalize == NormalizeType.LUMI
            or self.main.normalize == NormalizeType.LUMI_WEIGHT
        ):
            axis_titleY += (
                " $(#mathcal{L}_{#mathrm{int}} = "
                + str(self.main.lumi)
                + "# #mathrm{fb}^{-1})$ "
            )
        elif self.main.normalize == NormalizeType.NONE:
            axis_titleY += " $(#mathrm{not}# #mathrm{normalized})$"

        if ref.titleY != "":
            axis_titleY = PlotFlow.NiceTitle(ref.titleY)
        axis_titleY = axis_titleY.replace("#", "\\")
        outputPy.write('    plt.ylabel(r"' + axis_titleY + '",\\\n')
        outputPy.write('               fontsize=16,color="black")\n')
        outputPy.write("\n")

        # Tag Log/Linear
        is_logx = False
        if ref.logX and ntot != 0:
            is_logx = True
        is_logy = False
        if ref.logY and ntot != 0:
            is_logy = True

        # Bound y
        outputPy.write("    # Boundary of y-axis\n")
        myweights = ""
        if stackmode:
            for ind in range(0, len(histos)):
                if ind >= 1:
                    myweights += "+"
                myweights += "y" + histos[ind].name + "_" + str(ind) + "_weights"
        else:
            myweights = "numpy.array(["
            for ind in range(0, len(histos)):
                if ind >= 1:
                    myweights += ","
                myweights += "y" + histos[ind].name + "_" + str(ind) + "_weights.max()"
            myweights += "])"
        if ref.ymax == []:
            outputPy.write("    ymax=(" + myweights + ").max()*1.1\n")
        else:
            outputPy.write("    ymax=" + str(ref.ymax) + "\n")
        outputPy.write("    ")
        if ref.ymin == []:
            if is_logy:
                outputPy.write("#")
            outputPy.write("ymin=0 # linear scale\n")
        else:
            if is_logy and ref.ymin <= 0:
                outputPy.write("#")
            outputPy.write("ymin=" + str(ref.ymin) + " # linear scale\n")

        myweights = ""
        if stackmode:
            for ind in range(0, len(histos)):
                if ind >= 1:
                    myweights += "+"
                myweights += "y" + histos[ind].name + "_" + str(ind) + "_weights"
        else:
            myweights = "numpy.array(["
            for ind in range(0, len(histos)):
                if ind >= 1:
                    myweights += ","
                myweights += "y" + histos[ind].name + "_" + str(ind) + "_weights.min()"
            myweights += ",1.])"
        outputPy.write("    ")
        if ref.ymin == []:
            if not is_logy:
                outputPy.write("#")
            outputPy.write(
                "ymin=min([x for x in (" + myweights + ") if x])/100. # log scale\n"
            )
        else:
            if is_logy and ref.ymin <= 0:
                outputPy.write("#")
            outputPy.write("ymin=" + str(ref.ymin) + " # log scale\n")
        outputPy.write("    plt.gca().set_ylim(ymin,ymax)\n")
        outputPy.write("\n")

        # X axis
        outputPy.write("    # Log/Linear scale for X-axis\n")
        # - Linear
        outputPy.write("    ")
        if is_logx:
            outputPy.write("#")
        outputPy.write('plt.gca().set_xscale("linear")\n')
        # - Log
        outputPy.write("    ")
        if not is_logx:
            outputPy.write("#")
        outputPy.write('plt.gca().set_xscale("log",nonpositive="clip")\n')
        outputPy.write("\n")

        # Y axis
        outputPy.write("    # Log/Linear scale for Y-axis\n")
        # - Linear
        outputPy.write("    ")
        if is_logy:
            outputPy.write("#")
        outputPy.write('plt.gca().set_yscale("linear")\n')
        # - Log
        outputPy.write("    ")
        if not is_logy:
            outputPy.write("#")
        outputPy.write('plt.gca().set_yscale("log",nonpositive="clip")\n')
        outputPy.write("\n")

        # Labels
        if frequencyhisto:
            outputPy.write("    # Labels for x-Axis\n")
            outputPy.write("    xLabels = numpy.array([")
            for bin in range(0, xnbin):
                if bin >= 1:
                    outputPy.write(",")
                outputPy.write(
                    '"' + str(histos[0].stringlabels[bin]).replace("_", "\_") + '"'
                )
            outputPy.write("])\n")
            outputPy.write('    plt.xticks(xData, xLabels, rotation="vertical")\n')
            outputPy.write("\n")

        ### BENJ: not necessary for getting the png and pdf files
        # Draw
        #        outputPy.write('    # Draw\n')
        #        outputPy.write('    plt.show()\n')
        #        outputPy.write('\n')

        # Legend
        if legendmode:

            # Reminder for 'loc'
            # -'best'         : 0, (only implemented for axes legends)
            # -'upper right'  : 1,
            # -'upper left'   : 2,
            # -'lower left'   : 3,
            # -'lower right'  : 4,
            # -'right'        : 5,
            # -'center left'  : 6,
            # -'center right' : 7,
            # -'lower center' : 8,
            # -'upper center' : 9,
            # -'center'       : 10,

            outputPy.write("    # Legend\n")
            outputPy.write(
                "    plt.legend(bbox_to_anchor=(1.05,1), loc=2," + " borderaxespad=0.)\n"
            )
            outputPy.write("\n")

        # Producing the image
        outputPy.write("    # Saving the image\n")
        for outputname in outputnames:
            outputPy.write("    plt.savefig('" + outputname + "')\n")
        outputPy.write("\n")

        # Call the function
        outputPy.write("# Running!\n")
        outputPy.write("if __name__ == '__main__':\n")
        outputPy.write("    " + function_name + "()\n")

        # Close the file
        try:
            outputPy.close()
        except:
            logging.getLogger("MA5").error("Impossible to close the file: " + outputPy)
            return False

        # Ok
        return True

    def DrawMULTIWEIGHT(self, histos, ref, filenamePy, outputnames) -> bool:

        # Is there any legend?
        legendmode = False
        if len(self.main.datasets) > 1:
            legendmode = True

        # Stacking or superimposing histos?
        # Default in the multiweight case: superimposed
        stackmode = False
        if ref.stack == StackingMethodType.STACK or self.main.stack == StackingMethodType.STACK:
            stackmode = True

        # Open the file in write-mode
        try:
            outputPy = open(filenamePy, "w")
        except Exception as err:
            logging.getLogger("MA5").error("Impossible to write the file: " + filenamePy)
            logging.getLogger("MA5").debug(err)
            return False

        # File header
        function_name = filenamePy[:-3]
        function_name = function_name.split("/")[-1]
        outputPy.write("def " + function_name + "():\n")
        outputPy.write("\n")

        # Import Libraries
        outputPy.write("    # Library import\n")
        outputPy.write("    import numpy\n")
        outputPy.write("    import matplotlib\n")
        outputPy.write("    import matplotlib.pyplot   as plt\n")
        outputPy.write("    import matplotlib.gridspec as gridspec\n")
        outputPy.write("    import matplotlib.patches  as patches\n")
        outputPy.write("\n")

        # Binning
        # Loop over datasets and histos
        xnbin = histos[0].description.nbins
        outputPy.write("    # Histo binning\n")
        outputPy.write(
            f"    xBinning=numpy.linspace({histos[0].description.xmin}, "
            f"{histos[0].description.xmax}, {xnbin+1}, "
            "endpoint=True)\n"
        )
        outputPy.write("    bin_centers = 0.5 * (xBinning[1:] + xBinning[:-1])\n")
        outputPy.write("    bin_width = xBinning[1] - xBinning[0]\n")
        outputPy.write("\n")

        # Loop over datasets and histos
        ntot = 0
        for ind in range(0, len(histos)):
            if not histos[ind]:
                continue

            # Creating a new histo
            histotag  = "y_" + histos[ind].name + "_" + str(ind)
            histoname = histotag + "_weights"
            outputPy.write(f"    # Creating weights for histo: {histotag}\n")
            current_weights = histos[ind].weights * histos[ind].scale
            ntot+= sum(current_weights)
            outputPy.write(
                f"    {histoname} = numpy.array(["
                + ", ".join([f"{c:.5e}" if c != 0.0 else "0.0" for c in current_weights])
                + "])\n"
            )
            outputPy.write(f"    {histotag}, _ = numpy.histogram(a=numpy.array(bin_centers), bins=xBinning, weights={histoname})\n\n")

            # Extract uncertainties
            lower_unc, upper_unc = histos[ind].uncertainties

            # total curve with error
            if ind==0:
                tot_lower_unc = lower_unc
                tot_upper_unc = upper_unc
                tot_central = current_weights
            else:
                tot_lower_unc = np.sqrt(tot_lower_unc**2 + lower_unc**2)
                tot_upper_unc = np.sqrt(tot_upper_unc**2 + upper_unc**2)
                tot_central = tot_central + current_weights

            # Write upper limits
            uppertag  = "yup_" + histos[ind].name + "_" + str(ind)
            uppername = uppertag + "_weights"
            outputPy.write(f"    # Upper error bar for: {histotag}\n")
            outputPy.write(
                f"    {uppername}  = numpy.array(["
                + ", ".join([f"{u:.5e}" if u != 0.0 else "0.0" for u in upper_unc])
                + "])\n"
            )
            outputPy.write(f"    {uppertag} = {histotag} + {uppername}\n\n")

            # Writing lower limits
            lowertag  = "ylow_" + histos[ind].name + "_" + str(ind)
            lowername = lowertag + "_weights"
            outputPy.write(f"    # Lower error bar for: {histotag}\n")
            outputPy.write(
                f"    {lowername} = numpy.array(["
                + ", ".join([f"{l:.5e}" if l != 0.0 else "0.0" for l in lower_unc])
                + "])\n"
            )
            outputPy.write(f"    {lowertag} = {histotag} - {lowername}\n\n")

        # Canvas
        outputPy.write("    # Creating a new Canvas\n")
        dpi = 80
        height = 500
        widthx = 700
        if legendmode:
            widthx = 1000
        outputPy.write(
            "    fig   = plt.figure("
            f"figsize=({widthx / dpi},{height / dpi}), dpi={dpi})\n"
        )
        if not legendmode:
            outputPy.write("    frame = gridspec.GridSpec(1,1)\n")
        else:
            outputPy.write("    frame = gridspec.GridSpec(1,1,right=0.7)\n")
        outputPy.write("    pad   = fig.add_subplot(frame[0])\n\n")

        # Styles and colours
        linecolors = [
            [9], [9, 46], [9, 46, 8], [9, 46, 8, 4], [9, 46, 8, 4, 6], [9, 46, 8, 4, 6, 2],
            [9, 46, 8, 4, 6, 2, 7], [9, 46, 8, 4, 6, 2, 7, 3], [9, 46, 8, 4, 6, 2, 7, 3, 42],
            [9, 46, 8, 4, 6, 2, 7, 3, 42, 48]
        ]
        backstyles = [
            ['///'], ['///','\\\\\\\\\\\\'], ['///','\\\\\\\\\\','-'], ['///','\\\\\\\\\\\\','---','|||'], ['///','\\\\\\\\\\\\','---','|||','+++'],
            ['///','\\\\\\\\\\\\','---','|','+++','xxx'], ['///','\\\\\\\\\\\\','---','|||','+++','xxx','ooo'],
            ['///','\\\\\\\\\\\\','---','|','+++','xxx','ooo','...'], ['///','\\\\\\\\\\\\','---','|||','+++','xxx','ooo','...','***'],
            ['///','\\\\\\\\\\\\','---','|','+++','xxx','ooo','...','***','/\\']
        ]

        # Stack
        titles = []
        for ind in range(len(histos) - 1, -1, -1):
            mytitle = ('"' + PlotFlow.NiceTitleMatplotlib(self.main.datasets[ind].title) + '"')
            mytitle = mytitle.replace("_", "\_")

            # Set linecolor based on the length of histos
            if len(histos) <= 10:
                linecolor = linecolors[len(histos) - 1][ind]
                backcolor = linecolors[len(histos) - 1][ind]
                backstyle = "'" + backstyles[len(histos) - 1][ind] + "'"
            else:
                linecolor = self.color
                backcolor = self.color
                self.color += 1

            # line colour, style and width
            if self.main.datasets[ind].linecolor != ColorType.AUTO:
                linecolor = ColorType.convert2root(
                    self.main.datasets[ind].linecolor, self.main.datasets[ind].lineshade
                )
            mylinecolor = ('"' + madanalysis.enumeration.color_hex.color_hex[linecolor] + '"')
            titles.append([mytitle, mylinecolor, backstyle])
            linestyle = LineStyleType.convert2matplotlib(self.main.datasets[ind].linestyle)
            linewidth = self.main.datasets[ind].linewidth

            # background style and colour
            if self.main.datasets[ind].backcolor != ColorType.AUTO:
                backcolor = ColorType.convert2root(
                    self.main.datasets[ind].backcolor, self.main.datasets[ind].backshade
                )
            mybackcolor = ('"' + madanalysis.enumeration.color_hex.color_hex[backcolor] + '"')

            # No stacking: error rectangles around each bin
            if not stackmode:
                # Rectangles around the error bar for each bin
                outputPy.write("    # Add rectangles for each bin\n")
                outputPy.write("    for center, y_val, y_up, y_low in zip(bin_centers, y_" + histos[ind].name + "_" + str(ind) + ", yup_" + histos[ind].name + "_" + str(ind) + ", ylow_" + histos[ind].name + "_" + str(ind) + "):\n")
                outputPy.write("        rect = patches.Rectangle( (center - bin_width / 2, y_low), bin_width, y_up - y_low, facecolor="+mybackcolor+", edgecolor=" + mylinecolor + ", hatch=" + backstyle + ", alpha=0.3)\n")
                outputPy.write("        pad.add_patch(rect)\n")
                # Error bars
                outputPy.write("    # Plot the error bars\n")
                outputPy.write(
                    "    pad.errorbar(bin_centers, y_" + histos[ind].name + "_" + str(ind) 
                    + ", yerr=[ylow_" + histos[ind].name + "_" + str(ind) + "_weights,  yup_" + histos[ind].name + "_" + str(ind) + "_weights],"
                    + "label=" + mytitle + ","
                    + f" fmt='.', elinewidth=1, capsize=3, color='black')\n\n"
                )

            # Stacking: combined error bar
            else:
                try:
                    import matplotlib.pyplot as plt
                    plt.hist([0], normed=True)
                    norm_key="normed"
                except Exception:
                    norm_key = "density"
                outputPy.write(
                    "    pad.hist(x=bin_centers, bins=xBinning, "
                    + "weights=y_" + histos[ind].name + "_" + str(ind) + ",\n"
                    + "         label=" + mytitle + ", "
                )
                if ntot != 0:
                    outputPy.write("histtype='stepfilled', ")
                outputPy.write(f"rwidth=1.0, color={mybackcolor}, \n")
                if ind==0:
                    outputPy.write("         bottom=None, ")
                else:
                    outputPy.write("         bottom=y_" + histos[ind-1].name + "_" + str(ind-1) + ", ")
                outputPy.write(f"cumulative=False, {norm_key}=False, align=\"mid\", orientation=\"vertical\")\n\n")
        outputPy.write("\n")

        # Rectangles around the error bar for each bin
        if stackmode:
            outputPy.write("    # Add rectangles for each bin around the total\n")
            outputPy.write(
                f"    tot_central  = numpy.array(["
                + ", ".join([f"{u:.5e}" if u != 0.0 else "0.0" for u in tot_central])
                + "])\n"
            )
            outputPy.write(
                f"    tot_up  = numpy.array(["
                + ", ".join([f"{u:.5e}" if u != 0.0 else "0.0" for u in tot_upper_unc])
                + "])\n"
            )
            outputPy.write(
                f"    tot_low = numpy.array(["
                + ", ".join([f"{u:.5e}" if u != 0.0 else "0.0" for u in tot_lower_unc])
                + "])\n"
            )
            outputPy.write("    ytot, _ = numpy.histogram(a=numpy.array(bin_centers), bins=xBinning, weights=tot_central)\n")
            outputPy.write("    yup  = ytot + tot_up\n")
            outputPy.write("    ylow = ytot - tot_low\n")
            outputPy.write("    for center, y, y_up, y_low in zip(bin_centers, ytot, yup, ylow):\n")
            outputPy.write("        rect = patches.Rectangle( (center - bin_width / 2, y_low), bin_width, y_up - y_low, facecolor='white', edgecolor='black' , hatch='////', alpha=0.3)\n")
            outputPy.write("        pad.add_patch(rect)\n\n")

        # Label
        outputPy.write("    # Axis\n")
        outputPy.write("    plt.rc('text',usetex=False)\n")

        # X-axis
        if ref.titleX == "":
            axis_titleX = ref.GetXaxis_Matplotlib()
        else:
            axis_titleX = ref.titleX
        axis_titleX = axis_titleX.replace("#DeltaR", "#Delta R")
        axis_titleX = axis_titleX.replace("#slash", "\\not\!\!\!\!")
        axis_titleX = axis_titleX.replace("#", "\\")
        outputPy.write('    plt.xlabel(r"' + axis_titleX + '",\\\n')
        outputPy.write('               fontsize=16,color="black")\n')

        # Y-axis
        axis_titleY = ref.GetYaxis_Matplotlib()

        # Scale to one ?
        scale2one = False
        if ref.stack == StackingMethodType.NORMALIZE2ONE or (
            self.main.stack == StackingMethodType.NORMALIZE2ONE
            and ref.stack == StackingMethodType.AUTO
        ):
            scale2one = True

        if scale2one:
            axis_titleY += " $(#mathrm{scaled}\ #mathrm{to}# #mathrm{one})$"
        elif (
            self.main.normalize == NormalizeType.LUMI
            or self.main.normalize == NormalizeType.LUMI_WEIGHT
        ):
            axis_titleY += (
                " $(#mathcal{L}_{#mathrm{int}} = "
                + str(self.main.lumi)
                + "# #mathrm{fb}^{-1})$ "
            )
        elif self.main.normalize == NormalizeType.NONE:
            axis_titleY += " $(#mathrm{not}# #mathrm{normalized})$"

        if ref.titleY != "":
            axis_titleY = PlotFlow.NiceTitle(ref.titleY)
        axis_titleY = axis_titleY.replace("#", "\\")
        outputPy.write('    plt.ylabel(r"' + axis_titleY + '",\\\n')
        outputPy.write('               fontsize=16,color="black")\n')
        outputPy.write("\n")

        # Tag Log/Linear
        is_logx = False
        if ref.logX and ntot != 0:
            is_logx = True
        is_logy = False
        if ref.logY and ntot != 0:
            is_logy = True

        # Bound y
        outputPy.write("    # Boundary of y-axis\n")
        if ref.ymax == []:
            if stackmode:
                outputPy.write("    ymax = yup.max()*1.1\n")
            else:
                myweights = "numpy.array([" + ",".join([f"yup_{histos[ind].name}_{ind}.max()" for ind in range(len(histos))]) + "])"
                outputPy.write("    ymax=(" + myweights + ").max()*1.1\n")
        else:
            outputPy.write("    ymax=" + str(ref.ymax) + "\n")
        if ref.ymin == []:
            if stackmode and is_logy:
                outputPy.write("    ymin = min([x for x in ylow if x])/100\n")
            elif stackmode and not is_logy:
                outputPy.write("    ymin = 0 # linear scale\n")
            elif is_logy:
                myweights = "numpy.array([" + ",".join([f"min([x for x in ylow_{histos[ind].name}_{ind} if x>0])" for ind in range(len(histos))]) + "])"
                outputPy.write("    ymin =(" + myweights + ").min()/100\n")
            else:
                outputPy.write("    ymin = 0 # linear scale")
        else:
            if is_logy and ref.ymin <= 0:
                outputPy.write("#")
            outputPy.write("ymin=" + str(ref.ymin) + " # linear scale\n")
        outputPy.write("    plt.gca().set_ylim(ymin,ymax)\n")
        outputPy.write("\n")

        # X axis
        outputPy.write("    # Log/Linear scale for X-axis\n")
        # - Linear
        outputPy.write("    ")
        if is_logx:
            outputPy.write("#")
        outputPy.write('plt.gca().set_xscale("linear")\n')
        # - Log
        outputPy.write("    ")
        if not is_logx:
            outputPy.write("#")
        outputPy.write('plt.gca().set_xscale("log",nonpositive="clip")\n')
        outputPy.write("\n")

        # Y axis
        outputPy.write("    # Log/Linear scale for Y-axis\n")
        # - Linear
        outputPy.write("    ")
        if is_logy:
            outputPy.write("#")
        outputPy.write('plt.gca().set_yscale("linear")\n')
        # - Log
        outputPy.write("    ")
        if not is_logy:
            outputPy.write("#")
        outputPy.write('plt.gca().set_yscale("log",nonpositive="clip")\n')
        outputPy.write("\n")

        # Legend
        if legendmode:
            outputPy.write("    # Legend\n")
            outputPy.write("    import matplotlib.patches as patches\n")
            outputPy.write("    # Create custom legend handles\n")
            outputPy.write("    legend_handles = [\n")
            for label,color,hatch in titles:
                if stackmode:
                    outputPy.write("        patches.Patch(color=" + color + ", alpha=0.3, label=" + label + "),\n")
                else:
                    outputPy.write("        patches.Patch(color=" + color + ", hatch=" + hatch +", alpha=0.3, label=" + label + "),\n")
            outputPy.write("    ]\n")
            outputPy.write("    pad.legend(handles=legend_handles, bbox_to_anchor=(1.05,1), loc=2," + " borderaxespad=0.)\n\n")

        # Producing the image
        outputPy.write("    # Saving the image\n")
        for outputname in outputnames:
            outputPy.write("    plt.savefig('" + outputname + "')\n")
        outputPy.write("    plt.close('all')\n")

        # Call the function
        outputPy.write("# Running!\n")
        outputPy.write("if __name__ == '__main__':\n")
        outputPy.write("    " + function_name + "()\n")

        # Close the file
        try:
            outputPy.close()
        except Exception as err:
            logging.getLogger("MA5").error("Impossible to close the file: " + outputPy)
            logging.getLogger("MA5").debug(err)
            return False

        # Ok
        return True
