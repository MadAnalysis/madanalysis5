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
 * @file RecParticleFormat.h
 * @brief Base class of the reconstructed objects.
 */

#ifndef RecParticleFormat_h
#define RecParticleFormat_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/ParticleBaseFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


namespace MA5
{

class MCParticleFormat;
class LHEReader;
class LHCOReader;
class DelphesTreeReader;
class DelphesMA5tuneTreeReader;
class DetectorDelphes;
class DetectorDelphesMA5tune;
class RecLeptonFormat;
class DelphesMemoryInterface;

/**
 * @brief Base class of the reconstructed objects (jets, leptons, photons, taus, tracks, ...).
 *
 * In addition to the kinematics, it stores the number of tracks, the ratio of the
 * hadronic to electromagnetic energies, the matched Monte Carlo particle, the
 * Delphes references and the production vertex.
 */
class RecParticleFormat : public ParticleBaseFormat
{
  friend class LHEReader;
  friend class LHCOReader;
  friend class DelphesTreeReader;
  friend class DelphesMA5tuneTreeReader;
  friend class DetectorDelphes;
  friend class DetectorDelphesMA5tune;
  friend class RecLeptonFormat;
  friend class DelphesMemoryInterface;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /** @brief Number of tracks. */
  MAuint16 ntracks_;   /// number of tracks
  /** @brief Ratio of the hadronic to electromagnetic energies. */
  MAfloat32               HEoverEE_;    /// hadronic energy over electromagnetic energy
  /** @brief Matched Monte Carlo particle (may be null). */
  const MCParticleFormat* mc_ ;         /// mother generated particle
  /** @brief Delphes references (unique identifiers of the underlying objects). */
  std::vector<MAuint64>   delphesTags_; /// tag reference for Delphes
  /** @brief Production vertex. */
  MALorentzVector         vertex_prod_; /// information on the production vertex

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor (members reset). */
  RecParticleFormat()
  { Reset(); }

  /** @brief Destructor. */
  virtual ~RecParticleFormat()
  {}

  /** @brief Reset all the members. */
  virtual void Reset()
  {
    momentum_.SetPxPyPzE(0.,0.,0.,0.);
    delphesTags_.clear();
    HEoverEE_=0.;
    closest_approach_.SetXYZ(0.,0.,0.);
    d0_=0.; d0_approx_=0.;
    // FIXME: d0_approx_ is reset twice and dz_approx_ is not reset.
    dz_=0.; d0_approx_=0.;
    vertex_prod_.Reset();
    mc_=0;
    ntracks_ = 0;
  }

  /** @brief Print the object properties (an error is raised if there is no matched Monte Carlo particle). */
  virtual void Print() const
  {
    INFO << "momentum=(" << /*set::setw(8)*/"" << std::left << momentum_.Px()
         << ", "<</*set::setw(8)*/"" << std::left << momentum_.Py()  
         << ", "<</*set::setw(8)*/"" << std::left << momentum_.Pz() 
         << ", "<</*set::setw(8)*/"" << std::left << momentum_.E() << ") - "
         << "EHoverEE=" << /*set::setw(8)*/"" << std::left << HEoverEE_
         << " - ";

    try
    {
      if (mc_==0) throw EXCEPTION_ERROR("NoMCmum","",0);
    }
    catch(const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
    }    
  }

  /**
   * @brief Accessor to the matched Monte Carlo particle.
   *
   * @return the particle, or 0.
   */
  const MCParticleFormat* mc() const {return mc_;}

  /**
   * @brief Set the matched Monte Carlo particle.
   *
   * @param mc particle.
   */
  void setMc(const MCParticleFormat* mc) {mc_=mc;}

  /**
   * @brief Accessor to the ratio of the hadronic to electromagnetic energies.
   *
   * @return the ratio.
   */
  const MAfloat32& HEoverEE() const {return HEoverEE_;}

  /**
   * @brief Accessor to the ratio of the electromagnetic to hadronic energies.
   *
   * @return the ratio (0 if the hadronic/EM ratio vanishes).
   */
  const MAfloat32 EEoverHE() const 
  {
    if (HEoverEE_!=0) return 1./HEoverEE_; 
    else return 0.;
  }

  /// Accessor to the number of tracks
//  virtual const MAuint16 ntracks() const
//  { return 0; }

  /**
   * @brief Is the object isolated? (overridden by the derived classes)
   *
   * @return false by default.
   */
  virtual const MAbool isolated() const
  { return false; }

  /**
   * @brief Accessor to the electric charge (overridden by the derived classes).
   *
   * @return 0 by default.
   */
  virtual const MAint32 charge() const
  { return 0; }

  /**
   * @brief Accessor to the number of tracks.
   *
   * @return the number of tracks.
   */
  const MAuint16 ntracks() const
  {return ntracks_;}

  /**
   * @brief Accessor to the Delphes references.
   *
   * @return the references.
   */
  const std::vector<MAuint64>& delphesTags() const {return delphesTags_;}

  /**
   * @brief Check whether the object shares a Delphes reference with a list.
   *
   * Despite its name, the method returns true when a common reference is found
   * (i.e. when the objects are NOT unique).
   *
   * @param delphesTags references to compare with.
   * @return true if a reference is shared.
   */
  MAbool isDelphesUnique(const std::vector<MAuint64>& delphesTags) const
  {
    for (MAuint32 i=0;i<delphesTags_.size();i++)
      for (MAuint32 j=0;j<delphesTags.size();j++)
    {
      if (delphesTags_[i]==delphesTags[j]) return true;
    }
    return false;
  }

  /**
   * @brief Check whether the object shares a Delphes reference with another object.
   *
   * @param part other object.
   * @return true if a reference is shared.
   */
  MAbool isDelphesUnique(const RecParticleFormat* part) const
  { return isDelphesUnique(part->delphesTags()); }

  /**
   * @brief Check whether the object shares a Delphes reference with another object.
   *
   * @param part other object.
   * @return true if a reference is shared.
   */
  MAbool isDelphesUnique(const RecParticleFormat& part) const
  { return isDelphesUnique(part.delphesTags()); }

  /**
   * @brief Accessor to the production vertex.
   *
   * @return the vertex.
   */
  const MALorentzVector& ProductionVertex() const { return vertex_prod_; }
  /**
   * @brief Set the production vertex.
   *
   * @param v vertex.
   */
  void setProductionVertex(const MALorentzVector& v) { vertex_prod_=v; }

};

}

#endif
