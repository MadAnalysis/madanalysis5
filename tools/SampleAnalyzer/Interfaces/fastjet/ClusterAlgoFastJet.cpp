////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://github.com/MadAnalysis/madanalysis5>
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
#include "SampleAnalyzer/Commons/Base/SmearerBase.h"

// FastJet headers
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>


using namespace MA5;

ClusterAlgoFastJet::ClusterAlgoFastJet(std::string Algo):ClusterAlgoBase(Algo)
{ JetAlgorithm_=Algo; JetDefinition_=0; }

ClusterAlgoFastJet::~ClusterAlgoFastJet() 
{ if (JetDefinition_!=0) delete JetDefinition_; }

template<class Type>
void SetConeRadius(std::vector<MAfloat64> cone_radius, std::vector<Type>& objects,
                   MCParticleFormat part, MAbool ischarged=false, MAbool addself=false)
{
  for (MAuint32 iR=0; iR<cone_radius.size(); iR++)
  {
    for (MAuint32 i=0; i<objects.size(); i++)
    {
      IsolationConeType* current_isocone = objects[i].GetIsolCone(cone_radius[iR]);
      if (objects[i].dr(part.momentum()) < cone_radius[iR])
      {
          current_isocone->addsumPT(part.pt());
          current_isocone->addSumET(part.et());
          if (addself)
          {
            current_isocone->setSelfPT(objects[i].pt());
            current_isocone->setSelfET(objects[i].et());
          }
          // if (ischarged) current_isocone->addNtracks(1);
          // ntracks depends on track matching
      }
    }
  }
}


MAbool ClusterAlgoFastJet::Execute(SampleFormat& mySample, EventFormat& myEvent, MAbool ExclusiveId,   
                                 const std::vector<MAbool>& vetos,
                                 const std::set<const MCParticleFormat*> vetos2,
                                 SmearerBase* smearer)
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


    // Smearer module returns a smeared MCParticleFormat object
    // Default: NullSmearer, that does nothing
    // Reminder: 0 is reserved for the jet constituents
    MCParticleFormat smeared = smearer->Execute(&part, 0);
    if (smeared.pt() <= 1e-10) continue;

    // Filling good particle for clustering
    inputs.push_back(fastjet::PseudoJet(smeared.px(),
                                        smeared.py(),
                                        smeared.pz(),
                                        smeared.e() ));
    inputs.back().set_user_index(i);

    // Set track isolation
    // Isolation cone is applied to each particle that deposits energy in HCAL;
    // all hadronic activity assumed to reach to HCAL
    MAbool isCharged = PDG->IsCharged(myEvent.mc()->particles()[i].pdgid());
    SetConeRadius(isocone_track_radius_,    myEvent.rec()->tracks(),    smeared, isCharged, false);
    // Set Electron isolation
    SetConeRadius(isocone_electron_radius_, myEvent.rec()->electrons(), smeared, isCharged, !ExclusiveId);
    // Set Muon isolation
    SetConeRadius(isocone_muon_radius_,     myEvent.rec()->muons(),     smeared, isCharged, false);
    // Set Photon isolation
    SetConeRadius(isocone_photon_radius_,   myEvent.rec()->photons(),   smeared, isCharged, !ExclusiveId);

  }

  // Clustering
  fastjet::ClusterSequence clust_seq(inputs, *JetDefinition_);

  // Getting jets with PTmin = 0
  std::vector<fastjet::PseudoJet> jets; 
  if (Exclusive_) jets = clust_seq.exclusive_jets(0.);
  else jets = clust_seq.inclusive_jets(0.);

  // Smearing if necessary
  if (smearer->isJetSmearerOn())
  {
    for (MAuint32 i=0;i<jets.size();i++)
    {
        // Smearer module returns a smeared MCParticleFormat object
        // Default: NullSmearer, that does nothing
        // Reminder: 21 is reserved for the reco-jets
        MCParticleFormat current_jet;
        current_jet.momentum().SetPxPyPzE(jets[i].px(),jets[i].py(),jets[i].pz(),jets[i].e());
        MCParticleFormat smeared = smearer->Execute(dynamic_cast<const MCParticleFormat*>(&current_jet), 21);
        jets[i].reset_momentum(smeared.px(),smeared.py(),smeared.pz(),smeared.e());
    }
    // Sort pseudojets
    jets = fastjet::sorted_by_pt(jets);
  }

  // Calculating the MET
  ParticleBaseFormat* MET = myEvent.rec()->GetNewMet();
  ParticleBaseFormat* MHT = myEvent.rec()->GetNewMht();

  // shortcut for Meff, TET & THT
  MAfloat64 & TET = myEvent.rec()->TET();
  MAfloat64 & THT = myEvent.rec()->THT();
  MAfloat64 & Meff= myEvent.rec()->Meff();

  // Storing
  for (MAuint32 i=0;i<jets.size();i++)
  {
    if (jets[i].pt() <= 1e-10) continue;
    MALorentzVector q(jets[i].px(),jets[i].py(),jets[i].pz(),jets[i].e());
    (*MET) -= q;
    (*MHT) -= q;
    THT += jets[i].pt();
    TET += jets[i].pt();
    Meff += jets[i].pt();

    if(jets[i].pt() < Ptmin_) continue;

    // Saving jet information
    RecJetFormat * jet = myEvent.rec()->GetNewJet();
    jet->setMomentum(MALorentzVector(jets[i].px(),jets[i].py(),jets[i].pz(),jets[i].e()));
    std::vector<fastjet::PseudoJet> constituents = clust_seq.constituents(jets[i]);
    MAuint32 tracks = 0;
    for (MAuint32 j=0;j<constituents.size();j++)
    {
      jet->AddConstituent(constituents[j].user_index());
      if (PDG->IsCharged(myEvent.mc()->particles()[constituents[j].user_index()].pdgid())) tracks++;
    }
    jet->ntracks_ = tracks;


  }
  Meff += MET->pt();

  // Filling the dataformat with jets
  return true;
}


