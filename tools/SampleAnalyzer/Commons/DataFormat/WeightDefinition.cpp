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


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/WeightDefinition.h"

using namespace MA5;


/// Add a new weight group
void WeightDefinition::AddGroup(std::string name, std::string combine)
{
  // combine: Moving to lower case
  std::transform(combine.begin(), combine.end(),
                 combine.begin(), std::ptr_fun<int, int>(std::tolower));
    
  // Checking the 'combine' value
  MAbool test = combine=="none" || combine=="gaussian" || combine=="hessian" || combine=="envelope";
  try
  {
    if (!test) throw EXCEPTION_WARNING("The WeightGroup '"+name+
                                       "' has a unknown value for 'combine' variable: "+combine,"",0);
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }
    
  // Adding the WeightGroup to the database
  groups_.push_back(WeightGroup(combine,name));

  // Setting the last group
  lastgroup_=&(groups_[groups_.size()-1]);
}
  
  
/// Add a new Weight
MAbool WeightDefinition::AddWeight(MAuint32 id, std::string name)
{
  // Is there a Weight
  try
  {
    if (lastgroup_==0)
    {
      std::stringstream str;
      str << id;
      std::string number;
      str >> number;
      throw EXCEPTION_WARNING("The Weight '"+number+"' is not associated to a WeightGroup. I will be skipped.","",0);
    }
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
    return false;
  }

  // Try to add the Weight to the database
  std::pair<std::map<MAuint32,WeightEntry>::iterator,bool>
    ret = weights_.insert ( std::make_pair(id, WeightEntry(id,name,lastgroup_)) );
    
  // Error?
  try
  {
    if (!ret.second)
    {
      std::stringstream str;
      str << id;
      std::string number;
      str >> number;
      throw EXCEPTION_WARNING("The Weight '"+number+"' is already defined. The redundant definition will be skipped.","",0);
    }
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
    return false;
  }

  // Adding the Weight to the WeighGroup
  lastgroup_->weights.push_back(&(ret.first->second));
    
  // Ok
  return true;
}


/// Add a new weight group
void WeightDefinition::Print() const
{
  // Numbers
  INFO << "#WeightGroups: " << groups_.size() << endmsg;
  INFO << "#Weights:      " << weights_.size() << endmsg;

  // Loop over WeightGroups
  for (MAuint32 j=0;j<groups_.size();j++)
  {
    INFO << "|- WeightGroup = '" << groups_[j].name << "' with combine='" << groups_[j].combine << "' " << endmsg;

    // Loop over Weights
    for (MAuint32 i=0;i<groups_[j].weights.size();i++)
    {
      INFO << "  + Weight = " << groups_[j].weights[i]->id << " with name = '" << groups_[j].weights[i]->name << "'" << endmsg; 
    }
  }
}
  
