////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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


// STL headers
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/JetClustering/TauTagger.h"
#include "SampleAnalyzer/Commons/Service/RandomService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


using namespace MA5;


void TauTagger::Method1 (SampleFormat& mySample, EventFormat& myEvent)
{
  // Performing mis-id
  if (doMisefficiency_)
  {
    std::vector<MAuint32> toRemove;
    for (MAuint32 i=0;i<myEvent.rec()->jets().size();i++)
    {
      // keeping only light jets
      if (myEvent.rec()->jets()[i].true_ctag_ || 
          myEvent.rec()->jets()[i].true_btag_) continue;

      // simulating mis-id
      if (RANDOM->flat() < misid_ljet_)
      {
        RecTauFormat* myTau = myEvent.rec()->GetNewTau();
        Jet2Tau(&myEvent.rec()->jets()[i], myTau, myEvent);
        toRemove.push_back(i);
      }
    }
    for (MAint32 i=toRemove.size()-1; i>=0;i--)
    {
      myEvent.rec()->jets().erase(myEvent.rec()->jets().begin()
                                  + toRemove[i]);
    }
  }

}

void TauTagger::Method2 (SampleFormat& mySample, EventFormat& myEvent)
{
  std::vector<RecJetFormat*> Candidates;

  for (MAuint32 i=0;i<myEvent.rec()->jets().size();i++)
  {
    if (myEvent.rec()->jets()[i].ntracks()!=1 && myEvent.rec()->jets()[i].ntracks()!=3) continue;

    MAbool tag = false;
    // Loop on the jets constituents
    for (MAuint32 j=0;j<myEvent.rec()->jets()[i].Constituents_.size();j++)
    {
      // Searching for a tau in the history
      MAint32 N = myEvent.rec()->jets()[i].Constituents_[j];
      MCParticleFormat* particle = & myEvent.mc()->particles()[N];
      while (!tag)
      {
        if (particle==0)
        {
          ERROR << "No particle" << endmsg;
          break;
        }
  if (particle->statuscode()==3) break;

        if (std::abs(particle->pdgid())==15)
        {
          tag = true;
          myEvent.rec()->jets()[i].mc_ = particle;
          break;
        }

      if (particle->mothers().size()>1 && particle->mothers()[1]!=particle->mothers()[0]) break;

        particle = particle->mothers()[0];
      }

      if (tag) break;
    }

    if (tag) Candidates.push_back(& myEvent.rec()->jets()[i]);
  }

  if (Exclusive_)
  {
    MAuint32 i = 0;
    MAuint32 n = Candidates.size();

    while (i<n)
    {
      MAuint32 j = i+1;

      MAfloat32 DeltaR = Candidates[i]->mc()->dr(Candidates[i]);

      while (j<n)
      {
        // If two candidates are matching with the same tau, erasing the one with the greater Delta R and reducing n (the size of the vector)
        if (Candidates[i]->mc()==Candidates[j]->mc())
        {
          MAfloat32 DeltaR2 = Candidates[j]->mc()->dr(Candidates[j]);

          if (DeltaR2<DeltaR) std::swap(Candidates[i], Candidates[j]);

          Candidates.erase(Candidates.begin()+j);
          n--;
  }
  else j++;
      }

      i++;
    }
  }

  sort(Candidates.begin(),Candidates.end());

  for (MAuint32 i=Candidates.size();i>0;i--)
  {
    RecTauFormat* myTau = myEvent.rec()->GetNewTau();
    Jet2Tau(Candidates[i-1], myTau, myEvent);
    // BENJ PB COMPIL:   myEvent.rec()->jets().erase((std::vector<RecJetFormat>::iterator) Candidates[i-1]);
    
    // Remove the tau-identified jet from the list
    MAuint32 pos=myEvent.rec()->jets().size();
    for (MAuint32 ind=0;ind<myEvent.rec()->jets().size();ind++)
    {
      if (&(myEvent.rec()->jets()[ind])==Candidates[i-1]) {pos=ind;break;}
    }
    if (pos!=myEvent.rec()->jets().size()) // must never happen
        myEvent.rec()->jets().erase(myEvent.rec()->jets().begin()+pos);
  }
  Candidates.clear();
}

