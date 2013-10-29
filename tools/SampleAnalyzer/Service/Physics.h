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
#include "SampleAnalyzer/Service/MCconfig.h"
#include "SampleAnalyzer/Service/RECconfig.h"
#include "SampleAnalyzer/Service/LogService.h"
#include "SampleAnalyzer/DataFormat/MCEventFormat.h"
#include "SampleAnalyzer/DataFormat/RecEventFormat.h"


#define PHYSICS MA5::PhysicsService::getInstance()


namespace MA5
{

enum OrderingObservable{Eordering, Pordering, PTordering, 
                        ETordering, PXordering, PYordering,
                        PZordering, ETAordering};



struct PointerComparison
{
  template<typename T>
  static bool ESortPredicate(T* part1, 
                             T* part2)
  { return part1->e() > part2->e(); }

  template<typename T>
  static bool ETSortPredicate(T* part1, 
                              T* part2)
  { return part1->et() > part2->et(); }

  template<typename T>
  static bool PSortPredicate(T* part1, 
                             T* part2)
  { return part1->p() > part2->p(); }

  template<typename T>
  static bool PTSortPredicate(T* part1, 
                              T* part2)
  { return part1->pt() > part2->pt(); }

  template<typename T>
  static bool ETASortPredicate(T* part1, 
                               T* part2)
  { return part1->eta() > part2->eta(); }

  template<typename T>
  static bool PXSortPredicate(T* part1, 
                              T* part2)
  { return part1->px() > part2->px(); }

  template<typename T>
  static bool PYSortPredicate(T* part1, 
                       T* part2)
  { return part1->py() > part2->py(); }

  template<typename T>
  static bool PZSortPredicate(T* part1, 
                              T* part2)
  { return part1->pz() > part2->pz(); }

};


class PhysicsService
{

  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
 protected:

  MCconfig mcConfig_;
  RECconfig recConfig_;
  Int_t finalstate_;
  Int_t initialstate_;
  static PhysicsService* service_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

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


  /// Is Initial State
  Bool_t IsInitialState(const MCParticleFormat& part) const
  {
    //    return (part.statuscode()==initialstate_);
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
    initialstate_=myEvent->particles()[myEvent->particles().size()-1].statuscode(); 
  }

  /// Set Final State
  void SetFinalState(const MCEventFormat* myEvent)
  {
   
    if (myEvent==0) return; 
    if (myEvent->particles().empty()) return;
    finalstate_=myEvent->particles()[myEvent->particles().size()-1].statuscode(); 
  }

  /// Get MCconfig
  const MCconfig& mcConfig() const  
  { return mcConfig_; }
  MCconfig& mcConfig()
  { return mcConfig_; }

  /// Get RECconfig
  const RECconfig& recConfig() const  
  { return recConfig_; }
  RECconfig& recConfig()
  { return recConfig_; }

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

  /// Is invisible ?
  inline bool IsInvisible(const MCParticleFormat& part) const
  {
    std::set<Int_t>::iterator found = mcConfig_.invisible_ids_.find(part.pdgid());
    if (found==mcConfig_.invisible_ids_.end()) return false; else return true;
  }

