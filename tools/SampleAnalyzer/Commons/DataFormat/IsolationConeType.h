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


#ifndef IsolationConeType_h
#define IsolationConeType_h

// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// RecParticleFormat
#include "SampleAnalyzer/Commons/Service/LogService.h"

namespace MA5
{

class LHCOReader;
class ROOTReader;
class DelphesTreeReader;
class DelphesMA5tuneTreeReader;
class DetectorDelphes;
class DetectorDelphesMA5tune;

class IsolationConeType
{

  friend class LHCOReader;
  friend class ROOTReader;
  friend class JetClusteringFastJet;
  friend class bTagger;
  friend class TauTagger;
  friend class cTagger;
  friend class DetectorDelphes;
  friend class DetectorDelphesMA5tune;
  friend class DelphesTreeReader;
  friend class DelphesMA5tuneTreeReader;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  UShort_t ntracks_;   /// number of tracks
  Float_t sumPT_;      /// sum PT
  Float_t sumET_;      /// sum ET
  Float_t deltaR_;     /// deltaR of the cone

  // -------------------------------------------------------------
  //                        method members
  // -------------------------------------------------------------
 public:

  /// Constructor without arguments
  IsolationConeType()
  { Reset(); }

  /// Destructor
  virtual ~IsolationConeType()
  {}

  /// Dump information
  virtual void Print() const
  {
  }

  /// Clear all information
  virtual void Reset()
  {
    ntracks_ = 0; 
    sumPT_   = 0.;
    sumET_   = 0.;
    deltaR_  = 0.;
  }

  /// Accessor to the number of tracks
  virtual const UShort_t ntracks() const
  {return ntracks_;}

  /// Accessor to sumPT
  const Float_t& sumPT() const
  {return sumPT_;}

  /// Accessor to sumET
  const Float_t& sumET() const
  {return sumET_;}

  /// Accessor to deltaR
  const Float_t& deltaR() const
  {return deltaR_;}

};

}

#endif
