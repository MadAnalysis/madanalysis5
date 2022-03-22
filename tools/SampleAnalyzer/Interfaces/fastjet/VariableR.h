////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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

#ifndef MADANALYSIS5_VARIABLER_H
#define MADANALYSIS5_VARIABLER_H

// STL headers
#include <vector>
#include <algorithm>

// FastJet headers
#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/contrib/VariableRPlugin.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/Service/PDGService.h"

using namespace std;

namespace MA5 {
    namespace Substructure {

        enum ClusterType {CALIKE, KTLIKE, AKTLIKE};

        enum Strategy {
            Best,      ///< currently N2Tiled or N2Plain for FJ>3.2.0, Native for FastJet<3.2.0
            N2Tiled,   ///< the default (faster in most cases) [requires FastJet>=3.2.0]
            N2Plain,   ///< [requires FastJet>=3.2.0]
            NNH,       ///< slower but already available for FastJet<3.2.0
            Native     ///< original local implemtation of the clustering [the default for FastJet<3.2.0]
        };

        class VariableR {

            //---------------------------------------------------------------------------------
            //                                 data members
            //---------------------------------------------------------------------------------
            protected:
                MAfloat32 rho_;   // mass scale for effective radius (i.e. R ~ rho/pT)
                MAfloat32 minR_; //minimum jet radius
                MAfloat32 maxR_; // maximum jet radius
                MAfloat32 ptmin_; // Minimum pT
                fastjet::contrib::VariableRPlugin::ClusterType clusterType_; // whether to use CA-like, kT-like, or anti-kT-like distance
                                                            // measure (this value is the same as the p exponent in
                                                            // generalized-kt, with anti-kt = -1.0, CA = 0.0, and
                                                            // kT = 1.0)
                MAbool precluster_; // whether to use optional kT subjets (of size min_r) for preclustering
                                    // (true is much faster, default=false). At the moment, the only option
                                    // for preclustering is kT (use fastjet::NestedDefsPjugin otherwise)
                fastjet::contrib::VariableRPlugin::Strategy strategy_; // decodes which algorithm to apply for the clustering

                // Shared Cluster sequence
                std::shared_ptr<fastjet::ClusterSequence> clust_seq;

                MAbool isExclusive_; // if false return a vector of all jets (in the sense of the inclusive algorithm)
                                     // with pt >= ptmin. Time taken should be of the order of the number of jets
                                     // returned. if True return a vector of all jets (in the sense of the exclusive
                                     // algorithm) that would be obtained when running the algorithm with the given ptmin.

                /// Note that pre-clustering is deprecated and will likely be
                /// removed in a future releasse of this contrib.

            public:

                /// Constructor without argument
                VariableR() {}

                /// Destructor
                virtual ~VariableR() {}

                // Constructor with arguments
                VariableR(
                    MAfloat32 rho,
                    MAfloat32 minR,
                    MAfloat32 maxR,
                    Substructure::ClusterType clusterType,
                    Substructure::Strategy strategy = Substructure::Best,
                    MAfloat32 ptmin = 0.,
                    MAbool isExclusive = false
                )
                { Initialize(rho, minR, maxR, clusterType, strategy, ptmin, isExclusive); }

                // Initialization method
                void Initialize(
                    MAfloat32 rho,
                    MAfloat32 minR,
                    MAfloat32 maxR,
                    Substructure::ClusterType clusterType,
                    Substructure::Strategy strategy = Substructure::Best,
                    MAfloat32 ptmin = 0.,
                    MAbool isExclusive = false
                )
                {
                    if (clusterType == Substructure::CALIKE)
                        clusterType_ = fastjet::contrib::VariableRPlugin::CALIKE;
                    else if (clusterType == Substructure::KTLIKE)
                        clusterType_ = fastjet::contrib::VariableRPlugin::KTLIKE;
                    else if (clusterType == Substructure::AKTLIKE)
                        clusterType_ = fastjet::contrib::VariableRPlugin::AKTLIKE;

                    if (strategy == Substructure::Best)
                        strategy_ = fastjet::contrib::VariableRPlugin::Best;
                    else if (strategy == Substructure::N2Tiled)
                        strategy_ = fastjet::contrib::VariableRPlugin::N2Tiled;
                    else if (strategy == Substructure::N2Plain)
                        strategy_ = fastjet::contrib::VariableRPlugin::N2Plain;
                    else if (strategy == Substructure::NNH)
                        strategy_ = fastjet::contrib::VariableRPlugin::NNH;
                    else if (strategy == Substructure::Native)
                        strategy_ = fastjet::contrib::VariableRPlugin::Native;

                    rho_ = rho; minR_ = minR; maxR_ = maxR; ptmin_ = ptmin; isExclusive_ = isExclusive;
                    precluster_ = false;
                }

