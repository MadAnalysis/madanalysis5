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
        MAfloat32 btag_matching_deltaR = 0.3;
        MAbool btag_exclusive = true;
        MAfloat32 ctag_matching_deltaR = 0.3;
        MAbool ctag_exclusive = true;
        MAfloat32 tautag_matching_deltaR = 0.3;
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

        /// B-jet tagging efficiency (b as b)
        virtual MAfloat32 b_tagging_eff(const RecJetFormat &jet) const { return 1.; }

        /// B-jet mistagging as C-jet (b as c)
        virtual MAfloat32 b_mistag_c(const RecJetFormat &jet) const { return 0.; }

        //===============//
        //   C-Tagging   //
        //===============//

        /// C-jet tagging efficiency (c as c)
        virtual MAfloat32 c_tagging_eff(const RecJetFormat &jet) const { return 1.; }

        /// C-jet mistagging as C-jet (c as b)
        virtual MAfloat32 c_mistag_b(const RecJetFormat &jet) const { return 0.; }

        //=======================//
        //   Light-Jet Tagging   //
        //=======================//

        /// Light-Jet mistagging as b-jet (j as b)
        virtual MAfloat32 lightjet_mistag_b(const RecJetFormat &jet) const { return 0.; }

        /// Light-Jet mistagging as c jet (j as c)
        virtual MAfloat32 lightjet_mistag_c(const RecJetFormat &jet) const { return 0.; }

        /// Light-Jet mistagging as tau (j as ta)
        virtual MAfloat32 lightjet_mistag_tau(const RecJetFormat &jet) const { return 0.; }

        /// Light-Jet mistagging as electron (j as e)
        virtual MAfloat32 lightjet_mistag_electron(const RecJetFormat &jet) const { return 0.; }

        /// Light-Jet mistagging as photon (j as photon)
        virtual MAfloat32 lightjet_mistag_photon(const RecJetFormat &jet) const { return 0.; }

        //=================//
        //   Tau Tagging   //
        //=================//

        /// Convert jet to tau
        MAfloat32 tau_tagging_eff(const RecJetFormat &jet) const
        {
            RecTauFormat myTau;
            myTau.setMomentum(jet.momentum());
            myTau.ntracks_   = jet.ntracks();
            myTau.mc_        = jet.mc_;

            return tau_tagging_eff(myTau);
        }

        /// Tau tagging efficiency (ta as ta)
        virtual MAfloat32 tau_tagging_eff(const RecTauFormat &tau) const { return 1.; }

        //=======================//
        //   Electron Tagging   //
        //======================//

        /// Electron mistagging as muon (e as mu)
        virtual MAfloat32 electron_mistag_muon(const RecLeptonFormat &electron) const { return 0.; }

        /// Electron mistagging as photon (e as a)
        virtual MAfloat32 electron_mistag_photon(const RecLeptonFormat &electron) const { return 0.; }

        /// Electron mistagging as light jet (e as j)
        virtual MAfloat32 electron_mistag_lightjet(const RecLeptonFormat &electron) const { return 0.; }

        //==================//
        //   Muon Tagging   //
        //==================//

        /// Electron mistagging as electron (mu as e)
        virtual MAfloat32 muon_mistag_electron(const RecLeptonFormat &muon) const { return 0.; }

        /// Electron mistagging as photon (mu as a)
        virtual MAfloat32 muon_mistag_photon(const RecLeptonFormat &muon) const { return 0.; }

        /// Electron mistagging as light jet (mu as j)
        virtual MAfloat32 muon_mistag_lightjet(const RecLeptonFormat &muon) const { return 0.; }

        //====================//
        //   Photon Tagging   //
        //====================//

        /// Electron mistagging as electron (a as e)
        virtual MAfloat32 photon_mistag_electron(const RecPhotonFormat &photon) const { return 0.; }

        /// Electron mistagging as muon (a as mu)
        virtual MAfloat32 photon_mistag_muon(const RecPhotonFormat &photon) const { return 0.; }

        /// Electron mistagging as light jet (a as j)
        virtual MAfloat32 photon_mistag_lightjet(const RecPhotonFormat &photon) const { return 0.; }
    };
}

#endif //MADANALYSIS5_SFSTAGGERBASE_H
