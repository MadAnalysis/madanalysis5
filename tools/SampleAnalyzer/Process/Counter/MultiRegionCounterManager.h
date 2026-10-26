////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://github.com/MadAnalysis/madanalysis5>
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


/**
 * @file MultiRegionCounterManager.h
 * @brief Collection of the cuts of an analysis.
 */

#ifndef __MULTIREGIONCOUNTERMANAGER_H
#define __MULTIREGIONCOUNTERMANAGER_H


// STL headers
#include <vector>
#include <string>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Counter/MultiRegionCounter.h"
#include "SampleAnalyzer/Process/RegionSelection/RegionSelection.h"


namespace MA5
{

/** @brief Collection of the cuts of an analysis (owned). */
class MultiRegionCounterManager
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:
  /** @brief Cuts. */
  std::vector<MultiRegionCounter*> cuts_;

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:
  /** @brief Constructor. */
  MultiRegionCounterManager() {};

  /** @brief Destructor (the cuts are deleted by Reset()/Finalize()). */
  ~MultiRegionCounterManager() { };

  /** @brief Delete all the cuts. */
  void Reset()
  {
    for (MAuint32 i=0;i<cuts_.size();i++)
      { if (cuts_[i]!=0) delete cuts_[i]; }
    cuts_.clear();
  }

  /** @brief Delete all the cuts. */
  void Finalize() { Reset(); }

  /**
   * @brief Accessor to the cuts.
   *
   * @return a copy of the collection.
   */
  std::vector<MultiRegionCounter*> GetCuts()
    { return cuts_; }

  /**
   * @brief Number of cuts.
   *
   * @return the number of cuts.
   */
  MAuint32 GetNcuts()
    { return cuts_.size(); }

  /**
   * @brief Create a cut attached to regions.
   *
   * @param name name of the cut.
   * @param regions regions.
   */
  void AddCut(const std::string& name,std::vector<RegionSelection*> regions)
  {
    MultiRegionCounter* mycut = new MultiRegionCounter(name);
    mycut->AddRegionSelection(regions);
    cuts_.push_back(mycut);
  }

};

}

#endif
