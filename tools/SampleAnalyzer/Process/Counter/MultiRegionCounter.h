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


#ifndef __MULTIREGIONCOUNTER_H
#define __MULTIREGIONCOUNTER_H

// STL headers
#include <string>
#include <vector>

// SampleAnalyzer
#include "SampleAnalyzer/Process/RegionSelection/RegionSelection.h"


namespace MA5
{

class MultiRegionCounter
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:
  std::string name_;
  std::vector<RegionSelection*> regions_;

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:
  /// constructor without argument
  MultiRegionCounter() {name_="";};

  /// Constructor with argument
  MultiRegionCounter(const std::string& name) { name_=name; };

  /// Destructor
  ~MultiRegionCounter() {};

  /// Get methods
  std::string GetName()
    { return name_; }

  std::vector<RegionSelection *> Regions()
    { return regions_; }

  /// Set methods
  void SetName(std::string ThisName)
    { name_=ThisName; }

  /// methods to associate a vector of regions to this cut
  void AddRegionSelection(std::vector<RegionSelection*> RSVector)
  {
    for (unsigned int i=0; i<RSVector.size(); i++)
      { RSVector[i]->AddCut(name_); }
    regions_.insert(regions_.end(), RSVector.begin(),RSVector.end());
  }

};

}

#endif
