################################################################################
#  
#  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://launchpad.net/madanalysis5>
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


import madanalysis.observable.observable_list as observable_list

class JobSmearerRecoMain:

    ## Initialization
    def __init__(self, fastsim):
        self.fastsim = fastsim
        self.electron_smearing    = False
        self.muon_smearing        = False
        self.photon_smearing      = False
        self.tau_smearing         = False
        self.jet_smearing         = False
        self.constituent_smearing = False
        for key, val in self.fastsim.smearer.rules.items():
            if val['id_true'] in ['21','j']:
                self.jet_smearing         = True
                self.constituent_smearing = True
            elif val['id_true'] in ['22','a']:
                self.photon_smearing      = True
            elif val['id_true'] in ['13','mu']:
                self.muon_smearing        = True
            elif val['id_true'] in ['11','e']:
                self.electron_smearing    = True
            elif val['id_true'] in ['15','ta']:
                self.tau_smearing         = True
        for key, val in self.fastsim.reco.rules.items():
            if val['id_reco'] in ['21','j']:
                self.jet_smearing         = True
                self.constituent_smearing = True
            elif val['id_reco'] in ['22','a']:
                self.photon_smearing      = True
            elif val['id_reco'] in ['13','mu']:
                self.muon_smearing        = True
            elif val['id_reco'] in ['11','e']:
                self.electron_smearing    = True
            elif val['id_reco'] in ['15','ta']:
                self.tau_smearing         = True
        if self.fastsim.jetrecomode == 'constituents':
            self.jet_smearing         = False
        else:
            self.constituent_smearing = False

    ## Writing NewTagger.h
    def WriteNewSmearerRecoSource(self, file):
        # header
        file.write('#include "SampleAnalyzer/User/Analyzer/new_smearer_reco.h"\n')
        if self.fastsim.smearer.rules != {}:
            file.write('#include "SampleAnalyzer/User/Analyzer/sigmas.h"\n')
        if self.fastsim.reco.rules != {}:
            file.write('#include "SampleAnalyzer/User/Analyzer/reco.h"\n')
        file.write('using namespace MA5;\n')
        file.write('\n')
        if self.jet_smearing:
            self.WriteSmearingMethod(file,'Jet',['21', 'j'])
        if self.constituent_smearing:
            self.WriteSmearingMethod(file,'Constituent',['21', 'j'])
        if self.tau_smearing:
            self.WriteSmearingMethod(file,'Tau',['15', 'ta'])
        if self.muon_smearing:
            self.WriteSmearingMethod(file,'Muon',['13', 'mu'])
        if self.electron_smearing:
            self.WriteSmearingMethod(file,'Electron',['11', 'e'])
        if self.photon_smearing:
            self.WriteSmearingMethod(file,'Photon',['22', 'a'])

    def WriteSmearingMethod(self,file,obj,reco_list):
        file.write('/// '+obj+' smearing method\n')
        file.write('MCParticleFormat NewSmearer::'+obj+'Smearer(const MCParticleFormat * part)\n')
        file.write('{\n')
        file.write('    MCParticleFormat *'+obj+' = &(output_);\n')
        file.write('    '+obj+'->Reset();\n')
        self.PrintReco(reco_list,file,'part')
        #if not eliminated set hadron momentum
        file.write('    '+obj+'->momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());\n')
        self.PrintSmearer(reco_list, ['PT','ETA','PHI','E','PX','PY','PZ'],file,obj)
        file.write('    return output_;\n')
        file.write('}\n\n')


    def PrintSmearer(self, true_list, list_obs, file, obj):
        check_initializer = 0
        for key, val in self.fastsim.smearer.rules.items():
            if val['id_true'] in true_list and val['obs'] in list_obs:
                eff_str = []
                initializer = 'MAdouble64 '
                if check_initializer > 0:
                    initializer = ''
                for eff_key, eff_val in val['efficiencies'].items():
                    my_eff_str = eff_val['bounds'].tocpp_call(obj,\
                      'bnd_'+str(val['id_true'])+'_'+str(val['obs'])+'_'+str(eff_key))
                    my_eff_str +=' * '
                    my_eff_str += eff_val['function'].tocpp_call(obj,\
                      'eff_'+str(val['id_true'])+'_'+str(val['obs'])+'_'+str(eff_key))
                    eff_str.append(my_eff_str)
                if check_initializer > 0:
                    file.write('    sigma = 0.;\n')
                file.write('    '+initializer+'sigma = ' + ' + '.join(eff_str) +';\n')
                file.write('    if ( sigma != 0. )\n    {\n')
                file.write('      MAdouble64 smeared_object = Gaussian(sigma,'+obj+'->'+\
                               observable_list.__dict__[val['obs']].code_reco+');\n')
                # we dont want momentum and energy to be negative
                if val['obs'] == 'PT':
                    file.write('      if (smeared_object < 0.) smeared_object = 0.;\n')
                    file.write('      '+obj+'->momentum().SetPtEtaPhiE(smeared_object, '+\
                               obj+'->eta(), '+obj+'->phi(), '+obj+'->e());\n')
                elif val['obs'] == 'ETA':
                    file.write('      '+obj+\
                               '->momentum().SetPtEtaPhiE('+obj+'->pt(), '+\
                               'smeared_object, '+obj+'->phi(), '+ obj+'->e());\n')
                elif val['obs'] == 'PHI':
                    file.write('      '+obj+\
                               '->momentum().SetPtEtaPhiE('+obj+'->pt(), '+\
                               +obj+'->eta(), smeared_object, '+ obj+'->e());\n')
                elif val['obs'] == 'PX':
                    file.write('      if (smeared_object < 0.) smeared_object = 0.;\n')
                    file.write('      '+obj+\
                               '->momentum().SetPxPyPzE(smeared_object,'+\
                               obj+'->py(), '+obj+'->pz(), '+obj+'->e());\n')
                elif val['obs'] == 'PY':
                    file.write('      if (smeared_object < 0.) smeared_object = 0.;\n')
                    file.write('      '+obj+\
                               '->momentum().SetPxPyPzE('+obj+'->px(),'+\
                               'smeared_object, '+obj+'->pz(), '+obj+'->e());\n')
                elif val['obs'] == 'PZ':
                    file.write('      if (smeared_object < 0.) smeared_object = 0.;\n')
                    file.write('      '+obj+\
                               '->momentum().SetPxPyPzE('+obj+'->px(),'+\
                               obj+'->py(), smeared_object, '+obj+'->e());\n')
                elif val['obs'] == 'E':
                    file.write('      if (smeared_object < 0.) smeared_object = 0.;\n')
                    file.write('      '+obj+'->momentum().SetPtEtaPhiE('+obj+'->pt(), '+\
                               obj+'->eta(), '+obj+'->phi(), '+'smeared_object);\n')
                file.write('    }\n')
                check_initializer+=1


    def PrintReco(self, reco_list, file, obj):
        for key, val in self.fastsim.reco.rules.items():
            if val['id_reco'] in reco_list:
                eff_str = []
                for eff_key, eff_val in val['efficiencies'].items():
                    my_eff_str = eff_val['bounds'].tocpp_call(obj,\
                                'reco_bnd_'+str(val['id_reco'])+'_'+str(eff_key))
                    my_eff_str +=' * '
                    my_eff_str += eff_val['function'].tocpp_call(obj,\
                                 'reco_'+str(val['id_reco'])+'_'+str(eff_key))
                    eff_str.append(my_eff_str)
                file.write('      MAdouble64 acceptance = ' + ' + '.join(eff_str) +';\n')
                file.write('      if (RANDOM->flat() > acceptance)\n')
                file.write('      {\n')
                file.write('          return output_;\n')
                file.write('      }\n')

