////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2020 Jack Araz, Eric Conte & Benjamin Fuks
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
#include "SampleAnalyzer/Commons/Base/SmearerBase.h"

// FastJet headers
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>


using namespace MA5;

// Create data structure for track isocones
struct TrackIsoCone
{
    // Initialization will cause warnings this is only allowed in c++11
    MAfloat32 sumPT, sumET, deltaR;
    MAuint16 ntracks;

    void Initialize()
    {
        sumPT = 0.;
        sumET = 0.;
        deltaR = 0.;
        ntracks = 0;
    };
};


ClusterAlgoFastJet::ClusterAlgoFastJet(std::string Algo):ClusterAlgoBase(Algo)
{ JetAlgorithm_=Algo; JetDefinition_=0; }

ClusterAlgoFastJet::~ClusterAlgoFastJet() 
{ if (JetDefinition_!=0) delete JetDefinition_; }

MAbool ClusterAlgoFastJet::Execute(SampleFormat& mySample, EventFormat& myEvent, MAbool ExclusiveId,   
                                 const std::vector<MAbool>& vetos,
                                 const std::set<const MCParticleFormat*> vetos2,
                                 SmearerBase* smearer)
{
  // Creating a container for inputs
  std::vector<fastjet::PseudoJet> inputs;

  // Track info will get tracker iteration number and info
  std::map<MAfloat64, std::vector<TrackIsoCone> > TrackInfo;
  for (MAuint32 iR=0; iR<isocone_radius_.size(); iR++)
  {
    TrackIsoCone current_cone;
    current_cone.Initialize();
    current_cone.deltaR = isocone_radius_[iR];
    std::vector<TrackIsoCone> tmp_info;
    for (MAuint32 itrk=0; itrk<myEvent.rec()->tracks().size(); itrk++)
        tmp_info.push_back(current_cone);
    TrackInfo[isocone_radius_[iR]] = tmp_info;
  }

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
    for (MAuint32 iR=0; iR<isocone_radius_.size(); iR++)
    {
      for (MAuint32 itrk=0; itrk<myEvent.rec()->tracks().size(); itrk++)
      {
        if (myEvent.rec()->tracks()[itrk].dr(smeared.momentum()) < isocone_radius_[iR])
        {
            TrackInfo[isocone_radius_[iR]][itrk].sumPT += smeared.pt();
            TrackInfo[isocone_radius_[iR]][itrk].sumET += smeared.et();
            if (PDG->IsCharged(myEvent.mc()->particles()[i].pdgid()))
                TrackInfo[isocone_radius_[iR]][itrk].ntracks += 1;
        }
      }
    }

  }

  if (isocone_radius_.size() > 0)
  {
      for (MAuint32 itrk=0; itrk<myEvent.rec()->tracks().size(); itrk++)
      {
        for (MAuint32 iR=0; iR<isocone_radius_.size(); iR++)
        {
            IsolationConeType* current_isocone = myEvent.rec()->tracks()[itrk].GetNewIsolCone();
            current_isocone->setDeltaR(isocone_radius_[iR]);
            current_isocone->setsumPT(TrackInfo[isocone_radius_[iR]][itrk].sumPT);
            current_isocone->setSumET(TrackInfo[isocone_radius_[iR]][itrk].sumET);
            current_isocone->setNtracks(TrackInfo[isocone_radius_[iR]][itrk].ntracks);
        }
      }
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


