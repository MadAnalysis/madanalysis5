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
 * @file Identification.h
 * @brief Identification of Monte Carlo particles (status, hadronic, invisible, B/C hadrons) and of isolated muons.
 */

#ifndef IDENTIFICATION_SERVICE_h
#define IDENTIFICATION_SERVICE_h


// STL headers
#include <iostream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/MCconfig.h"
#include "SampleAnalyzer/Commons/DataFormat/MCEventFormat.h"


namespace MA5
{

/**
 * @brief Identification methods (PHYSICS->Id).
 *
 * The status codes of the initial and final states can be adapted to the generator
 * with SetInitialState()/SetFinalState().
 */
class Identification
{
  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  private:
    /** @brief Status codes of the final-state and initial-state particles. */
    MAint32 finalstate_;
    MAint32 initialstate_;
    /** @brief Monte Carlo configuration (hadronic and invisible PDG codes). */
    MCconfig mcConfig_;
    /** @brief Reconstruction configuration (isolation). */
    RECconfig recConfig_;

  public:

    /** @brief Constructor (initial state: -1, final state: 1). */
    Identification()
    {
      initialstate_=-1; finalstate_=1;
    }

    /** @brief Destructor. */
    ~Identification() { }

    /**
     * @brief Accessor to the Monte Carlo configuration.
     *
     * @return the configuration.
     */
    const MCconfig& mcConfig() const
    {  return mcConfig_; }
    /**
     * @brief Accessor to the Monte Carlo configuration.
     *
     * @return the configuration.
     */
    MCconfig& mcConfig()
    {  return mcConfig_; }
    /**
     * @brief Accessor to the reconstruction configuration.
     *
     * @return the configuration.
     */
    const RECconfig& recConfig() const
    { return recConfig_; }
    /**
     * @brief Accessor to the reconstruction configuration.
     *
     * @return the configuration.
     */
    RECconfig& recConfig()
    { return recConfig_; }

  /**
   * @brief Is the particle in the initial state (status -1 or 11-19)?
   *
   * @param part particle.
   * @return true for an initial-state particle.
   */
  // NOTE: the reference and pointer versions of IsInitialState/IsInterState use different definitions
  // (the pointer versions use initialstate_).
  MAbool IsInitialState(const MCParticleFormat& part) const
  {
    return (part.statuscode()==-1 || (part.statuscode()>=11 && part.statuscode()<=19));
  }

  /**
   * @brief Is the particle in the final state?
   *
   * @param part particle.
   * @return true for a final-state particle.
   */
  MAbool IsFinalState(const MCParticleFormat& part) const
  {
    return (part.statuscode()==finalstate_);
  }

  /**
   * @brief Is the particle an intermediate state (neither initial nor final)?
   *
   * @param part particle.
   * @return true for an intermediate particle.
   */
  MAbool IsInterState(const MCParticleFormat& part) const
  {
    return (!IsInitialState(part) && !IsFinalState(part));
  }

  /**
   * @brief Is the particle in the initial state (status equal to initialstate_)?
   *
   * @param part particle.
   * @return true for an initial-state particle.
   */
  MAbool IsInitialState(const MCParticleFormat* part) const
  {
    return (part->statuscode()==initialstate_);
  }

  /**
   * @brief Is the particle in the final state?
   *
   * @param part particle.
   * @return true for a final-state particle.
   */
  MAbool IsFinalState(const MCParticleFormat* part) const
  {
    return (part->statuscode()==finalstate_);
  }

  /**
   * @brief Is the particle an intermediate state?
   *
   * @param part particle.
   * @return true for an intermediate particle.
   */
  MAbool IsInterState(const MCParticleFormat* part) const
  {
    return (part->statuscode()!=finalstate_ && part->statuscode()!=initialstate_);
  }

  /**
   * @brief Take the initial-state status code from the first particle of an event.
   *
   * @param myEvent Monte Carlo event.
   */
  void SetInitialState(const MCEventFormat* myEvent)
  {
    if (myEvent==0) return; 
    if (myEvent->particles().empty()) return;
    initialstate_=myEvent->particles()[0].statuscode(); 
  }

  /**
   * @brief Take the final-state status code from the last particle of an event.
   *
   * @param myEvent Monte Carlo event.
   */
  void SetFinalState(const MCEventFormat* myEvent)
  {
    if (myEvent==0) return; 
    if (myEvent->particles().empty()) return;
    finalstate_=myEvent->particles()[myEvent->particles().size()-1].statuscode(); 
  }

  /**
   * @brief Is the reconstructed object a jet?
   *
   * @param part object.
   * @return true for a jet.
   */
  inline MAbool IsHadronic(const RecParticleFormat* part) const
  {
    if (dynamic_cast<const RecJetFormat*>(part)==0) return false;
    else return true;
  }

  /**
   * @brief Is the reconstructed object invisible?
   *
   * @param part object.
   * @return always false.
   */
  inline MAbool IsInvisible(const RecParticleFormat* part) const
  {
    return false;
  }

  /**
   * @brief Is the reconstructed object a jet?
   *
   * @param part object.
   * @return true for a jet.
   */
  inline MAbool IsHadronic(const RecParticleFormat& part) const
  {
    return IsHadronic(&part);
  }

  /**
   * @brief Is the reconstructed object invisible?
   *
   * @param part object.
   * @return always false.
   */
  inline MAbool IsInvisible(const RecParticleFormat& part) const
  {
    return IsInvisible(&part);
  }


