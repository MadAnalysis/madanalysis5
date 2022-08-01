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

#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"

#ifdef MA5_FASTJET_MODE

namespace MA5 {

    /// Add one constituent
    void RecJetFormat::AddConstituent(const MAint32& index) { Constituents_.push_back(index); }

    /// get constituent collections
    const std::vector<MAint32>& RecJetFormat::constituents() const { return Constituents_; }

    /// Add one isolation cone
    void RecJetFormat::AddIsolCone (const IsolationConeType& cone) { isolCones_.push_back(cone); }

    /// get the collection of isolation cones
    const std::vector<IsolationConeType>& RecJetFormat::isolCones() const { return isolCones_; }

    // return a vector of all subjets of the current jet (in the sense of the exclusive algorithm)
    // that would be obtained when running the algorithm with the given dcut.
    std::vector<const RecJetFormat *> RecJetFormat::exclusive_subjets(MAfloat32 dcut) const
    {
        std::vector<const RecJetFormat *> output_jets;
                try {
            if (!RecJetFormat::has_exclusive_subjets())
                throw EXCEPTION_ERROR("This jet structure does not contain exclusive_subjets",
                                      "Exclusive subjets only exist for jets clustered through an exclusive algorithm.", 0);
        } catch (const std::exception& err) {
                MANAGE_EXCEPTION(err);
                return output_jets;
            }
        std::vector<fastjet::PseudoJet> current_jets = fastjet::sorted_by_pt(pseudojet_.exclusive_subjets(dcut));
        output_jets.reserve(current_jets.size());
        for (auto &jet: current_jets)
        {
            RecJetFormat * NewJet = new RecJetFormat(jet);
            output_jets.push_back(NewJet);
        }
        return output_jets;
    }

    // return the list of subjets obtained by unclustering the supplied jet down to nsub subjets.
    std::vector<const RecJetFormat *> RecJetFormat::exclusive_subjets(MAint32 nsub) const
    {
        std::vector<const RecJetFormat *> output_jets;
        try {
            if (!RecJetFormat::has_exclusive_subjets())
                throw EXCEPTION_ERROR("This jet structure does not contain exclusive_subjets",
                                      "Exclusive subjets only exist for jets clustered through an exclusive algorithm.", 0);
        } catch (const std::exception& err) {
                MANAGE_EXCEPTION(err);
                return output_jets;
            }
        std::vector<fastjet::PseudoJet> current_jets = fastjet::sorted_by_pt(pseudojet_.exclusive_subjets(nsub));
        output_jets.reserve(current_jets.size());
        for (auto &jet: current_jets)
        {
            RecJetFormat * NewJet = new RecJetFormat(jet);
            output_jets.push_back(NewJet);
        }
        return output_jets;
    }

    //returns true if the PseudoJet has support for exclusive subjets
    MAbool RecJetFormat::has_exclusive_subjets() const { return pseudojet_.has_exclusive_subjets(); }
}
#endif
