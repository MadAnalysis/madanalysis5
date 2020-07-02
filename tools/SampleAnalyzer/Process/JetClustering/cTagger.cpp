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


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/JetClustering/cTagger.h"

// STL headers
#include <cmath>


using namespace MA5;

// Matching using dr
void cTagger::Method1 (SampleFormat& mySample, EventFormat& myEvent)
{
  std::vector<RecJetFormat*> Candidates;

  // loop on the particles searching for last c
  for (MAuint32 i=0;i<myEvent.mc()->particles().size();i++)
  {
    if (PHYSICS->Id->IsInitialState(myEvent.mc()->particles()[i])) continue;
    if (std::abs(myEvent.mc()->particles()[i].pdgid())!=4) continue;
    if (!IsLast(&myEvent.mc()->particles()[i], myEvent)) continue;

    MAbool tag = false;
    MAfloat64 DeltaRmax = DeltaRmax_;

    // loop on the jets
    for (MAuint32 j=0;j<myEvent.rec()->jets().size();j++)
    {
      if (myEvent.rec()->jets()[j].btag()) continue;

      // Calculating DeltaR
      MAfloat32 DeltaR = myEvent.mc()->particles()[i].dr(myEvent.rec()->jets()[j]);

      // Adding the jet to the candidates if DeltaR <= DeltaRmax
      if (DeltaR <= DeltaRmax) 
      {
        if (Exclusive_)
        {
          if (tag) Candidates.pop_back();
          tag = true;
          DeltaRmax = DeltaR;
        }
        Candidates.push_back(& myEvent.rec()->jets()[j]);
      }
    }

    // Tagging the candidates
    for (MAuint32 i=0;i<Candidates.size();i++)
    {
      Candidates[i]->true_ctag_ = true;
    }
    
    Candidates.clear();
  }
}

// Matching using history
void cTagger::Method2 (SampleFormat& mySample, EventFormat& myEvent)
{ 
  std::vector<RecJetFormat*> Candidates;

  for (MAuint32 i=0;i<myEvent.rec()->jets().size();i++)
  {
    if (myEvent.rec()->jets()[i].btag()) continue;

    MAbool c = false;

    for (MAuint32 j=0;j<myEvent.rec()->jets()[i].Constituents_.size();j++)
    {
      MAint32 N = myEvent.rec()->jets()[i].Constituents_[j];
      MCParticleFormat* particle = & myEvent.mc()->particles()[N];
      while (!c)
      {
        if (particle==0) 
        {
          INFO << "No particle" << endmsg;
          break;
        }

        if (particle->statuscode()==3) break;

        if (PHYSICS->Id->IsCHadron(particle->pdgid()) && IsLastCHadron(particle, myEvent))
        {
          c = true;
          myEvent.rec()->jets()[i].mc_ = particle;
          break;
        }

        if (particle->mothers()[1]!=0 && particle->mothers()[1]!=particle->mothers()[0]) break;

        particle = particle->mothers()[0];
      }

      if (c) break;
    }

    if (c) Candidates.push_back(& myEvent.rec()->jets()[i]);
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
    Candidates[i]->true_ctag_ = true;
  }

  Candidates.clear();
}

void cTagger::Method3 (SampleFormat& mySample, EventFormat& myEvent)
{
  std::vector<RecJetFormat*> Candidates;

  // jet preselection using method 2
  for (MAuint32 i=0;i<myEvent.rec()->jets().size();i++)
  {
    if (myEvent.rec()->jets()[i].btag()) continue;

    MAbool c = false;

    for (MAuint32 j=0;j<myEvent.rec()->jets()[i].Constituents_.size();j++)
    {
      MAint32 N = myEvent.rec()->jets()[i].Constituents_[j];
      MCParticleFormat* particle = & myEvent.mc()->particles()[N];
      while (!c)
      {
        if (particle==0)
        {
          INFO << "No particle" << endmsg;
          break;
        }

        if (particle->statuscode()==3) break;

        if (PHYSICS->Id->IsCHadron(particle->pdgid()) && IsLastCHadron(particle, myEvent))
        {
          c = true;
          myEvent.rec()->jets()[i].mc_ = particle;
          break;
        }

        if (particle->mothers()[1]!=0 && particle->mothers()[1]!=particle->mothers()[0]) break;

        particle = particle->mothers()[0];
      }

      if (c) break;
    }

    if (c) Candidates.push_back(& myEvent.rec()->jets()[i]);
  }

  // c-tagging using method 1
  for (MAuint32 i=0;i<myEvent.mc()->particles().size();i++)
  {
    if (std::abs(myEvent.mc()->particles()[i].pdgid())!=4) continue;

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
      Candidates[Candidates.size()-1]->true_ctag_=true;
      Candidates.pop_back();
    }
  }

  Candidates.clear();
}

MAbool cTagger::IsLastCHadron(MCParticleFormat* part, EventFormat& myEvent)
{
  for (MAuint32 i=0; i<myEvent.mc()->particles().size(); i++)
  {
    if (myEvent.mc()->particles()[i].mothers()[0]== part)
    {
      if (PHYSICS->Id->IsCHadron(myEvent.mc()->particles()[i].pdgid())) return false;
    }
  }
  return true;
}

std::string cTagger::GetParameters()
{
  std::stringstream str;
  //  str << "R=" << R_ << " ; p=" << p_ << " ; PTmin=" << Ptmin_;
  return str.str();
}