  /**
   * @brief Is the particle hadronic (PDG code in the `hadronic` multiparticle)?
   *
   * @param part particle.
   * @return true if hadronic.
   */
  inline MAbool IsHadronic(const MCParticleFormat& part) const
  {
    std::set<MAint32>::iterator found = mcConfig_.hadronic_ids_.find(part.pdgid());
    if (found==mcConfig_.hadronic_ids_.end()) return false; else return true;
  }

  /**
   * @brief Is the PDG code hadronic (in the `hadronic` multiparticle)?
   *
   * @param pdgid PDG code.
   * @return true if hadronic.
   */
  inline MAbool IsHadronic(MAint32 pdgid) const
  {
    std::set<MAint32>::iterator found = mcConfig_.hadronic_ids_.find(pdgid);
    if (found==mcConfig_.hadronic_ids_.end()) return false; else return true;
  }

  /**
   * @brief Is the particle hadronic?
   *
   * @param part particle (0 allowed).
   * @return true if hadronic.
   */
  inline MAbool IsHadronic(const MCParticleFormat* part) const
  {
    if (part==0) return false;
    return IsHadronic(*part);
  }

  /**
   * @brief Is the particle invisible (PDG code in the `invisible` multiparticle)?
   *
   * @param part particle.
   * @return true if invisible.
   */
  inline MAbool IsInvisible(const MCParticleFormat& part) const
  {
    std::set<MAint32>::iterator found = mcConfig_.invisible_ids_.find(part.pdgid());
    if (found==mcConfig_.invisible_ids_.end()) return false; else return true;
  }

  /**
   * @brief Is the particle invisible?
   *
   * @param part particle (0 allowed).
   * @return true if invisible.
   */
  inline MAbool IsInvisible(const MCParticleFormat* part) const
  {
    if (part==0) return false;
    return IsInvisible(*part);
  }

  /**
   * @brief Is the PDG code a B hadron?
   *
   * @param pdg PDG code.
   * @return true for a B hadron.
   */
  MAbool IsBHadron(MAint32 pdg)
  {
    MAuint32 apdg = std::abs(pdg);
    MAbool btag;
    return btag = ( (apdg >=500 && apdg <= 599) ||
                    (apdg>=5000 && apdg <= 5999) ||
                    (apdg>=10500 && apdg <= 10599 ) ||
                    ( apdg>=20500 && apdg <=20599 ) );
  }

  /**
   * @brief Is the particle a B hadron?
   *
   * @param part particle.
   * @return true for a B hadron.
   */
  MAbool IsBHadron(const MCParticleFormat& part)
  {
    return IsBHadron(part.pdgid());
  }

  /**
   * @brief Is the particle a B hadron?
   *
   * @param part particle (0 allowed).
   * @return true for a B hadron.
   */
  MAbool IsBHadron(const MCParticleFormat* part)
  {
    if (part==0) return false;
    return IsBHadron(part->pdgid());
  }

  /**
   * @brief Is the PDG code a C hadron?
   *
   * @param pdg PDG code.
   * @return true for a C hadron.
   */
  MAbool IsCHadron(MAint32 pdg)
  {
    MAuint32 apdg = std::abs(pdg);
    MAbool ctag;
    return ctag = ( (apdg >=400 && apdg <= 499) ||
                    (apdg>=4000 && apdg <= 4999) ||
                    (apdg>=10400 && apdg <= 10499 ) ||
                    ( apdg>=20400 && apdg <=20499 ) );
  }

  /**
   * @brief Is the particle a C hadron?
   *
   * @param part particle.
   * @return true for a C hadron.
   */
  MAbool IsCHadron(const MCParticleFormat& part)
  {
    return IsCHadron(part.pdgid());
  }

  /**
   * @brief Is the particle a C hadron?
   *
   * @param part particle (0 allowed).
   * @return true for a C hadron.
   */
  MAbool IsCHadron(const MCParticleFormat* part)
  {
    if (part==0) return false;
    return IsCHadron(part->pdgid());
  }

  /**
   * @brief Is the muon isolated?
   *
   * With the DeltaR method, the muon must be separated from all the jets; with the
   * SumPT method, the isolation sums of the muon are compared with the thresholds of
   * the reconstruction configuration.
   *
   * @param muon muon.
   * @param event reconstructed event.
   * @return true if isolated (false for null pointers).
   */
  MAbool IsIsolatedMuon(const RecLeptonFormat* muon,
                        const RecEventFormat* event) const
  {
    // Safety
    if (muon==0 || event==0) return false;

    // Method : DeltaR
    if (recConfig_.deltaRalgo_)
    {
      // Loop over jets
      for (MAuint32 i=0;i<event->jets().size();i++)
      {
        if ( muon->dr(event->jets()[i]) < recConfig_.deltaR_ ) return false;
      }
      return true;
    }

    // Method : SumPT
    else
    {
      return ( muon->sumPT_isol() < recConfig_.sumPT_ && 
               muon->ET_PT_isol() < recConfig_.ET_PT_  );
    }

    return true;
  } 

  /**
   * @brief Is the muon isolated?
   *
   * @param part muon.
   * @param event reconstructed event.
   * @return true if isolated.
   */
  MAbool IsIsolatedMuon(const RecLeptonFormat& part,
                        const RecEventFormat* event) const
  {
    return IsIsolatedMuon(&part,event);
  }



};

}

#endif
