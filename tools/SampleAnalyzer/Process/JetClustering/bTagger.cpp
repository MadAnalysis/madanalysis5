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


// SampleAnalyzer headesr
#include "SampleAnalyzer/Commons/Service/RandomService.h"
#include "SampleAnalyzer/Process/JetClustering/bTagger.h"
#include <cmath>


using namespace MA5;


void bTagger::Method1 (SampleFormat& mySample, EventFormat& myEvent)
{
  std::vector<RecJetFormat*> Candidates;

  // Matching b-quarks to jets
  for (MAuint32 i=0;i<myEvent.rec()->MCBquarks_.size();i++)
  {
    RecJetFormat* tag = 0;
    MAfloat64 DeltaRmax = DeltaRmax_;

    // loop on the jets
    for (MAuint32 j=0;j<myEvent.rec()->jets().size();j++)
    {
      if (myEvent.rec()->jets()[j].pt()<1e-10) continue;
      MAfloat32 DeltaR = myEvent.rec()->MCBquarks_[i]->dr(myEvent.rec()->jets()[j]);

      if (DeltaR <= DeltaRmax) 
      {
        if (Exclusive_)
        {
          tag = &(myEvent.rec()->jets()[j]);
          DeltaRmax = DeltaR;
        }
        else Candidates.push_back(& myEvent.rec()->jets()[j]);
      }
    }
    if (Exclusive_ && tag!=0) Candidates.push_back(tag);
  }

  // Tagging the b-jet 
  for (MAuint32 i=0;i<Candidates.size();i++)
  {
    Candidates[i]->true_btag_ = true;
  }
  Candidates.clear();

  // Matching c-quarks to jets
  for (MAuint32 i=0;i<myEvent.rec()->MCCquarks_.size();i++)
  {
    RecJetFormat* tag = 0;
    MAfloat64 DeltaRmax = DeltaRmax_;

    // loop on the jets
    for (MAuint32 j=0;j<myEvent.rec()->jets().size();j++)
    {
      if (myEvent.rec()->jets()[j].pt()<1e-10) continue;

      MAfloat32 DeltaR = 
          myEvent.rec()->MCCquarks_[i]->dr(myEvent.rec()->jets()[j]);

      if (DeltaR <= DeltaRmax) 
      {
        if (Exclusive_)
        {
          tag = &(myEvent.rec()->jets()[j]);
          DeltaRmax = DeltaR;
        }
        else Candidates.push_back(& myEvent.rec()->jets()[j]);
      }
    }
    if (Exclusive_ && tag!=0) Candidates.push_back(tag);
  }

  // Tagging the c-jet 
  for (MAuint32 i=0;i<Candidates.size();i++)
  {
    if (Candidates[i]->true_btag_) continue;
    Candidates[i]->true_ctag_ = true;
  }

  // Identification and misidentification
  for (MAuint32 i=0;i<myEvent.rec()->jets().size();i++)
  {
    RecJetFormat* jet = &(myEvent.rec()->jets()[i]);

    // 100% identification
    if (jet->true_btag_) jet->btag_=true;
    if (!doEfficiency_ && !doMisefficiency_) continue;

    // identification efficiency
    if (doEfficiency_ && jet->true_btag_)
    {
      if (RANDOM->flat()  >= Efficiency_) jet->btag_=false;
    }

    // mis-identification (c-quark)
    if (doMisefficiency_ && !jet->true_btag_ && jet->true_ctag_)
    {
      if (RANDOM->flat() < misid_cjet_) jet->btag_=true;
    }

    // mis-identification (light quarks)
    else if (doMisefficiency_ && !jet->true_btag_ && !jet->true_ctag_)
    {
      if (RANDOM->flat() < misid_ljet_) jet->btag_=true;
    }
  }

}

