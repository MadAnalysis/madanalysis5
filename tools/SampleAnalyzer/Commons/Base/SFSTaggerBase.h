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
//  You should have const Received a copy of the GNU General Public License
//  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MADANALYSIS5_SFSTAGGERBASE_H
#define MADANALYSIS5_SFSTAGGERBASE_H

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"

namespace MA5 {

    /// Status of the tagger
    enum TaggerStatus {LOOSE, MID, TIGHT};

    struct SFSTaggerBaseOptions {
        /// @brief Tagging options
        ///
        /// @code btag_matching_deltaR : double @endcode
        /// matching deltaR is the threshold for maximum dR distance between a jet and B-hadron
        /// @code  btag_exclusive : bool @endcode
        /// exclusive algorithm is designed to dynamically find the jet that best matches to the B-hadron
        ///
        /// options starting with ctag and tautag are the ones corresponding to c-jet and tau-jet tagging
        ///
        /// @code taujtag_jetbased : bool@endcode
        /// States the nature of tau tagging proecedure. If false, the hadronic tau in the event history
        /// will be found and used as hadronic tau object. if True, this hadronic tau will be matched with
        /// a jet and the jet will be considered as tau jet.
        ///
        /// @warning DR matching for tau tagging is only available on jet based method
        MAfloat32 btag_matching_deltaR = 0.5;
        MAbool btag_exclusive = true;
        MAbool enable_ctagging = false;
        MAfloat32 ctag_matching_deltaR = 0.5;
        MAbool ctag_exclusive = true;
        MAfloat32 tautag_matching_deltaR = 0.5;
        MAbool tautag_exclusive = true;
        MAbool tautag_jetbased = false;
    };

    /// @brief Tagger base is designed to accommodate both truth and detector level B/C/Hadronic Tau tagging.
    ///
    /// Tagging efficiencies are designed to redefine in new_tagger.h/cpp files locally for the analysis
    /// and used during tagging. If not defined, by default truth level tagging will be applied.

    class SFSTaggerBase {
    private:
        /// Code efficiency booleans
        MAbool _isJetTaggingOn, _isTauTaggingEffOn, _isMuonTaggingOn, _isElectronTaggingOn, _isPhotonTaggingOn;
        SFSTaggerBaseOptions _options;

    public:
        /// Constructor without argument
        SFSTaggerBase() {}

        /// Destructor
        virtual ~SFSTaggerBase() {}

        virtual void Initialize()
        {
            /// @brief Booleans for code efficiency
            /// Turn on the usage of tau tagging efficiency
            _isTauTaggingEffOn = false;
            /// Turn on the usage of jet (mis)tagging efficiency
            _isJetTaggingOn = false;
            /// Turn on the usage of muon (mis)tagging efficiency
            _isMuonTaggingOn = false;
            /// Turn on the usage of electron (mis)tagging efficiency
            _isElectronTaggingOn = false;
            /// Turn on the usage of photon (mis)tagging efficiency
            _isPhotonTaggingOn = false;
        }

        /// @brief Execution: execute truth and detector level tagging algorithm
        void Execute(EventFormat& myEvent) const;

        /// Initialize options
        void SetOptions(SFSTaggerBaseOptions &opt) { _options = opt; }

        /// Print parameters
        void PrintParam() const;

        /// Accesor to options
        SFSTaggerBaseOptions options() const { return _options; }

        /// Truth B-Jet tagging
        void BJetTagging(EventFormat& myEvent) const;

        /// Truth C-Jet tagging
        void CJetTagging(EventFormat& myEvent) const;

        /// Hadronic tau tagging: this method implements both truth and detector level tagging simultaneously
        void JetBasedTauTagging(EventFormat& myEvent) const;

        /// Convert Jet object to tau object
        void Jet2Tau(const RecJetFormat * myJet, RecTauFormat *myTau, EventFormat &myEvent) const;

        ///==========================================//
        ///                                          //
        ///          Tagging Efficiencies            //
        ///                                          //
        ///==========================================//

        /// @brief By default, all efficiencies are initialised as perfect detector

        ///===============//
        ///   B-Tagging   //
        ///===============//

        /// loose B-jet tagging efficiency (b as b)
        virtual MAfloat32 loose_b_tagging_eff(const RecJetFormat &object) const { return 2.; }

        /// loose B-jet mistagging as C-jet (b as c)
        virtual MAfloat32 loose_b_mistag_c(const RecJetFormat &object) const { return -1.; }

        /// mid B-jet tagging efficiency (b as b)
        virtual MAfloat32 mid_b_tagging_eff(const RecJetFormat &object) const { return 2.; }

        /// loose B-jet mistagging as C-jet (b as c)
        virtual MAfloat32 mid_b_mistag_c(const RecJetFormat &object) const { return -1.; }

        /// tight B-jet tagging efficiency (b as b)
        virtual MAfloat32 tight_b_tagging_eff(const RecJetFormat &object) const { return 2.; }

        /// loose B-jet mistagging as C-jet (b as c)
        virtual MAfloat32 tight_b_mistag_c(const RecJetFormat &object) const { return -1.; }

        //===============//
        //   C-Tagging   //
        //===============//

        /// loose C-jet tagging efficiency (c as c)
        virtual MAfloat32 loose_c_tagging_eff(const RecJetFormat &object) const { return 2.; }

