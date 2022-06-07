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
#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecTauFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecPhotonFormat.h"

namespace MA5 {

    struct SFSTaggerBaseOptions {
        // btag includes bjet tagging options
        // matching deltaR is the threshold for maximum dR distance between a jet and B-hadron
        // exclusive algorithm is designed to dynamically find the jet that best matches to the B-hadron
        // options starting with ctag and tautag are the ones corresponding to c-jet and tau-jet tagging
        // taujtag_jetbased can be either hadron based or jet based. DR matching for tau tagging is
        // only available on jet based method
        MAfloat32 btag_matching_deltaR = 0.3;
        MAbool btag_exclusive = true;
        MAfloat32 ctag_matching_deltaR = 0.3;
        MAbool ctag_exclusive = true;
        MAfloat32 tautag_matching_deltaR = 0.3;
        MAbool tautag_jetbased = false;
    };


    class SFSTaggerBase {
    private:
        MAbool _isTaggerOn;
        MAbool _isJetTaggingOn;
        SFSTaggerBaseOptions _options;

    public:
        /// Constructor without argument
        SFSTaggerBase() {}

        /// Destructor
        virtual ~SFSTaggerBase() {}

        virtual void Initialize()
        {
            _isTaggerOn = false;
            _isJetTaggingOn = false;
        }

        // Execution
        void Execute(EventFormat& myEvent);

        // Initialize options
        void SetOptions(SFSTaggerBaseOptions &opt) { _options = opt; }

        // Accesor to options
        SFSTaggerBaseOptions options() const { return _options; }

        void BJetTagging(EventFormat& myEvent) const;
        void CJetTagging(EventFormat& myEvent) const;
        void TauTagging(EventFormat& myEvent) const;
        void Jet2Tau(const RecJetFormat * myJet, RecTauFormat *myTau, EventFormat &myEvent) const;
        void Tau2Jet(RecJetFormat * myJet, const RecTauFormat *myTau, EventFormat &myEvent) const;

        //<><><><><><><><><><><><><><><><><><><><><>//
        //                                          //
        //          Tagging Efficiencies            //
        //                                          //
        //<><><><><><><><><><><><><><><><><><><><><>//

        // By default, all efficiencies are initialised as perfect detector

        //===============//
        //   B-Tagging   //
        //===============//

        // B-jet tagging efficiency
        virtual MAfloat32 b_tagging_eff(const RecJetFormat jet) const { return 1.; }

        // B-jet mistagging as C-jet
        virtual MAfloat32 b_mistag_c(const RecJetFormat jet) const { return 0.; }

        // B-jet mistagging as light jet
        virtual MAfloat32 b_mistag_lightjet(const RecJetFormat jet) const { return 0.; }

        //===============//
        //   C-Tagging   //
        //===============//

        // C-jet tagging efficiency
        virtual MAfloat32 c_tagging_eff(const RecJetFormat jet) const { return 1.; }

        // C-jet mistagging as C-jet
        virtual MAfloat32 c_mistag_b(const RecJetFormat jet) const { return 0.; }

        // C-jet mistagging as light jet
        virtual MAfloat32 c_mistag_lightjet(const RecJetFormat jet) const { return 0.; }

        //=======================//
        //   Light-Jet Tagging   //
        //=======================//

        // Light-Jet mistagging as b-jet
        virtual MAfloat32 lightjet_mistag_b(const RecJetFormat jet) const { return 0.; }

        // Light-Jet mistagging as c jet
        virtual MAfloat32 lightjet_mistag_c(const RecJetFormat jet) const { return 0.; }

        // Light-Jet mistagging as tau
        virtual MAfloat32 lightjet_mistag_tau(const RecJetFormat jet) const { return 0.; }

        // Light-Jet mistagging as electron
        virtual MAfloat32 lightjet_mistag_electron(const RecJetFormat jet) const { return 0.; }

        // Light-Jet mistagging as muon
        virtual MAfloat32 lightjet_mistag_muon(const RecJetFormat jet) const { return 0.; }

        // Light-Jet mistagging as photon
        virtual MAfloat32 lightjet_mistag_photon(const RecJetFormat jet) const { return 0.; }

        //=================//
        //   Tau Tagging   //
        //=================//

        // Tau tagging efficiency
        virtual MAfloat32 tau_tagging_eff(const RecJetFormat* jet) const
        {
            RecTauFormat * myTau;
            return tau_tagging_eff(Jet2Tau(jet, myTau))
        }
        virtual MAfloat32 tau_tagging_eff(const RecTauFormat tau) const { return 1.; }

        // Tau mistagging as lightjet
        virtual MAfloat32 tau_mistag_lightjet(const RecTauFormat tau) const { return 0.; }
        
        //=======================//
        //   Electron Tagging   //
        //======================//

        // Electron mistagging as muon
        virtual MAfloat32 electron_mistag_muon(const RecLeptonFormat electron) const { return 0.; }

        // Electron mistagging as photon
        virtual MAfloat32 electron_mistag_photon(const RecLeptonFormat electron) const { return 0.; }

        //==================//
        //   Muon Tagging   //
        //==================//

        // Electron mistagging as electron
        virtual MAfloat32 muon_mistag_electron(const RecLeptonFormat muon) const { return 0.; }

        // Electron mistagging as photon
        virtual MAfloat32 muon_mistag_photon(const RecLeptonFormat muon) const { return 0.; }

        //====================//
        //   Photon Tagging   //
        //====================//

        // Electron mistagging as electron
        virtual MAfloat32 photon_mistag_electron(const RecPhotonFormat photon) const { return 0.; }

        // Electron mistagging as muon
        virtual MAfloat32 photon_mistag_muon(const RecPhotonFormat photon) const { return 0.; }
    };
}

#endif //MADANALYSIS5_SFSTAGGERBASE_H
