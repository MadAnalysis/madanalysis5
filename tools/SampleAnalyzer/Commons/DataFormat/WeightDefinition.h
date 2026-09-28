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
 * @file WeightDefinition.h
 * @brief Definition of the weights of a sample (LHE <initrwgt> groups).
 */

#ifndef WEIGHT_DEFINITION_H
#define WEIGHT_DEFINITION_H


// STL headers
#include <map>
#include <set>
#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <algorithm>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


namespace MA5
{

struct WeightEntry;
  
/** @brief Group of weights (e.g. scale or PDF variations). */
struct WeightGroup
{
  /**
   * @brief Constructor.
   *
   * @param combin combination method (none, gaussian, hessian, envelope).
   * @param nam name of the group.
   */
  WeightGroup(std::string combin, std::string nam){combine=combin; name=nam;}
  std::string               name; // Lower & Upper case
  std::string               combine;
  
  std::vector<WeightEntry*> weights;
  /**
   * @brief Compare two groups (name, combination method and number of weights).
   *
   * @param v other group.
   * @return true if equal.
   */
  MAbool operator==(const WeightGroup& v) const
  {
    return (name==v.name &&
            combine==v.combine &&
            weights.size()==v.weights.size());
  }
};

 
/** @brief Definition of one weight. */
struct WeightEntry
{
  /**
   * @brief Constructor.
   *
   * @param i identifier.
   * @param nam name.
   * @param grou group of the weight.
   */
  // FIXME: 'grou=group' assigns the (uninitialised) member to the parameter: 'group' is never set.
  WeightEntry(MAuint32 i, std::string nam, WeightGroup* grou) {id=i;name=nam; grou=group;}
  MAuint32 id;
  std::string  name; // Lower & Upper case
  WeightGroup* group;
  
  /**
   * @brief Compare two weights (name, identifier and group name).
   *
   * @param v other weight.
   * @return true if equal.
   */
  // NOTE: dereferences 'group', which is never set (see the constructor).
  MAbool operator==(const WeightEntry& v) const
  {
    return ( name==v.name &&
             id==v.id &&
             group->name==v.group->name );
  }
  
};

 
/** @brief Definition of the weights of a sample, organised in groups. */
class WeightDefinition
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:
  /** @brief Groups, weights (by identifier) and group being filled. */
  std::vector<WeightGroup>          groups_; 
  std::map<MAuint32,WeightEntry>    weights_;
  WeightGroup*                      lastgroup_;


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /** @brief Constructor. */
  WeightDefinition()
  { Reset(); }

  /** @brief Destructor. */
  ~WeightDefinition()
  { }

  /** @brief Remove all the groups and weights. */
  void Reset()
  { groups_.clear(); weights_.clear(); lastgroup_=0; }
  /** @brief Remove all the groups and weights (alias of Reset()). */
  void clear()
  { Reset(); }

  /**
   * @brief Compare two definitions.
   *
   * @param v other definition.
   * @return true if equal.
   */
  MAbool Compare(const WeightDefinition& v) const
  { return (v.groups_==groups_ && v.weights_==weights_); }

  /**
   * @brief Add a new group; the next weights are attached to it.
   *
   * @param name name.
   * @param combine combination method.
   */
  void AddGroup(std::string name, std::string combine);
  
  /**
   * @brief Add a weight to the last group.
   *
   * @param id identifier.
   * @param name name.
   * @return false if there is no group or the identifier already exists.
   */
  MAbool AddWeight(MAuint32 id, std::string name);
    
  /** @brief Print the groups and weights. */
  void Print() const;

  /**
   * @brief Number of weights.
   *
   * @return the number of weights.
   */
  MAuint32 size() const
  { return weights_.size(); }
  
};

}

#endif
