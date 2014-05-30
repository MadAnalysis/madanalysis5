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
#include "SampleAnalyzer/Process/Plot/MergingPlotType.h"

// STL headers
#include <sstream>

using namespace MA5;


const UInt_t  MergingPlotType::nbins = 100;
const Double_t MergingPlotType::xmin  = 0.;
const Double_t MergingPlotType::xmax  = 3.;


void MergingPlotType::Initialize(unsigned int ncontrib, const std::string& name)
{
  contribution.resize(ncontrib);
  for (unsigned int i=0;i<contribution.size();i++)
  {
    std::stringstream str;
    str << name << "_" << i << "jet";
    std::string title;
    str >> title;
    contribution[i]=new Histo(title,
                              MergingPlotType::nbins,
                              MergingPlotType::xmin,
                              MergingPlotType::xmax);
  }
  std::stringstream str;
  str << name << "_total";
  std::string title;
  str >> title;
  total=new Histo(title,
                  MergingPlotType::nbins,
                  MergingPlotType::xmin,
                  MergingPlotType::xmax);
}


void MergingPlotType::Finalize()
{
  if (total!=0) delete total;

  for (unsigned int i=0;i<contribution.size();i++)
    if (contribution[i]!=0) delete contribution[i];

  contribution.clear();
}
