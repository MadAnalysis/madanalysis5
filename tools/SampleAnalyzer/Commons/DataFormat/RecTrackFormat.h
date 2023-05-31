////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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


#ifndef RecTrackFormat_h
#define RecTrackFormat_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/IsolationConeType.h"
#include "SampleAnalyzer/Commons/DataFormat/ParticleBaseFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/DataFormat/MCParticleFormat.h"


namespace MA5
{

class LHCOReader;
class ROOTReader;
class DelphesTreeReader;
class DelphesMA5tuneTreeReader;
class DetectorDelphes;
class DetectorDelphesMA5tune;
class DelphesMemoryInterface;

class RecTrackFormat : public RecParticleFormat
{

  friend class LHCOReader;
  friend class ROOTReader;
  friend class JetClusteringFastJet;
  friend class bTagger;
  friend class TauTagger;
  friend class cTagger;
  friend class DelphesTreeReader;
  friend class DelphesMA5tuneTreeReader;
  friend class DetectorDelphes;
  friend class DetectorDelphesMA5tune;
  friend class DelphesMemoryInterface;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  MAint32 pdgid_;   /// PDG identity code
  MAbool charge_;      /// electric charge
  MAfloat64 etaOuter_;  /// eta @ first layer of calo
  MAfloat64 phiOuter_;  /// phi @ first layer of calo
  std::vector<IsolationConeType> isolCones_; // isolation cones

  // -------------------------------------------------------------
  //                        method members
  // -------------------------------------------------------------
 public:

  /// Constructor without arguments
  RecTrackFormat()
  { Reset(); }

  /// Destructor
  virtual ~RecTrackFormat()
  {}

  /// Dump information
  virtual void Print() const
  {
    INFO << "pdgid = " << pdgid_ << ", "  
         << "charge = " << charge_ << ", "
         << "etaOuter = " << etaOuter_ << ", "
         << "phiOuter = " << phiOuter_ << endmsg;
    ParticleBaseFormat::Print();
  }

  /// Clear all information
  virtual void Reset()
  {
    pdgid_    = -1;
    mc_       = 0;
    charge_   = false;
    etaOuter_ = 0.;
    phiOuter_ = 0.;
    ParticleBaseFormat::Reset();
    isolCones_.clear();
  }

  /// Accessor to the pdgid
  const MAint32 pdgid() const
  {
    // @JACK: use MC pdgid if there is no PDGID smearing
    //        setter can be used for PDGID smearing.
    if (pdgid_ == -1) return mc_->pdgid();
    else return pdgid_;
  }

  // Setter for pdgid
  void setPdgid(MAint32 v)   {pdgid_=v;}

  /// Accessor to etaCalo (only for Delphes)
  const MAfloat64& etaCalo() const
  {return etaOuter_;}

  /// Accessor to etaCalo (only for Delphes)
  const MAfloat64& phiCalo() const
  {return phiOuter_;}

  /// Accessor to charge
  virtual const MAint32 charge() const
  {if (charge_) return +1; else return -1;}

  /// Mutator related to the electric charge
  virtual void SetCharge(MAint32 charge)
  { if (charge>0) charge_=true; else charge_=false; }

  /// giving a new isolation cone entry
  IsolationConeType* GetNewIsolCone()
  {
    isolCones_.push_back(IsolationConeType());
    return &isolCones_.back();
  }

  // Accessor to Isolation cone with speciffic radius. (only for SFS)
  IsolationConeType* GetIsolCone(MAfloat32 radius)
  {
    for (MAuint32 i=0; i<isolCones_.size(); i++)
        if (radius == isolCones_[i].deltaR()) return &isolCones_[i];

    isolCones_.push_back(IsolationConeType());
    isolCones_.back().setDeltaR(radius);
    return &isolCones_.back();
  }

  /// get the collection of isolation cones
  const std::vector<IsolationConeType>& isolCones() const
  { return isolCones_; }


};

}

#endif