        /// loose C-jet mistagging as C-jet (c as b)
        virtual MAfloat32 loose_c_mistag_b(const RecJetFormat &object) const { return -1.; }

        /// mid C-jet tagging efficiency (c as c)
        virtual MAfloat32 mid_c_tagging_eff(const RecJetFormat &object) const { return 2.; }

        /// mid C-jet mistagging as C-jet (c as b)
        virtual MAfloat32 mid_c_mistag_b(const RecJetFormat &object) const { return -1.; }

        /// tight C-jet tagging efficiency (c as c)
        virtual MAfloat32 tight_c_tagging_eff(const RecJetFormat &object) const { return 2.; }

        /// tight C-jet mistagging as C-jet (c as b)
        virtual MAfloat32 tight_c_mistag_b(const RecJetFormat &object) const { return -1.; }

        //=======================//
        //   Light-Jet Tagging   //
        //=======================//

        /// loose Light-Jet mistagging as b-jet (j as b)
        virtual MAfloat32 lightjet_mistag_b_loose(const RecJetFormat &object) const { return -1.; }

        /// loose Light-Jet mistagging as c jet (j as c)
        virtual MAfloat32 lightjet_mistag_c_loose(const RecJetFormat &object) const { return -1.; }

        /// loose Light-Jet mistagging as tau (j as ta)
        virtual MAfloat32 lightjet_mistag_tau_loose(const RecJetFormat &object) const { return -1.; }

        /// mid Light-Jet mistagging as b-jet (j as b)
        virtual MAfloat32 lightjet_mistag_b_mid(const RecJetFormat &object) const { return -1.; }

        /// mid Light-Jet mistagging as c jet (j as c)
        virtual MAfloat32 lightjet_mistag_c_mid(const RecJetFormat &object) const { return -1.; }

        /// mid Light-Jet mistagging as tau (j as ta)
        virtual MAfloat32 lightjet_mistag_tau_mid(const RecJetFormat &object) const { return -1.; }

        /// tight Light-Jet mistagging as b-jet (j as b)
        virtual MAfloat32 lightjet_mistag_b_tight(const RecJetFormat &object) const { return -1.; }

        /// tight Light-Jet mistagging as c jet (j as c)
        virtual MAfloat32 lightjet_mistag_c_tight(const RecJetFormat &object) const { return -1.; }

        /// tight Light-Jet mistagging as tau (j as ta)
        virtual MAfloat32 lightjet_mistag_tau_tight(const RecJetFormat &object) const { return -1.; }

        /// Light-Jet mistagging as electron (j as e)
        virtual MAfloat32 lightjet_mistag_electron(const RecJetFormat &object) const { return -1.; }

        /// Light-Jet mistagging as photon (j as photon)
        virtual MAfloat32 lightjet_mistag_photon(const RecJetFormat &object) const { return -1.; }

        //=================//
        //   Tau Tagging   //
        //=================//

        /// Convert jet to tau
        MAfloat32 tau_tagging_eff(const RecJetFormat &jet, TaggerStatus status) const;

        /// loose_Tau tagging efficiency (ta as ta)
        virtual MAfloat32 loose_tau_tagging_eff(const RecTauFormat &object) const { return 2.; }

        /// mid_Tau tagging efficiency (ta as ta)
        virtual MAfloat32 mid_tau_tagging_eff(const RecTauFormat &object) const { return 2.; }

        /// tight_Tau tagging efficiency (ta as ta)
        virtual MAfloat32 tight_tau_tagging_eff(const RecTauFormat &object) const { return 2.; }

        //=======================//
        //   Electron Tagging   //
        //======================//

        /// Electron mistagging as muon (e as mu)
        virtual MAfloat32 electron_mistag_muon(const RecLeptonFormat &object) const { return -1.; }

        /// Electron mistagging as photon (e as a)
        virtual MAfloat32 electron_mistag_photon(const RecLeptonFormat &object) const { return -1.; }

        /// Electron mistagging as light jet (e as j)
        virtual MAfloat32 electron_mistag_lightjet(const RecLeptonFormat &object) const { return -1.; }

        //==================//
        //   Muon Tagging   //
        //==================//

        /// Electron mistagging as electron (mu as e)
        virtual MAfloat32 muon_mistag_electron(const RecLeptonFormat &object) const { return -1.; }

        /// Electron mistagging as photon (mu as a)
        virtual MAfloat32 muon_mistag_photon(const RecLeptonFormat &object) const { return -1.; }

        /// Electron mistagging as light jet (mu as j)
        virtual MAfloat32 muon_mistag_lightjet(const RecLeptonFormat &object) const { return -1.; }

        //====================//
        //   Photon Tagging   //
        //====================//

        /// Electron mistagging as electron (a as e)
        virtual MAfloat32 photon_mistag_electron(const RecPhotonFormat &object) const { return -1.; }

        /// Electron mistagging as muon (a as mu)
        virtual MAfloat32 photon_mistag_muon(const RecPhotonFormat &object) const { return -1.; }

        /// Electron mistagging as light jet (a as j)
        virtual MAfloat32 photon_mistag_lightjet(const RecPhotonFormat &object) const { return -1.; }
    };
}

#endif //MADANALYSIS5_SFSTAGGERBASE_H
