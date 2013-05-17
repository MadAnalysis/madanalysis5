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


class RootConfig:
    @staticmethod
    def Init():
        from ROOT import gStyle
        from ROOT import gROOT

        gROOT.ProcessLine("gErrorIgnoreLevel = 2000;")
        gStyle.SetCanvasColor(0);
        gStyle.SetCanvasBorderMode(0);
        gStyle.SetCanvasBorderSize(3);

        gStyle.SetPadLeftMargin(0.125);
        gStyle.SetPadBottomMargin(0.12);
        gStyle.SetPadColor(0);
        gStyle.SetPadBorderMode(0);

        gStyle.SetFrameBorderMode(0);
        gStyle.SetFrameBorderSize(0);
        gStyle.SetFrameFillColor(0);
        gStyle.SetOptStat(0);

        gStyle.SetLabelOffset(0.005,"X");
        gStyle.SetLabelSize(0.03,"X");
        gStyle.SetLabelFont(22,"X");

        gStyle.SetTitleOffset(.85,"X");
        gStyle.SetTitleSize(0.04,"X");
        gStyle.SetTitleFont(22,"X");

        gStyle.SetLabelOffset(0.005,"Y");
        gStyle.SetLabelSize(0.03,"Y");
        gStyle.SetLabelFont(22,"Y");

        gStyle.SetTitleOffset(.98,"Y");
        gStyle.SetTitleSize(0.04,"Y");
        gStyle.SetTitleFont(22,"Y");

        gStyle.SetStatColor(0);
        gStyle.SetStatBorderSize(0);
        gStyle.SetTextFont(2);
        gStyle.SetTextSize(.05);
        gStyle.SetLegendBorderSize(1);
        gStyle.SetHistLineWidth(2);
        gStyle.SetTitleFillColor(0);
        gStyle.SetTitleFontSize(0.06);
        gStyle.SetTitleBorderSize(0);
        gStyle.SetTitleAlign(13);
        gStyle.SetTextAlign(22);
                                                                            
