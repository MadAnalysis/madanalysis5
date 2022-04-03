//////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/substructure/SoftDrop.h"

namespace MA5 {
    namespace Substructure {
        //============================//
        //        Initialization      //
        //============================//

        void SoftDrop::Initialize(MAfloat32 beta, MAfloat32 symmetry_cut, MAfloat32 R0)
        { softDrop_ = new fastjet::contrib::SoftDrop(beta, symmetry_cut, R0); }

        //=======================//
        //        Execution      //
        //=======================//

        // Execute with a single jet
        const RecJetFormat * SoftDrop::Execute(const RecJetFormat *jet) const
        {
            fastjet::PseudoJet sd_jet = (*softDrop_)(jet->pseudojet());
            return ClusterBase().__transform_jet(sd_jet);
        }

        // Execute with a list of jets
        std::vector<const RecJetFormat *> SoftDrop::Execute(std::vector<const RecJetFormat *> &jets) const
        {
            std::vector<const RecJetFormat *> output_jets;
            for (auto &jet: jets)
                output_jets.push_back(Execute(jet));

            // Sort with respect to jet pT
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