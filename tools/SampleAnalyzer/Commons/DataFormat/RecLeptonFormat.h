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
 * @file RecLeptonFormat.h
 * @brief Reconstructed electron or muon.
 */

#ifndef RecLeptonFormat_h
#define RecLeptonFormat_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/IsolationConeType.h"
#include "SampleAnalyzer/Commons/DataFormat/RecParticleFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class LHCOReader;
class ROOTReader;
class DelphesTreeReader;
class DelphesMA5tuneTreeReader;
class DelphesMemoryInterface;

/** @brief Reconstructed charged lepton (electron or muon). */
class RecLeptonFormat : public RecParticleFormat
{

  friend class LHCOReader;
  friend class ROOTReader;
  friend class DelphesTreeReader;
  friend class DelphesMA5tuneTreeReader;
  friend class DelphesMemoryInterface;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------             
 protected:

  /** @brief Electric charge (false: -1, true: +1). */
  MAbool charge_;       /// charge of the particle 0 = -1, 1 = +1
  /** @brief Sum of the transverse energies in the isolation cone (LHCO/old Delphes). */
  MAfloat32 sumET_isol_;  /// sumET in an isolation cone
  /** @brief Sum of the transverse momenta in the isolation cone (LHCO/old Delphes). */
  MAfloat32 sumPT_isol_;  /// sumPT in an isolation cone
  /** @brief Isolation cones of various radii. */
  std::vector<IsolationConeType> isolCones_; // isolation cones
  /** @brief Reference to the Monte Carlo particle (Delphes). */
  MAuint64 refmc_;
  /** @brief PDG code (11 electron, 13 muon). */
  MAuint32   pdg_;

  /** @brief Members kept for backward compatibility (closest point and impact-parameter uncertainties). */
  MALorentzVector closest_point_;
  MAfloat32  d0error_;
  MAfloat32  dzerror_;


  // -------------------------------------------------------------
  //                        method members
  // -------------------------------------------------------------             
 public:

  /** @brief Constructor (members reset). */
  RecLeptonFormat()
  { Reset(); }

  /**
   * @brief Constructor from a reconstructed particle (kinematics, energy ratio and Monte Carlo match).
   *
   * @param part particle.
   */
  // NOTE: refmc_ is left uninitialised by this constructor.
  RecLeptonFormat(const RecParticleFormat& part)
  { 
    Reset();
    mc_       = part.mc_;
    HEoverEE_ = part.HEoverEE_;
    momentum_ = part.momentum_;
  }

  /**
   * @brief Constructor from a reconstructed particle (kinematics, energy ratio and Monte Carlo match).
   *
   * @param part particle.
   */
  RecLeptonFormat(const RecParticleFormat* part)
  { 
    Reset();
    mc_       = part->mc_;
    HEoverEE_ = part->HEoverEE_;
    momentum_ = part->momentum_;
    refmc_    = 0;
  }

  /** @brief Destructor. */
  virtual ~RecLeptonFormat()
  {}

  /** @brief Print the lepton properties. */
  void Print() const
  {
    INFO << "charge ="   << /*set::setw(8)*/"" << std::left << charge_  << ", "  
         << "sumET_isol_ = " << /*set::setw(8)*/"" << std::left << sumET_isol_ << ", "
         << "sumPT_isol_ = " << /*set::setw(8)*/"" << std::left << sumPT_isol_;

    RecParticleFormat::Print();
  }

  /** @brief Reset the lepton-specific members. */
  virtual void Reset()
  // NOTE: the members of RecParticleFormat (momentum, mc_, HEoverEE_, ntracks_) and refmc_ are not reset
  //   here.
  {
    charge_=false;
    sumET_isol_=0.;
    sumPT_isol_=0.;
    pdg_=0;
    isolCones_.clear();
    closest_approach_.SetXYZ(0.,0.,0.);
    d0_=0.; d0_approx_=0.; d0error_=0.;
    dz_=0.; dz_approx_=0.; dzerror_=0.;
    vertex_prod_.Reset();
  }

  /**
   * @brief Accessor to the electric charge.
   *
   * @return +1 or -1.
   */
  virtual const MAint32 charge() const
  { if (charge_) return +1; else return -1; }

  /**
   * @brief Set the electric charge.
   *
   * @param charge charge (> 0: positive, otherwise negative).
   */
  virtual void SetCharge(MAint32 charge)
  { if (charge>0) charge_=true; else charge_=false; }

  /**
   * @brief Accessor to the sum of the transverse energies in the isolation cone.
   *
   * @return the sum.
   */
  const MAfloat32 sumET_isol() const
  { return sumET_isol_; }

  /**
   * @brief Accessor to the sum of the transverse momenta in the isolation cone.
   *
   * @return the sum.
   */
  const MAfloat32 sumPT_isol() const
  { return sumPT_isol_; }

  /**
   * @brief Accessor to the ratio sumET/sumPT in the isolation cone.
   *
   * @return the ratio (0 if sumPT vanishes).
   */
  const MAfloat32 ET_PT_isol() const
  { if (sumPT_isol_!=0) return sumET_isol_/sumPT_isol_;
    else return 0; }

  /**
   * @brief Accessor to the isolation cones.
   *
   * @return the cones.
   */
  const std::vector<IsolationConeType>& isolCones() const
  { return isolCones_; }

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
   * @brief Get the isolation cone of a given radius (created if needed).
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
   * @brief Set the isolation cones.
   *
   * @param cones cones.
   */
  void setIsolCones(const std::vector<IsolationConeType>& cones)
  { isolCones_ = cones; }

  /**
   * @brief Accessor to the reference to the Monte Carlo particle.
   *
   * @return the reference.
   */
  const MAuint64& refmc() const {return refmc_;}

  /**
   * @brief Is the lepton an electron?
   *
   * @return true for an electron.
   */
  MAbool isElectron() const
  { return (pdg_==11); }

  /**
   * @brief Is the lepton a muon?
   *
   * @return true for a muon.
   */
  MAbool isMuon() const
  { return (pdg_==13); }

  /** @brief Identify the lepton as an electron. */
  void setElectronId()
  { pdg_=11; }

  /** @brief Identify the lepton as a muon. */
  void setMuonId()
  { pdg_=13; }

  //   --------------------------------------    //
  // older methods for backwards compatibility
  //   --------------------------------------    //

  /**
   * @brief Accessor to the uncertainty on d0 (backward compatibility).
   *
   * @return the uncertainty.
   */
  MAfloat32 d0error() const { return d0error_; }
  /**
   * @brief Accessor to the uncertainty on dz (backward compatibility).
   *
   * @return the uncertainty.
   */
  MAfloat32 dzerror() const { return dzerror_; }

  /**
   * @brief Accessor to the closest point (backward compatibility).
   *
   * @return the point.
   */
  const MALorentzVector& closestPoint() const { return closest_point_; }
  /**
   * @brief Set the closest point (backward compatibility).
   *
   * @param v point.
   */
  void setClosestPoint(const MALorentzVector& v) { closest_point_= v; }

  /**
   * @brief Accessor to the production vertex (backward compatibility).
   *
   * @return the vertex.
   */
  const MALorentzVector& vertexProd()        const { return vertex_prod_; }
  /**
   * @brief Set the production vertex (backward compatibility).
   *
   * @param v vertex.
   */
  void setVertexPoint(const MALorentzVector& v)      { vertex_prod_=v; }

};

}

#endif
