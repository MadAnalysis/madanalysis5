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


#ifndef __MULTIREGIONCOUNTERMANAGER_H
#define __MULTIREGIONCOUNTERMANAGER_H

// STL headers
#include <vector>
#include <string>

// SampleAnalyzer
#include "SampleAnalyzer/Process/Counter/MultiRegionCounter.h"
#include "SampleAnalyzer/Process/RegionSelection/RegionSelection.h"

namespace MA5
{

class MultiRegionCounterManager
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:
  /// Collection of cuts
  std::vector<MultiRegionCounter*> cuts_;

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:
  /// constructor
  MultiRegionCounterManager() {};

  /// Destructor
  ~MultiRegionCounterManager() { };

  /// Reset
  void Reset()
  {
    for (unsigned int i=0;i<cuts_.size();i++)
      { if (cuts_[i]!=0) delete cuts_[i]; }
    cuts_.clear();
  }

  /// Finalizing
  void Finalize() { Reset(); }

  /// Get methods
  std::vector<MultiRegionCounter*> GetCuts()
    { return cuts_; }

  unsigned int GetNcuts()
    { return cuts_.size(); }

  /// Adding a Cut to the manager with the link to the appropriate SRs
  void AddCut(const std::string& name,std::vector<RegionSelection*> regions)
  {
    MultiRegionCounter* mycut = new MultiRegionCounter(name);
    mycut->AddRegionSelection(regions);
    cuts_.push_back(mycut);
  }

};

}

#endif
