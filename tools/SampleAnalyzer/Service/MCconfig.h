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


#ifndef MCTOOLCONFIG_h
#define MCTOOLCONFIG_h

// STL headers
#include <set>
#include <string>

// ROOT headers
#include "TRint.h"

namespace MA5
{

class Tools;

struct MCconfig
{
  friend class PhysicsService;
  friend class Identification;

  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  protected:

  /// list of PDG ids related to invisible particles
  std::set<Int_t> invisible_ids_;

  /// list of PDG ids related to partons
  std::set<Int_t> hadronic_ids_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
  public:

  /// Constructor without argument
  MCconfig()
  { }

  /// Destructor
  ~MCconfig()
  { }

  /// Reset
  void Reset()
  {
    invisible_ids_.clear();
    hadronic_ids_.clear();
  } 

  /// Add hadronic id
  void AddHadronicId(Int_t id)
  {
    hadronic_ids_.insert(id);
  } 

  /// Add invisible id
  void AddInvisibleId(Int_t id)
  {
    invisible_ids_.insert(id);
  } 

};

}

#endif
