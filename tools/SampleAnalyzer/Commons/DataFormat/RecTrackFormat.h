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
 * @file RecTrackFormat.h
 * @brief Reconstructed track.
 */

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

/** @brief Reconstructed track. */
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

  /** @brief PDG code (-1: taken from the matched Monte Carlo particle). */
  MAint32 pdgid_;   /// PDG identity code
  /** @brief Electric charge (false: -1, true: +1). */
  MAbool charge_;      /// electric charge
  /** @brief Pseudorapidity at the entrance of the calorimeter (Delphes). */
  MAfloat64 etaOuter_;  /// eta @ first layer of calo
  /** @brief Azimuthal angle at the entrance of the calorimeter (Delphes). */
  MAfloat64 phiOuter_;  /// phi @ first layer of calo
  /** @brief Isolation cones of various radii. */
  std::vector<IsolationConeType> isolCones_; // isolation cones

  // -------------------------------------------------------------
  //                        method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor (members reset). */
  RecTrackFormat()
  { Reset(); }

  /** @brief Destructor. */
  virtual ~RecTrackFormat()
  {}

  /** @brief Print the track properties. */
  virtual void Print() const
  {
    INFO << "pdgid = " << pdgid_ << ", "  
         << "charge = " << charge_ << ", "
         << "etaOuter = " << etaOuter_ << ", "
         << "phiOuter = " << phiOuter_ << endmsg;
    ParticleBaseFormat::Print();
  }

  /** @brief Reset all the members. */
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

  /**
   * @brief Accessor to the PDG code.
   *
   * @return the PDG code, taken from the matched Monte Carlo particle if not set.
   */
  const MAint32 pdgid() const
  {
    // @JACK: use MC pdgid if there is no PDGID smearing
    //        setter can be used for PDGID smearing.
    // NOTE: dereferences mc_ without checking that it is not null.
    if (pdgid_ == -1) return mc_->pdgid();
    else return pdgid_;
  }

  /**
   * @brief Set the PDG code (e.g. for PDG-code smearing).
   *
   * @param v PDG code.
   */
  void setPdgid(MAint32 v)   {pdgid_=v;}

  /**
   * @brief Accessor to the pseudorapidity at the calorimeter (Delphes only).
   *
   * @return the pseudorapidity.
   */
  const MAfloat64& etaCalo() const
  {return etaOuter_;}

  /**
   * @brief Accessor to the azimuthal angle at the calorimeter (Delphes only).
   *
   * @return the azimuthal angle.
   */
  const MAfloat64& phiCalo() const
  {return phiOuter_;}

  /**
   * @brief Accessor to the electric charge.
   *
   * @return +1 or -1.
   */
  virtual const MAint32 charge() const
  {if (charge_) return +1; else return -1;}

  /**
   * @brief Set the electric charge.
   *
   * @param charge charge (> 0: positive, otherwise negative).
   */
  virtual void SetCharge(MAint32 charge)
  { if (charge>0) charge_=true; else charge_=false; }

  /**
   * @brief Append a new isolation cone.
   *
   * @return a pointer to the new cone.
   */
  IsolationConeType* GetNewIsolCone()
  {
    isolCones_.push_back(IsolationConeType());
    return &isolCones_.back();
  }

  /**
   * @brief Get the isolation cone of a given radius, created if needed (SFS only).
   *
   * @param radius radius of the cone.
   * @return a pointer to the cone.
   */
  IsolationConeType* GetIsolCone(MAfloat32 radius)
  {
    for (MAuint32 i=0; i<isolCones_.size(); i++)
        if (radius == isolCones_[i].deltaR()) return &isolCones_[i];

    isolCones_.push_back(IsolationConeType());
    isolCones_.back().setDeltaR(radius);
    return &isolCones_.back();
  }

  /**
   * @brief Accessor to the isolation cones.
   *
   * @return the cones.
   */
  const std::vector<IsolationConeType>& isolCones() const
  { return isolCones_; }


};

}

#endif
