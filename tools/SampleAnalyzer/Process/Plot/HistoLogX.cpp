////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://launchpad.net/madanalysis5>
//  
//  MadAnalysis 5 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  MadAnalysis 5 is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
//  
////////////////////////////////////////////////////////////////////////////////


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/HistoLogX.h"

using namespace MA5;

/// Write the plot in a Text file
void HistoLogX::Write_TextFormat(std::ostream* output)
{
  // Header
	*output << "<HistoLogX>" << std::endl;

  // Write the body
  Write_TextFormatBody(output);

  // Foot
  *output << "</HistoLogX>" << std::endl;
  *output << std::endl;
}


/// Write the plot in a ROOT file
void HistoLogX::Write_RootFormat(std::pair<TH1F*,TH1F*>& histo)
{
  // Creating binning for histograms
  Double_t binnings[histo_.size()+1];
  for (unsigned int i=0;i<histo_.size();i++)
  {
    binnings[i]=std::pow(static_cast<Float_t>(10.),static_cast<Float_t>(log_xmin_+i*step_));
  }
  binnings[histo_.size()]=xmax_;

  // Creating ROOT histograms
  histo.first  -> SetBins(nbins_,binnings);
  histo.second -> SetBins(nbins_,binnings);

  // Filling histos
  for (unsigned int i=0;i<histo_.size();i++)
  {
    histo.first  -> SetBinContent(i+1,histo_[i].first);
    histo.second -> SetBinContent(i+1,histo_[i].second);
  }
  histo.first  -> SetBinContent(0,underflow_.first);
  histo.second -> SetBinContent(0,underflow_.second);
  histo.first  -> SetBinContent(histo_.size()+1,overflow_.first);
  histo.second -> SetBinContent(histo_.size()+1,overflow_.second);

  // Filling statistics for histo with positive weight
  histo.first  -> SetEntries(nentries_.first);
  Double_t stats[4];
  stats[0]=sum_w_.first;
  stats[1]=sum_ww_.first;
  stats[2]=sum_xw_.first;
  stats[3]=sum_xxw_.first;
  histo.first -> PutStats(stats);

  histo.second -> SetEntries(nentries_.second);
  stats[0]=sum_w_.second;
  stats[1]=sum_ww_.second;
  stats[2]=sum_xw_.second;
  stats[3]=sum_xxw_.second;
  histo.second -> PutStats(stats);
}

