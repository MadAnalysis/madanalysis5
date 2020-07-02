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


#ifndef IsolationConeType_h
#define IsolationConeType_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// SampleAnalyzer headers
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

  MAuint16 ntracks_;    /// number of tracks
  MAfloat32 sumPT_;       /// sum PT
  MAfloat32 eflow_sumPT_; /// sum PT eflow
  MAfloat32 sumET_;       /// sum ET
  MAfloat32 deltaR_;      /// deltaR of the cone

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
    ntracks_     = 0; 
    sumPT_       = 0.;
    sumET_       = 0.;
    eflow_sumPT_ = 0.;
    deltaR_      = 0.;
  }

  /// Accessor to the number of tracks
  const MAuint16 ntracks() const
  {return ntracks_;}

  /// Accessor to sumPT
  const MAfloat32& sumPT() const
  {return sumPT_;}

  /// Accessor to sumET
  const MAfloat32& sumET() const
  {return sumET_;}

  /// Accessor to deltaR
  const MAfloat32& deltaR() const
  {return deltaR_;}

  /// Accessor to sumPTeflow
  const MAfloat32& sumPTeflow() const
  {return eflow_sumPT_;}

  /// Mutator to the number of tracks
  void setNtracks(MAuint16 tracks)
  {ntracks_=tracks;}

  /// Mutator to sumPT
  void setsumPT(MAfloat32 sumPT)
  {sumPT_=sumPT;}

  /// Mutator to sumET
  void setSumET(MAfloat32 sumET)
  {sumET_=sumET;}

  /// Mutator to deltaR
  void setDeltaR(MAfloat32 deltaR)
  {deltaR_=deltaR;}

  /// Mutator to sumPTeflow
  void setSumPTeflow(MAfloat32 eflow_sumPT)
  {eflow_sumPT_=eflow_sumPT;}

};

}

#endif
