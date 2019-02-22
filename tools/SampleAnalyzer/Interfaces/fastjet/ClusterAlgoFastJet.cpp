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


// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoFastJet.h"
#include "SampleAnalyzer/Commons/Service/LoopService.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"
#include "SampleAnalyzer/Commons/Service/PDGService.h"

// FastJet headers
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>


using namespace MA5;


ClusterAlgoFastJet::ClusterAlgoFastJet(std::string Algo):ClusterAlgoBase(Algo)
{ JetAlgorithm_=Algo; JetDefinition_=0; }

ClusterAlgoFastJet::~ClusterAlgoFastJet() 
{ if (JetDefinition_!=0) delete JetDefinition_; }

MAbool ClusterAlgoFastJet::Execute(SampleFormat& mySample, EventFormat& myEvent, MAbool ExclusiveId,   
                                 const std::vector<MAbool>& vetos,
                                 const std::set<const MCParticleFormat*> vetos2)
{
  // Creating a container for inputs
  std::vector<fastjet::PseudoJet> inputs;

  // Putting the good inputs into the containter
  // Good inputs = - final state
  //               - visible
  //               - if exclusiveID=1: particles not vetoed
  //               - if exclusiveID=0: all particles except muons 
  for (MAuint32 i=0;i<myEvent.mc()->particles().size();i++)
  {
    const MCParticleFormat& part = myEvent.mc()->particles()[i];

    // Selecting input for jet clustering
    // | final state only
    if (part.statuscode()!=1) continue;
    // ! not invisible: reject neutrinos, neutralinos, ...
    if (PHYSICS->Id->IsInvisible(part)) continue;

    // ExclusiveId mode
    if (ExclusiveId)
    {
      if (vetos[i]) continue;
      if (vetos2.find(&part)!=vetos2.end()) continue;
    }

    // NonExclusive Id mode
    else if (std::abs(part.pdgid())==13) continue;

    // Filling good particle for clustering
    inputs.push_back(fastjet::PseudoJet( part.px(), 
                                         part.py(),
                                         part.pz(),
                                         part.e()   ));
    inputs.back().set_user_index(i);
  }

  /*  for (MAuint32 i=0;i<inputs.size();i++)
  {
    std::cout << "px=" << inputs[i].px() << " py=" << inputs[i].py() << " pz=" << inputs[i].pz() << " e=" << inputs[i].e() << std::endl;
    }*/

  // Clustering
  fastjet::ClusterSequence clust_seq(inputs, *JetDefinition_);

  // Getting jets with PTmin = 0
  std::vector<fastjet::PseudoJet> jets; 
  if (Exclusive_) jets = clust_seq.exclusive_jets(0.);
  else jets = clust_seq.inclusive_jets(0.);

  // Calculating the MET  
  ParticleBaseFormat* MET = myEvent.rec()->GetNewMet();
  ParticleBaseFormat* MHT = myEvent.rec()->GetNewMht();

  // shortcut for Meff, TET & THT
  MAfloat64 & TET = myEvent.rec()->TET();
  MAfloat64 & THT = myEvent.rec()->THT();
  MAfloat64 & Meff= myEvent.rec()->Meff();

  for (MAuint32 i=0;i<jets.size();i++)
  {
    MALorentzVector q(jets[i].px(),jets[i].py(),jets[i].pz(),jets[i].e());
    (*MET) -= q;
    (*MHT) -= q;
    THT += jets[i].pt();
    TET += jets[i].pt();
    Meff += jets[i].pt();
  }
  Meff += MET->pt();

  // Getting jets with PTmin
  if (Exclusive_) jets = clust_seq.exclusive_jets(Ptmin_);
  else jets = clust_seq.inclusive_jets(Ptmin_);

  // Filling the dataformat with jets
  for (MAuint32 i=0;i<jets.size();i++)
  {
    RecJetFormat * jet = myEvent.rec()->GetNewJet();
    jet->setMomentum(MALorentzVector(jets[i].px(),jets[i].py(),jets[i].pz(),jets[i].e()));
    std::vector<fastjet::PseudoJet> constituents = clust_seq.constituents(jets[i]);
    MAuint32 tracks = 0;
    for (MAuint32 j=0;j<constituents.size();j++)
    {
      jet->AddConstituent(constituents[j].user_index());
      //      if (std::abs(myEvent.mc()->particles()[constituents[j].user_index()].pdgid())==11) continue;
      if (PDG->IsCharged(myEvent.mc()->particles()[constituents[j].user_index()].pdgid())) tracks++;
    }
    jet->ntracks_ = tracks;
  }

  return true;
}


