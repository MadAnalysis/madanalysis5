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

#ifndef MADANALYSIS5_VARIABLER_H
#define MADANALYSIS5_VARIABLER_H

// FastJet headers
#include "fastjet/contrib/VariableRPlugin.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/substructure/ClusterBase.h"

using namespace std;

namespace MA5 {
    namespace Substructure {
        class VariableR : public ClusterBase {

            //---------------------------------------------------------------------------------
            //                                 data members
            //---------------------------------------------------------------------------------
            protected:
                fastjet::contrib::VariableRPlugin::ClusterType clusterType_; // whether to use CA-like, kT-like,
                                                            // or anti-kT-like distance measure
                                                            // (this value is the same as the p exponent in
                                                            // generalized-kt, with anti-kt = -1.0, CA = 0.0, and
                                                            // kT = 1.0)

                fastjet::contrib::VariableRPlugin::Strategy strategy_;
                // decodes which algorithm to apply for the clustering


            public:

                enum ClusterType {CALIKE, KTLIKE, AKTLIKE};

                enum Strategy {
                    Best,      ///< currently N2Tiled or N2Plain for FJ>3.2.0, Native for FastJet<3.2.0
                    N2Tiled,   ///< the default (faster in most cases) [requires FastJet>=3.2.0]
                    N2Plain,   ///< [requires FastJet>=3.2.0]
                    NNH,       ///< slower but already available for FastJet<3.2.0
                    Native     ///< original local implemtation of the clustering [the default for FastJet<3.2.0]
                };

                /// Constructor without argument
                VariableR() {}

                /// Destructor
                ~VariableR() {}

                //============================//
                //        Initialization      //
                //============================//
                // Initialize the parameters of the algorithm. Initialization includes multiple if conditions
                // Hence it would be optimum execution to initialize the algorithm during the initialisation
                // of the analysis

                // Constructor with arguments
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

                // Initialization method
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
