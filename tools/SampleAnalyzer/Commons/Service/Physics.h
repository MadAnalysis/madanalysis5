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
 * @file Physics.h
 * @brief Singleton gathering the physics toolboxes (PHYSICS).
 */

#ifndef PHYSICS_SERVICE_h
#define PHYSICS_SERVICE_h


// STL headers
#include <set>
#include <string>
#include <algorithm>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/MCconfig.h"
#include "SampleAnalyzer/Commons/Service/RECconfig.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/DataFormat/MCEventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecEventFormat.h"
#include "SampleAnalyzer/Commons/Service/Identification.h"
#include "SampleAnalyzer/Commons/Service/Isolation.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"
#include "SampleAnalyzer/Commons/Service/RestFramesHelper.h"
#include "SampleAnalyzer/Commons/Service/TransverseVariables.h"


/** @brief Shortcut to the PhysicsService singleton. */
#define PHYSICS MA5::PhysicsService::getInstance()


namespace MA5
{

/**
 * @brief Singleton gathering the physics toolboxes.
 *
 * PHYSICS->Id (identification, e.g. IsFinalState), PHYSICS->Isol (isolation),
 * PHYSICS->Transverse (MT2, alphaT, ...), PHYSICS->RF (RestFrames helper), and a few
 * event-level helpers.
 */
class PhysicsService
{

  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
 protected:
  /** @brief Unique instance. */
  static PhysicsService* service_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Toolbox of transverse variables (MT2, alphaT, ...). */
  TransverseVariables *Transverse;
  /** @brief Helper owning RestFrames objects. */
  RestFramesHelper *RF;

  /** @brief Toolbox of identification methods. */
  Identification *Id;

  /** @brief Toolbox of isolation methods. */
  Isolation* Isol;

  /**
   * @brief Get the unique instance (created at the first call).
   *
   * @return the instance.
   */
  static PhysicsService* getInstance()
  {
    if (service_==0) service_ = new PhysicsService;
    return service_;
  }

  /** @brief Delete the unique instance. */
  static void kill()
  {
    if (service_!=0) delete service_;
    service_=0;
  }

  /**
   * @brief Accessor to the Monte Carlo configuration (hadronic/invisible PDG codes).
   *
   * @return the configuration.
   */
  const MCconfig& mcConfig() const  
  { return Id->mcConfig(); }
  /**
   * @brief Accessor to the Monte Carlo configuration.
   *
   * @return the configuration.
   */
  MCconfig& mcConfig()
  { return Id->mcConfig(); }

  /**
   * @brief Accessor to the reconstruction configuration (isolation).
   *
   * @return the configuration.
   */
  const RECconfig& recConfig() const  
  { return Id->recConfig(); }
  /**
   * @brief Accessor to the reconstruction configuration.
   *
   * @return the configuration.
   */
  RECconfig& recConfig()
  { return Id->recConfig(); }

  /**
   * @brief Nominal weight of an event.
   *
   * @param event Monte Carlo event.
   * @return the weight.
   */
  inline MAfloat64 weights(const MCEventFormat* event) const
  {
    return event->weight();
  }

  /**
   * @brief Transverse mass of a particle and the missing transverse momentum.
   *
   * @param part particle.
   * @param event Monte Carlo event (MET).
   * @return the transverse mass (0 if the squared mass is negative).
   */
  MAfloat32 MT(const MCParticleFormat& part, const MCEventFormat* event)
  {
    // Computing ET sum
    MAfloat64 ETsum = part.et() + event->MET().et();

    // Computing PT sum
    MALorentzVector pt = part.momentum() + event->MET().momentum();

    MAfloat64 value = ETsum*ETsum - pt.Pt()*pt.Pt();
    if (value<0) return 0;
    else return sqrt(value);
  }


  /**
   * @brief Transverse mass of a particle and the missing transverse momentum.
   *
   * @param part particle (0 allowed).
   * @param event Monte Carlo event (MET).
   * @return the transverse mass (0 for a null pointer).
   */
  MAfloat32 MT(const MCParticleFormat* part, const MCEventFormat* event)
  {
    if (part==0) return false;
    return MT(*part,event);
  }

  /**
   * @brief Decay mode of a Monte Carlo tau from its daughters.
   *
   * @param part tau.
   * @return 1 e nu nu, 2 mu nu nu, 3 K nu, 4 K* nu, 5 rho nu, 6 a1 (-> pi 2pi0) nu, 7 a1 (-> 3pi) nu,
   *         8 pi nu, 9 3pi pi0 nu, 0 other, -1 error (not a tau or null pointer).
   */
  MAint32 GetTauDecayMode (const MCParticleFormat* part)
  {
    if (part==0) return -1;

    try
    {
      if (std::abs(part->pdgid())!=15) throw EXCEPTION_WARNING("Particle is not a Tau.","",0);
    }
    catch(const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
      return -1;
    }    

    MAint32 npi = 0;

    for (MAuint32 i=0;i<part->daughters().size();i++)
    {
      MAint32 pdgid = part->daughters()[i]->pdgid();
      if (std::abs(pdgid) == 11) return 1;
      else if (std::abs(pdgid) == 13) return 2;
      else if (std::abs(pdgid) == 321) return 3;
      else if (std::abs(pdgid) == 323) return 4;
      else if (std::abs(pdgid) == 213) return 5;
      else if (std::abs(pdgid) == 20213)
      {
        MAint32 pi = 0;
        for (MAuint32 j=0;j<part->daughters()[i]->daughters().size();j++)
        {
          if (std::abs(part->daughters()[i]->daughters()[j]->pdgid()) == 211) pi++;
        }
        if (pi == 1) return 6;
        else if (pi == 3) return 7;
      }
      else if (std::abs(pdgid) == 211) npi++;
      else if (std::abs(pdgid) == 24)
      {
        for (MAuint32 j=0;j<part->daughters()[i]->daughters().size();j++)
        {
          if (std::abs(part->daughters()[i]->daughters()[j]->pdgid()) == 211) npi++;
        }
      }
    }

    if (npi == 1) return 8;
    else if (npi == 3) return 9;
    else return 0;
  }

  /**
   * @brief Decay mode of a Monte Carlo tau from its daughters.
   *
   * @param part tau.
   * @return the decay mode (see the pointer version).
   */
  MAint32 GetTauDecayMode (const MCParticleFormat& part)
  {
    return GetTauDecayMode(&part);
  }

  /**
   * @brief Partonic centre-of-mass energy, computed from the initial-state particles.
   *
   * @param event Monte Carlo event.
   * @return sqrt(s-hat).
   */
  inline MAfloat64 SqrtS(const MCEventFormat* event) const
  {
    MALorentzVector q(0.,0.,0.,0.);
    for (MAuint32 i=0;i<event->particles().size();i++)
    {
      if ( Id->IsInitialState(event->particles()[i]) )
        q += event->particles()[i].momentum();
    }
    return sqrt(q.Mag2());
  }

 private:

  /** @brief Constructor (creates the toolboxes). */
  PhysicsService()  
  {
    RF = new RestFramesHelper();
    Transverse = new TransverseVariables();
    Id = new Identification();
    Isol = new Isolation();
  }

  /** @brief Destructor. */
  ~PhysicsService()
  {
    delete RF;
    delete Transverse;
    delete Id;
    delete Isol;
  }

};

}

#endif
