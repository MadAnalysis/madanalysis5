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

/**
 * @file ClusterBase.h
 * @brief Base class of the substructure (re)clustering tools.
 */

#ifndef MADANALYSIS5_CLUSTERBASE_H
#define MADANALYSIS5_CLUSTERBASE_H

// STL headers
#include <vector>
#include <algorithm>

// FastJet headers
#include "fastjet/ClusterSequence.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/Service/PDGService.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Interfaces/substructure/Commons.h"

namespace fastjet
{
    class JetDefinition;
    class PseudoJet;
}

namespace MA5{
    namespace Substructure {

        /** @brief Forward declaration (friend class). */
        class Recluster;

        /**
         * @brief Base class of the substructure clustering tools (Cluster, VariableR, Recluster).
         *
         * Holds a FastJet jet definition (or plugin) and the last cluster sequence. The jets
         * can be clustered from an event, from the constituents of a jet or from the
         * constituents of a collection of jets.
         */
        class ClusterBase {

            friend class Recluster;

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

            /** @brief FastJet jet definition (owned; used when isPlugin_ is false). */
            fastjet::JetDefinition* JetDefinition_;
            /** @brief FastJet jet-definition plugin (owned; used when isPlugin_ is true). */
            fastjet::JetDefinition::Plugin* JetDefPlugin_;
            /** @brief Whether the plugin is used instead of the jet definition. */
            MAbool isPlugin_;
            /** @brief Whether a cluster sequence is available (required by exclusive_jets_up_to()). */
            MAbool isClustered_;

            /** @brief Cluster sequence of the last clustering (shared with the returned jets). */
            std::shared_ptr<fastjet::ClusterSequence> clust_seq;

        public:

            /** @brief Constructor without argument. */
            // FIXME: the default constructor leaves JetDefinition_, JetDefPlugin_, isPlugin_, isClustered_, ptmin_
            //   and isExclusive_ uninitialised, while ~ClusterBase() deletes both pointers (only one of them is
            //   ever allocated by the derived classes).
            ClusterBase() {}

            /** @brief Destructor (deletes the jet definition and the plugin). */
            virtual ~ClusterBase()
            {
                // clean heap allocation
                delete JetDefinition_;
                delete JetDefPlugin_;
            }

            /**
             * @brief Set a standard jet definition (no plugin).
             *
             * @param algorithm clustering algorithm.
             * @param radius jet radius.
             */
            void SetJetDef(Algorithm algorithm, MAfloat32 radius);

            //=======================//
            //        Execution      //
            //=======================//

            /**
             * @brief Cluster the event and store the jets in a new jet collection.
             *
             * @param event event (its RecEventFormat is modified through a const_cast).
             * @param JetID identifier of the new jet collection (must not exist yet).
             */
            virtual void Execute(const EventFormat& event, std::string JetID);

            /**
             * @brief Recluster the constituents of a jet.
             *
             * The returned jets are allocated on the heap and are owned by the caller.
             *
             * @param jet jet to process (its constituents are used).
             * @return the reclustered jets, pT-ordered.
             */
            std::vector<const RecJetFormat *> Execute(const RecJetFormat *jet);

            /**
             * @brief Recluster the constituents of a jet and keep the subjets accepted by a filter.
             *
             * The returned jets are allocated on the heap and are owned by the caller.
             *
             * @tparam Func callable `bool(const RecJetFormat* jet, const RecJetFormat* subjet)`.
             * @param jet jet to process (its constituents are used).
             * @param func filter applied to each reclustered jet.
             * @return the accepted jets, pT-ordered.
             */
            template<typename Func>
            std::vector<const RecJetFormat *> Execute(const RecJetFormat *jet, Func func);

            /**
             * @brief Recluster the combined constituents of a collection of jets.
             *
             * The returned jets are allocated on the heap and are owned by the caller.
             *
             * @param jets jets to process.
             * @return the reclustered jets, pT-ordered.
             */
            virtual std::vector<const RecJetFormat *> Execute(std::vector<const RecJetFormat *> &jets);

            /**
             * @brief Cluster the constituents of a jet and keep the cluster sequence (see exclusive_jets_up_to()).
             *
             * @param jet jet to process (its constituents are used).
             */
            void cluster(const RecJetFormat *jet);

            /**
             * @brief Exclusive jets of the last cluster sequence, clustered to exactly `njets`.
             *
             * If there are fewer than `njets` particles, all of them are returned. The returned jets are allocated on the heap and are owned by the caller.
             *
             * @param njets number of exclusive jets.
             * @return the jets, pT-ordered.
             */
            std::vector<const RecJetFormat *> exclusive_jets_up_to(MAint32 njets);

        private:

            /**
             * @brief Cluster a set of particles with the current jet definition.
             *
             * @param particles input particles.
             * @return the inclusive (or exclusive) jets above ptmin_, pT-ordered.
             */
            std::vector<fastjet::PseudoJet> __cluster(std::vector<fastjet::PseudoJet> particles);

            /**
             * @brief Convert a PseudoJet into a new RecJetFormat.
             *
             * @param jet FastJet jet.
             * @return the new jet (owned by the caller).
             */
            RecJetFormat * __transform_jet(fastjet::PseudoJet jet) const;

            /**
             * @brief Convert PseudoJets into new RecJetFormat objects.
             *
             * @param jets FastJet jets.
             * @return the new jets (owned by the caller).
             */
            std::vector<const RecJetFormat *> __transform_jets(std::vector<fastjet::PseudoJet> jets) const;

            /**
             * @brief Convert an MA5 algorithm into a FastJet algorithm.
             *
             * @param algorithm MA5 algorithm.
             * @return the FastJet algorithm (an exception is thrown for an unknown algorithm).
             */
            fastjet::JetAlgorithm __get_clustering_algorithm(Substructure::Algorithm algorithm) const;

            /**
             * @brief Cluster the event inputs into a new jet collection of RecEventFormat.
             *
             * @param myEvent event.
             * @param JetID identifier of the new jet collection.
             * @return false if the identifier already exists.
             */
            MAbool __execute(EventFormat& myEvent, std::string JetID);
        };
    }
}

#endif //MADANALYSIS5_CLUSTERBASE_H
