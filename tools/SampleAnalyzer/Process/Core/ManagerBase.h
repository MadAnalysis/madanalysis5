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
 * @file ManagerBase.h
 * @brief Registry of named objects (readers, writers, analyses, ...).
 */

#ifndef MANAGER_BASE_h
#define MANAGER_BASE_h


// STL headers
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h" 
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

/**
 * @brief Registry mapping (case-insensitive) names to objects.
 *
 * @tparam T type of the registered objects.
 */
template <typename T>
class ManagerBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /** @brief Registered objects (not owned). */
  std::vector<T*> Objects_;

  /** @brief Index of the object associated with each (lower-case) name. */
  std::map<std::string, MAuint32> Names_;

  /** @brief Forbidden names and the reason why. */
  std::map<std::string, std::string> ForbiddenNames_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  ManagerBase()
  {}

  /** @brief Destructor (the objects are not deleted). */
  ~ManagerBase()
  { 
    //    for (MAuint32 i=0;i<Objects_.size();i++)
    //      if (Objects_[i]!=0) delete Objects_[i];
  }

  /**
   * @brief Get the object registered under a name.
   *
   * @param name name (case-insensitive).
   * @return the object, or 0 if not found.
   */
  T* Get(std::string name);

  /**
   * @brief Register an object under a name (an object can have several names).
   *
   * @param name name (case-insensitive).
   * @param object object.
   * @return false if the name is already used.
   */
  MAbool Add(std::string name, T* object);

  /**
   * @brief Forbid a name.
   *
   * @param name name (case-insensitive).
   * @param motivation reason displayed to the user.
   * @return false if the name is already forbidden.
   */
  MAbool AddForbidden(std::string name, std::string motivation);

  /**
   * @brief Print the content of a registry.
   *
   * @param Objects objects.
   * @param Names names.
   * @param os logger (unused: INFO is always used).
   */
  void Print(const std::vector<T*>& Objects, 
             const std::map<std::string, MAuint32>& Names,
             LogStream& os=INFO) const;

  /**
   * @brief Is a name forbidden?
   *
   * @param name name (case-insensitive).
   * @param motivation output: the reason (empty if not forbidden).
   * @return true if forbidden.
   */
  MAbool IsItForbidden(std::string name, std::string& motivation);

};


// -----------------------------------------------------------------------------
// Get
// -----------------------------------------------------------------------------
template <typename T>
T* ManagerBase<T>::Get(std::string name)
{
  // Set the extension in lower case
  std::transform(name.begin(), name.end(), name.begin(), ::tolower);

  // Seach the name
  std::map<std::string, MAuint32>::iterator it = Names_.find(name); 

  // Found
  if (it!=Names_.end())
  {
    return Objects_[it->second];
  }

  // No reader found : return null pointer
  else return 0;
}


// -----------------------------------------------------------------------------
// IsItForbidden?
// -----------------------------------------------------------------------------
template <typename T>
MAbool ManagerBase<T>::IsItForbidden(std::string name, std::string& motivation)
{
  // Set the name in lower case
  std::transform(name.begin(), name.end(), name.begin(), ::tolower);

  // Seach the name
  std::map<std::string, std::string>::iterator it = ForbiddenNames_.find(name); 

  // Found
  if (it!=ForbiddenNames_.end())
  {
    motivation=std::string(it->second);
    return true;
  }

  // No reader found
  else
  {
    motivation=std::string("");
    return false;
  }
}


// -----------------------------------------------------------------------------
// Add
// -----------------------------------------------------------------------------
template <typename T>
MAbool ManagerBase<T>::Add(std::string name, T* object)
{
  // Set the name in lower case
  std::transform(name.begin(), name.end(), name.begin(), ::tolower);

  // Insert name in the data base
  std::pair<std::map<std::string,MAuint32>::iterator,bool>
    found = Names_.insert(std::make_pair(name,0));
  
  // Check if name insertion is failed
  if (!found.second) return false;

  // Look for object in the data base
  for (MAuint32 i=0;i<Objects_.size();i++)
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
// AddForbidden
// -----------------------------------------------------------------------------
template <typename T>
MAbool ManagerBase<T>::AddForbidden(std::string name, std::string motivation)
{
  // Set the name in lower case
  std::transform(name.begin(), name.end(), name.begin(), ::tolower);

  // Insert name in the data base
  std::pair<std::map<std::string,std::string>::iterator,bool>
    found = ForbiddenNames_.insert(std::make_pair(name,motivation));
  
  // Check if name insertion is failed
  if (!found.second) return false;
  else return true;  
}


// -----------------------------------------------------------------------------
// Print
// -----------------------------------------------------------------------------
template <typename T>
void ManagerBase<T>::Print(const std::vector<T*>& Objects, 
                           const std::map<std::string, MAuint32>& Names,
                           LogStream& os) const
{
  // Header
  INFO << "------------------------------------------" << endmsg;
  INFO << "Number of items: " << Names.size() << endmsg;

  // Loop over names
  for (std::map<std::string, MAuint32>::const_iterator
         it = Names.begin(); it != Names.end(); it++)
  {
    INFO << " - ";
    INFO.width(20); 
    INFO << it->first;
    INFO << " : ";
    // NOTE: typeid of a pointer: the static type name is printed, not the dynamic one.
    INFO << typeid(Objects[it->second]).name();
    INFO << " @ " ;
    INFO << Objects[it->second];
    INFO << endmsg; 
  }

  // Foot
  INFO << "------------------------------------------" << endmsg;
}

}

#endif
