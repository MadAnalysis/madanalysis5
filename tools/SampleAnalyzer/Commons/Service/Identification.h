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

#ifndef IDENTIFICATION_SERVICE_h
#define IDENTIFICATION_SERVICE_h

// STL headers

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/MCconfig.h"
#include "SampleAnalyzer/Commons/DataFormat/MCEventFormat.h"


namespace MA5
{

class Identification
{
  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  private:
    int finalstate_;
    int initialstate_;
    MCconfig mcConfig_;
    RECconfig recConfig_;

  public:

    /// Constructor
    Identification()
    {
      initialstate_=-1; finalstate_=1;
    }

    /// Destructor
    ~Identification() { }

    /// Accessors to the config objects
    const MCconfig& mcConfig() const
    {  return mcConfig_; }
    MCconfig& mcConfig()
    {  return mcConfig_; }
    const RECconfig& recConfig() const
    { return recConfig_; }
    RECconfig& recConfig()
    { return recConfig_; }

  /// Is Initial State
  Bool_t IsInitialState(const MCParticleFormat& part) const
  {
    return (part.statuscode()==-1 || (part.statuscode()>=11 && part.statuscode()<=19));
  }

  /// Is Final State
  Bool_t IsFinalState(const MCParticleFormat& part) const
  {
    return (part.statuscode()==finalstate_);
  }

  /// Is Inter State
  Bool_t IsInterState(const MCParticleFormat& part) const
  {
    return (!IsInitialState(part) && !IsFinalState(part));
  }

  /// Is Initial State
  Bool_t IsInitialState(const MCParticleFormat* part) const
  {
    return (part->statuscode()==initialstate_);
  }

  /// Is Final State
  Bool_t IsFinalState(const MCParticleFormat* part) const
  {
    return (part->statuscode()==finalstate_);
  }

  /// Is Inter State
  Bool_t IsInterState(const MCParticleFormat* part) const
  {
    return (part->statuscode()!=finalstate_ && part->statuscode()!=initialstate_);
  }

  /// Set Initial State
  void SetInitialState(const MCEventFormat* myEvent)
  {
    if (myEvent==0) return; 
    if (myEvent->particles().empty()) return;
    initialstate_=myEvent->particles()[0].statuscode(); 
  }

  /// Set Final State
  void SetFinalState(const MCEventFormat* myEvent)
  {
    if (myEvent==0) return; 
    if (myEvent->particles().empty()) return;
    finalstate_=myEvent->particles()[myEvent->particles().size()-1].statuscode(); 
  }

  /// Is hadronic ?
  inline bool IsHadronic(const RecParticleFormat* part) const
  {
    if (dynamic_cast<const RecJetFormat*>(part)==0) return false;
    else return true;
  }

  /// Is invisible ?
  inline bool IsInvisible(const RecParticleFormat* part) const
  {
    return false;
  }

  inline bool IsHadronic(const RecParticleFormat& part) const
  {
    return IsHadronic(&part);
  }

  /// Is invisible ?
  inline bool IsInvisible(const RecParticleFormat& part) const
  {
    return IsInvisible(&part);
  }


  /// Is hadronic ?
  inline bool IsHadronic(const MCParticleFormat& part) const
  {
    std::set<Int_t>::iterator found = mcConfig_.hadronic_ids_.find(part.pdgid());
    if (found==mcConfig_.hadronic_ids_.end()) return false; else return true;
  }

  /// Is hadronic ?
  inline bool IsHadronic(Int_t pdgid) const
  {
    std::set<Int_t>::iterator found = mcConfig_.hadronic_ids_.find(pdgid);
    if (found==mcConfig_.hadronic_ids_.end()) return false; else return true;
  }

  /// Is hadronic ?
  inline bool IsHadronic(const MCParticleFormat* part) const
  {
    if (part==0) return false;
    return IsHadronic(*part);
  }

  /// Is invisible ?
  inline bool IsInvisible(const MCParticleFormat& part) const
  {
    std::set<Int_t>::iterator found = mcConfig_.invisible_ids_.find(part.pdgid());
    if (found==mcConfig_.invisible_ids_.end()) return false; else return true;
  }

  /// Is invisible ?
  inline bool IsInvisible(const MCParticleFormat* part) const
  {
    if (part==0) return false;
    return IsInvisible(*part);
  }

  ///Is B Hadron ?
  Bool_t IsBHadron(Int_t pdg)
  {
    UInt_t apdg = std::abs(pdg);
    Bool_t btag;
    return btag = ( (apdg >=500 && apdg <= 599) ||
                    (apdg>=5000 && apdg <= 5999) ||
                    (apdg>=10500 && apdg <= 10599 ) ||
                    ( apdg>=20500 && apdg <=20599 ) );
  }

  ///Is B Hadron ?
  Bool_t IsBHadron(const MCParticleFormat& part)
  {
    return IsBHadron(part.pdgid());
  }

  ///Is B Hadron ?
  Bool_t IsBHadron(const MCParticleFormat* part)
  {
    if (part==0) return false;
    return IsBHadron(part->pdgid());
  }

  ///Is C Hadron ?
  Bool_t IsCHadron(Int_t pdg)
  {
    UInt_t apdg = std::abs(pdg);
    Bool_t ctag;
    return ctag = ( (apdg >=400 && apdg <= 499) ||
                    (apdg>=4000 && apdg <= 4999) ||
                    (apdg>=10400 && apdg <= 10499 ) ||
                    ( apdg>=20400 && apdg <=20499 ) );
  }

  ///Is C Hadron ?
  Bool_t IsCHadron(const MCParticleFormat& part)
  {
    return IsCHadron(part.pdgid());
  }

  ///Is C Hadron ?
  Bool_t IsCHadron(const MCParticleFormat* part)
  {
    if (part==0) return false;
    return IsCHadron(part->pdgid());
  }

  /// Muon isolation
  Bool_t IsIsolatedMuon(const RecLeptonFormat* muon,
                        const RecEventFormat* event) const
  {
    // Safety
    if (muon==0 || event==0) return false;

    // Method : DeltaR
    if (recConfig_.deltaRalgo_)
    {
      // Loop over jets
      for (unsigned int i=0;i<event->jets().size();i++)
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

  /// Muon isolation
  Bool_t IsIsolatedMuon(const RecLeptonFormat& part,
                        const RecEventFormat* event) const
  {
    return IsIsolatedMuon(&part,event);
  }



};

}

#endif