  /// Is hadronic ?
  inline bool IsHadronic(const MCParticleFormat* part) const
  {
    if (part==0) return false;
    return IsHadronic(*part);
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

  ///Is B Hadron ?
  Float_t MT(const MCParticleFormat* part, const MCEventFormat* event)
  {
    if (part==0) return false;
    return MT(*part,event);
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

    for (unsigned int i=0;i<part->Daughters().size();i++)
    {
      Int_t pdgid = part->Daughters()[i]->pdgid();
      if (fabs(pdgid) == 11) return 1;
      else if (fabs(pdgid) == 13) return 2;
      else if (fabs(pdgid) == 321) return 3;
      else if (fabs(pdgid) == 323) return 4;
      else if (fabs(pdgid) == 213) return 5;
      else if (fabs(pdgid) == 20213)
      {
       	Int_t pi = 0;
        for (unsigned int j=0;j<part->Daughters()[i]->Daughters().size();j++)
        {
          if (fabs(part->Daughters()[i]->Daughters()[j]->pdgid()) == 211) pi++;
        }
        if (pi == 1) return 6;
        else if (pi == 3) return 7;
      }
      else if (fabs(pdgid) == 211) npi++;
      else if (fabs(pdgid) == 24)
      {
       	for (unsigned int j=0;j<part->Daughters()[i]->Daughters().size();j++)
        {
          if (fabs(part->Daughters()[i]->Daughters()[j]->pdgid()) == 211) npi++;
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

  /// Compute the total transverse energy
  inline double EventTET(const MCEventFormat* event) const
  { 
    /*    double energy=0;
    for (unsigned int i=0;i<event->particles().size();i++)
    {
      if (event->particles()[i].statuscode()==finalstate_)
          energy+=event->particles()[i].pt();
    }
    return energy;*/
    return event->TET();
  }

  /// Compute the missing transverse energy
  inline double EventMET(const MCEventFormat* event) const
  {
    /*    TLorentzVector q(0.,0.,0.,0.);
    for (unsigned int i=0;i<event->particles().size();i++)
    {
      if (event->particles()[i].statuscode()==finalstate_)
        if (!IsInvisible(event->particles()[i]))
          q+=event->particles()[i].momentum();
    }
    return q.Perp();
    */
    return event->MET().pt();
  }

  /// Compute the Alpha_T
  void LoopForAlphaT(const unsigned int n1, const std::vector<const MCParticleFormat*> jets,
    double &MinDHT, const int last, std::vector<unsigned int> Ids) const
  {
     // We have enough information to form the pseudo jets
     if(Ids.size()==n1)
     {
       // Forming the two pseudo jets
       std::vector<const MCParticleFormat*> jets1;
       std::vector<const MCParticleFormat*> jets2=jets;
       for (int j=n1-1; j>=0; j--)
       { 
          jets1.push_back(jets[Ids[j]]);
          jets2.erase(jets2.begin()+Ids[j]);
       }

       // Computing the DeltaHT of the pseudo jets and checking if minimum
       double THT1 = 0; double THT2 = 0;
       for (unsigned int i=0;i<jets1.size();i++) THT1+=jets1[i]->et();
       for (unsigned int i=0;i<jets2.size();i++) THT2+=jets2[i]->et();
       double DeltaHT = fabs(THT1-THT2);
       if (DeltaHT<MinDHT) MinDHT=DeltaHT;

      // Exit
      return;
     }

     // The first pseudo jet is incomplete -> adding one element
     std::vector<unsigned int> Save=Ids;
     for(unsigned int i=last+1; i<=jets.size()-n1+Save.size(); i++)
     {
       Ids = Save;   
       Ids.push_back(i);   
       LoopForAlphaT(n1, jets, MinDHT, i, Ids); 
     }
  }

  inline double AlphaT(const MCEventFormat* event) const
  {
    std::vector<const MCParticleFormat*> jets;

    // Creating jet collection
    for (unsigned int i=0;i<event->particles().size();i++)
    {
      if (event->particles()[i].statuscode()!=finalstate_) continue;
      if (event->particles()[i].pdgid()!=21 && (abs(event->particles()[i].pdgid())<1 || 
          abs(event->particles()[i].pdgid())>5)) continue;
      jets.push_back(&event->particles()[i]);
    }

    // safety
    if (jets.size()<2) return 0;

    // dijet event
    if (jets.size()==2) return std::min(jets[0]->et(),jets[1]->et()) / 
      (*(jets[0])+*(jets[1])).mt();

    double MinDeltaHT = 1e6;

    // compute vectum sum of jet momenta
    TLorentzVector q(0.,0.,0.,0.);
    for (unsigned int i=0;i<jets.size();i++) q+=jets[i]->momentum();
    double MHT = q.Pt();

    // compute HT
    double THT = 0;
    for (unsigned int i=0;i<jets.size();i++) THT+=jets[i]->et();

    // Safety
    if (THT==0) return -1.;
    else if (MHT/THT>=1) return -1.;

    // more than 3 jets : split into 2 sets
    // n1 = number of jets in the first set
    // n2 = number of jets in the second set
    for (unsigned int n1=1; n1<=(jets.size()/2); n1++)
    {
      std::vector<unsigned int> DummyJet;
      LoopForAlphaT(n1,jets,MinDeltaHT,-1,DummyJet);
    }

    // Final
    return 0.5*(1.-MinDeltaHT/THT)/sqrt(1.-MHT/THT*MHT/THT);
  }



   void LoopForAlphaT(const unsigned int n1, std::vector<RecJetFormat> jets,
    double &MinDHT, const int last, std::vector<unsigned int> Ids) const
  {
     // We have enough information to form the pseudo jets
     if(Ids.size()==n1)
     {
       // Forming the two pseudo jets
       std::vector<RecJetFormat> jets1;
       std::vector<RecJetFormat> jets2=jets;
       for (int j=n1-1; j>=0; j--)
       { 
          jets1.push_back(jets[Ids[j]]);
          jets2.erase(jets2.begin()+Ids[j]);
       }

       // Computing the DeltaHT of the pseudo jets and checking if minimum
       double THT1 = 0; double THT2 = 0;
       for (unsigned int i=0;i<jets1.size();i++) THT1+=jets1[i].et();
       for (unsigned int i=0;i<jets2.size();i++) THT2+=jets2[i].et();
       double DeltaHT = fabs(THT1-THT2);
       if (DeltaHT<MinDHT) MinDHT=DeltaHT;

      // Exit
      return;
     }

     // The first pseudo jet is incomplete -> adding one element
     std::vector<unsigned int> Save=Ids;
     for(unsigned int i=last+1; i<=jets.size()-n1+Save.size(); i++)
     {
       Ids = Save;   
       Ids.push_back(i);   
       LoopForAlphaT(n1, jets, MinDHT, i, Ids); 
     }
  }

 inline double AlphaT(const RecEventFormat* event) const
  {
    // jets
    std::vector<RecJetFormat> jets = event->jets();

    // safety
    if (jets.size()<2) return 0;

    // dijet event
    if (jets.size()==2) return std::min(jets[0].et(),jets[1].et()) / 
      ((jets[0])+(jets[1])).mt();

    double MinDeltaHT = 1e6;

    // compute vectum sum of jet momenta
    TLorentzVector q(0.,0.,0.,0.);
    for (unsigned int i=0;i<jets.size();i++) q+=jets[i].momentum();
    double MHT = q.Pt();

    // compute HT
    double THT = 0;
    for (unsigned int i=0;i<jets.size();i++) THT+=jets[i].et();

    // Safety
    if (THT==0) return -1.;
    else if (MHT/THT>=1) return -1.;

    // more than 3 jets : split into 2 sets
    // n1 = number of jets in the first set
    // n2 = number of jets in the second set
    for (unsigned int n1=1; n1<=(jets.size()/2); n1++)
	    {
	      std::vector<unsigned int> DummyJet;
      LoopForAlphaT(n1,jets,MinDeltaHT,-1,DummyJet);
    }

    // Final
    return 0.5*(1.-MinDeltaHT/THT)/sqrt(1.-MHT/THT*MHT/THT);
  }


  /// Compute the total hadronic transverse energy
  inline double EventTHT(const MCEventFormat* event) const
  {
    /*    double energy=0;
    for (unsigned int i=0;i<event->particles().size();i++)
    {
      if (event->particles()[i].statuscode()==finalstate_)
        if (IsHadronic(event->particles()[i]))
          energy+=event->particles()[i].pt();
    }
    return energy;*/
    return event->THT();
  }

  /// Compute the missing hadronic transverse energy
  inline double EventMHT(const MCEventFormat* event) const
  {
    /*    TLorentzVector q(0.,0.,0.,0.);
    for (unsigned int i=0;i<event->particles().size();i++)
    {
      // rejecting non finalstate particle
      if (event->particles()[i].statuscode()!=finalstate_) continue;

      if (!IsInvisible(event->particles()[i]) &&
          IsHadronic(event->particles()[i]))
        q+=event->particles()[i].momentum();
    }
    return q.Perp();*/
    return event->MHT().pt();
  }

  /// Compute the total transverse energy
  inline double EventTET(const RecEventFormat* event) const
  { 
    /*    double energy=0;

    for (unsigned int i=0;i<event->jets().size();i++)
      energy+=event->jets()[i].et();
    for (unsigned int i=0;i<event->electrons().size();i++)
      energy+=event->electrons()[i].et();
    for (unsigned int i=0;i<event->muons().size();i++)
      energy+=event->muons()[i].et();
    for (unsigned int i=0;i<event->taus().size();i++)
      energy+=event->taus()[i].et();
  
      return energy;*/
    return event->TET();
  }

  /// Compute the missing transverse energy
  inline double EventMET(const RecEventFormat* event) const
  {
    return event->MET().pt();
  }

  /// Compute the total hadronic transverse energy
  inline double EventTHT(const RecEventFormat* event) const
  {
    /*    double energy=0;
    for (unsigned int i=0;i<event->jets().size();i++)
    {
      energy+=event->jets()[i].et();
    }
    return energy;*/
    return event->THT();
  }

  /// Compute the missing hadronic transverse energy
  inline double EventMHT(const RecEventFormat* event) const
  {
    /*    TLorentzVector q(0.,0.,0.,0.);
    for (unsigned int i=0;i<event->jets().size();i++)
    {
      q+=event->jets()[i].momentum();
    }
    return q.Et(); 
    */
    return event->MHT().pt();
  }

  /// rank filter
  std::vector<const MCParticleFormat*> 
  rankFilter(std::vector<const MCParticleFormat*> ref, Short_t rank,
             OrderingObservable obs=PTordering) const
  {
    // rejecting case where rank equal to zero
    if (rank==0)
    {
      WARNING << "Rank equal to 0 is not possible. "
              << "Allowed values are 1,2,3,... and -1,-2,-3,..." << endmsg;
      return std::vector<const MCParticleFormat*>();
    }

    // Number of particle is not correct
    if ( (static_cast<Int_t>(ref.size()) - 
          static_cast<Int_t>(std::abs(rank)))<0 ) 
      return std::vector<const MCParticleFormat*>();

    // Sorting reference collection of particles
    sort(ref,obs);

    // Keeping the only particle
    std::vector<const MCParticleFormat*> parts(1);
    if (rank>0) parts[0]=ref[rank-1];
    else parts[0]=ref[ref.size()+rank];

    // Saving tmp
    return parts;
  }

  /// rank filter
  std::vector<const RecParticleFormat*> 
  rankFilter(std::vector<const RecParticleFormat*> ref, Short_t rank,
             OrderingObservable obs=PTordering) const
  {
    // rejecting case where rank equal to zero
    if (rank==0)
    {
      WARNING << "Rank equal to 0 is not possible. "
              << "Allowed values are 1,2,3,... and -1,-2,-3,..." << endmsg;
      return std::vector<const RecParticleFormat*>();
    }

    // Number of particle is not correct
    if ( (static_cast<Int_t>(ref.size()) - 
          static_cast<Int_t>(std::abs(rank)))<0 ) 
      return std::vector<const RecParticleFormat*>();

    // Sorting reference collection of particles
    sort(ref,obs);

    // Keeping the only particle
    std::vector<const RecParticleFormat*> parts(1);
    if (rank>0) parts[0]=ref[rank-1];
    else parts[0]=ref[ref.size()+rank];

    // Saving tmp
    return parts;
  }


  /// sort particle
  void sort(std::vector<const MCParticleFormat*>& parts,
            OrderingObservable obs=PTordering) const
  {
    if (obs==PTordering) 
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PTSortPredicate<const MCParticleFormat>);
    else if (obs==ETordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ETSortPredicate<const MCParticleFormat>);
    else if (obs==Eordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ESortPredicate<const MCParticleFormat>);
    else if (obs==ETAordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ETASortPredicate<const MCParticleFormat>);
    else if (obs==PXordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PXSortPredicate<const MCParticleFormat>);
    else if (obs==PYordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PYSortPredicate<const MCParticleFormat>);
    else if (obs==PZordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PZSortPredicate<const MCParticleFormat>);
  }

  /// sort particle
  void sort(std::vector<const RecParticleFormat*>& parts,
            OrderingObservable obs=PTordering) const
  {
    if (obs==PTordering) 
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PTSortPredicate<const RecParticleFormat>);
    else if (obs==ETordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ETSortPredicate<const RecParticleFormat>);
    else if (obs==Eordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ESortPredicate<const RecParticleFormat>);
    else if (obs==ETAordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ETASortPredicate<const RecParticleFormat>);
    else if (obs==PXordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PXSortPredicate<const RecParticleFormat>);
    else if (obs==PYordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PYSortPredicate<const RecParticleFormat>);
    else if (obs==PZordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PZSortPredicate<const RecParticleFormat>);
  }


  void ToRestFrame(MCParticleFormat& part, const MCParticleFormat* boost) const
  {
    if (boost==0) return;
    ToRestFrame(part,*boost);
  }

  void ToRestFrame(MCParticleFormat& part, const MCParticleFormat& boost) const
  {
    TVector3 b = -1. * boost.momentum().BoostVector();
    part.momentum().Boost(b);
  }

  /// Compute srqt(S)
  inline double SqrtS(const MCEventFormat* event) const
  {
    TLorentzVector q(0.,0.,0.,0.);
    for (UInt_t i=0;i<event->particles().size();i++)
    {
      if ( event->particles()[i].statuscode() == initialstate_ )
        q += event->particles()[i].momentum();
    }
    return sqrt(q.Mag2());
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

 private:

  /// Constructor
  PhysicsService()  
  { initialstate_=-1; finalstate_=1; }

  /// Destructor
  ~PhysicsService()
  {}
};

}

#endif
