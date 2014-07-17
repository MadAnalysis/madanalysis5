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
#include "SampleAnalyzer/Process/Plot/Histo.h"

using namespace MA5;


/// Write the plot in a Text file
void Histo::Write_TextFormat(std::ostream* output)
{
  // Header
	*output << "<Histo>" << std::endl;

  // Write the body
  Write_TextFormatBody(output);

  // Foot
  *output << "</Histo>" << std::endl;
  *output << std::endl;
}


/// Write the plot in a Text file
void Histo::Write_TextFormatBody(std::ostream* output)
{
  // Description
	*output << "<Description>" << std::endl;

  // Name
	*output << "\"" << name_ << "\"" << std::endl;

  // Title 
  output->width(15);
	*output << std::left << "# nbins";
  output->width(15);
  *output << std::left << "xmin";
  output->width(15);
  *output << std::left << "xmax" << std::endl;

 // Data
  output->width(15);
	*output << std::left << nbins_;
  output->width(15);
  *output << std::left << xmin_; 
  output->width(15);
  *output << std::left << xmax_ << std::endl;

  // SelectionRegions
  if(regions_.size()!=0)
  {
    *output << std::left << "# associated RegionSelections" << std::endl;
    for(unsigned int i=0; i < regions_.size(); i++)
    {
      int nsp = 50-regions_[i]->GetName().size();
      if(nsp<0) nsp=0;
      *output << " " << regions_[i]->GetName();
      for (int jj=0; jj<nsp;jj++) *output << " ";
      *output << "# Region nr. " << i+1 << std::endl;
    }
 }

 

	*output << "</Description>" << std::endl;

  // Statistics
  *output << "<Statistics>" << std::endl;

  *output << nevents_.first << " " 
          << nevents_.second << " # nevents" << std::endl;
  *output << nevents_w_.first << " " 
          << nevents_w_.second 
          << " # sum of event-weights over events" << std::endl;
  *output << nentries_.first << " " 
          << nentries_.second << " # nentries" << std::endl;
  *output << sum_w_.first << " " 
          << sum_w_.second 
          << " # sum of event-weights over entries" << std::endl;
  *output << sum_ww_.first << " " 
          << sum_ww_.second << " # sum weights^2"<<std::endl;
  *output << sum_xw_.first << " " 
          << sum_xw_.second << " # sum value*weight"<<std::endl;
  *output << sum_xxw_.first << " " 
          << sum_xxw_.second << " # sum value^2*weight"<<std::endl;
  *output << "</Statistics>" << std::endl;

  // Data
  *output << "<Data>" << std::endl;
  *output << underflow_.first << " " << 
             underflow_.second << " # underflow" << std::endl;
  for (unsigned int i=0;i<histo_.size();i++)
  {
    *output << histo_[i].first << " " << histo_[i].second;
    if (i<2 || i>=(histo_.size()-2)) 
      *output << " # bin " << i+1 << " / " << histo_.size();
    *output << std::endl;
      
  }
  *output << overflow_.first << " " 
          << overflow_.second << " # overflow" << std::endl;
  *output << "</Data>" << std::endl;
}


/// Write the plot in a ROOT file
void Histo::Write_RootFormat(std::pair<TH1F*,TH1F*>& histo)
{
  // Creating ROOT histograms
  histo.first  -> SetBins(nbins_,xmin_,xmax_);
  histo.second -> SetBins(nbins_,xmin_,xmax_);

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

