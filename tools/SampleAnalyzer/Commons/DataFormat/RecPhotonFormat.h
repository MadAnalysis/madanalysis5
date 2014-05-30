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


#ifndef RecPhotonFormat_h
#define RecPhotonFormat_h

// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// RecParticleFormat
#include "SampleAnalyzer/Commons/DataFormat/IsolationConeType.h"
#include "SampleAnalyzer/Commons/DataFormat/RecParticleFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

namespace MA5
{

class LHCOReader;
class ROOTReader;

class RecPhotonFormat : public RecParticleFormat
{

  friend class LHCOReader;
  friend class ROOTReader;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------             
 protected:
  std::vector<IsolationConeType> isolCones_; // isolation cones


  // -------------------------------------------------------------
  //                        method members
  // -------------------------------------------------------------             
 public:

  /// Constructor without arguments
  RecPhotonFormat()
  { Reset(); }

  /// Destructor
  virtual ~RecPhotonFormat()
  {}

  /// Dump information
  virtual void Print() const
  {

    RecParticleFormat::Print();
  }

  /// Clear all information
  virtual void Reset()
  {
    isolCones_.clear(); 
  }

  /// get the collection of isolation cones
  const std::vector<IsolationConeType>& isolCones() const
  { return isolCones_; }

  /// giving a new isolation cone entry
  IsolationConeType* GetNewIsolCone()
  {
    isolCones_.push_back(IsolationConeType());
    return &isolCones_.back();
  }

};

}

#endif
