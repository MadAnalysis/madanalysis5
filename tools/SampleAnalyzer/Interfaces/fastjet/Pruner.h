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

// FastJet headers
#include "fastjet/tools/Pruner.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/fastjet/ClusterBase.h"

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

                //============================//
                //        Initialization      //
                //============================//
                // Initialize the parameters of the algorithm. Initialization includes multiple if conditions
                // Hence it would be optimum execution to initialize the algorithm during the initialisation
                // of the analysis

                // Constructor with arguments
                Pruner(Substructure::Algorithm algorithm, MAfloat32 R, MAfloat32 zcut, MAfloat32 Rcut_factor)
                { Initialize(algorithm, R, zcut, Rcut_factor); }

                // Constructor with arguments
                Pruner(Substructure::Algorithm algorithm, MAfloat32 zcut, MAfloat32 Rcut_factor)
                { Initialize(algorithm, -1., zcut, Rcut_factor); }

                void Initialize(Substructure::Algorithm algorithm, MAfloat32 zcut, MAfloat32 Rcut_factor)
                { Initialize(algorithm, -1., zcut, Rcut_factor); }

                // Initialize with all the arguments. Note that if R <= 0 max allowable radius will be used
                void Initialize(Substructure::Algorithm algorithm, MAfloat32 R, MAfloat32 zcut, MAfloat32 Rcut_factor)
                {
                    fastjet::JetAlgorithm algo_ = ClusterBase().__get_clustering_algorithm(algorithm);

                    if (R<=0.)
                        JetDefinition_ = new fastjet::JetDefinition(algo_, fastjet::JetDefinition::max_allowable_R);
                    else
                        JetDefinition_ = new fastjet::JetDefinition(algo_, R);

                    zcut_ = zcut; Rcut_factor_=Rcut_factor;
                }

                //=======================//
                //        Execution      //
                //=======================//

                // Method to prune a given jet with respect to initialization parameters
                const RecJetFormat * Execute(const RecJetFormat *jet)
                {
                    fastjet::PseudoJet pruned_jet = __prune(jet->pseudojet());
                    return ClusterBase().__transform_jet(pruned_jet);
                }

                // Method to prune each given jet individually with respect to initialization parameters
                std::vector<const RecJetFormat *> Execute(std::vector<const RecJetFormat *> &jets)
                {
                    std::vector<const RecJetFormat *> output_jets;
                    for (auto &jet: jets)
                        output_jets.push_back(Execute(jet));
                    return output_jets;
                }

            private:

                fastjet::PseudoJet __prune(fastjet::PseudoJet jet)
                {
                    fastjet::Pruner pruner(
                            *const_cast<const fastjet::JetDefinition*>(JetDefinition_), zcut_, Rcut_factor_
                    );
                    return pruner(jet);
                }

        };
    }
}

#endif //MADANALYSIS5_PRUNER_H
