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

#ifndef MADANALYSIS5_CLUSTER_H
#define MADANALYSIS5_CLUSTER_H

// STL headers
#include <vector>
#include <algorithm>

// FastJet headers
#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/Service/PDGService.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

namespace fastjet
{
    class JetDefinition;
}

using namespace std;

namespace MA5 {
    namespace Substructure{

        class Recluster;

        // Accessor for jet clustering algorithms
        enum Algorithm {antikt, cambridge, kt};

        class Cluster {

            friend class Recluster;

            //---------------------------------------------------------------------------------
            //                                 data members
            //---------------------------------------------------------------------------------
            protected:

                // External parameters
                MAfloat32 ptmin_; // minimum transverse momentum
                MAbool isExclusive_; // use exclusive or inclusive jets

                /// Jet definition
                fastjet::JetDefinition* JetDefinition_;

                // Shared Cluster sequence
                std::shared_ptr<fastjet::ClusterSequence> clust_seq;

            // -------------------------------------------------------------
            //                       method members
            // -------------------------------------------------------------
            public:

                /// Constructor without argument
                Cluster() {}

                /// Destructor
                virtual ~Cluster() {}

                //============================//
                //        Initialization      //
                //============================//
                // Initialize the parameters of the algorithm. Initialization includes multiple if conditions
                // Hence it would be optimum execution to initialize the algorithm during the initialisation
                // of the analysis

                // Constructor with arguments
                Cluster(Algorithm algorithm, MAfloat32 radius, MAfloat32 ptmin=0., MAbool isExclusive = false)
                { Initialize(algorithm, radius, ptmin, isExclusive); }

                void Initialize(Algorithm algorithm, MAfloat32 radius, MAfloat32 ptmin=0., MAbool isExclusive = false)
                {
                    SetJetDef(algorithm, radius);
                    ptmin_ = ptmin < 0. ? 0. : ptmin; isExclusive_ = isExclusive;
                }

                //=======================//
                //        Execution      //
                //=======================//

                // Wrapper for event based execution
                void Execute(const EventFormat& event, std::string JetID)
                { __execute(const_cast<EventFormat&>(event), JetID); }

                // Execute with a single jet. This method reclusters the given jet using its constituents
                std::vector<const RecJetFormat *> Execute(const RecJetFormat *jet)
                {
                    std::vector<const RecJetFormat *> output_jets;
                    std::vector<fastjet::PseudoJet> reclustered_jets = __cluster(jet->pseudojet().constituents());

                    for (auto &recjet: reclustered_jets)
                    {
                        RecJetFormat *NewJet = __transform_jet(recjet);
                        output_jets.push_back(NewJet);
                    }

                    return output_jets;
                }

                // Execute with a single jet. This method reclusters the given jet using its constituents by filtering
                // reclustered events with respect to the initial jet
                template<class Func>
                std::vector<const RecJetFormat *> Execute(const RecJetFormat *jet, Func func)
                {
                    std::vector<const RecJetFormat *> output_jets;
                    std::vector<fastjet::PseudoJet> reclustered_jets = __cluster(jet->pseudojet().constituents());

                    for (auto &recjet: reclustered_jets)
                    {
                        RecJetFormat *NewJet = __transform_jet(recjet);
                        if (func(jet, const_cast<const RecJetFormat *>(NewJet))) output_jets.push_back(NewJet);
                    }

                    return output_jets;
                }

                // Execute with a list of jets. This method reclusters the given collection
                // of jets by combining their constituents
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

                    std::vector<fastjet::PseudoJet> reclustered_jets = __cluster(constituents);

                    for (auto &recjet: reclustered_jets)
                    {
                        RecJetFormat *NewJet = __transform_jet(recjet);
                        output_jets.push_back(NewJet);
                    }

                    return output_jets;
                }

            private:

                // Accessor to Jet Definition
                fastjet::JetDefinition* JetDefinition() { return JetDefinition_; }

                // Set the Jet definition using algorithm and radius input
                void SetJetDef(Algorithm algorithm, MAfloat32 radius)
                {
                    if (algorithm == Substructure::antikt)
                    {
                        JetDefinition_ = new fastjet::JetDefinition(fastjet::antikt_algorithm, radius);
                    }
                    else if (algorithm == Substructure::cambridge)
                    {
                        JetDefinition_ = new fastjet::JetDefinition(fastjet::cambridge_algorithm, radius);
                    }
                    else if (algorithm == Substructure::kt)
                    {
                        JetDefinition_ = new fastjet::JetDefinition(fastjet::kt_algorithm, radius);
                    }
                }

                // Generic clustering method
                std::vector<fastjet::PseudoJet> __cluster(std::vector<fastjet::PseudoJet> particles)
                {
                    clust_seq.reset(new fastjet::ClusterSequence(
                            particles, const_cast<const fastjet::JetDefinition &>(*JetDefinition_)
                    ));
                    std::vector<fastjet::PseudoJet> jets;
                    if (isExclusive_) jets = clust_seq->exclusive_jets(ptmin_);
                    else jets = clust_seq->inclusive_jets(ptmin_);

                    return fastjet::sorted_by_pt(jets);
                }

                // Method to transform pseudojet into recjetformat
                RecJetFormat * __transform_jet(fastjet::PseudoJet jet)
                {
                    RecJetFormat * NewJet = new RecJetFormat();
                    NewJet->Reset();
                    MALorentzVector q(jet.px(), jet.py(), jet.pz(), jet.e());
                    NewJet->setMomentum(q);
                    NewJet->setPseudoJet(jet);
                    return NewJet;
                }

                // Execute with the Reconstructed event. This method creates a new Jet in RecEventFormat which
                // can be accessed via JetID. The algorithm will only be executed if a unique JetID is given
                MAbool __execute(EventFormat& myEvent, std::string JetID)
                {
                    try {
                        if (myEvent.rec()->hasJetID(JetID))
                            throw EXCEPTION_ERROR("Substructure::Cluster - Jet ID `" + JetID + \
                                                  "` already exits. Skipping execution.","",1);
                    } catch (const std::exception& err) {
                        MANAGE_EXCEPTION(err);
                        return false;
                    }

                    std::vector <fastjet::PseudoJet> jets = __cluster(myEvent.rec()->cluster_inputs());

                    for (auto &jet: jets) {
                        RecJetFormat *current_jet = myEvent.rec()->GetNewJet(JetID);
                        MALorentzVector q(jet.px(), jet.py(), jet.pz(), jet.e());
                        current_jet->setMomentum(q);
                        current_jet->setPseudoJet(jet);
                        std::vector <fastjet::PseudoJet> constituents = clust_seq->constituents(jet);
                        MAuint32 tracks = 0;
                        for (MAuint32 j = 0; j < constituents.size(); j++) {
                            current_jet->AddConstituent(constituents[j].user_index());
                            if (PDG->IsCharged(myEvent.mc()->particles()[constituents[j].user_index()].pdgid()))
                                tracks++;
                        }
                        current_jet->ntracks_ = tracks;
                    }
                    if (jets.size() == 0) myEvent.rec()->CreateEmptyJetAccesor(JetID);
                    return true;
                }
        };
    }
}

#endif //MADANALYSIS5_CLUSTER_H
