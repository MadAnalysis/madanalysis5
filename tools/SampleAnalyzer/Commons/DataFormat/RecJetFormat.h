////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://github.com/MadAnalysis/madanalysis5>
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
 * @file RecJetFormat.h
 * @brief Reconstructed jet with b/c/tau tags.
 */

#ifndef RecJetFormat_h
#define RecJetFormat_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/IsolationConeType.h"
#include "SampleAnalyzer/Commons/DataFormat/RecParticleFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

// FastJet headers
#ifdef MA5_FASTJET_MODE
#include "fastjet/PseudoJet.hh"
#endif

namespace MA5
{
    namespace Substructure {
        class ClusterBase;
        class Pruner;
        class Nsubjettiness;
        class SoftDrop;
        class Filter;
        class EnergyCorrelator;
    }
    class LHCOReader;
    class ROOTReader;
    class DelphesTreeReader;
    class DelphesMA5tuneTreeReader;
    class DetectorDelphes;
    class DetectorDelphesMA5tune;
    class DelphesMemoryInterface;
    class SFSTaggerBase;

    /**
     * @brief Reconstructed jet.
     *
     * Tags come in four categories: truth (matching with B/C hadrons or hadronic taus,
     * before any detector effect) and the loose, medium and tight working points
     * (after the detector effects of the SFS tagger). For backward compatibility,
     * btag(), ctag() and tautag() return the loose tags. In FastJet mode, the
     * corresponding fastjet::PseudoJet is kept for substructure studies.
     */
    class RecJetFormat : public RecParticleFormat
    {

        friend class LHCOReader;
        friend class ROOTReader;
        friend class ClusterAlgoFastJet;
        friend class bTagger;
        friend class TauTagger;
        friend class cTagger;
        friend class SFSTaggerBase;
        friend class DetectorDelphes;
        friend class DetectorDelphesMA5tune;
        friend class DelphesTreeReader;
        friend class DelphesMA5tuneTreeReader;
        friend class DelphesMemoryInterface;

        // Substructure methods
        friend class Substructure::ClusterBase;
        friend class Substructure::Pruner;
        friend class Substructure::Nsubjettiness;
        friend class Substructure::SoftDrop;
        friend class Substructure::Filter;
        friend class Substructure::EnergyCorrelator;

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    protected:

        /** @brief Number of tracks. */
        // NOTE: this member shadows RecParticleFormat::ntracks_.
        MAuint16 ntracks_;   /// number of tracks

        /** @brief Loose b-, c- and tau-tags. */
        MAbool loose_btag_;        /// loose b-tag
        MAbool loose_ctag_;        /// loose c-tag
        MAbool loose_tautag_;      /// loose tau-tag

        /** @brief Medium b-, c- and tau-tags. */
        MAbool mid_btag_;        /// tight b-tag
        MAbool mid_ctag_;        /// tight c-tag
        MAbool mid_tautag_;      /// tight tau-tag

        /** @brief Tight b-, c- and tau-tags. */
        MAbool tight_btag_;        /// tight b-tag
        MAbool tight_ctag_;        /// tight c-tag
        MAbool tight_tautag_;      /// tight tau-tag

        /** @brief Truth-level c-, b- and tau-tags (before identification or misidentification). */
        MAbool true_ctag_;   /// c-tag (before id or misid)
        MAbool true_btag_;   /// b-tag (before id or misid)
        MAbool true_tautag_; /// tau-tag (before id or misid)

        /** @brief Indices of the constituents in the Monte Carlo particle collection. */
        std::vector<MAint32> Constituents_;  /// indices of the MC particles
        /** @brief Isolation cones. */
        std::vector<IsolationConeType> isolCones_; // isolation cones

#ifdef MA5_FASTJET_MODE
        // @Jack: Save the modified jet as pseudojet for jet substructure applications
    //        This will make it faster and avoid extra for loops.
    fastjet::PseudoJet pseudojet_;
#endif

        // -------------------------------------------------------------
        //                        method members
        // -------------------------------------------------------------
    public:

        /** @brief Constructor (members reset). */
        RecJetFormat()
        { clear(); }

        /**
         * @brief Constructor from (pT, eta, phi, m).
         *
         * @param pt transverse momentum.
         * @param eta pseudorapidity.
         * @param phi azimuthal angle.
         * @param m mass.
         */
        RecJetFormat(MAfloat64 pt, MAfloat64 eta, MAfloat64 phi, MAfloat64 m)
        { clear(); momentum_.SetPtEtaPhiM(pt,eta,phi,m); }

        /**
         * @brief Constructor from a four-vector.
         *
         * @param p four-momentum.
         */
        RecJetFormat(const MALorentzVector& p)
        { clear(); momentum_.SetPxPyPzE(p.Px(),p.Py(),p.Pz(),p.E()); }

#ifdef MA5_FASTJET_MODE
        /**
         * @brief Constructor from a FastJet jet (the pseudojet is kept).
         *
         * @param jet FastJet jet.
         */
        RecJetFormat(fastjet::PseudoJet& jet)
        {
            clear();
            momentum_.SetPxPyPzE(jet.px(), jet.py(), jet.pz(), jet.e());
            pseudojet_=jet;
        }
#endif

