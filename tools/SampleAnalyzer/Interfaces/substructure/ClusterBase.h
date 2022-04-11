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

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/Service/PDGService.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

namespace fastjet
{
    class JetDefinition;
    class PseudoJet;
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
        class HTT;

        class ClusterBase {

            friend class Recluster;
            friend class SoftDrop;
            friend class Pruner;
            friend class Filter;
            friend class HTT;

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
            virtual ~ClusterBase()
            {
                // clean heap allocation
                delete JetDefinition_;
                delete JetDefPlugin_;
            }

            // Set the Jet definition using algorithm and radius input
            void SetJetDef(Algorithm algorithm, MAfloat32 radius);

            //=======================//
            //        Execution      //
            //=======================//

            // Wrapper for event based execution
            virtual void Execute(const EventFormat& event, std::string JetID);

            // Execute with a single jet. This method reclusters the given jet using its constituents
            std::vector<const RecJetFormat *> Execute(const RecJetFormat *jet);

            // Execute with a single jet. This method reclusters the given jet using its constituents by filtering
            // reclustered events with respect to the initial jet
            template<typename Func>
            std::vector<const RecJetFormat *> Execute(const RecJetFormat *jet, Func func);

            // Execute with a list of jets. This method reclusters the given collection
            // of jets by combining their constituents
            virtual std::vector<const RecJetFormat *> Execute(std::vector<const RecJetFormat *> &jets);

            // Handler for clustering step
            void cluster(const RecJetFormat *jet);

            // return a vector of all jets when the event is clustered (in the exclusive sense) to exactly njets.
            // If there are fewer than njets particles in the ClusterSequence the function just returns however many
            // particles there were.
            std::vector<const RecJetFormat *> exclusive_jets_up_to(MAint32 njets);

        private:

            // Generic clustering method
            std::vector<fastjet::PseudoJet> __cluster(std::vector<fastjet::PseudoJet> particles);

            // Method to transform pseudojet into recjetformat
            RecJetFormat * __transform_jet(fastjet::PseudoJet jet) const;

            // Transform pseudojets into RecJetFormat
            std::vector<const RecJetFormat *> __transform_jets(std::vector<fastjet::PseudoJet> jets) const;

            // Method to get jet algorithm
            fastjet::JetAlgorithm __get_clustering_algorithm(Substructure::Algorithm algorithm) const;

            // Execute with the Reconstructed event. This method creates a new Jet in RecEventFormat which
            // can be accessed via JetID. The algorithm will only be executed if a unique JetID is given
            MAbool __execute(EventFormat& myEvent, std::string JetID);
        };
    }
}

#endif //MADANALYSIS5_CLUSTERBASE_H
