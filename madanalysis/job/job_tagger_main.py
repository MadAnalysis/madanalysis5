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


class JobTaggerMain:

    # structure: (true_id, reco_id) : "..."
    base = {
        (5, 5): "/// B-jet tagging efficiency (b as b)\nMAfloat32 NewTagger::b_tagging_eff(const RecJetFormat &object) const",
        (5, 4): "/// B-jet mistagging as C-jet (b as c)\nMAfloat32 NewTagger::b_mistag_c(const RecJetFormat &object) const",
        (4, 4): "/// C-jet tagging efficiency (c as c)\nMAfloat32 NewTagger::c_tagging_eff(const RecJetFormat &object) const",
        (4, 5): "/// C-jet mistagging as C-jet (c as b)\nMAfloat32 NewTagger::c_mistag_b(const RecJetFormat &object) const",
        (21, 5): "/// Light-Jet mistagging as b-jet (j as b)\nMAfloat32 NewTagger::lightjet_mistag_b(const RecJetFormat &object) const",
        (21, 4): "/// Light-Jet mistagging as c jet (j as c)\nMAfloat32 NewTagger::lightjet_mistag_c(const RecJetFormat &object) const",
        (21, 15): "/// Light-Jet mistagging as tau (j as ta)\nMAfloat32 NewTagger::lightjet_mistag_tau(const RecJetFormat &object) const",
        (21, 11): "/// Light-Jet mistagging as electron (j as e)\nMAfloat32 NewTagger::lightjet_mistag_electron(const RecJetFormat &object) const",
        (21, 22): "/// Light-Jet mistagging as photon (j as photon)\nMAfloat32 NewTagger::lightjet_mistag_photon(const RecJetFormat &object) const",
        (15, 15): "/// Tau tagging efficiency (ta as ta)\nMAfloat32 NewTagger::tau_tagging_eff(const RecTauFormat &object) const",
        (11, 13): "/// Electron mistagging as muon (e as mu)\nMAfloat32 NewTagger::electron_mistag_muon(const RecLeptonFormat &object) const",
        (11, 22): "/// Electron mistagging as photon (e as a)\nMAfloat32 NewTagger::electron_mistag_photon(const RecLeptonFormat &object) const",
        (11, 21): "/// Electron mistagging as light jet (e as j)\nMAfloat32 NewTagger::electron_mistag_lightjet(const RecLeptonFormat &object) const",
        (13, 11): "/// Electron mistagging as electron (mu as e)\nMAfloat32 NewTagger::muon_mistag_electron(const RecLeptonFormat &object) const",
        (13, 22): "/// Electron mistagging as photon (mu as a)\nMAfloat32 NewTagger::muon_mistag_photon(const RecLeptonFormat &object) const",
        (13, 21): "/// Electron mistagging as light jet (mu as j)\nMAfloat32 NewTagger::muon_mistag_lightjet(const RecLeptonFormat &object) const",
        (22, 11): "/// Electron mistagging as electron (a as e)\nMAfloat32 NewTagger::photon_mistag_electron(const RecPhotonFormat &object) const",
        (22, 13): "/// Electron mistagging as muon (a as mu)\nMAfloat32 NewTagger::photon_mistag_muon(const RecPhotonFormat &object) const",
        (22, 21): "/// Electron mistagging as light jet (a as j)\nMAfloat32 NewTagger::photon_mistag_lightjet(const RecPhotonFormat &object) const",
    }

    ## Initialization
    def __init__(self, fastsim):
        self.fastsim = fastsim


    ## Writing NewTagger.h
    def WriteNewTaggerSource(self, file):
        # header
        file.write('#include "SampleAnalyzer/User/Analyzer/new_tagger.h"\n')
        file.write('#include "SampleAnalyzer/User/Analyzer/efficiencies.h"\n')
        file.write('using namespace MA5;\n')

        unique_rules = []
        for key, rule in self.fastsim.tagger.rules.items():
            if (int(rule["id_true"]), int(rule["id_reco"])) not in unique_rules:
                unique_rules.append((int(rule["id_true"]), int(rule["id_reco"])))

        for true_id, reco_id in unique_rules:
            file.write("\n"+JobTaggerMain.base[(true_id, reco_id)] + " {\n")

            check_initializer = 0
            for key, val in self.fastsim.tagger.rules.items():
                if val['id_true'] in [str(true_id)] and val['id_reco'] in [str(reco_id)]:
                    eff_str = []
                    initializer = 'MAfloat32 '
                    if check_initializer > 0:
                        initializer = ''
                    for eff_key, eff_val in val['efficiencies'].items():
                        my_eff_str = eff_val['bounds'].tocpp_call(
                            "object",'bnd_'+str(val['id_true'])+'_'+str(val['id_reco'])+'_'+str(eff_key), pointer=".",
                        )
                        my_eff_str +=' * '
                        my_eff_str += eff_val['function'].tocpp_call(
                            "object", 'eff_'+str(val['id_true'])+'_'+str(val['id_reco'])+'_'+str(eff_key), pointer=".",
                        )
                        eff_str.append(my_eff_str)
                    file.write('    ' + initializer  + ' efficiency = ' + ' + '.join(eff_str) +';\n')
                    check_initializer += 1
            file.write("    return efficiency;\n}\n")
