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

#ifndef MADANALYSIS5_CLUSTERBASE_H
#define MADANALYSIS5_CLUSTERBASE_H

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

namespace MA5{
    namespace Substructure {

        // Accessor for jet clustering algorithms
        enum Algorithm {antikt, cambridge, kt};

        class Recluster;
        class SoftDrop;
        class Pruner;
        class Filter;

        class ClusterBase {

            friend class Recluster;
            friend class SoftDrop;
            friend class Pruner;
            friend class Filter;

        //---------------------------------------------------------------------------------
        //                                 data members
        //---------------------------------------------------------------------------------
        protected:

            // External parameters
            MAfloat32 ptmin_; // minimum transverse momentum
            MAbool isExclusive_; // if false return a vector of all jets (in the sense of the inclusive algorithm)
                                // with pt >= ptmin. Time taken should be of the order of the number of jets
                                // returned. if True return a vector of all jets (in the sense of the exclusive
                                // algorithm) that would be obtained when running the algorithm with the given ptmin.

            /// Jet definition
            fastjet::JetDefinition* JetDefinition_;
            fastjet::JetDefinition::Plugin* JetDefPlugin_;
            MAbool isPlugin_;
            MAbool isClustered_;

            // Shared Cluster sequence
            std::shared_ptr<fastjet::ClusterSequence> clust_seq;

        public:

            /// Constructor without argument
            ClusterBase() {}

            /// Destructor
            virtual ~ClusterBase() {}

            // Set the Jet definition using algorithm and radius input
            void SetJetDef(Algorithm algorithm, MAfloat32 radius)
            {
                isPlugin_ = false;
                JetDefinition_ = new fastjet::JetDefinition(__get_clustering_algorithm(algorithm), radius);
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
                std::vector<fastjet::PseudoJet> reclustered_jets = __cluster(jet->pseudojet().constituents());
                return __transform_jets(reclustered_jets);
            }

            // Execute with a single jet. This method reclusters the given jet using its constituents by filtering
            // reclustered events with respect to the initial jet
            template<typename Func>
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

                return __transform_jets(reclustered_jets);
            }

            // Handler for clustering step
            void cluster(const RecJetFormat *jet)
            {
                if (isPlugin_)
                {
                    fastjet::JetDefinition jetDefinition(JetDefPlugin_);
                    clust_seq.reset(new fastjet::ClusterSequence(jet->pseudojet().constituents(), jetDefinition));
                } else {
                    clust_seq.reset(new fastjet::ClusterSequence(
                            jet->pseudojet().constituents(),
                            const_cast<const fastjet::JetDefinition &>(*JetDefinition_)
                    ));
                }
                isClustered_ = true;
            }

            // return a vector of all jets when the event is clustered (in the exclusive sense) to exactly njets.
            // If there are fewer than njets particles in the ClusterSequence the function just returns however many
            // particles there were.
            std::vector<const RecJetFormat *> exclusive_jets_up_to(MAint32 njets)
            {
                if (!isClustered_) throw EXCEPTION_ERROR("No clustered jet available", "", 1);
                std::vector<fastjet::PseudoJet> clustered_jets = fastjet::sorted_by_pt(
                        clust_seq->exclusive_jets_up_to(njets)
                );
                return __transform_jets(clustered_jets);
            }

        private:

            // Generic clustering method
            std::vector<fastjet::PseudoJet> __cluster(std::vector<fastjet::PseudoJet> particles)
            {
                if (isPlugin_)
                {
                    fastjet::JetDefinition jetDefinition(JetDefPlugin_);
                    clust_seq.reset(new fastjet::ClusterSequence(particles, jetDefinition));
                } else {
                    clust_seq.reset(new fastjet::ClusterSequence(
                            particles, const_cast<const fastjet::JetDefinition &>(*JetDefinition_)
                    ));
                }
                isClustered_ = true;
                std::vector<fastjet::PseudoJet> jets;
                if (isExclusive_) jets = clust_seq->exclusive_jets(ptmin_);
                else jets = clust_seq->inclusive_jets(ptmin_);

                return fastjet::sorted_by_pt(jets);
            }

            // Method to transform pseudojet into recjetformat
            static RecJetFormat * __transform_jet(fastjet::PseudoJet jet)
            {
                RecJetFormat * NewJet = new RecJetFormat(jet.px(), jet.py(), jet.pz(), jet.e());
                NewJet->setPseudoJet(jet);
                return NewJet;
            }

            static std::vector<const RecJetFormat *> __transform_jets(std::vector<fastjet::PseudoJet> jets)
            {
                std::vector<const RecJetFormat *> output_jets;
                output_jets.reserve(jets.size());
                for (auto &jet: jets)
                    output_jets.push_back(__transform_jet(jet));
                return output_jets;
            }

            // Method to get jet algorithm
            static fastjet::JetAlgorithm __get_clustering_algorithm(Substructure::Algorithm algorithm)
            {
                fastjet::JetAlgorithm algo_;
                if (algorithm == Substructure::antikt)         algo_ = fastjet::antikt_algorithm;
                else if (algorithm == Substructure::cambridge) algo_ = fastjet::cambridge_algorithm;
                else if (algorithm == Substructure::kt)        algo_ = fastjet::kt_algorithm;
                else throw EXCEPTION_ERROR("Unknown algorithm","",1);
                return algo_;
            }

            // Execute with the Reconstructed event. This method creates a new Jet in RecEventFormat which
            // can be accessed via JetID. The algorithm will only be executed if a unique JetID is given
            MAbool __execute(EventFormat& myEvent, std::string JetID)
            {
                try {
                    if (myEvent.rec()->hasJetID(JetID))
                        throw EXCEPTION_ERROR("Substructure::ClusterBase - Jet ID `" + JetID + \
                                                  "` already exits. Skipping execution.","",1);
                } catch (const std::exception& err) {
                    MANAGE_EXCEPTION(err);
                    return false;
                }

                std::vector <fastjet::PseudoJet> jets = __cluster(myEvent.rec()->cluster_inputs());
                std::vector<RecJetFormat> output_jets;
                output_jets.reserve(jets.size());
                for (auto &jet: jets) {
                    RecJetFormat current_jet = *__transform_jet(jet);
                    std::vector <fastjet::PseudoJet> constituents = clust_seq->constituents(jet);
                    current_jet.ntracks_ = 0;
                    for (auto &constit: constituents) {
                        current_jet.AddConstituent(constit.user_index());
                        if (PDG->IsCharged(myEvent.mc()->particles()[constit.user_index()].pdgid()))
                            current_jet.ntracks_++;
                    }
                    output_jets.push_back(current_jet);
                }
                myEvent.rec()->jetcollection_.insert(std::make_pair(JetID, output_jets));
                isClustered_ = false;
                return true;
            }
        };
    }
}

#endif //MADANALYSIS5_CLUSTERBASE_H
