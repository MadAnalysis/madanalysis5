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


#ifndef RECTOOLCONFIG_h
#define RECTOOLCONFIG_h

// STL headers
#include <set>
#include <string>

// ROOT headers
#include "TRint.h"

namespace MA5
{

class Tools;

struct RECconfig
{
  friend class PhysicsService;
  friend class Identification;
  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  protected:

  /// Muon isolation algorithm
  Bool_t deltaRalgo_;

  /// Parameter : deltaR
  Float_t deltaR_;

  /// Parameter : sumPT
  Float_t sumPT_;

  /// Parameter : ET_PT
  Float_t ET_PT_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
  public:

  /// Constructor without argument
  RECconfig()
  { deltaRalgo_=true; deltaR_=0.5; sumPT_=1.; ET_PT_=1.; }

  /// Destructor
  ~RECconfig()
  { }

  /// Reset
  void Reset()
  { deltaRalgo_=true; deltaR_=0.5; sumPT_=1.; ET_PT_=1.; }

  /// Specify algo DeltaR
  void UseDeltaRIsolation(Float_t deltaR=0.5)
  {
    deltaRalgo_=true; deltaR_=deltaR;
  } 

  /// Specify algo SumPT
  void UseSumPTIsolation(Float_t sumPT, Float_t ET_PT)
  {
    deltaRalgo_=false; sumPT_=sumPT; ET_PT_=ET_PT;
  } 

};

}

#endif
