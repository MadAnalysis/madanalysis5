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

#ifndef MADANALYSIS5_SOFTDROP_H
#define MADANALYSIS5_SOFTDROP_H

// STL headers
#include <vector>
#include <algorithm>

// FastJet headers
#include "fastjet/contrib/SoftDrop.hh"
#include "fastjet/PseudoJet.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"

using namespace std;

namespace MA5 {
    namespace Substructure {
        class SoftDrop {

            //---------------------------------------------------------------------------------
            //                                 data members
            //---------------------------------------------------------------------------------
            protected :

                // SoftDrop input variables
                MAfloat32 beta_;
                MAfloat32 symmetry_cut_;

            // -------------------------------------------------------------
            //                       method members
            // -------------------------------------------------------------
            public:

                /// Constructor without argument
                SoftDrop() {}

                /// Destructor
                virtual ~SoftDrop() {}

                // Constructor with arguments
                SoftDrop(MAfloat32 beta, MAfloat32 symmetry_cut) { Initialize(beta, symmetry_cut); }

                void Initialize(MAfloat32 beta, MAfloat32 symmetry_cut)
                {
                    beta_ = beta;
                    symmetry_cut_ = symmetry_cut;
                }

                // Execute with a single jet
                const RecJetFormat *Execute(const RecJetFormat *jet)
                {
                    RecJetFormat *NewJet = new RecJetFormat();
                    NewJet->Reset();
                    fastjet::contrib::SoftDrop sd(beta_, symmetry_cut_);
                    fastjet::PseudoJet sd_jet = sd(jet->pseudojet());
                    MALorentzVector q(sd_jet.px(), sd_jet.py(), sd_jet.pz(), sd_jet.e());
                    NewJet->setMomentum(q);
                    NewJet->setPseudoJet(sd_jet);
                    return NewJet;
                }

                // Execute with a list of jets
                std::vector<const RecJetFormat *> Execute(std::vector<const RecJetFormat *> &jets)
                {
                    std::vector<const RecJetFormat *> output_jets;
                    for (auto &jet: jets)
                        output_jets.push_back(Execute(jet));

                    return output_jets;
                }

        };
    }
}
#endif //MADANALYSIS5_SOFTDROP_H
