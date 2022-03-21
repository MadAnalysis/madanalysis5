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

namespace fastjet
{
    class JetDefinition;
}

using namespace std;

namespace MA5 {
    namespace Substructure{

        // Jet clustering algorithms
        enum Algorithm {antikt, cambridge, kt};

        class Cluster {
            //---------------------------------------------------------------------------------
            //                                 data members
            //---------------------------------------------------------------------------------
            protected:

                // External parameters
                MAfloat32 ptmin_;

                /// Jet definition
                fastjet::JetDefinition* JetDefinition_;

                // Shared Cluster sequence for primary jet
                std::shared_ptr<fastjet::ClusterSequence> clust_seq;

            // -------------------------------------------------------------
            //                       method members
            // -------------------------------------------------------------
            public:

                /// Constructor without argument
                Cluster() {}

                /// Destructor
                virtual ~Cluster() {}

                // Constructor with arguments
                Cluster(Algorithm algorithm, MAfloat32 radius, MAfloat32 ptmin=0.)
                { Initialize(algorithm, radius, ptmin); }

                void Initialize(Algorithm algorithm, MAfloat32 radius, MAfloat32 ptmin=0.)
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
                    ptmin_ = ptmin < 0. ? 0. : ptmin;
                }

                // Execute with a single jet
                std::vector<const RecJetFormat *> Execute(const RecJetFormat *jet)
                {
                    std::vector<const RecJetFormat *> output_jets;

                    clust_seq.reset(new fastjet::ClusterSequence(jet->pseudojet().constituents(), *JetDefinition_));
                    std::vector<fastjet::PseudoJet> reclustered_jets = sorted_by_pt(clust_seq->exclusive_jets(ptmin_));

                    for (auto &jet: reclustered_jets)
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

                    clust_seq.reset(new fastjet::ClusterSequence(constituents, *JetDefinition_));
                    std::vector<fastjet::PseudoJet> reclustered_jets = sorted_by_pt(clust_seq->exclusive_jets(ptmin_));

                    for (auto &jet: reclustered_jets)
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

#endif //MADANALYSIS5_CLUSTER_H
