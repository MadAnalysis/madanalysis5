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


#include "SampleAnalyzer/Process/JetClustering/cTagger.h"
using namespace MA5;

// Matching using dr
void cTagger::Method1 (SampleFormat& mySample, EventFormat& myEvent)
{
  std::vector<RecJetFormat*> Candidates;

  // loop on the particles searching for last c
  for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
  {
    if (PHYSICS->Id->IsInitialState(myEvent.mc()->particles()[i])) continue;
    if (fabs(myEvent.mc()->particles()[i].pdgid())!=4) continue;
    if (!IsLast(&myEvent.mc()->particles()[i], myEvent)) continue;

    Bool_t tag = false;
    Double_t DeltaRmax = DeltaRmax_;

    // loop on the jets
    for (unsigned int j=0;j<myEvent.rec()->jets().size();j++)
    {
      if (myEvent.rec()->jets()[j].btag()) continue;

      // Calculating DeltaR
      Float_t DeltaR = myEvent.mc()->particles()[i].dr(myEvent.rec()->jets()[j]);

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
    for (unsigned int i=0;i<Candidates.size();i++)
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

  for (unsigned int i=0;i<myEvent.rec()->jets().size();i++)
  {
    if (myEvent.rec()->jets()[i].btag()) continue;

    Bool_t c = false;

    for (unsigned int j=0;j<myEvent.rec()->jets()[i].Constituents_.size();j++)
    {
      Int_t N = myEvent.rec()->jets()[i].Constituents_[j];
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

        if (particle->mother2()!=0 && particle->mother2()!=particle->mother1()) break;

        particle = particle->mother1();
      }

      if (c) break;
    }

    if (c) Candidates.push_back(& myEvent.rec()->jets()[i]);
  }

  if (Exclusive_)
  {
    UInt_t i = 0;
    UInt_t n = Candidates.size();

    while (i<n)
    {
      UInt_t j = i+1;

      Float_t DeltaR = Candidates[i]->mc()->dr(Candidates[i]);

      while (j<n)
      {
        if (Candidates[i]->mc()==Candidates[j]->mc())
        {
          Float_t DeltaR2 = Candidates[j]->mc()->dr(Candidates[j]);

          if (DeltaR2<DeltaR) std::swap(Candidates[i], Candidates[j]);

          Candidates.erase(Candidates.begin()+j);
          n--;
        }
        else j++;
      }

      i++;
    }
  }

  for (unsigned int i=0;i<Candidates.size();i++)
  {
    Candidates[i]->true_ctag_ = true;
  }

  Candidates.clear();
}

void cTagger::Method3 (SampleFormat& mySample, EventFormat& myEvent)
{
  std::vector<RecJetFormat*> Candidates;

  // jet preselection using method 2
  for (unsigned int i=0;i<myEvent.rec()->jets().size();i++)
  {
    if (myEvent.rec()->jets()[i].btag()) continue;

    Bool_t c = false;

    for (unsigned int j=0;j<myEvent.rec()->jets()[i].Constituents_.size();j++)
    {
      Int_t N = myEvent.rec()->jets()[i].Constituents_[j];
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

        if (particle->mother2()!=0 && particle->mother2()!=particle->mother1()) break;

        particle = particle->mother1();
      }

      if (c) break;
    }

    if (c) Candidates.push_back(& myEvent.rec()->jets()[i]);
  }

  // c-tagging using method 1
  for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
  {
    if (fabs(myEvent.mc()->particles()[i].pdgid())!=4) continue;

    if (!IsLast(&myEvent.mc()->particles()[i], myEvent)) continue;

    UInt_t k = 0;

    for (unsigned int j=Candidates.size();j>0;j--)
    {
      Float_t DeltaR = myEvent.mc()->particles()[i].dr(Candidates[j-1]);

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

    for (unsigned int j=0;j<k;j++)
    {
      Candidates[Candidates.size()-1]->true_ctag_=true;
      Candidates.pop_back();
    }
  }

  Candidates.clear();
}

Bool_t cTagger::IsLastCHadron(MCParticleFormat* part, EventFormat& myEvent)
{
  for (unsigned int i=0; i<myEvent.mc()->particles().size(); i++)
  {
    if (myEvent.mc()->particles()[i].mother1()== part)
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
