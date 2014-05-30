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


#include "SampleAnalyzer/Process/JetClustering/bTagger.h"
using namespace MA5;

void bTagger::Method1 (SampleFormat& mySample, EventFormat& myEvent)
{
  std::vector<RecJetFormat*> Candidates;

  // Matching b-quarks to jets
  for (unsigned int i=0;i<myEvent.rec()->MCBquarks_.size();i++)
  {
    RecJetFormat* tag = 0;
    Double_t DeltaRmax = DeltaRmax_;

    // loop on the jets
    for (unsigned int j=0;j<myEvent.rec()->jets().size();j++)
    {
      if (myEvent.rec()->jets()[j].pt()<1e-10) continue;
      Float_t DeltaR = myEvent.rec()->MCBquarks_[i]->dr(myEvent.rec()->jets()[j]);

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
  for (unsigned int i=0;i<Candidates.size();i++)
  {
    Candidates[i]->true_btag_ = true;
  }
  Candidates.clear();

  // Matching c-quarks to jets
  for (unsigned int i=0;i<myEvent.rec()->MCCquarks_.size();i++)
  {
    RecJetFormat* tag = 0;
    Double_t DeltaRmax = DeltaRmax_;

    // loop on the jets
    for (unsigned int j=0;j<myEvent.rec()->jets().size();j++)
    {
      if (myEvent.rec()->jets()[j].pt()<1e-10) continue;

      Float_t DeltaR = 
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
  for (unsigned int i=0;i<Candidates.size();i++)
  {
    if (Candidates[i]->true_btag_) continue;
    Candidates[i]->true_ctag_ = true;
  }

  // Identification and misidentification
  for (unsigned int i=0;i<myEvent.rec()->jets().size();i++)
  {
    RecJetFormat* jet = &(myEvent.rec()->jets()[i]);

    // 100% identification
    if (jet->true_btag_) jet->btag_=true;
    if (!doEfficiency_ && !doMisefficiency_) continue;

    // identification efficiency
    if (doEfficiency_ && jet->true_btag_)
    {
      if (gRandom->Rndm()  >= Efficiency_) jet->btag_=false;
    }

    // mis-identification (c-quark)
    if (doMisefficiency_ && !jet->true_btag_ && jet->true_ctag_)
    {
      if (gRandom->Rndm() < misid_cjet_) jet->btag_=true;
    }

    // mis-identification (light quarks)
    else if (doMisefficiency_ && !jet->true_btag_ && !jet->true_ctag_)
    {
      if (gRandom->Rndm() < misid_ljet_) jet->btag_=true;
    }
  }

}

void bTagger::Method2 (SampleFormat& mySample, EventFormat& myEvent)
{ 
  std::vector<RecJetFormat*> Candidates;

  for (unsigned int i=0;i<myEvent.rec()->jets().size();i++)
  {
    Bool_t b = false;

    for (unsigned int j=0;j<myEvent.rec()->jets()[i].Constituents_.size();j++)
    {
      Int_t N = myEvent.rec()->jets()[i].Constituents_[j];
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

	if (particle->mother2()!=0 && particle->mother2()!=particle->mother1()) break;

        particle = particle->mother1();
      }

      if (b) break;
    }

    if (b) Candidates.push_back(& myEvent.rec()->jets()[i]);
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
    Candidates[i]->btag_ = true;
  }
}

void bTagger::Method3 (SampleFormat& mySample, EventFormat& myEvent)
{
  std::vector<RecJetFormat*> Candidates;

  // jet preselection using method 2
  for (unsigned int i=0;i<myEvent.rec()->jets().size();i++)
  {
    Bool_t b = false;

    for (unsigned int j=0;j<myEvent.rec()->jets()[i].Constituents_.size();j++)
    {
      Int_t N = myEvent.rec()->jets()[i].Constituents_[j];
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

	if (particle->mother2()!=0 && particle->mother2()!=particle->mother1()) break;

        particle = particle->mother1();
      }

      if (b) break;
    }

    if (b) Candidates.push_back(& myEvent.rec()->jets()[i]);
  }

  // b-tagging using method 1
  for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
  {
    if (fabs(myEvent.mc()->particles()[i].pdgid())!=5) continue;

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
      Candidates[Candidates.size()-1]->btag_=true;
      Candidates.pop_back();
    }
  }

  Candidates.clear();
}

Bool_t bTagger::IsLastBHadron(MCParticleFormat* part, EventFormat& myEvent)
{
  for (unsigned int i=0; i<myEvent.mc()->particles().size(); i++)
  {
    if (myEvent.mc()->particles()[i].mother1()== part)
    {
      if (PHYSICS->Id->IsBHadron(myEvent.mc()->particles()[i].pdgid())) return false;
    }
  }
  return true;
}


bool bTagger::SetParameter(const std::string& key, 
                           const std::string& value, 
                           std::string header)
{
  // miss-id efficiency
  if (key=="misid_ljet")
  {
    Float_t tmp=0;
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
    Float_t tmp=0;
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
