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


#include "SampleAnalyzer/JetClustering/JetClusteringFastJet.h"
#ifdef FASTJET_USE

using namespace MA5;

bool JetClusteringFastJet::Execute(SampleFormat& mySample, EventFormat& myEvent)
{
  if (mySample.mc()==0 ||  myEvent.mc()==0) return false;
  if (mySample.rec()==0) mySample.InitializeRec();
  if (myEvent.rec() ==0) myEvent.InitializeRec();

  // Reseting the reconstructed event
  myEvent.rec()->Reset();

  // Veto
  std::vector<bool> vetos(myEvent.mc()->particles().size(),false);

  // Filling the dataformat with electron/muon
  for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
  {
    // Keeping only final states
    if (myEvent.mc()->particles()[i].statuscode()!=1) continue;

    if (std::abs(myEvent.mc()->particles()[i].pdgid())==11 && myEvent.mc()->particles()[i].mother1()!=0 && 15==std::abs(myEvent.mc()->particles()[i].mother1()->pdgid())) std::cout << "mumu from tautau" << std::endl;


    // Be more constraining in ExclusiveId
    if (ExclusiveId_) 
    {
      // Need a mother particle
      if (myEvent.mc()->particles()[i].mother1()==0) continue;

      // HEP2LHE trick
      if (myEvent.mc()->particles()[i].pdgid() != 
          myEvent.mc()->particles()[i].mother1()->pdgid()) continue;
    }

    // Muons
    if (std::abs(myEvent.mc()->particles()[i].pdgid())==13)
    {
      vetos[i]=true;
      RecLeptonFormat * muon = myEvent.rec()->GetNewMuon();
      muon->setMomentum(myEvent.mc()->particles()[i].momentum());
      muon->setMc(&(myEvent.mc()->particles()[i]));
      if (myEvent.mc()->particles()[i].pdgid()==13) muon->SetCharge(-1);
      else muon->SetCharge(+1);
    }

    // Electrons
    else if (std::abs(myEvent.mc()->particles()[i].pdgid())==11)
    {
      vetos[i]=true;
      RecLeptonFormat * elec = myEvent.rec()->GetNewElectron();
      elec->setMomentum(myEvent.mc()->particles()[i].momentum());
      elec->setMc(&(myEvent.mc()->particles()[i]));
      if (myEvent.mc()->particles()[i].pdgid()==11) elec->SetCharge(-1);
      else elec->SetCharge(+1);
    }

    // Photons
    else if (std::abs(myEvent.mc()->particles()[i].pdgid())==22)
    {
      vetos[i]=true;
      RecPhotonFormat * photon = myEvent.rec()->GetNewPhoton();
      photon->setMomentum(myEvent.mc()->particles()[i].momentum());
      photon->setMc(&(myEvent.mc()->particles()[i]));
    }
  }

  double & TET = myEvent.rec()->TET();
  double & THT = myEvent.rec()->THT();

  // Preparing inputs
  std::vector<fastjet::PseudoJet> inputs;
  for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
  {
    // Selecting input for jet clustering
    if (myEvent.mc()->particles()[i].statuscode()!=1)       continue;
    if (PHYSICS->IsInvisible(myEvent.mc()->particles()[i])) continue;

    // ExclusiveId mode
    if (ExclusiveId_ && vetos[i]) continue;

    // NonExclusive Id mode
    else if (std::abs(myEvent.mc()->particles()[i].pdgid())==13) continue;

    // Filling good particle for clustering
    inputs.push_back(
          fastjet::PseudoJet ( myEvent.mc()->particles()[i].px(), 
                               myEvent.mc()->particles()[i].py(),
                               myEvent.mc()->particles()[i].pz(),
                               myEvent.mc()->particles()[i].e()   ));
    inputs.back().set_user_index(i);
  }

  // Clustering
  fastjet::ClusterSequence clust_seq(inputs, JetDefinition_);

  // Getting jets with PTmin = 0
  std::vector<fastjet::PseudoJet> jets; 
  if (Exclusive_) jets = clust_seq.exclusive_jets(0.);
  else jets = clust_seq.inclusive_jets(0.);

  // Calculating the MET  
  ParticleBaseFormat* MET = myEvent.rec()->GetNewMet();
  ParticleBaseFormat* MHT = myEvent.rec()->GetNewMht();

  for (unsigned int i=0;i<jets.size();i++)
  {
    TLorentzVector q(jets[i].px(),jets[i].py(),jets[i].pz(),jets[i].e());
    (*MET) -= q;
    (*MHT) -= q;
    THT += jets[i].pt();
    TET += jets[i].pt();
  }

  // Getting jets with PTmin
  if (Exclusive_) jets = clust_seq.exclusive_jets(Ptmin_);
  else jets = clust_seq.inclusive_jets(Ptmin_);

  // Filling the dataformat with jets
  for (unsigned int i=0;i<jets.size();i++)
  {
    RecJetFormat * jet = myEvent.rec()->GetNewJet();
    jet->setMomentum(TLorentzVector(jets[i].px(),jets[i].py(),jets[i].pz(),jets[i].e()));
    std::vector<fastjet::PseudoJet> constituents = clust_seq.constituents(jets[i]);
    UInt_t tracks = 0;
    for (unsigned int j=0;j<constituents.size();j++)
    {
      jet->AddConstituent(constituents[j].user_index());
      //      if (std::abs(myEvent.mc()->particles()[constituents[j].user_index()].pdgid())==11) continue;
      if (PDG->IsCharged(myEvent.mc()->particles()[constituents[j].user_index()].pdgid())) tracks++;
    }
    jet->ntracks_ = tracks;
  }


  if (ExclusiveId_)
  {
    for (unsigned int i=0;i<myEvent.rec()->electrons().size();i++)
    {
      (*MET) -= myEvent.rec()->electrons()[i].momentum();
      TET += myEvent.rec()->electrons()[i].pt();
    }
    for (unsigned int i=0;i<myEvent.rec()->photons().size();i++)
    {
      (*MET) -= myEvent.rec()->photons()[i].momentum();
      TET += myEvent.rec()->photons()[i].pt();
    }
  }

  for (unsigned int i=0;i<myEvent.rec()->muons().size();i++)
  {
    (*MET) -= myEvent.rec()->muons()[i].momentum();
    TET += myEvent.rec()->muons()[i].pt();
  }

  MET->momentum().SetPz(0.);
  MET->momentum().SetE(MET->momentum().Pt());
  MHT->momentum().SetPz(0.);
  MHT->momentum().SetE(MHT->momentum().Pt());

  myBtagger_->Execute(mySample,myEvent);
  myTAUtagger_->Execute(mySample,myEvent);

  return true;
}

#endif