                // Cluster given particles
                std::vector<fastjet::PseudoJet> __cluster(std::vector<fastjet::PseudoJet> particles)
                {
                    fastjet::contrib::VariableRPlugin variableR(
                        rho_, minR_, maxR_, clusterType_, precluster_, strategy_
                    );
                    fastjet::JetDefinition jetDefinition(&variableR);
                    clust_seq.reset(new fastjet::ClusterSequence(particles, jetDefinition));

                    std::vector<fastjet::PseudoJet> variableR_jets;
                    if (isExclusive_) variableR_jets = clust_seq->exclusive_jets(ptmin_);
                    else variableR_jets = clust_seq->inclusive_jets(ptmin_);

                    return fastjet::sorted_by_pt(variableR_jets);
                }

                // Execute with the Reconstructed event
                void Execute(EventFormat& myEvent, std::string JetID)
                {
                    std::vector<fastjet::PseudoJet> variableR_jets = __cluster(myEvent.rec()->cluster_inputs());

                    for (auto &VR_jet: variableR_jets)
                    {
                        RecJetFormat * jet = myEvent.rec()->GetNewJet(JetID);
                        MALorentzVector q(VR_jet.px(),VR_jet.py(),VR_jet.pz(),VR_jet.e());
                        jet->setMomentum(q);
                        jet->setPseudoJet(VR_jet);
                        std::vector<fastjet::PseudoJet> constituents = clust_seq->constituents(VR_jet);
                        MAuint32 tracks = 0;
                        for (MAuint32 j=0;j<constituents.size();j++)
                        {
                            jet->AddConstituent(constituents[j].user_index());
                            if (PDG->IsCharged(myEvent.mc()->particles()[constituents[j].user_index()].pdgid()))
                                tracks++;
                        }
                        jet->ntracks_ = tracks;
                    }
                    if (variableR_jets.size() == 0) myEvent.rec()->CreateEmptyJetAccesor(JetID);
                }

                // Wrapper for event based execution
                void Execute(const EventFormat& event, std::string JetID)
                {
                    Execute(const_cast<EventFormat&>(event), JetID);
                }

                // Execute with a list of jets
                std::vector<const RecJetFormat *> Execute(std::vector<const RecJetFormat *> &jets)
                {
                    std::vector<const RecJetFormat *> output_jets;

                    std::vector<fastjet::PseudoJet> constituents;
                    for (auto &jet: jets)
                    {
                        std::vector<fastjet::PseudoJet> current_constituents = jet->pseudojet().constituents();
                        constituents.reserve(constituents.size() + current_constituents.size());
                        constituents.insert(
                                constituents.end(), current_constituents.begin(), current_constituents.end()
                        );
                    }

                    std::vector<fastjet::PseudoJet> variableR_jets = __cluster(constituents);

                    for (auto &jet: variableR_jets)
                    {
                        RecJetFormat *NewJet = new RecJetFormat();
                        NewJet->Reset();
                        MALorentzVector q(jet.px(), jet.py(), jet.pz(), jet.e());
                        NewJet->setMomentum(q);
                        NewJet->setPseudoJet(jet);
                        output_jets.push_back(NewJet);
                    }

                    return output_jets;
                }

                // Execute with a single jet
                std::vector<const RecJetFormat *> Execute(const RecJetFormat *jet)
                {
                    std::vector<const RecJetFormat *> output_jets;

                    std::vector<fastjet::PseudoJet> variableR_jets = __cluster(jet->pseudojet().constituents());

                    for (auto &jet: variableR_jets)
                    {
                        RecJetFormat *NewJet = new RecJetFormat();
                        NewJet->Reset();
                        MALorentzVector q(jet.px(), jet.py(), jet.pz(), jet.e());
                        NewJet->setMomentum(q);
                        NewJet->setPseudoJet(jet);
                        output_jets.push_back(NewJet);
                    }

                    return output_jets;
                }
        };
    }
}


#endif //MADANALYSIS5_VARIABLER_H
