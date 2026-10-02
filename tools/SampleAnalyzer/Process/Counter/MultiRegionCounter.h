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
 * @file MultiRegionCounter.h
 * @brief Cut shared by several signal regions.
 */

#ifndef __MULTIREGIONCOUNTER_H
#define __MULTIREGIONCOUNTER_H


// STL headers
#include <string>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/RegionSelection/RegionSelection.h"


namespace MA5
{

/** @brief Cut shared by several signal regions. */
class MultiRegionCounter
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:
  /** @brief Name of the cut. */
  std::string name_;
  /** @brief Regions to which the cut applies. */
  std::vector<RegionSelection*> regions_;

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:
  /** @brief Constructor. */
  MultiRegionCounter() {name_="";};

  /**
   * @brief Constructor.
   *
   * @param name name of the cut.
   */
  MultiRegionCounter(const std::string& name) { name_=name; };

  /** @brief Destructor. */
  ~MultiRegionCounter() {};

  /**
   * @brief Accessor to the name of the cut.
   *
   * @return the name.
   */
  std::string GetName()
    { return name_; }

  /**
   * @brief Accessor to the regions of the cut.
   *
   * @return a copy of the regions.
   */
  std::vector<RegionSelection *> Regions()
    { return regions_; }

  /**
   * @brief Set the name of the cut.
   *
   * @param ThisName name.
   */
  void SetName(std::string ThisName)
    { name_=ThisName; }

  /**
   * @brief Attach the cut to regions (the cut is added to their cut-flows).
   *
   * @param RSVector regions.
   */
  void AddRegionSelection(std::vector<RegionSelection*> RSVector)
  {
    for (MAuint32 i=0; i<RSVector.size(); i++)
      { RSVector[i]->AddCut(name_); }
    regions_.insert(regions_.end(), RSVector.begin(),RSVector.end());
  }

};

}

#endif