void bTagger::Method2 (SampleFormat& mySample, EventFormat& myEvent)
{ 
  std::vector<RecJetFormat*> Candidates;

  for (MAuint32 i=0;i<myEvent.rec()->jets().size();i++)
  {
    MAbool b = false;

    for (MAuint32 j=0;j<myEvent.rec()->jets()[i].Constituents_.size();j++)
    {
      MAint32 N = myEvent.rec()->jets()[i].Constituents_[j];
      MCParticleFormat* particle = & myEvent.mc()->particles()[N];
      while (!b)
      {
        if (particle==0)
        {
          INFO << "No particle" << endmsg;
          break;
        }

  if (particle->statuscode()==3) break;

        if (PHYSICS->Id->IsBHadron(particle->pdgid()) && IsLastBHadron(particle, myEvent))
        {
          b = true;
          myEvent.rec()->jets()[i].mc_ = particle;
          break;
        }

        if (particle->mothers().size()>1 && particle->mothers()[1]!=particle->mothers()[0]) break;

        particle = particle->mothers()[0];
      }

      if (b) break;
    }

    if (b) Candidates.push_back(& myEvent.rec()->jets()[i]);
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

  for (MAuint32 i=0;i<Candidates.size();i++)
  {
    Candidates[i]->btag_ = true;
  }
}

void bTagger::Method3 (SampleFormat& mySample, EventFormat& myEvent)
{
  std::vector<RecJetFormat*> Candidates;

  // jet preselection using method 2
  for (MAuint32 i=0;i<myEvent.rec()->jets().size();i++)
  {
    MAbool b = false;

    for (MAuint32 j=0;j<myEvent.rec()->jets()[i].Constituents_.size();j++)
    {
      MAint32 N = myEvent.rec()->jets()[i].Constituents_[j];
      MCParticleFormat* particle = & myEvent.mc()->particles()[N];
      while (!b)
      {
       if (particle==0)
        {
          INFO << "No particle" << endmsg;
          break;
        }

  if (particle->statuscode()==3) break;

        if (PHYSICS->Id->IsBHadron(particle->pdgid()) && IsLastBHadron(particle, myEvent))
        {
          b = true;
          myEvent.rec()->jets()[i].mc_ = particle;
          break;
        }

        if (particle->mothers().size()>1 && particle->mothers()[1]!=particle->mothers()[0]) break;

        particle = particle->mothers()[0];
      }

      if (b) break;
    }

    if (b) Candidates.push_back(& myEvent.rec()->jets()[i]);
  }

  // b-tagging using method 1
  for (MAuint32 i=0;i<myEvent.mc()->particles().size();i++)
  {
    if (std::abs(myEvent.mc()->particles()[i].pdgid())!=5) continue;

    if (!IsLast(&myEvent.mc()->particles()[i], myEvent)) continue;

    MAuint32 k = 0;

    for (MAuint32 j=Candidates.size();j>0;j--)
    {
      MAfloat32 DeltaR = myEvent.mc()->particles()[i].dr(Candidates[j-1]);

      if (DeltaR <= DeltaRmax_)
      {
        k++;
        std::swap (Candidates[j-1], Candidates[Candidates.size()-k]);
      }
    }

    if (Exclusive_)
    {
      while (k>1)
      {
        if (Candidates[Candidates.size()-1]->e() > Candidates[Candidates.size()-2]->e()) std::swap(Candidates[Candidates.size()-1], Candidates[Candidates.size()-2]);
        Candidates.pop_back();
        k--;
      }
    }

    for (MAuint32 j=0;j<k;j++)
    {
      Candidates[Candidates.size()-1]->btag_=true;
      Candidates.pop_back();
    }
  }

  Candidates.clear();
}

MAbool bTagger::IsLastBHadron(MCParticleFormat* part, EventFormat& myEvent)
{
  for (MAuint32 i=0; i<myEvent.mc()->particles().size(); i++)
  {
    if (myEvent.mc()->particles()[i].mothers()[0]== part)
    {
      if (PHYSICS->Id->IsBHadron(myEvent.mc()->particles()[i].pdgid())) return false;
    }
  }
  return true;
}


MAbool bTagger::SetParameter(const std::string& key, 
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
    if (misid_ljet_!=0.0) doMisefficiency_=true;
  }

  // miss-id efficiency
  else if (key=="misid_cjet")
  {
    MAfloat32 tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    if (tmp<0) 
    {
      WARNING << "'misid_cjet' efficiency must be a positive value. "
              << "Using the default value = " 
              << misid_cjet_ << endmsg;
    }
    else if (tmp>1)
    {
      WARNING << "'misid_cjet' efficiency cannot be greater than 1. "
              << "Using the default value = " 
              << misid_cjet_ << endmsg;
    }
    else misid_cjet_=tmp;
    if (misid_cjet_!=0.0) doMisefficiency_=true;
  }

  // Other
  else return TaggerBase::SetParameter(key,value,header);
  return true;
}

std::string bTagger::GetParameters()
{
  std::stringstream str;
  str << "dR=" << DeltaRmax_ << " ; ";
  if (Exclusive_) str << "Exclusive ; "; else str << "Inclusive ; ";
  str << "IDeff=" << Efficiency_;
  str << " ; MisID(c)=" << misid_cjet_;
  str << " ; MisID(q)=" << misid_ljet_;
  return str.str();
}
