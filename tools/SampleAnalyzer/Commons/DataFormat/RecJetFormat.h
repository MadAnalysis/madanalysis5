////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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

        MAuint16 ntracks_;   /// number of tracks

        /// Tags are separated into 4 cathegories truth, loose, mid, tight.
        /// Truth is used for MC event tagging rest is from detector simulations.
        /// Loose tagging will be used as default for backwards compatibility btag()
        /// ctag(), tautag() will return loose tagging criteria.
        MAbool loose_btag_;        /// loose b-tag
        MAbool loose_ctag_;        /// loose c-tag
        MAbool loose_tautag_;      /// loose tau-tag

        MAbool mid_btag_;        /// tight b-tag
        MAbool mid_ctag_;        /// tight c-tag
        MAbool mid_tautag_;      /// tight tau-tag

        MAbool tight_btag_;        /// tight b-tag
        MAbool tight_ctag_;        /// tight c-tag
        MAbool tight_tautag_;      /// tight tau-tag

        MAbool true_ctag_;   /// c-tag (before id or misid)
        MAbool true_btag_;   /// b-tag (before id or misid)
        MAbool true_tautag_; /// tau-tag (before id or misid)

        std::vector<MAint32> Constituents_;  /// indices of the MC particles
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

        /// Constructor without arguments
        RecJetFormat()
        { clear(); }

        /// Constructor with argument
        RecJetFormat(MAfloat64 pt, MAfloat64 eta, MAfloat64 phi, MAfloat64 m)
        { clear(); momentum_.SetPtEtaPhiM(pt,eta,phi,m); }

        /// Constructor with argument
        RecJetFormat(const MALorentzVector& p)
        { clear(); momentum_.SetPxPyPzE(p.Px(),p.Py(),p.Pz(),p.E()); }

#ifdef MA5_FASTJET_MODE
        /// Constructor with argument
        RecJetFormat(fastjet::PseudoJet& jet)
        {
            clear();
            momentum_.SetPxPyPzE(jet.px(), jet.py(), jet.pz(), jet.e());
            pseudojet_=jet;
        }
#endif

        /// Destructor
        virtual ~RecJetFormat()
        {}

        /// Dump information
        virtual void Print() const
        {
            INFO << "ntracks ="   << /*set::setw(8)*/"" << std::left << ntracks_  << ", "
                 << "btag = " << /*set::setw(8)*/"" << std::left << loose_btag_ << ", "
                 << "ctag = " << /*set::setw(8)*/"" << std::left << loose_ctag_ << ", ";
            RecParticleFormat::Print();
        }

        /// Clear all information
        virtual void Reset() { clear(); }

        /// Clear all information
        void clear();

        /// Accessor to the number of tracks
        const MAuint16 ntracks() const;

        /// Setting ntracks
        void setNtracks(MAuint16 ntracks);

        /// Tagger options

        /// Accessor to the loose b-tag
        const MAbool& btag() const;

        /// Accessor to the loose c-tag
        const MAbool& ctag() const;

        /// Accessor to the loose c-tag
        const MAbool& tautag() const;

        /// Accessor to the loose b-tag
        const MAbool& loose_btag() const;

        /// Accessor to the loose c-tag
        const MAbool& loose_ctag() const;

        /// Accessor to the loose c-tag
        const MAbool& loose_tautag() const;

        /// Accessor to the mid b-tag
        const MAbool& mid_btag() const;

        /// Accessor to the mid c-tag
        const MAbool& mid_ctag() const;

        /// Accessor to the mid c-tag
        const MAbool& mid_tautag() const;

        /// Accessor to the tight b-tag
        const MAbool& tight_btag() const;

        /// Accessor to the tight c-tag
        const MAbool& tight_ctag() const;

        /// Accessor to the tight c-tag
        const MAbool& tight_tautag() const;

        /// Accessor to the true c-tag
        const MAbool& true_ctag() const;

        /// Accessor to the true b-tag
        const MAbool& true_btag() const;

        /// Accessor to the true tau-tag
        const MAbool& true_tautag() const;

        /// Setting a new true_btag_ value
        void setTrueBtag(const MAbool& tag);

        /// Setting a new true_ctag_ value
        void setTrueCtag(const MAbool& tag);

        /// Setting a new true_tautag_ value
        void setTrueTautag(const MAbool& tag);

        /// Setting a new loose_btag_ value
        void setBtag(const MAbool& tag);

        /// Setting a new loose_ctag_ value
        void setCtag(const MAbool& tag);

        /// Setting a new loose_tautag_ value
        void setTautag(const MAbool& tag);

        /// Setting a new loose_btag_ value
        void setLooseBtag(const MAbool& tag);

        /// Setting a new loose_ctag_ value
        void setLooseCtag(const MAbool& tag);

        /// Setting a new loose_tautag_ value
        void setLooseTautag(const MAbool& tag);

        /// Setting a new Mid_btag_ value
        void setMidBtag(const MAbool& tag);

        /// Setting a new Mid_ctag_ value
        void setMidCtag(const MAbool& tag);

        /// Setting a new Mid_tautag_ value
        void setMidTautag(const MAbool& tag);

        /// Setting a new Tight_btag_ value
        void setTightBtag(const MAbool& tag);

        /// Setting a new Tight_ctag_ value
        void setTightCtag(const MAbool& tag);

        /// Setting a new Tight_tautag_ value
        void setTightTautag(const MAbool& tag);

        /// Set all b-tags
        void setAllBtags(const MAbool& tag);

        /// Set all c-tags
        void setAllCtags(const MAbool& tag);

        /// Set all tau-tags
        void setAllTautags(const MAbool& tag);

        /// Add one constituent
        void AddConstituent (const MAint32& index);

        /// get constituent collections
        const std::vector<MAint32>& constituents() const;

        /// Add one isolation cone
        void AddIsolCone (const IsolationConeType& cone);

        /// get the collection of isolation cones
        const std::vector<IsolationConeType>& isolCones() const;

#ifdef MA5_FASTJET_MODE
     // Accessor for pseudojets
    const fastjet::PseudoJet& pseudojet() const {return pseudojet_;}

    // return a vector of all subjets of the current jet (in the sense of the exclusive algorithm)
    // that would be obtained when running the algorithm with the given dcut.
    std::vector<const RecJetFormat *> exclusive_subjets(MAfloat32 dcut) const;

    // return the list of subjets obtained by unclustering the supplied jet down to nsub subjets.
    std::vector<const RecJetFormat *> exclusive_subjets(MAint32 nsub) const;

    //returns true if the PseudoJet has support for exclusive subjets
    MAbool has_exclusive_subjets() const;

    // Add one pseudojet
    void setPseudoJet (const fastjet::PseudoJet& v) {pseudojet_=v;}
    void setPseudoJet (MALorentzVector& v) 
    {
        pseudojet_=fastjet::PseudoJet(v.Px(), v.Py(), v.Pz(), v.E());
    }
#endif
    };
}

#endif