void TauTagger::Method3 (SampleFormat& mySample, EventFormat& myEvent)
{
  std::vector<RecJetFormat*> Candidates;

  // Jets preselection using method 2
  for (MAuint32 i=0;i<myEvent.rec()->jets().size();i++)
  {
    if (myEvent.rec()->jets()[i].ntracks()!=1 && myEvent.rec()->jets()[i].ntracks()!=3) continue;

    MAbool tag = false;
    for (MAuint32 j=0;j<myEvent.rec()->jets()[i].Constituents_.size();j++)
    {
      MAint32 N = myEvent.rec()->jets()[i].Constituents_[j];
      MCParticleFormat* particle = & myEvent.mc()->particles()[N];
      while (!tag)
      {
        if (particle==0)
        {
          ERROR << "No particle" << endmsg;
          break;
        }

        if (particle->statuscode()==3) break;

        if (std::abs(particle->pdgid())==15)
        {
          tag = true;
          myEvent.rec()->jets()[i].mc_ = particle;
          break;
        }

        if (particle->mothers().size()>1 && particle->mothers()[1]!=particle->mothers()[0]) break;

        particle = particle->mothers()[0];
      }

      if (tag) break;
    }

    if (tag) Candidates.push_back(& myEvent.rec()->jets()[i]);
  }

  std::vector<RecJetFormat*> Taus;

  // tau-tagging using method 1
  for (MAuint32 i=0;i<myEvent.mc()->particles().size();i++)
  {
    if (std::abs(myEvent.mc()->particles()[i].pdgid())!=15) continue;

    if (!IsLast(&myEvent.mc()->particles()[i], myEvent)) continue;

    MAfloat64 DeltaRmax = DeltaRmax_; 
    MAbool tag = false;

    for (MAuint32 j=Candidates.size();j>0;j--)
    {
      MAfloat32 DeltaR = myEvent.mc()->particles()[i].dr(Candidates[j-1]);

      if (DeltaR <= DeltaRmax)
      {
        if (Exclusive_)
        {
          if (tag) Taus.pop_back();
          tag = true;
          DeltaRmax = DeltaR;
        }
     Taus.push_back(Candidates[j-1]);
     Candidates.erase(Candidates.begin()+j-1);
      }
    }
  }

  sort(Taus.begin(),Taus.end());

  for (MAuint32 j=Taus.size();j>0;j--)
  {
    RecTauFormat* myTau = myEvent.rec()->GetNewTau();
    Jet2Tau(Taus[j-1], myTau, myEvent);
    // PB Benj compil:  myEvent.rec()->jets().erase((std::vector<RecJetFormat>::iterator) Taus[j-1]);

    // Remove the tau-identified jet from the list
    MAuint32 pos=myEvent.rec()->jets().size();
    for (MAuint32 ind=0;ind<myEvent.rec()->jets().size();ind++)
    {
      if (&(myEvent.rec()->jets()[ind])==Taus[j-1]) {pos=ind;break;}
    }
    if (pos!=myEvent.rec()->jets().size()) // must never happen
        myEvent.rec()->jets().erase(myEvent.rec()->jets().begin()+pos);


  }

  Taus.clear();
  Candidates.clear();
}


void TauTagger::Jet2Tau (RecJetFormat* myJet, RecTauFormat* myTau, EventFormat& myEvent)
{
  myTau->setMomentum(myJet->momentum());
  myTau->ntracks_   = myJet->ntracks();
  myTau->mc_        = myJet->mc_;
  myTau->DecayMode_ = PHYSICS->GetTauDecayMode(myTau->mc_);

  MAint32 charge = 0;

  for (MAuint32 i=0;i<myJet->Constituents_.size();i++)
  {
    charge += PDG->GetCharge(myEvent.mc()->particles()[myJet->Constituents_[i]].pdgid());
  }

  if (charge>0) myTau->charge_ = true;
  else if (charge<=0) myTau->charge_ = false;
}


MAbool TauTagger::SetParameter(const std::string& key, 
                             const std::string& value, 
                             std::string header)
{
  // miss-id efficiency
  if (key=="misid_ljet")
  {
    MAfloat32 tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    if (tmp<0) 
    {
      WARNING << "'misid_ljet' efficiency must be a positive value. "
              << "Using the default value = " 
              << misid_ljet_ << endmsg;
    }
    else if (tmp>1)
    {
      WARNING << "'misid_ljet' efficiency cannot be greater than 1. "
              << "Using the default value = " 
              << misid_ljet_ << endmsg;
    }
    else misid_ljet_=tmp;
    if (misid_ljet_==0.0) doMisefficiency_=false; else doMisefficiency_=true;
  }

  // Other
  else return TaggerBase::SetParameter(key,value,header);

  return true;

}


std::string TauTagger::GetParameters()
{
  std::stringstream str;
  str << "IDeff=" << Efficiency_;
  str << " ; MisID(q)=" << misid_ljet_;
  return str.str();
}
