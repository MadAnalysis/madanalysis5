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
#include "SampleAnalyzer/Interfaces/fastjet/ClusterBase.h"

using namespace std;

namespace MA5 {
    namespace Substructure {

        enum ClusterType {CALIKE, KTLIKE, AKTLIKE};

        enum Strategy {
            Best,      ///< currently N2Tiled or N2Plain for FJ>3.2.0, Native for FastJet<3.2.0
            N2Tiled,   ///< the default (faster in most cases) [requires FastJet>=3.2.0]
            N2Plain,   ///< [requires FastJet>=3.2.0]
            NNH,       ///< slower but already available for FastJet<3.2.0
            Native     ///< original local implemtation of the clustering [the default for FastJet<3.2.0]
        };

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

                /// Constructor without argument
                VariableR() {}

                /// Destructor
                virtual ~VariableR() {}

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
                    Substructure::ClusterType clusterType,
                    Substructure::Strategy strategy = Substructure::Best,
                    MAfloat32 ptmin = 0.,                           // Minimum pT
                    MAbool isExclusive = false
                )
                { Initialize(rho, minR, maxR, clusterType, strategy, ptmin, isExclusive); }

                // Initialization method
                void Initialize(
                    MAfloat32 rho,
                    MAfloat32 minR,
                    MAfloat32 maxR,
                    Substructure::ClusterType clusterType,
                    Substructure::Strategy strategy = Substructure::Best,
                    MAfloat32 ptmin = 0.,
                    MAbool isExclusive = false
                )
                {
                    if (clusterType == Substructure::CALIKE)
                        clusterType_ = fastjet::contrib::VariableRPlugin::CALIKE;
                    else if (clusterType == Substructure::KTLIKE)
                        clusterType_ = fastjet::contrib::VariableRPlugin::KTLIKE;
                    else if (clusterType == Substructure::AKTLIKE)
                        clusterType_ = fastjet::contrib::VariableRPlugin::AKTLIKE;

                    if (strategy == Substructure::Best)
                        strategy_ = fastjet::contrib::VariableRPlugin::Best;
                    else if (strategy == Substructure::N2Tiled)
                        strategy_ = fastjet::contrib::VariableRPlugin::N2Tiled;
                    else if (strategy == Substructure::N2Plain)
                        strategy_ = fastjet::contrib::VariableRPlugin::N2Plain;
                    else if (strategy == Substructure::NNH)
                        strategy_ = fastjet::contrib::VariableRPlugin::NNH;
                    else if (strategy == Substructure::Native)
                        strategy_ = fastjet::contrib::VariableRPlugin::Native;

                    ptmin_ = ptmin; isExclusive_ = isExclusive;

                    JetDefPlugin_ = new fastjet::contrib::VariableRPlugin(
                            rho, minR, maxR, clusterType_, false, strategy_
                    );
                    isPlugin_ = true;

                    /// Note that pre-clustering is deprecated and will likely be
                    /// removed in a future releasse of this contrib.
                    /// (precluster = false at the moment)

                }
        };
    }
}


#endif //MADANALYSIS5_VARIABLER_H