        /** @brief Destructor. */
        virtual ~RecJetFormat()
        {}

        /** @brief Print the jet properties. */
        virtual void Print() const
        {
            INFO << "ntracks ="   << /*set::setw(8)*/"" << std::left << ntracks_  << ", "
                 << "loose  btag   = " <<  std::left << loose_btag_ << ", "
                 << "loose  ctag   = " <<  std::left << loose_ctag_ << ", "
                 << "medium btag   = " <<  std::left << mid_btag_ << ", "
                 << "medium ctag   = " <<  std::left << mid_ctag_ << ", "
                 << "tight  btag   = " <<  std::left << tight_btag_ << ", "
                 << "tight  ctag   = " <<  std::left << tight_ctag_ << ", "
                 << "loose  tautag = " <<  std::left << loose_tautag_ << ", "
                 << "medium tautag = " <<  std::left << mid_tautag_ << ", "
                 << "tight  tautag = " <<  std::left << tight_tautag_ << ", ";
            RecParticleFormat::Print();
        }

        /** @brief Reset the jet-specific members. */
        virtual void Reset() { clear(); }

        /** @brief Reset the jet-specific members (the momentum is not reset). */
        void clear()
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

        /**
         * @brief Accessor to the number of tracks.
         *
         * @return the number of tracks.
         */
        const MAuint16 ntracks() const {return ntracks_;}

        /**
         * @brief Set the number of tracks.
         *
         * @param ntracks number of tracks.
         */
        void setNtracks(MAuint16 ntracks) { ntracks_ = ntracks; }

        ///==================///
        /// Tagger accessors ///
        ///==================///

        /**
         * @brief Accessor to the loose b-tag.
         *
         * @return the tag.
         */
        const MAbool& btag() const { return loose_btag(); }

        /**
         * @brief Accessor to the loose c-tag.
         *
         * @return the tag.
         */
        const MAbool& ctag() const { return loose_ctag(); }

        /**
         * @brief Accessor to the loose tau-tag.
         *
         * @return the tag.
         */
        const MAbool& tautag() const { return loose_tautag(); }

        /**
         * @brief Accessor to the loose b-tag.
         *
         * @return the tag.
         */
        const MAbool& loose_btag() const { return loose_btag_; }

        /**
         * @brief Accessor to the loose c-tag.
         *
         * @return the tag.
         */
        const MAbool& loose_ctag() const { return loose_ctag_; }

        /**
         * @brief Accessor to the loose tau-tag.
         *
         * @return the tag.
         */
        const MAbool& loose_tautag() const { return loose_tautag_; }

        /**
         * @brief Accessor to the medium b-tag.
         *
         * @return the tag.
         */
        const MAbool& mid_btag() const { return mid_btag_; }

        /**
         * @brief Accessor to the medium c-tag.
         *
         * @return the tag.
         */
        const MAbool& mid_ctag() const { return mid_ctag_; }

        /**
         * @brief Accessor to the medium tau-tag.
         *
         * @return the tag.
         */
        const MAbool& mid_tautag() const { return mid_tautag_; }

        /**
         * @brief Accessor to the tight b-tag.
         *
         * @return the tag.
         */
        const MAbool& tight_btag() const { return tight_btag_; }

        /**
         * @brief Accessor to the tight c-tag.
         *
         * @return the tag.
         */
        const MAbool& tight_ctag() const { return tight_ctag_; }

        /**
         * @brief Accessor to the tight tau-tag.
         *
         * @return the tag.
         */
        const MAbool& tight_tautag() const { return tight_tautag_; }

        /**
         * @brief Accessor to the truth-level c-tag.
         *
         * @return the tag.
         */
        const MAbool& true_ctag() const {return true_ctag_;}

        /**
         * @brief Accessor to the truth-level b-tag.
         *
         * @return the tag.
         */
        const MAbool& true_btag() const {return true_btag_;}

        /**
         * @brief Accessor to the truth-level tau-tag.
         *
         * @return the tag.
         */
        const MAbool& true_tautag() const {return true_tautag_;}

        /// Setters for tagger

        /**
         * @brief Set the truth-level b-tag.
         *
         * @param tag value.
         */
        void setTrueBtag(const MAbool& tag) { true_btag_ = tag;}

        /**
         * @brief Set the truth-level c-tag.
         *
         * @param tag value.
         */
        void setTrueCtag(const MAbool& tag) { true_ctag_ = tag;}

        /**
         * @brief Set the truth-level tau-tag.
         *
         * @param tag value.
         */
        void setTrueTautag(const MAbool& tag) { true_tautag_ = tag;}

        /**
         * @brief Set the loose b-tag.
         *
         * @param tag value.
         */
        void setBtag(const MAbool& tag) { setLooseBtag(tag); }

        /**
         * @brief Set the loose c-tag.
         *
         * @param tag value.
         */
        void setCtag(const MAbool& tag) { setLooseCtag(tag); }

