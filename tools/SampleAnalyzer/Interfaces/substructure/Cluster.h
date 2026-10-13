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
 * @file Cluster.h
 * @brief Jet clustering with a standard algorithm (anti-kt, C/A, kt).
 */

#ifndef MADANALYSIS5_CLUSTER_H
#define MADANALYSIS5_CLUSTER_H

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/substructure/ClusterBase.h"

namespace MA5 {
    namespace Substructure{
        /** @brief Jet clustering with a standard FastJet algorithm (anti-kt, Cambridge/Aachen, kt). */
        class Cluster : public ClusterBase {

            // -------------------------------------------------------------
            //                       method members
            // -------------------------------------------------------------
            public:

                /** @brief Constructor without argument (call Initialize() before use). */
                Cluster() {}

                /** @brief Destructor. */
                ~Cluster() {}

                //============================//
                //        Initialization      //
                //============================//
                // Initialize the parameters of the algorithm. Initialization includes multiple if conditions
                // Hence it would be optimum execution to initialize the algorithm during the initialisation
                // of the analysis

                /**
                 * @brief Constructor with arguments (calls Initialize()).
                 *
                 * @param algorithm clustering algorithm.
                 * @param radius jet radius.
                 * @param ptmin minimum jet pT.
                 * @param isExclusive if true, return exclusive jets (dcut = ptmin^2); otherwise inclusive jets with pT >= ptmin.
                 */
                Cluster(Algorithm algorithm, MAfloat32 radius, MAfloat32 ptmin=0., MAbool isExclusive = false)
                { Initialize(algorithm, radius, ptmin, isExclusive); }

                /**
                 * @brief Set the jet definition and the clustering options.
                 *
                 * @param algorithm clustering algorithm.
                 * @param radius jet radius.
                 * @param ptmin minimum jet pT.
                 * @param isExclusive exclusive (true) or inclusive (false) jets.
                 */
                // FIXME: Initialize() is declared but never defined: Cluster.cpp contains a stale copy of a
                //   NullSmearer class instead, so using Substructure::Cluster fails at link time.
                void Initialize(
                    Algorithm algorithm, MAfloat32 radius, MAfloat32 ptmin=0., MAbool isExclusive = false
                );

        };
    }
}

#endif //MADANALYSIS5_CLUSTER_H
