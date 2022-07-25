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

// FastJet headers
#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"

#include "SampleAnalyzer/Interfaces/substructure/ClusterBase.h"

namespace MA5 {
    namespace Substructure {

        //=======================//
        //       Initialize      //
        //=======================//

        // Set the Jet definition using algorithm and radius input
        void ClusterBase::SetJetDef(Algorithm algorithm, MAfloat32 radius)
        {
            isPlugin_ = false;
            JetDefinition_ = new fastjet::JetDefinition(__get_clustering_algorithm(algorithm), radius);
        }

        //=======================//
        //        Execution      //
        //=======================//

        // Wrapper for event based execution
        void ClusterBase::Execute(const EventFormat& event, std::string JetID)
        { __execute(const_cast<EventFormat&>(event), JetID); }

        // Execute with a single jet. This method reclusters the given jet using its constituents
        std::vector<const RecJetFormat *> ClusterBase::Execute(const RecJetFormat *jet)
        {
            std::vector<fastjet::PseudoJet> reclustered_jets = __cluster(jet->pseudojet().constituents());
            return __transform_jets(reclustered_jets);
        }

        // Execute with a single jet. This method reclusters the given jet using its constituents by filtering
        // reclustered events with respect to the initial jet
        template<typename Func>
        std::vector<const RecJetFormat *> ClusterBase::Execute(const RecJetFormat *jet, Func func)
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
        std::vector<const RecJetFormat *> ClusterBase::Execute(std::vector<const RecJetFormat *> &jets)
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
        void ClusterBase::cluster(const RecJetFormat *jet)
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
        std::vector<const RecJetFormat *> ClusterBase::exclusive_jets_up_to(MAint32 njets)
        {
            if (!isClustered_) throw EXCEPTION_ERROR("No clustered jet available", "", 1);
            std::vector<fastjet::PseudoJet> clustered_jets = fastjet::sorted_by_pt(
                    clust_seq->exclusive_jets_up_to(njets)
            );
            return __transform_jets(clustered_jets);
        }

        //=======================//
        //   Private Functions   //
        //=======================//

        // Generic clustering method
        std::vector<fastjet::PseudoJet> ClusterBase::__cluster(std::vector<fastjet::PseudoJet> particles)
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
        RecJetFormat * ClusterBase::__transform_jet(fastjet::PseudoJet jet) const
        {
            RecJetFormat * NewJet = new RecJetFormat(jet);
            return NewJet;
        }

        // Transform pseudojets into RecJetFormat
        std::vector<const RecJetFormat *> ClusterBase::__transform_jets(std::vector<fastjet::PseudoJet> jets) const
        {
            std::vector<const RecJetFormat *> output_jets;
            output_jets.reserve(jets.size());
            for (auto &jet: jets)
                output_jets.push_back(__transform_jet(jet));
            return output_jets;
        }

        // Method to get jet algorithm
        fastjet::JetAlgorithm ClusterBase::__get_clustering_algorithm(Substructure::Algorithm algorithm) const
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
        MAbool ClusterBase::__execute(EventFormat& myEvent, std::string JetID)
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
                output_jets.emplace_back(jet);
                std::vector <fastjet::PseudoJet> constituents = clust_seq->constituents(jet);
                output_jets.back().Constituents_.reserve(constituents.size());
                output_jets.back().ntracks_ = 0;
                for (auto &constit: constituents) {
                    output_jets.back().Constituents_.emplace_back(constit.user_index());
                    if (PDG->IsCharged(myEvent.mc()->particles()[constit.user_index()].pdgid()))
                        output_jets.back().ntracks_++;
                }
            }
            myEvent.rec()->jetcollection_.insert(std::make_pair(JetID, output_jets));
            isClustered_ = false;
            return true;
        }
    }
}
