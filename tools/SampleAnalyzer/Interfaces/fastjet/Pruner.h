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

#ifndef MADANALYSIS5_PRUNER_H
#define MADANALYSIS5_PRUNER_H

// STL headers
#include <vector>
#include <algorithm>

// FastJet headers
#include "fastjet/PseudoJet.hh"
#include "fastjet/tools/Pruner.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"
#include "SampleAnalyzer/Interfaces/fastjet/Cluster.h"

using namespace std;

namespace MA5 {
    namespace Substructure {
        class Pruner {
            //---------------------------------------------------------------------------------
            //                                 data members
            //---------------------------------------------------------------------------------
            protected:

                /// Jet definition
                fastjet::JetDefinition *JetDefinition_;

                MAfloat32 zcut_; // pt-fraction cut in the pruning
                MAfloat32 Rcut_factor_; // the angular distance cut in the pruning will be Rcut_factor * 2m/pt

            public:

                /// Constructor without argument
                Pruner() {}

                /// Destructor
                virtual ~Pruner() {}

                // Constructor with arguments
                Pruner(Substructure::Algorithm algorithm, MAfloat32 R, MAfloat32 zcut, MAfloat32 Rcut_factor)
                { Initialize(algorithm, R, zcut, Rcut_factor); }

                // Constructor with arguments
                Pruner(Substructure::Algorithm algorithm, MAfloat32 zcut, MAfloat32 Rcut_factor)
                { Initialize(algorithm, -1., zcut, Rcut_factor); }

                void Initialize(Substructure::Algorithm algorithm, MAfloat32 zcut, MAfloat32 Rcut_factor)
                { Initialize(algorithm, -1., zcut, Rcut_factor); }

                void Initialize(Substructure::Algorithm algorithm, MAfloat32 R, MAfloat32 zcut, MAfloat32 Rcut_factor)
                {
                    fastjet::JetAlgorithm algo_;
                    if (algorithm == Substructure::antikt) algo_ = fastjet::antikt_algorithm;
                    else if (algorithm == Substructure::cambridge) algo_ = fastjet::cambridge_algorithm;
                    else if (algorithm == Substructure::kt) algo_ = fastjet::kt_algorithm;

                    if (R<=0.)
                        JetDefinition_ = new fastjet::JetDefinition(algo_, fastjet::JetDefinition::max_allowable_R);
                    else
                        JetDefinition_ = new fastjet::JetDefinition(algo_, R);

                    zcut_ = zcut; Rcut_factor_=Rcut_factor;
                }

                // Execute with a single jet
                const RecJetFormat * Execute(const RecJetFormat *jet)
                {

                    fastjet::Pruner pruner(
                            *const_cast<const fastjet::JetDefinition*>(JetDefinition_), zcut_, Rcut_factor_
                    );
                    fastjet::PseudoJet pruned_jet = pruner(jet->pseudojet());

                    RecJetFormat *NewJet = new RecJetFormat();
                    NewJet->Reset();
                    MALorentzVector q(pruned_jet.px(), pruned_jet.py(), pruned_jet.pz(), pruned_jet.e());
                    NewJet->setMomentum(q);
                    NewJet->setPseudoJet(pruned_jet);

                    return NewJet;
                }
        };
    }
}

#endif //MADANALYSIS5_PRUNER_H
