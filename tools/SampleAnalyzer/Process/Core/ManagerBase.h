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


#ifndef MANAGER_BASE_h
#define MANAGER_BASE_h

// STL headers
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>

// ROOT headers
#include <Rtypes.h> 

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

template <typename T>
class ManagerBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /// List of objects
  std::vector<T*> Objects_;

  /// Mapping between names (lower case) and objects
  std::map<std::string, UInt_t> Names_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  ManagerBase()
  {}

	/// Destructor
  ~ManagerBase()
  { 
    //    for (unsigned int i=0;i<Objects_.size();i++)
    //      if (Objects_[i]!=0) delete Objects_[i];
  }

  /// Find an object(non-const) for a given name
  T* Get(std::string name);

  /// Add an object in the collection
  bool Add(std::string name, T* object);

  /// Display the content of the Manager
  void Print(LogStream& os=INFO) const;

};


// -----------------------------------------------------------------------------
// Get
// -----------------------------------------------------------------------------
template <typename T>
T* ManagerBase<T>::Get(std::string name)
{
  // Set the extension in lower case
  std::transform(name.begin(), name.end(),
                 name.begin(), std::ptr_fun<int, int>(std::tolower));

  // Seach the name
  std::map<std::string, UInt_t>::iterator it = Names_.find(name); 

  // Found
  if (it!=Names_.end())
  {
    return Objects_[it->second];
  }

  // No reader found : return null pointer
  else return 0;
}


// -----------------------------------------------------------------------------
// Add
// -----------------------------------------------------------------------------
template <typename T>
bool ManagerBase<T>::Add(std::string name, T* object)
{
  // Set the name in lower case
  std::transform(name.begin(), name.end(),
                 name.begin(), std::ptr_fun<int, int>(std::tolower));

  // Insert name in the data base
  std::pair<std::map<std::string,UInt_t>::iterator,bool>
    found = Names_.insert(std::make_pair(name,0));
  
  // Check if name insertion is failed
  if (!found.second) return false;

  // Look for object in the data base
  for (UInt_t i=0;i<Objects_.size();i++)
  {
    if (Objects_[i]==object)
    {
      found.first->second=i;
      return true;
    } 
  }

  // Case where the object is not found in the data base
  Objects_.push_back(object);
  found.first->second=(Objects_.size()-1);
  return true;  
}

// -----------------------------------------------------------------------------
// Print
// -----------------------------------------------------------------------------
template <typename T>
void ManagerBase<T>::Print(LogStream& os) const
{
  // Header
  INFO << "------------------------------------------" << endmsg;

  // Loop over names
  for (std::map<std::string, UInt_t>::const_iterator
         it = Names_.begin(); it != Names_.end(); it++)
  {
    INFO.width(10); 
    INFO << it->first;
    INFO << " : ";
    INFO << typeid(Objects_[it->second]).name();
    INFO << " @ " ;
    INFO << Objects_[it->second];
    INFO << endmsg; 
  }

  // Foot
  INFO << "------------------------------------------" << endmsg;
}

}

#endif
