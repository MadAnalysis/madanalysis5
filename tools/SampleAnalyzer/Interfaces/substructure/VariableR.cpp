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

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/substructure/VariableR.h"

// FastJet headers
#include "fastjet/contrib/VariableRPlugin.hh"

namespace MA5 {
    namespace Substructure {
        // Initialization method
        void VariableR::Initialize(
                MAfloat32 rho,
                MAfloat32 minR,
                MAfloat32 maxR,
                Substructure::VariableR::ClusterType clusterType,
                Substructure::VariableR::Strategy strategy,
                MAfloat32 ptmin,
                MAbool isExclusive
        )
        {
            if (clusterType == Substructure::VariableR::CALIKE)
                clusterType_ = fastjet::contrib::VariableRPlugin::CALIKE;
            else if (clusterType == Substructure::VariableR::KTLIKE)
                clusterType_ = fastjet::contrib::VariableRPlugin::KTLIKE;
            else if (clusterType == Substructure::VariableR::AKTLIKE)
                clusterType_ = fastjet::contrib::VariableRPlugin::AKTLIKE;

            if (strategy == Substructure::VariableR::Best)
                strategy_ = fastjet::contrib::VariableRPlugin::Best;
            else if (strategy == Substructure::VariableR::N2Tiled)
                strategy_ = fastjet::contrib::VariableRPlugin::N2Tiled;
            else if (strategy == Substructure::VariableR::N2Plain)
                strategy_ = fastjet::contrib::VariableRPlugin::N2Plain;
            else if (strategy == Substructure::VariableR::NNH)
                strategy_ = fastjet::contrib::VariableRPlugin::NNH;
            else if (strategy == Substructure::VariableR::Native)
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
    }
}