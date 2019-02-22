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


#ifndef MERGING_PLOT_TYPE_H
#define MERGING_PLOT_TYPE_H


// STL headers
#include <vector>
#include <string>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/Histo.h"
#include "SampleAnalyzer/Process/RegionSelection/RegionSelectionManager.h"


namespace MA5
{

class MergingPlotType
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
 public:

  MAuint32 n_contribs;
//  std::vector<Histo*> contribution;
//  Histo* total;

  static const MAuint32   nbins;
  static const MAfloat64 xmin;
  static const MAfloat64 xmax;


//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
 public:
  MergingPlotType()
  {}

  ~MergingPlotType()
  {}

  void Initialize(unsigned int, const std::string&, RegionSelectionManager*);

  void Finalize() { };

};

}

#endif
