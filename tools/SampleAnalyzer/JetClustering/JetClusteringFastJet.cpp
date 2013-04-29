////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <http://madanalysis.irmp.ucl.ac.be>
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
  myEvent.rec()->Reset();

  // Preparing inputs
  std::vector<fastjet::PseudoJet> inputs;
  for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
  {
    // Selecting input for jet clustering
    if (myEvent.mc()->particles()[i].statuscode()!=1)       continue;
    if (PHYSICS->IsInvisible(myEvent.mc()->particles()[i])) continue;
    if (fabs(myEvent.mc()->particles()[i].pdgid())==13)     continue;

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
    MET->momentum().SetPx(MET->momentum().Px() - jets[i].px());
    MET->momentum().SetPy(MET->momentum().Py() - jets[i].py());
  }
  (*MHT)=(*MET);

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
      //      if (fabs(myEvent.mc()->particles()[constituents[j].user_index()].pdgid())==11) continue;
      if (PDG->IsCharged(myEvent.mc()->particles()[constituents[j].user_index()].pdgid())) tracks++;
    }
    jet->ntracks_ = tracks;
  }

  // Filling the dataformat with electron/muon
  for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
  {
    if (myEvent.mc()->particles()[i].statuscode()!=1) continue;
    if (fabs(myEvent.mc()->particles()[i].pdgid())==13)
    {
      RecLeptonFormat * muon = myEvent.rec()->GetNewMuon();
      muon->setMomentum(myEvent.mc()->particles()[i].momentum());
      if (myEvent.mc()->particles()[i].pdgid()==13) muon->SetCharge(-1);
      else muon->SetCharge(+1);
    }
    if (fabs(myEvent.mc()->particles()[i].pdgid())==11)
    {
      RecLeptonFormat * elec = myEvent.rec()->GetNewElectron();
      elec->setMomentum(myEvent.mc()->particles()[i].momentum());
      if (myEvent.mc()->particles()[i].pdgid()==11) elec->SetCharge(-1);
      else elec->SetCharge(+1);
    }
  }


  for (unsigned int i=0;i<myEvent.rec()->muons().size();i++)
  {
    (*MET) -= myEvent.rec()->muons()[i].momentum();
  }

  MET->momentum().SetPz(0.);
  MET->momentum().SetE(MET->momentum().Pt());
  MHT->momentum().SetPz(0.);
  MHT->momentum().SetE(MHT->momentum().Pt());

  myBtagger_->Execute(mySample,myEvent);
  myCtagger_->Execute(mySample,myEvent);
  myTAUtagger_->Execute(mySample,myEvent);

  return true;
}

#endif
