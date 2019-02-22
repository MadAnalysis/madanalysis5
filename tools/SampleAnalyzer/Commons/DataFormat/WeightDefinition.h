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
  
struct WeightGroup
{
  WeightGroup(std::string combin, std::string nam){combine=combin; name=nam;}
  std::string               name; // Lower & Upper case
  std::string               combine;
  
  std::vector<WeightEntry*> weights;
  MAbool operator==(const WeightGroup& v) const
  {
    return (name==v.name &&
            combine==v.combine &&
            weights.size()==v.weights.size());
  }
};

 
struct WeightEntry
{
  WeightEntry(MAuint32 i, std::string nam, WeightGroup* grou) {id=i;name=nam; grou=group;}
  MAuint32 id;
  std::string  name; // Lower & Upper case
  WeightGroup* group;
  
  MAbool operator==(const WeightEntry& v) const
  {
    return ( name==v.name &&
             id==v.id &&
             group->name==v.group->name );
  }
  
};

 
class WeightDefinition
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:
  std::vector<WeightGroup>          groups_; 
  std::map<MAuint32,WeightEntry>    weights_;
  WeightGroup*                      lastgroup_;


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor withtout arguments
  WeightDefinition()
  { Reset(); }

  /// Destructor
  ~WeightDefinition()
  { }

  /// Clear all the content
  void Reset()
  { groups_.clear(); weights_.clear(); lastgroup_=0; }
  void clear()
  { Reset(); }

  /// Compare the weights
  MAbool Compare(const WeightDefinition& v) const
  { return (v.groups_==groups_ && v.weights_==weights_); }

  /// Add a new weight group
  void AddGroup(std::string name, std::string combine);
  
  /// Add a new Weight
  MAbool AddWeight(MAuint32 id, std::string name);
    
  /// Add a new weight group
  void Print() const;

  /// Size
  MAuint32 size() const
  { return weights_.size(); }
  
};

}

#endif
