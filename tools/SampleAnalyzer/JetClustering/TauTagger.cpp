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


#include "SampleAnalyzer/JetClustering/TauTagger.h"
using namespace MA5;


void TauTagger::Method1 (SampleFormat& mySample, EventFormat& myEvent)
{
  // loop on the particles searching for tau
  for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
  {
    // Keeping only taus
    if (fabs(myEvent.mc()->particles()[i].pdgid())!=15) continue;

    // Removing initial states
    if (PHYSICS->IsInitialState(myEvent.mc()->particles()[i])) continue;

    // Removing final states
    if (PHYSICS->IsFinalState(myEvent.mc()->particles()[i])) continue;

    // Keeping the last taus in the decay chain
    if (!IsLast(&myEvent.mc()->particles()[i], myEvent)) continue;

    // Removing taus decaying to leptons
    bool leptonic=true;
    bool muonic=false;
    bool electronic=false;
    for (unsigned int j=0;j<myEvent.mc()->particles()[i].Daughters().size();j++)
    {
      unsigned int pdgid = 
        std::abs(myEvent.mc()->particles()[i].Daughters()[j]->pdgid());
      if (pdgid==13) muonic=true;
      else if (pdgid==11) electronic=true;
      if (pdgid!=22 && !(pdgid>=11 && pdgid<=18)) {leptonic=false;}
    }
    if (leptonic && muonic)
    {
      bool found=false;
      for (unsigned int j=0;j<myEvent.rec()->MCMuonicTaus_.size();j++)
      {
        if (myEvent.rec()->MCMuonicTaus_[j]==&(myEvent.mc()->particles()[i])) 
        {found=true; break;}
      }
      if (!found) 
        myEvent.rec()->MCMuonicTaus_.push_back(&(myEvent.mc()->particles()[i]));
    }
    else if (leptonic && electronic)
    {
      bool found=false;
      for (unsigned int j=0;j<myEvent.rec()->MCElectronicTaus_.size();j++)
      {
        if (myEvent.rec()->MCElectronicTaus_[j]==&(myEvent.mc()->particles()[i])) 
        {found=true; break;}
      }
      if (!found) 
        myEvent.rec()->MCElectronicTaus_.push_back(&(myEvent.mc()->particles()[i]));
    }
    else if (!leptonic)
    {
      bool found=false;
      for (unsigned int j=0;j<myEvent.rec()->MCHadronicTaus_.size();j++)
      {
        if (myEvent.rec()->MCHadronicTaus_[j]==&(myEvent.mc()->particles()[i])) 
        {found=true; break;}
      }
      if (!found) 
        myEvent.rec()->MCHadronicTaus_.push_back(&(myEvent.mc()->particles()[i]));

    }
  }

  // Matching MCtaus and RECtaus
  std::vector<std::pair<RecJetFormat*,MCParticleFormat*> > Candidates;
  for (unsigned int i=0;i<myEvent.rec()->MCHadronicTaus_.size();i++)
  {
    RecJetFormat* tag = 0;
    Double_t DeltaRmax = DeltaRmax_;

    // loop on the jets
    for (unsigned int j=0;j<myEvent.rec()->jets().size();j++)
    {
      //      if (myEvent.rec()->jets()[j].ntracks()!=1 && 
      //    myEvent.rec()->jets()[j].ntracks()!=3) continue;

      // Calculating Delta R
      Float_t DeltaR = 
         myEvent.rec()->MCHadronicTaus_[i]->dr(myEvent.rec()->jets()[j]);

      if (DeltaR <= DeltaRmax)
      {
        if(Exclusive_)
        {
          tag = &(myEvent.rec()->jets()[j]);
          DeltaRmax = DeltaR;
        }
        else Candidates.push_back( 
             std::make_pair(& myEvent.rec()->jets()[j],
                            myEvent.rec()->MCHadronicTaus_[i]) );
      }
    }
    if (Exclusive_ && tag!=0) 
         Candidates.push_back(std::make_pair(tag,
                                             myEvent.rec()->MCHadronicTaus_[i]));
  }

  // Performing efficiency
  for (unsigned int i=0;i<Candidates.size();i++)
  {
    if (!IsIdentified()) continue;
    Candidates[i].first->mc_ = Candidates[i].second;
    RecTauFormat* myTau = myEvent.rec()->GetNewTau();
    Jet2Tau(Candidates[i].first, myTau, myEvent);
    //      myEvent.rec()->jets().erase((std::vector<RecJetFormat>::iterator) Candidates[j-1]);
  }

  // Performing mis-id
  if (doMisefficiency_)
  {
    for (unsigned int i=0;i<myEvent.rec()->jets().size();i++)
    {
      // keeping only light jets
      if (myEvent.rec()->jets()[i].true_ctag_ || 
          myEvent.rec()->jets()[i].true_btag_) continue;

      // simulating mis-id
      if (gRandom->Rndm() < misid_ljet_)
      {
        RecTauFormat* myTau = myEvent.rec()->GetNewTau();
        Jet2Tau(&myEvent.rec()->jets()[i], myTau, myEvent);
      }
    }
  }

}

