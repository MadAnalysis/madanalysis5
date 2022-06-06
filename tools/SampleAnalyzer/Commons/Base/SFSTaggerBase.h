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
    class SFSTaggerBase {
    public:
        /// Constructor without argument
        SFSTaggerBase() {}

        /// Destructor
        virtual ~SFSTaggerBase() {}

        // Execution
        void Execute(EventFormat& myEvent);

        //===============//
        //   B-Tagging   //
        //===============//

        // B-jet tagging efficiency
        virtual MAfloat32 b_tagging_eff(const RecJetFormat *jet) { return 1.; }

        // B-jet mistagging as C-jet
        virtual MAfloat32 b_mistag_c(const RecJetFormat *jet) { return 0.; }

        // B-jet mistagging as light jet
        virtual MAfloat32 b_mistag_lightjet(const RecJetFormat *jet) { return 0.; }

        //===============//
        //   C-Tagging   //
        //===============//

        // C-jet tagging efficiency
        virtual MAfloat32 c_tagging_eff(const RecJetFormat *jet) { return 1.; }

        // C-jet mistagging as C-jet
        virtual MAfloat32 c_mistag_b(const RecJetFormat *jet) { return 0.; }

        // C-jet mistagging as light jet
        virtual MAfloat32 c_mistag_lightjet(const RecJetFormat *jet) { return 0.; }

        //=======================//
        //   Light-Jet Tagging   //
        //=======================//

        // Light-Jet mistagging as b-jet
        virtual MAfloat32 lightjet_mistag_b(const RecJetFormat *jet) { return 0.; }

        // Light-Jet mistagging as c jet
        virtual MAfloat32 lightjet_mistag_c(const RecJetFormat *jet) { return 0.; }

        // Light-Jet mistagging as tau
        virtual MAfloat32 lightjet_mistag_tau(const RecJetFormat *jet) { return 0.; }

        // Light-Jet mistagging as electron
        virtual MAfloat32 lightjet_mistag_electron(const RecJetFormat *jet) { return 0.; }

        // Light-Jet mistagging as muon
        virtual MAfloat32 lightjet_mistag_muon(const RecJetFormat *jet) { return 0.; }

        // Light-Jet mistagging as photon
        virtual MAfloat32 lightjet_mistag_photon(const RecJetFormat *jet) { return 0.; }

        //=================//
        //   Tau Tagging   //
        //=================//

        // Electron mistagging as muon
        virtual MAfloat32 tau_mistag_lightjet(const RecTauFormat *tau) { return 0.; }
        
        //=======================//
        //   Electron Tagging   //
        //======================//

        // Electron mistagging as muon
        virtual MAfloat32 electron_mistag_muon(const RecLeptonFormat *electron) { return 0.; }

        // Electron mistagging as photon
        virtual MAfloat32 electron_mistag_photon(const RecLeptonFormat *electron) { return 0.; }

        //==================//
        //   Muon Tagging   //
        //==================//

        // Electron mistagging as electron
        virtual MAfloat32 muon_mistag_electron(const RecLeptonFormat *muon) { return 0.; }

        // Electron mistagging as photon
        virtual MAfloat32 muon_mistag_photon(const RecLeptonFormat *muon) { return 0.; }

        //====================//
        //   Photon Tagging   //
        //====================//

        // Electron mistagging as electron
        virtual MAfloat32 photon_mistag_electron(const RecPhotonFormat *photon) { return 0.; }

        // Electron mistagging as muon
        virtual MAfloat32 photon_mistag_muon(const RecPhotonFormat *photon) { return 0.; }
    };
}

#endif //MADANALYSIS5_SFSTAGGERBASE_H
