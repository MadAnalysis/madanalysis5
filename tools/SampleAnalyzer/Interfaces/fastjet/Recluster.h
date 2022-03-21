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

#ifndef MADANALYSIS5_RECLUSTER_H
#define MADANALYSIS5_RECLUSTER_H

// STL headers
#include <vector>
#include <algorithm>

// FastJet headers
#include "fastjet/contrib/Recluster.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"
#include "SampleAnalyzer/Interfaces/fastjet/Cluster.h"

using namespace std;

namespace MA5 {
    namespace Substructure {
        class Recluster {
            //---------------------------------------------------------------------------------
            //                                 data members
            //---------------------------------------------------------------------------------
            protected:

                /// Jet definition
                fastjet::JetDefinition* JetDefinition_;

            // -------------------------------------------------------------
            //                       method members
            // -------------------------------------------------------------
            public:

                /// Constructor without argument
                Recluster() {}

                /// Destructor
                virtual ~Recluster() {}

                // Constructor with arguments
                Recluster(Algorithm algorithm, MAfloat32 radius)
                { Initialize(algorithm, radius); }

                void Initialize(Algorithm algorithm, MAfloat32 radius)
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

                // Execute with a single jet
                const RecJetFormat* Execute(const RecJetFormat *jet)
                {
                    RecJetFormat *NewJet = new RecJetFormat();
                    NewJet->Reset();
                    fastjet::contrib::Recluster recluster(*JetDefinition_);
                    fastjet::PseudoJet reclustered_jet = recluster(jet->pseudojet());
                    MALorentzVector q(
                            reclustered_jet.px(), reclustered_jet.py(), reclustered_jet.pz(), reclustered_jet.e()
                    );
                    NewJet->setMomentum(q);
                    NewJet->setPseudoJet(reclustered_jet);
                    return NewJet;
                }

                // Execute with a list of jets
                std::vector<const RecJetFormat *> Execute(std::vector<const RecJetFormat *> &jets)
                {
                    std::vector<const RecJetFormat *> output_jets;
                    for (auto &jet: jets)
                        output_jets.push_back(Execute(jet));

                    std::sort(
                        output_jets.begin(),
                        output_jets.end(),
                        [](const RecJetFormat *j1, const RecJetFormat *j2)
                        {
                            return (j1->pt() > j2->pt());
                        }
                    );

                    return output_jets;
                }
        };
    }
}

#endif //MADANALYSIS5_RECLUSTER_H
