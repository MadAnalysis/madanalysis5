////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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
  *output << "  <Description>" << std::endl;

  // Name
  *output << "    \"" << name_ << "\"" << std::endl;

  // Title 
  *output << "    ";
  output->width(10);
  *output << std::left << "# nbins";
  output->width(15);
  *output << std::left << "xmin";
  output->width(15);
  *output << std::left << "xmax" << std::endl;

  // Data
  *output << "      ";
  output->width( 8); *output << std::left << nbins_;
  output->width(15); *output << std::left << std::scientific << xmin_;
  output->width(15); *output << std::left << std::scientific << xmax_ << std::endl;

  // SelectionRegions
  if(regions_.size()!=0)
  {
    MAuint32 maxlength=0;
    for(MAuint32 i=0; i < regions_.size(); i++)
      if (regions_[i]->GetName().size()>maxlength) maxlength=regions_[i]->GetName().size();
    *output << std::left << "    # Defined regions" << std::endl;
    for(MAuint32 i=0; i < regions_.size(); i++)
    {
      *output << "      " << std::setw(maxlength) << std::left << regions_[i]->GetName();
      *output << "    # Region nr. " << std::fixed << i+1 << std::endl;
    }
  }

  // End description
  *output << "  </Description>" << std::endl;

  // Statistics
  *output << "  <Statistics>" << std::endl;

  *output << "      ";
  output->width(15); *output << std::fixed << nevents_.first;
  output->width(15); *output << std::fixed << nevents_.second;
  *output << " # nevents" << std::endl;
  *output << "      ";
  output->width(15); *output << std::scientific << nevents_w_.first;
  output->width(15); *output << std::scientific << nevents_w_.second;
  *output << " # sum of event-weights over events" << std::endl;
  *output << "      ";
  output->width(15); *output << std::fixed << nentries_.first;
  output->width(15); *output << std::fixed << nentries_.second;
  *output << " # nentries" << std::endl;
  *output << "      ";
  output->width(15); *output << std::scientific << sum_w_.first;
  output->width(15); *output << std::scientific << sum_w_.second;
  *output << " # sum of event-weights over entries" << std::endl;
  *output << "      ";
  output->width(15); *output << std::scientific << sum_ww_.first;
  output->width(15); *output << std::scientific << sum_ww_.second;
  *output << " # sum weights^2" << std::endl;
  *output << "      ";
  output->width(15); *output << std::scientific << sum_xw_.first;
  output->width(15); *output << std::scientific << sum_xw_.second;
  *output << " # sum value*weight" << std::endl;
  *output << "      ";
  output->width(15); *output << std::scientific << sum_xxw_.first;
  output->width(15); *output << std::scientific << sum_xxw_.second;
  *output << " # sum value^2*weight" << std::endl;
  *output << "  </Statistics>" << std::endl;

  // Data
  *output << "  <Data>" << std::endl;
  *output << "      ";
  output->width(15); *output << std::scientific << underflow_.first;
  output->width(15); *output << std::scientific << underflow_.second;
  *output << " # underflow" << std::endl;
  for (MAuint32 i=0;i<histo_.size();i++)
  {
    *output << "      ";
    output->width(15); *output << std::scientific << histo_[i].first;
    output->width(15); *output << std::scientific << histo_[i].second;
    if (i<2 || i>=(histo_.size()-2))
      *output << " # bin " << i+1 << " / " << histo_.size();
    *output << std::endl;
  }
  *output << "      ";
  output->width(15); *output << std::scientific << overflow_.first;
  output->width(15); *output << std::scientific << overflow_.second;
  *output << " # overflow" << std::endl;
  *output << "  </Data>" << std::endl;
}

