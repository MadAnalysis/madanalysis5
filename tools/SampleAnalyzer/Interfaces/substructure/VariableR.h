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
 * @file VariableR.h
 * @brief Variable-R jet clustering (FastJet contrib VariableR).
 */

#ifndef MADANALYSIS5_VARIABLER_H
#define MADANALYSIS5_VARIABLER_H

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/substructure/ClusterBase.h"

namespace MA5 {
    namespace Substructure {
        /** @brief Variable-R jet clustering (FastJet contrib VariableRPlugin, effective radius R ~ rho/pT). */
        class VariableR : public ClusterBase {
            public:

                /** @brief Distance measure: C/A-like (p=0), kt-like (p=1) or anti-kt-like (p=-1). */
                enum ClusterType {CALIKE, KTLIKE, AKTLIKE};

                /** @brief Clustering strategy of the VariableR plugin. */
                enum Strategy {
                    Best,      ///< currently N2Tiled or N2Plain for FJ>3.2.0, Native for FastJet<3.2.0
                    N2Tiled,   ///< the default (faster in most cases) [requires FastJet>=3.2.0]
                    N2Plain,   ///< [requires FastJet>=3.2.0]
                    NNH,       ///< slower but already available for FastJet<3.2.0
                    Native     ///< original local implemtation of the clustering [the default for FastJet<3.2.0]
                };

                /** @brief Constructor without argument (call Initialize() before use). */
                VariableR() {}

                /** @brief Destructor. */
                ~VariableR() {}

                //============================//
                //        Initialization      //
                //============================//
                // Initialize the parameters of the algorithm. Initialization includes multiple if conditions
                // Hence it would be optimum execution to initialize the algorithm during the initialisation
                // of the analysis

                /**
                 * @brief Constructor with arguments (calls Initialize()).
                 *
                 * @param rho mass scale of the effective radius (R ~ rho/pT).
                 * @param minR minimum jet radius.
                 * @param maxR maximum jet radius.
                 * @param clusterType distance measure.
                 * @param strategy clustering strategy.
                 * @param ptmin minimum jet pT.
                 * @param isExclusive exclusive (true) or inclusive (false) jets.
                 */
                VariableR(
                    MAfloat32 rho,                                  // mass scale for effective radius (i.e. R ~ rho/pT)
                    MAfloat32 minR,                                 //minimum jet radius
                    MAfloat32 maxR,                                 // maximum jet radius
                    Substructure::VariableR::ClusterType clusterType,
                    Substructure::VariableR::Strategy strategy = Substructure::VariableR::Best,
                    MAfloat32 ptmin = 0.,                           // Minimum pT
                    MAbool isExclusive = false
                )
                { Initialize(rho, minR, maxR, clusterType, strategy, ptmin, isExclusive); }

                /**
                 * @brief Create the VariableR plugin and set the clustering options.
                 *
                 * @param rho mass scale of the effective radius (R ~ rho/pT).
                 * @param minR minimum jet radius.
                 * @param maxR maximum jet radius.
                 * @param clusterType distance measure.
                 * @param strategy clustering strategy.
                 * @param ptmin minimum jet pT.
                 * @param isExclusive exclusive (true) or inclusive (false) jets.
                 */
                void Initialize(
                    MAfloat32 rho,
                    MAfloat32 minR,
                    MAfloat32 maxR,
                    Substructure::VariableR::ClusterType clusterType,
                    Substructure::VariableR::Strategy strategy = Substructure::VariableR::Best,
                    MAfloat32 ptmin = 0.,
                    MAbool isExclusive = false
                );
        };
    }
}


#endif //MADANALYSIS5_VARIABLER_H
