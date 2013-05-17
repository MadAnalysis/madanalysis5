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


#ifndef PLOT_BASE_H
#define PLOT_BASE_H

// STL headers
#include <iostream>
#include <map>
#include <string>

// ROOT headers
#include <TH1F.h>

// SampleAnalyzer headers
#include "SampleAnalyzer/Service/LogService.h"

namespace MA5
{

class PlotBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected :

  /// Name of the plots
  std::string name_;

  /// Number of events
  std::pair<Long64_t,Long64_t> nevents_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public :

  /// Constructor without argument 
  PlotBase()
  { 
    nevents_  = std::make_pair(0,0);
  }

  /// Constructor with argument 
  PlotBase(const std::string& name)
  { 
    name_     = name;
    nevents_  = std::make_pair(0,0);
  }

  /// Destructor
  virtual ~PlotBase()
  { }

  /// Write the plot in a ROOT file
  virtual void Write_TextFormat(std::ostream* output) = 0;

  /// Write the plot in a ROOT file
  virtual void Write_RootFormat(std::pair<TH1F*,TH1F*>& histos) = 0;

  /// Increment number of events
  void IncrementNEvents(Float_t weight=1.0)
  {
	  if (weight>=0) nevents_.first++;
	  else nevents_.second++;
  }

  /// Return Number of events
  const std::pair<Long64_t,Long64_t>& GetNEvents()
 { return nevents_; }

};

}

#endif
