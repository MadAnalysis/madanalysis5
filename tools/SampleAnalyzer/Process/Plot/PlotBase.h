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


#ifndef PLOT_BASE_H
#define PLOT_BASE_H


// STL headers
#include <iostream>
#include <map>
#include <string>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogService.h"


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
  std::pair<MAint64,MAint64> nevents_;

  /// Number of entries
  std::pair<MAint64,MAint64> nentries_;

  /// Sum of event-weight over events
  std::pair<MAfloat64,MAfloat64> nevents_w_;

  /// Flag telling whether a given histo has already been modified for an event
  MAbool fresh_event_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public :

  /// Constructor without argument 
  PlotBase()
  {
    // Reseting statistical counters
    nevents_     = std::make_pair(0,0);
    nentries_    = std::make_pair(0,0);
    nevents_w_   = std::make_pair(0,0);
    fresh_event_ = true;
  }

  /// Constructor with argument 
  PlotBase(const std::string& name)
  {
    name_        = name;
    nevents_     = std::make_pair(0,0);
    nevents_w_   = std::make_pair(0,0);
    nentries_    = std::make_pair(0,0);
    fresh_event_ = true;
  }

  /// Destructor
  virtual ~PlotBase()
  { }

  /// Accesor for fresh_event
  MAbool FreshEvent() { return fresh_event_;}

  /// Modifier for fresh_event
  void SetFreshEvent(MAbool tag) { fresh_event_ = tag;}

  /// Write the plot in a ROOT file
  virtual void Write_TextFormat(std::ostream* output) = 0;

  /// Increment number of events
  void IncrementNEvents(MAfloat64 weight=1.0)
  {
    if (weight>=0) 
    {
      nevents_.first++;
      nevents_w_.first+=weight;
    }
    else
    {
      weight = std::abs(weight);
      nevents_.second++;
      nevents_w_.second+=weight;
    }
    SetFreshEvent(false);
  }

  /// Return Number of events
  const std::pair<MAint64,MAint64>& GetNEvents()
  { return nevents_; }

  // Return the name
  std::string GetName()
    { return name_; }

};

}

#endif
