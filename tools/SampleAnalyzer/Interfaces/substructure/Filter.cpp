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

// FastJet headers
#include "fastjet/tools/Filter.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/substructure/Filter.h"

namespace MA5 {
    namespace Substructure {
        //============================//
        //        Initialization      //
        //============================//

        void Filter::Initialize(Algorithm algorithm, MAfloat32 radius, Selector selector, MAfloat32 rho)
        {
            JetDefinition_ = new fastjet::JetDefinition(
                    ClusterBase().__get_clustering_algorithm(algorithm), radius
            );
            selector_ = selector.__get(); Rfilt_=-1.; rho_=rho; isJetDefined_=true;
            init_filter();
        }

        void Filter::Initialize(MAfloat32 Rfilt, Selector selector, MAfloat32 rho)
        {
            selector_ = selector.__get(); Rfilt_=Rfilt; rho_=rho; isJetDefined_=false;
            init_filter();
        }

        void Filter::init_filter()
        {
            if (isJetDefined_)
                JetFilter_ = new fastjet::Filter(*JetDefinition_, selector_, rho_);
            else
                JetFilter_ = new fastjet::Filter(Rfilt_, selector_, rho_);
        }

        //=======================//
        //        Execution      //
        //=======================//

        // Method to filter a given jet.
        const RecJetFormat* Filter::Execute(const RecJetFormat *jet) const
        {
            fastjet::PseudoJet filtered_jet = (*JetFilter_)(jet->pseudojet());
            return ClusterBase().__transform_jet(filtered_jet);
        }

        // Method to filter all the jets in a vector
        std::vector<const RecJetFormat*> Filter::Execute(std::vector<const RecJetFormat *> &jets) const
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
    }
}