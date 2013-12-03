////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2013-2014 Eric Conte, Benjamin Fuks, Chris Wymant
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

// SampleAnalyzer
#include "SampleAnalyzer/RegionSelection/RegionSelection.h"

using namespace MA5;

/// Printing the list of histograms
void RegionSelection::WriteHistoDefinition(SAFWriter &output)
{
  // name of the region
  *output.GetStream() << "  <Description>\n";
  *output.GetStream() << "    " + GetName() + "\n";
  *output.GetStream() << "  </Description>\n";
  *output.GetStream() << "  <Histos>\n";
  for (unsigned int i=0;i<plots_.size();i++)
  {
    int nsp = 50-plots_[i]->GetName().size();
    *output.GetStream() << "    " << plots_[i]->GetName();
    for (unsigned int jj=0; jj<nsp;jj++) *output.GetStream() << " ";
    *output.GetStream() <<  "  #  histo nr. " << i+1 << "\n";
  }
  *output.GetStream() << "  </Histos>\n";
}

