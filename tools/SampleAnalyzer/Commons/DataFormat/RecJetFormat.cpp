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

    /// Clear all information
    void RecJetFormat::clear()
    {
        ntracks_ = 0;
        loose_btag_ = false;
        loose_ctag_ = false;
        loose_tautag_ = false;
        mid_btag_ = false;
        mid_ctag_ = false;
        mid_tautag_ = false;
        tight_btag_ = false;
        tight_ctag_ = false;
        tight_tautag_ = false;
        true_ctag_ = false;
        true_btag_ = false;
        true_tautag_ = false;
        isolCones_.clear();
        Constituents_.clear();
    }

    /// Accessor to the number of tracks
    const MAuint16 RecJetFormat::ntracks() const {return ntracks_;}

    /// Setting ntracks
    void RecJetFormat::setNtracks(MAuint16 ntracks) { ntracks_=ntracks; }

    /// Add one constituent
    void RecJetFormat::AddConstituent(const MAint32& index) { Constituents_.push_back(index); }

    /// get constituent collections
    const std::vector<MAint32>& RecJetFormat::constituents() const { return Constituents_; }

    /// Add one isolation cone
    void RecJetFormat::AddIsolCone (const IsolationConeType& cone) { isolCones_.push_back(cone); }

    /// get the collection of isolation cones
    const std::vector<IsolationConeType>& RecJetFormat::isolCones() const { return isolCones_; }

    ///==================///
    /// Tagger accessors ///
    ///==================///

    /// Accessor to the loose b-tag
    const MAbool& RecJetFormat::btag() const { return loose_btag(); }

    /// Accessor to the loose c-tag
    const MAbool& RecJetFormat::ctag() const { return loose_ctag(); }

    /// Accessor to the loose c-tag
    const MAbool& RecJetFormat::tautag() const { return loose_tautag(); }

    /// Accessor to the loose b-tag
    const MAbool& RecJetFormat::loose_btag() const { return loose_btag_; }

    /// Accessor to the loose c-tag
    const MAbool& RecJetFormat::loose_ctag() const { return loose_ctag_; }

    /// Accessor to the loose c-tag
    const MAbool& RecJetFormat::loose_tautag() const { return loose_tautag_; }

    /// Accessor to the mid b-tag
    const MAbool& RecJetFormat::mid_btag() const { return mid_btag_; }

    /// Accessor to the mid c-tag
    const MAbool& RecJetFormat::mid_ctag() const { return mid_ctag_; }

    /// Accessor to the mid c-tag
    const MAbool& RecJetFormat::mid_tautag() const { return mid_tautag_; }

    /// Accessor to the tight b-tag
    const MAbool& RecJetFormat::tight_btag() const { return tight_btag_; }

    /// Accessor to the tight c-tag
    const MAbool& RecJetFormat::tight_ctag() const { return tight_ctag_; }

    /// Accessor to the tight c-tag
    const MAbool& RecJetFormat::tight_tautag() const { return tight_tautag_; }

    /// Accessor to the true c-tag
    const MAbool& RecJetFormat::true_ctag() const {return true_btag_;}

    /// Accessor to the true b-tag
    const MAbool& RecJetFormat::true_btag() const {return true_ctag_;}

    /// Accessor to the true tau-tag
    const MAbool& RecJetFormat::true_tautag() const {return true_tautag_;}

    /// Setters for tagger

    /// Setting a new true_btag_ value
    void RecJetFormat::setTrueBtag(const MAbool& tag) { true_btag_ = tag;}

    /// Setting a new true_ctag_ value
    void RecJetFormat::setTrueCtag(const MAbool& tag) { true_ctag_ = tag;}

    /// Setting a new true_tautag_ value
    void RecJetFormat::setTrueTautag(const MAbool& tag) { true_tautag_ = tag;}

    /// Setting a new loose_btag_ value
    void RecJetFormat::setBtag(const MAbool& tag) { setLooseBtag(tag); }

    /// Setting a new loose_ctag_ value
    void RecJetFormat::setCtag(const MAbool& tag) { setLooseCtag(tag); }

    /// Setting a new loose_tautag_ value
    void RecJetFormat::setTautag(const MAbool& tag) { setLooseTautag(tag); }

    /// Setting a new loose_btag_ value
    void RecJetFormat::setLooseBtag(const MAbool& tag) { loose_btag_ = tag; }

    /// Setting a new loose_ctag_ value
    void RecJetFormat::setLooseCtag(const MAbool& tag) { loose_ctag_ = tag; }

    /// Setting a new loose_tautag_ value
    void RecJetFormat::setLooseTautag(const MAbool& tag) { loose_tautag_ = tag; }

    /// Setting a new Mid_btag_ value
    void RecJetFormat::setMidBtag(const MAbool& tag) { mid_btag_ = tag; }

    /// Setting a new Mid_ctag_ value
    void RecJetFormat::setMidCtag(const MAbool& tag) { mid_ctag_ = tag; }

    /// Setting a new Mid_tautag_ value
    void RecJetFormat::setMidTautag(const MAbool& tag) { mid_tautag_ = tag; }

    /// Setting a new Tight_btag_ value
    void RecJetFormat::setTightBtag(const MAbool& tag) { tight_btag_ = tag; }

    /// Setting a new Tight_ctag_ value
    void RecJetFormat::setTightCtag(const MAbool& tag) { tight_ctag_ = tag; }

    /// Setting a new Tight_tautag_ value
    void RecJetFormat::setTightTautag(const MAbool& tag) { tight_tautag_ = tag; }

    /// Set all b-tags
    void RecJetFormat::setAllBtags(const MAbool &tag) {
        true_btag_ = tag;
        loose_btag_ = tag;
        mid_btag_ = tag;
        tight_btag_ = tag;
    }

    /// Set all c-tags
    void RecJetFormat::setAllCtags(const MAbool &tag) {
        true_ctag_ = tag;
        loose_ctag_ = tag;
        mid_ctag_ = tag;
        tight_ctag_ = tag;
    }

    /// Set all tau-tags
    void RecJetFormat::setAllTautags(const MAbool &tag) {
        true_tautag_ = tag;
        loose_tautag_ = tag;
        mid_tautag_ = tag;
        tight_tautag_ = tag;
    }

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