        /**
         * @brief Set the loose tau-tag.
         *
         * @param tag value.
         */
        void setTautag(const MAbool& tag) { setLooseTautag(tag); }

        /**
         * @brief Set the loose b-tag.
         *
         * @param tag value.
         */
        void setLooseBtag(const MAbool& tag) { loose_btag_ = tag; }

        /**
         * @brief Set the loose c-tag.
         *
         * @param tag value.
         */
        void setLooseCtag(const MAbool& tag) { loose_ctag_ = tag; }

        /**
         * @brief Set the loose tau-tag.
         *
         * @param tag value.
         */
        void setLooseTautag(const MAbool& tag) { loose_tautag_ = tag; }

        /**
         * @brief Set the medium b-tag.
         *
         * @param tag value.
         */
        void setMidBtag(const MAbool& tag) { mid_btag_ = tag; }

        /**
         * @brief Set the medium c-tag.
         *
         * @param tag value.
         */
        void setMidCtag(const MAbool& tag) { mid_ctag_ = tag; }

        /**
         * @brief Set the medium tau-tag.
         *
         * @param tag value.
         */
        void setMidTautag(const MAbool& tag) { mid_tautag_ = tag; }

        /**
         * @brief Set the tight b-tag.
         *
         * @param tag value.
         */
        void setTightBtag(const MAbool& tag) { tight_btag_ = tag; }

        /**
         * @brief Set the tight c-tag.
         *
         * @param tag value.
         */
        void setTightCtag(const MAbool& tag) { tight_ctag_ = tag; }

        /**
         * @brief Set the tight tau-tag.
         *
         * @param tag value.
         */
        void setTightTautag(const MAbool& tag) { tight_tautag_ = tag; }

        /**
         * @brief Set the truth-level and all the working-point b-tags.
         *
         * @param tag value.
         */
        void setAllBtags(const MAbool &tag) {
            true_btag_ = tag;
            loose_btag_ = tag;
            mid_btag_ = tag;
            tight_btag_ = tag;
        }

        /**
         * @brief Set the truth-level and all the working-point c-tags.
         *
         * @param tag value.
         */
        void setAllCtags(const MAbool &tag) {
            true_ctag_ = tag;
            loose_ctag_ = tag;
            mid_ctag_ = tag;
            tight_ctag_ = tag;
        }

        /**
         * @brief Set the truth-level and all the working-point tau-tags.
         *
         * @param tag value.
         */
        void setAllTautags(const MAbool &tag) {
            true_tautag_ = tag;
            loose_tautag_ = tag;
            mid_tautag_ = tag;
            tight_tautag_ = tag;
        }

        /**
         * @brief Add a constituent.
         *
         * @param index index of the constituent in the Monte Carlo particle collection.
         */
        // FIXME: AddConstituent, constituents, AddIsolCone and isolCones are declared unconditionally but
        // defined in RecJetFormat.cpp only in FastJet mode (MA5_FASTJET_MODE).
        void AddConstituent (const MAint32& index);

        /**
         * @brief Accessor to the constituents.
         *
         * @return the indices of the constituents.
         */
        const std::vector<MAint32>& constituents() const;

        /**
         * @brief Add an isolation cone.
         *
         * @param cone cone.
         */
        void AddIsolCone (const IsolationConeType& cone);

        /**
         * @brief Accessor to the isolation cones.
         *
         * @return the cones.
         */
        const std::vector<IsolationConeType>& isolCones() const;

#ifdef MA5_FASTJET_MODE
    /**
     * @brief Accessor to the FastJet pseudojet.
     *
     * @return the pseudojet.
     */
    const fastjet::PseudoJet& pseudojet() const {return pseudojet_;}

    /**
     * @brief Exclusive subjets obtained with a distance cut (exclusive algorithms only).
     *
     * The subjets are allocated with new and must be deleted by the caller.
     *
     * @param dcut distance cut.
     * @return the subjets sorted in pT (empty with an error if not available).
     */
    std::vector<const RecJetFormat *> exclusive_subjets(MAfloat32 dcut) const;

    /**
     * @brief Exclusive subjets obtained by declustering down to a given number of subjets.
     *
     * The subjets are allocated with new and must be deleted by the caller.
     *
     * @param nsub number of subjets.
     * @return the subjets sorted in pT (empty with an error if not available).
     */
    std::vector<const RecJetFormat *> exclusive_subjets(MAint32 nsub) const;

    /**
     * @brief Does the pseudojet support exclusive subjets?
     *
     * @return true if it does.
     */
    MAbool has_exclusive_subjets() const;

    /**
     * @brief Set the FastJet pseudojet.
     *
     * @param v pseudojet.
     */
    void setPseudoJet (const fastjet::PseudoJet& v) {pseudojet_=v;}
    /**
     * @brief Set the FastJet pseudojet from a four-vector.
     *
     * @param v four-momentum.
     */
    void setPseudoJet (MALorentzVector& v) 
    {
        pseudojet_=fastjet::PseudoJet(v.Px(), v.Py(), v.Pz(), v.E());
    }
#endif
    };
}

#endif
