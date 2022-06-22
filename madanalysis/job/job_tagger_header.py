################################################################################
#
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#
#  This file is part of MadAnalysis 5.
#  Official website: <https://github.com/MadAnalysis/madanalysis5>
#
#  MadAnalysis 5 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MadAnalysis 5 is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
#
################################################################################

from madanalysis.fastsim.tagger import TaggerStatus

class JobTaggerHeader:

    # structure: (true_id, reco_id) : "..."
    base = {
        (5, 5): lambda tag: f"/// B-jet tagging efficiency (b as b)\nMAfloat32 NewTagger::{tag}_b_tagging_eff(const RecJetFormat &object) const",
        (5, 4): lambda tag: f"/// B-jet mistagging as C-jet (b as c)\nMAfloat32 NewTagger::{tag}_b_mistag_c(const RecJetFormat &object) const",
        (4, 4): lambda tag: f"/// C-jet tagging efficiency (c as c)\nMAfloat32 NewTagger::{tag}_c_tagging_eff(const RecJetFormat &object) const",
        (4, 5): lambda tag: f"/// C-jet mistagging as C-jet (c as b)\nMAfloat32 NewTagger::{tag}_c_mistag_b(const RecJetFormat &object) const",
        (21, 5): lambda tag: f"/// Light-Jet mistagging as b-jet (j as b)\nMAfloat32 NewTagger::lightjet_mistag_b_{tag}(const RecJetFormat &object) const",
        (21, 4): lambda tag: f"/// Light-Jet mistagging as c jet (j as c)\nMAfloat32 NewTagger::lightjet_mistag_c_{tag}(const RecJetFormat &object) const",
        (21, 15): lambda tag: f"/// Light-Jet mistagging as tau (j as ta)\nMAfloat32 NewTagger::lightjet_mistag_tau_{tag}(const RecJetFormat &object) const",
        (21, 11): lambda tag: f"/// Light-Jet mistagging as electron (j as e)\nMAfloat32 NewTagger::lightjet_mistag_electron(const RecJetFormat &object) const",
        (21, 22): lambda tag: f"/// Light-Jet mistagging as photon (j as photon)\nMAfloat32 NewTagger::lightjet_mistag_photon(const RecJetFormat &object) const",
        (15, 15): lambda tag: f"/// Tau tagging efficiency (ta as ta)\nMAfloat32 NewTagger::{tag}_tau_tagging_eff(const RecTauFormat &object) const",
        (11, 13): lambda tag: f"/// Electron mistagging as muon (e as mu)\nMAfloat32 NewTagger::electron_mistag_muon(const RecLeptonFormat &object) const",
        (11, 22): lambda tag: f"/// Electron mistagging as photon (e as a)\nMAfloat32 NewTagger::electron_mistag_photon(const RecLeptonFormat &object) const",
        (11, 21): lambda tag: f"/// Electron mistagging as light jet (e as j)\nMAfloat32 NewTagger::electron_mistag_lightjet(const RecLeptonFormat &object) const",
        (13, 11): lambda tag: f"/// Electron mistagging as electron (mu as e)\nMAfloat32 NewTagger::muon_mistag_electron(const RecLeptonFormat &object) const",
        (13, 22): lambda tag: f"/// Electron mistagging as photon (mu as a)\nMAfloat32 NewTagger::muon_mistag_photon(const RecLeptonFormat &object) const",
        (13, 21): lambda tag: f"/// Electron mistagging as light jet (mu as j)\nMAfloat32 NewTagger::muon_mistag_lightjet(const RecLeptonFormat &object) const",
        (22, 11): lambda tag: f"/// Electron mistagging as electron (a as e)\nMAfloat32 NewTagger::photon_mistag_electron(const RecPhotonFormat &object) const",
        (22, 13): lambda tag: f"/// Electron mistagging as muon (a as mu)\nMAfloat32 NewTagger::photon_mistag_muon(const RecPhotonFormat &object) const",
        (22, 21): lambda tag: f"/// Electron mistagging as light jet (a as j)\nMAfloat32 NewTagger::photon_mistag_lightjet(const RecPhotonFormat &object) const",
    }


    ## Initialization
    def __init__(self, fastsim):
        self.fastsim = fastsim

        self.unique_rules = []
        for key, rule in self.fastsim.tagger.rules.items():
            if (int(rule["id_true"]), int(rule["id_reco"]), TaggerStatus.to_str(rule["tag"])) not in self.unique_rules:
                self.unique_rules.append((int(rule["id_true"]), int(rule["id_reco"]), TaggerStatus.to_str(rule["tag"])))


    ## Writing NewTagger.h
    def WriteNewTaggerHeader(self, file):
        file.write('#ifndef MADANALYSIS5_NEW_TAGGER_H\n')
        file.write('#define MADANALYSIS5_NEW_TAGGER_H\n')
        file.write('// SampleAnalyzer headers\n')
        file.write('#include "SampleAnalyzer/Commons/Base/SFSTaggerBase.h"\n')
        file.write('namespace MA5 {\n')
        file.write('    class NewTagger: public SFSTaggerBase {\n')
        file.write('    private:\n')
        file.write('        /// Code efficiency booleans\n')
        file.write('        MAbool _isTauTaggingEffOn, _isMuonTaggingOn, _isElectronTaggingOn, _isPhotonTaggingOn;\n')
        file.write('        SFSTaggerBaseOptions _options;\n')
        file.write('    public :\n')
        file.write('        /// Constructor without argument\n')
        file.write('        NewTagger() {}\n\n')
        file.write('        /// Destructor\n')
        file.write('        virtual ~NewTagger() {}\n\n')
        file.write('        void Initialize() {\n')
        file.write('            /// @brief Booleans for code efficiency\n')
        file.write('            /// Turn on the usage of tau tagging efficiency\n')
        file.write(f"            _isTauTaggingEffOn = {'true' if (5,5) in [(x[0], x[1]) for x in self.unique_rules] else 'false'};\n")
        # file.write('            /// Turn on the usage of jet (mis)tagging efficiency\n')
        # file.write(f"            _isJetTaggingOn = {'true' if any([x in self.unique_rules for x in [(5,5), (4,5), (21,5), (5,4), (4,4), (21,4), (21,15), (21,11), (21,22)]]) else 'false'};\n")
        file.write('            /// Turn on the usage of muon (mis)tagging efficiency\n')
        file.write(f"            _isMuonTaggingOn = {'true' if 13 in [x[0] for x in self.unique_rules] else 'false'};\n")
        file.write('            /// Turn on the usage of electron (mis)tagging efficiency\n')
        file.write(f"            _isElectronTaggingOn = {'true' if 11 in [x[0] for x in self.unique_rules] else 'false'};\n")
        file.write('            /// Turn on the usage of photon (mis)tagging efficiency\n')
        file.write(f"            _isPhotonTaggingOn = {'true' if 22 in [x[0] for x in self.unique_rules] else 'false'};\n")
        file.write('         }\n\n')
        file.write('        ///==========================================//\n')
        file.write('        ///                                          //\n')
        file.write('        ///          Tagging Efficiencies            //\n')
        file.write('        ///                                          //\n')
        file.write('        ///==========================================//\n\n')
        for rule in self.unique_rules:
            header = JobTaggerHeader.base[(rule[0], rule[1])](rule[2]).split("\n")
            file.write(f"\t{header[0]}\n")
            file.write(f"\t{header[1].replace('NewTagger::', '')};\n")
        file.write('    };\n}\n#endif //MADANALYSIS5_NEW_TAGGER_H')


    ## efficiencies and bounds
    def WriteNewTaggerEfficiencies(self,file):
        file.write('#ifndef EFF_H_INCLUDED\n')
        file.write('#define EFF_H_INCLUDED\n')
        file.write('#include <cmath>\n')
        file.write('#include <math.h>\n')
        file.write('#include <iostream>\n')
        for key, value in self.fastsim.tagger.rules.items():
            for eff_key in value['efficiencies'].keys():
                 eff_fnc = value['efficiencies'][eff_key]['function']
                 eff_bnd = value['efficiencies'][eff_key]['bounds'  ]
                 file.write(
                     eff_fnc.tocpp(
                         'MAdouble64',
                         f"eff_{value['id_true']}_{value['id_reco']}_{eff_key}_{TaggerStatus.to_str(value['tag'])}"
                     )+'\n'
                 )
                 file.write(
                     eff_bnd.tocpp(
                         'MAbool',
                         f"bnd_{value['id_true']}_{value['id_reco']}_{eff_key}_{TaggerStatus.to_str(value['tag'])}"
                     ) + '\n'
                 )
        file.write('#endif')

