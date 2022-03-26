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
#include <fastjet/PseudoJet.hh>


using namespace MA5;

ClusterAlgoFastJet::ClusterAlgoFastJet(std::string Algo):ClusterAlgoBase(Algo)
{ JetAlgorithm_=Algo; JetDefinition_=0; }

ClusterAlgoFastJet::~ClusterAlgoFastJet()
{ if (JetDefinition_!=0) delete JetDefinition_; }


MAbool ClusterAlgoFastJet::Execute(SampleFormat& mySample, EventFormat& myEvent, SmearerBase* smearer)
{
    // Clustering
    clust_seq.reset(new fastjet::ClusterSequence(myEvent.rec()->cluster_inputs(), *JetDefinition_));

    // Getting jets with PTmin = 0
    std::vector<fastjet::PseudoJet> jets;
    if (Exclusive_) jets = clust_seq->exclusive_jets(0.);
    else jets = clust_seq->inclusive_jets(0.);

    // Smearing if necessary
    MAint32 jet_counter = 0;
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
            if(jets[i].pt() >= Ptmin_) jet_counter++;
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

    std::vector<RecJetFormat> output_jets;
    if (smearer->isJetSmearerOn()) output_jets.reserve(jet_counter);

    // Storing
    for (auto &jet: jets)
    {
        if (jet.pt() <= 1e-10) continue;
        MALorentzVector q(jet.px(),jet.py(),jet.pz(),jet.e());
        (*MET) -= q;
        (*MHT) -= q;
        THT += jet.pt();
        TET += jet.pt();
        Meff += jet.pt();

        if(jet.pt() < Ptmin_) continue;

        // Saving jet information
        RecJetFormat current_jet(q);
        current_jet.setPseudoJet(jet);
        std::vector<fastjet::PseudoJet> constituents = clust_seq->constituents(jet);
        current_jet.ntracks_ = 0;
        for (MAuint32 j=0;j<constituents.size();j++)
        {
            current_jet.AddConstituent(constituents[j].user_index());
            if (PDG->IsCharged(myEvent.mc()->particles()[constituents[j].user_index()].pdgid())) current_jet.ntracks_++;
        }
        output_jets.push_back(current_jet);
    }
    myEvent.rec()->jetcollection_.insert(std::make_pair(myEvent.rec()->PrimaryJetID_, output_jets));
    Meff += MET->pt();
    return true;
}

// Additional jet clustering. needs execute to run before!!
MAbool ClusterAlgoFastJet::Cluster(EventFormat& myEvent, std::string JetID)
{
    // Clustering
    clust_seq.reset(new fastjet::ClusterSequence(myEvent.rec()->cluster_inputs(), *JetDefinition_));

    std::vector<fastjet::PseudoJet> jets;
    if (Exclusive_) jets = clust_seq->exclusive_jets(Ptmin_);
    else jets = clust_seq->inclusive_jets(Ptmin_);
    jets = fastjet::sorted_by_pt(jets);

    std::vector<RecJetFormat> output_jets;
    output_jets.reserve(jets.size());

    // Storing
    for (auto &jet: jets)
    {
        // Saving jet information
        RecJetFormat current_jet(jet.px(),jet.py(),jet.pz(),jet.e());
        current_jet.setPseudoJet(jet);
        std::vector<fastjet::PseudoJet> constituents = clust_seq->constituents(jet);
        current_jet.ntracks_ = 0;
        for (auto &constit: constituents)
        {
            current_jet.AddConstituent(constit.user_index());
            if (PDG->IsCharged(myEvent.mc()->particles()[constit.user_index()].pdgid()))
                current_jet.ntracks_++;
        }
        output_jets.push_back(current_jet);
    }

    myEvent.rec()->jetcollection_.insert(std::make_pair(JetID, output_jets));

    // Filling the dataformat with jets
    return true;
}
