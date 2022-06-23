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
#include "fastjet/tools/Pruner.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/substructure/Pruner.h"

namespace MA5 {
    namespace Substructure {

        Pruner::~Pruner()
        {
            delete JetDefinition_;
        }

        // Initialize with all the arguments. Note that if R <= 0 max allowable radius will be used
        void Pruner::Initialize(
            Substructure::Algorithm algorithm, MAfloat32 R, MAfloat32 zcut, MAfloat32 Rcut_factor
        )
        {
            fastjet::JetAlgorithm algo_ = fastjet::antikt_algorithm;
            if (algorithm == Substructure::cambridge) algo_ = fastjet::cambridge_algorithm;
            else if (algorithm == Substructure::kt)   algo_ = fastjet::kt_algorithm;

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
        const RecJetFormat * Pruner::Execute(const RecJetFormat *jet) const
        {
            fastjet::PseudoJet pruned_jet = __prune(jet->pseudojet());
            RecJetFormat * NewJet = new RecJetFormat(pruned_jet);
            return NewJet;
        }

        // Method to prune each given jet individually with respect to initialization parameters
        std::vector<const RecJetFormat *> Pruner::Execute(std::vector<const RecJetFormat *> &jets) const
        {
            std::vector<const RecJetFormat *> output_jets;
            output_jets.reserve(jets.size());
            for (auto &jet: jets)
                output_jets.push_back(Execute(jet));
            return output_jets;
        }

        fastjet::PseudoJet Pruner::__prune(fastjet::PseudoJet jet) const
        {
            fastjet::Pruner pruner(
                    *const_cast<const fastjet::JetDefinition*>(JetDefinition_), zcut_, Rcut_factor_
            );
            return pruner(jet);
        }
    }
}