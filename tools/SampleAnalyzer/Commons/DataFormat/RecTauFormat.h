////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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


#ifndef RecTauFormat_h
#define RecTauFormat_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/RecParticleFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class LHCOReader;
class ROOTReader;
class DelphesTreeReader;
class DelphesMA5tuneTreeReader;
class DetectorDelphes;
class DetectorDelphesMA5tune;
class DelphesMemoryInterface;

class RecTauFormat : public RecParticleFormat
{

  friend class LHCOReader;
  friend class ROOTReader;
  friend class TauTagger;
  friend class DelphesTreeReader;
  friend class DelphesMA5tuneTreeReader;
  friend class DetectorDelphes;
  friend class DetectorDelphesMA5tune;
  friend class DelphesMemoryInterface;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  MAbool charge_;    /// charge of the particle 0 = -1, 1 = +1
  MAuint16 ntracks_; /// number of tracks
  MAint32 DecayMode_; /// Decay mode :  1 = Tau --> e nu nu
                    ///               2 = Tau --> mu nu nu
                    ///               3 = Tau --> K nu
                    ///               4 = Tau --> K* nu
                    ///               5 = Tau --> Rho (--> pi pi0) nu
                    ///               6 = Tau --> A1 (--> pi 2pi0) nu
                    ///               7 = Tau --> A1 (--> 3pi) nu
                    ///               8 = Tau --> pi nu
                    ///               9 = Tau --> 3pi pi0 nu
                    ///               0 = other

  // -------------------------------------------------------------
  //                        method members
  // -------------------------------------------------------------
 public:

  /// Constructor without arguments
  RecTauFormat()
  { Reset(); }

  /// Destructor                                                      
  virtual ~RecTauFormat()
  {}

  /// Dump information
  virtual void Print() const
  {
    INFO << "charge ="   << /*set::setw(8)*/"" << std::left << charge_  << ", "  
         << "ntracks = " << /*set::setw(8)*/"" << std::left << ntracks_ << ", ";
    RecParticleFormat::Print();
  }

  /// Clear all information
  virtual void Reset()
  {
    charge_=0.; 
    ntracks_=0;
  }

  /// Accessor to the electric charge
  virtual const MAint32 charge() const
  { if (charge_) return +1; else return -1; }

  /// Mutator to the electric charge
  virtual void setCharge(MAfloat32 charge )
  { if (charge>0) charge_=true; else charge_=false; }

  /// Accessor to the number of tracks
  const MAuint16 ntracks() const
  { return ntracks_; }

  /// Mutator to the number of tracks
  virtual void setNtracks(MAuint16 ntracks)
  { ntracks_=ntracks; }

  /// Accessor to the decay mode
  const MAint32 DecayMode() const
  { return DecayMode_; }

  /// Mutator to the decay mode
  void setDecayMode(MAint32 mode)
  { DecayMode_=mode; }
};

}

#endif
