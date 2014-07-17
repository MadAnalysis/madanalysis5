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
#include "SampleAnalyzer/Commons/Service/TransverseVariables.h"
#include "SampleAnalyzer/Commons/Service/Identification.h"

#define PHYSICS MA5::PhysicsService::getInstance()


namespace MA5
{

class PhysicsService
{

  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
 protected:
  static PhysicsService* service_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Transverse variable toolbox
  TransverseVariables *Transverse;

  /// Identification method toolbox
  Identification *Id;

  /// GetInstance
  static PhysicsService* getInstance()
  {
    if (service_==0) service_ = new PhysicsService;
    return service_;
  }

  /// Kill
  static void kill()
  {
    if (service_!=0) delete service_;
    service_=0;
  }

  /// Get MCconfig
  const MCconfig& mcConfig() const  
  { return Id->mcConfig(); }
  MCconfig& mcConfig()
  { return Id->mcConfig(); }

  /// Get RECconfig
  const RECconfig& recConfig() const  
  { return Id->recConfig(); }
  RECconfig& recConfig()
  { return Id->recConfig(); }

  /// MT 
  Float_t MT(const MCParticleFormat& part, const MCEventFormat* event)
  {
    // Computing ET sum
    double ETsum = part.et() + event->MET().et();

    // Computing PT sum
    TLorentzVector pt = part.momentum() + event->MET().momentum();

    double value = ETsum*ETsum - pt.Pt()*pt.Pt();
    if (value<0) return 0;
    else return sqrt(value);
  }

  Float_t MT(const MCParticleFormat* part, const MCEventFormat* event)
  {
    if (part==0) return false;
    return MT(*part,event);
  }

  ///Get the decay mode : 1 = Tau --> e nu nu
  ///                     2 = Tau --> mu nu nu
  ///  	       	       	  3 = Tau --> K nu
  ///  	       	       	  4 = Tau --> K* nu
  ///  	       	       	  5 = Tau --> Rho (--> pi pi0) nu
  ///  	       	       	  6 = Tau --> A1 (--> pi 2pi0) nu
  ///  	       	       	  7 = Tau --> A1 (--> 3pi) nu
  ///  	       	       	  8 = Tau --> pi nu
  ///  	       	       	  9 = Tau --> 3pi pi0 nu
  ///                     0 = other
  ///                    -1 = error
  Int_t GetTauDecayMode (const MCParticleFormat* part)
  {
    if (part==0) return -1;

    if (fabs(part->pdgid())!=15) 
    {
      WARNING << "Particle is not a Tau" << endmsg;
      return -1;
    }

    Int_t npi = 0;

    for (unsigned int i=0;i<part->daughters().size();i++)
    {
      Int_t pdgid = part->daughters()[i]->pdgid();
      if (fabs(pdgid) == 11) return 1;
      else if (fabs(pdgid) == 13) return 2;
      else if (fabs(pdgid) == 321) return 3;
      else if (fabs(pdgid) == 323) return 4;
      else if (fabs(pdgid) == 213) return 5;
      else if (fabs(pdgid) == 20213)
      {
       	Int_t pi = 0;
        for (unsigned int j=0;j<part->daughters()[i]->daughters().size();j++)
        {
          if (fabs(part->daughters()[i]->daughters()[j]->pdgid()) == 211) pi++;
        }
        if (pi == 1) return 6;
        else if (pi == 3) return 7;
      }
      else if (fabs(pdgid) == 211) npi++;
      else if (fabs(pdgid) == 24)
      {
       	for (unsigned int j=0;j<part->daughters()[i]->daughters().size();j++)
        {
          if (fabs(part->daughters()[i]->daughters()[j]->pdgid()) == 211) npi++;
        }
      }
    }

    if (npi == 1) return 8;
    else if (npi == 3) return 9;
    else return 0;
  }

  Int_t GetTauDecayMode (const MCParticleFormat& part)
  {
    return GetTauDecayMode(&part);
  }

  /// Compute srqt(S)
  inline double SqrtS(const MCEventFormat* event) const
  {
    TLorentzVector q(0.,0.,0.,0.);
    for (UInt_t i=0;i<event->particles().size();i++)
    {
      if ( Id->IsInitialState(event->particles()[i]) )
        q += event->particles()[i].momentum();
    }
    return sqrt(q.Mag2());
  }

 private:

  /// Constructor
  PhysicsService()  
  {
    Transverse = new TransverseVariables();
    Id = new Identification();
  }

  /// Destructor
  ~PhysicsService()
  {}
};

}

#endif