void TauTagger::Method2 (SampleFormat& mySample, EventFormat& myEvent)
{
  std::vector<RecJetFormat*> Candidates;

  for (unsigned int i=0;i<myEvent.rec()->jets().size();i++)
  {
    if (myEvent.rec()->jets()[i].ntracks()!=1 && myEvent.rec()->jets()[i].ntracks()!=3) continue;

    Bool_t tag = false;
    // Loop on the jets constituents
    for (unsigned int j=0;j<myEvent.rec()->jets()[i].Constituents_.size();j++)
    {
      // Searching for a tau in the history
      Int_t N = myEvent.rec()->jets()[i].Constituents_[j];
      MCParticleFormat* particle = & myEvent.mc()->particles()[N];
      while (!tag)
      {
       	if (particle==0)
        {
          ERROR << "No particle" << endmsg;
          break;
        }
	if (particle->statuscode()==3) break;

        if (fabs(particle->pdgid())==15)
        {
          tag = true;
          myEvent.rec()->jets()[i].mc_ = particle;
          break;
        }

	if (particle->mother2()!=0 && particle->mother2()!=particle->mother1()) break;

        particle = particle->mother1();
      }

      if (tag) break;
    }

    if (tag) Candidates.push_back(& myEvent.rec()->jets()[i]);
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
        // If two candidates are matching with the same tau, erasing the one with the greater Delta R and reducing n (the size of the vector)
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

  sort(Candidates.begin(),Candidates.end());

  for (unsigned int i=Candidates.size();i>0;i--)
  {
    RecTauFormat* myTau = myEvent.rec()->GetNewTau();
    Jet2Tau(Candidates[i-1], myTau, myEvent);
    myEvent.rec()->jets().erase((std::vector<RecJetFormat>::iterator) Candidates[i-1]);
  }
  Candidates.clear();
}

void TauTagger::Method3 (SampleFormat& mySample, EventFormat& myEvent)
{
  std::vector<RecJetFormat*> Candidates;

  // Jets preselection using method 2
  for (unsigned int i=0;i<myEvent.rec()->jets().size();i++)
  {
    if (myEvent.rec()->jets()[i].ntracks()!=1 && myEvent.rec()->jets()[i].ntracks()!=3) continue;

    Bool_t tag = false;
    for (unsigned int j=0;j<myEvent.rec()->jets()[i].Constituents_.size();j++)
    {
      Int_t N = myEvent.rec()->jets()[i].Constituents_[j];
      MCParticleFormat* particle = & myEvent.mc()->particles()[N];
      while (!tag)
      {
       	if (particle==0)
        {
          ERROR << "No particle" << endmsg;
          break;
        }

        if (particle->statuscode()==3) break;

        if (fabs(particle->pdgid())==15)
        {
          tag = true;
          myEvent.rec()->jets()[i].mc_ = particle;
          break;
        }

	if (particle->mother2()!=0 && particle->mother2()!=particle->mother1()) break;

        particle = particle->mother1();
      }

      if (tag) break;
    }

    if (tag) Candidates.push_back(& myEvent.rec()->jets()[i]);
  }

  std::vector<RecJetFormat*> Taus;

  // tau-tagging using method 1
  for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
  {
    if (fabs(myEvent.mc()->particles()[i].pdgid())!=15) continue;

    if (!IsLast(&myEvent.mc()->particles()[i], myEvent)) continue;

    Double_t DeltaRmax = DeltaRmax_; 
    Bool_t tag = false;

    for (unsigned int j=Candidates.size();j>0;j--)
    {
      Float_t DeltaR = myEvent.mc()->particles()[i].dr(Candidates[j-1]);

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

  for (unsigned int j=Taus.size();j>0;j--)
  {
    RecTauFormat* myTau = myEvent.rec()->GetNewTau();
    Jet2Tau(Taus[j-1], myTau, myEvent);
    myEvent.rec()->jets().erase((std::vector<RecJetFormat>::iterator) Taus[j-1]);
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

  Int_t charge = 0;

  for (unsigned int i=0;i<myJet->Constituents_.size();i++)
  {
    charge += PDG->GetCharge(myEvent.mc()->particles()[myJet->Constituents_[i]].pdgid());
  }

  if (charge == 3) myTau->charge_ = true;
  else if (charge == -3) myTau->charge_ = false;
}


void TauTagger::SetParameter(const std::string& key, 
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
    if (misid_ljet_==0.0) doMisefficiency_=false; else doMisefficiency_=true;
  }

  // Other
  else TaggerBase::SetParameter(key,value,header);
}


std::string TauTagger::GetParameters()
{
  std::stringstream str;
  str << "dR=" << DeltaRmax_ << " ; ";
  if (Exclusive_) str << "Exclusive ; "; else str << "Inclusive ; ";
  str << "IDeff=" << Efficiency_;
  str << " ; MisID(q)=" << misid_ljet_;
  return str.str();
}
