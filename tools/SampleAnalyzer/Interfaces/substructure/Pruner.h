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
 * @file Pruner.h
 * @brief Jet pruning (FastJet Pruner).
 */

#ifndef MADANALYSIS5_PRUNER_H
#define MADANALYSIS5_PRUNER_H

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/substructure/Commons.h"

namespace fastjet {
    class JetDefinition;
}

namespace MA5 {
    namespace Substructure {
        /** @brief Jet pruning (Ellis, Vermilion and Walsh, arXiv:0903.5081) through the FastJet Pruner. */
        class Pruner {
            //---------------------------------------------------------------------------------
            //                                 data members
            //---------------------------------------------------------------------------------
            protected:

                /** @brief Jet definition used for the pruning (owned). */
                fastjet::JetDefinition *JetDefinition_;

                MAfloat32 zcut_; // pt-fraction cut in the pruning
                MAfloat32 Rcut_factor_; // the angular distance cut in the pruning will be Rcut_factor * 2m/pt

            public:

                /** @brief Constructor without argument (call Initialize() before use). */
                // FIXME: the default constructor leaves the pointer(s) uninitialised, but the destructor deletes them
                //   (undefined behaviour if Initialize() is never called).
                Pruner() {}

                /** @brief Destructor (deletes the jet definition). */
                ~Pruner();

                //============================//
                //        Initialization      //
                //============================//
                // Initialize the parameters of the algorithm. Initialization includes multiple if conditions
                // Hence it would be optimum execution to initialize the algorithm during the initialisation
                // of the analysis

                /**
                 * @brief Constructor with arguments (calls Initialize()).
                 *
                 * @param algorithm reclustering algorithm.
                 * @param R jet radius (the maximum allowed radius if R <= 0).
                 * @param zcut pT-fraction cut.
                 * @param Rcut_factor the angular cut is Rcut_factor * 2m/pT.
                 */
                Pruner(Substructure::Algorithm algorithm, MAfloat32 R, MAfloat32 zcut, MAfloat32 Rcut_factor)
                { Initialize(algorithm, R, zcut, Rcut_factor); }

                /**
                 * @brief Constructor with arguments, using the maximum allowed radius.
                 *
                 * @param algorithm reclustering algorithm.
                 * @param zcut pT-fraction cut.
                 * @param Rcut_factor the angular cut is Rcut_factor * 2m/pT.
                 */
                Pruner(Substructure::Algorithm algorithm, MAfloat32 zcut, MAfloat32 Rcut_factor)
                { Initialize(algorithm, -1., zcut, Rcut_factor); }

                /**
                 * @brief Initialise with the maximum allowed radius.
                 *
                 * @param algorithm reclustering algorithm.
                 * @param zcut pT-fraction cut.
                 * @param Rcut_factor the angular cut is Rcut_factor * 2m/pT.
                 */
                void Initialize(Substructure::Algorithm algorithm, MAfloat32 zcut, MAfloat32 Rcut_factor)
                { Initialize(algorithm, -1., zcut, Rcut_factor); }

                /**
                 * @brief Create the jet definition and store the pruning parameters.
                 *
                 * @param algorithm reclustering algorithm.
                 * @param R jet radius (the maximum allowed radius if R <= 0).
                 * @param zcut pT-fraction cut.
                 * @param Rcut_factor the angular cut is Rcut_factor * 2m/pT.
                 */
                void Initialize(
                    Substructure::Algorithm algorithm, MAfloat32 R, MAfloat32 zcut, MAfloat32 Rcut_factor
                );

                //=======================//
                //        Execution      //
                //=======================//

                /**
                 * @brief Prune a jet.
                 *
                 * The returned jets are allocated on the heap and are owned by the caller.
                 *
                 * @param jet jet to process (its constituents are used).
                 * @return the pruned jet.
                 */
                const RecJetFormat * Execute(const RecJetFormat *jet) const;

                /**
                 * @brief Prune each jet of a collection.
                 *
                 * The returned jets are allocated on the heap and are owned by the caller.
                 * The output is not re-ordered.
                 *
                 * @param jets jets to process.
                 * @return the pruned jets.
                 */
                std::vector<const RecJetFormat *> Execute(std::vector<const RecJetFormat *> &jets) const;

            private:

                /**
                 * @brief Prune a PseudoJet.
                 *
                 * @param jet FastJet jet.
                 * @return the pruned jet.
                 */
                fastjet::PseudoJet __prune(fastjet::PseudoJet jet) const;

        };
    }
}

#endif //MADANALYSIS5_PRUNER_H
