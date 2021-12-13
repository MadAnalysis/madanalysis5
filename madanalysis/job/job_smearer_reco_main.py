################################################################################
#  
#  Copyright (C) 2012-2020 Jack Araz, Eric Conte & Benjamin Fuks
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


from __future__ import absolute_import
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
        self.track_smearing       = False
        for key, val in self.fastsim.smearer.rules.items():
            if val['id_true'] in ['21','j']:
                self.jet_smearing         = (self.fastsim.jetrecomode == 'jets')
                self.constituent_smearing = (self.fastsim.jetrecomode == 'constituents')
            elif val['id_true'] in ['22','a']:
                self.photon_smearing      = True
            elif val['id_true'] in ['13','mu']:
                self.muon_smearing        = True
            elif val['id_true'] in ['11','e']:
                self.electron_smearing    = True
            elif val['id_true'] in ['15','ta']:
                self.tau_smearing         = True
            elif val['id_true'] in ['track']:
                self.track_smearing       = True
        for key, val in self.fastsim.reco.rules.items():
            if val['id_reco'] in ['21','j']:
                self.jet_smearing         = True
            elif val['id_reco'] in ['22','a']:
                self.photon_smearing      = True
            elif val['id_reco'] in ['13','mu']:
                self.muon_smearing        = True
            elif val['id_reco'] in ['11','e']:
                self.electron_smearing    = True
            elif val['id_reco'] in ['15','ta']:
                self.tau_smearing         = True
            elif val['id_reco'] in ['track']:
                self.track_smearing       = True
        for key, val in self.fastsim.scaling.rules.items():
            if val['id_true'] == 'JES':
                self.jet_smearing         = True
            elif val['id_true'] in ['21','j']:
                if self.fastsim.jetrecomode == 'jets':
                    self.jet_smearing         = True
                else:
                    self.constituent_smearing = True
            elif val['id_true'] in ['22','a']:
                self.photon_smearing      = True
            elif val['id_true'] in ['13','mu']:
                self.muon_smearing        = True
            elif val['id_true'] in ['11','e']:
                self.electron_smearing    = True
            elif val['id_true'] in ['15','ta']:
                self.tau_smearing         = True
            elif val['id_true'] in ['track']:
                self.track_smearing       = True


    ## Writing NewTagger.h
    def WriteNewSmearerRecoSource(self, file):
        # header
        file.write('#include "SampleAnalyzer/User/Analyzer/new_smearer_reco.h"\n')
        if self.fastsim.smearer.rules != {}:
            file.write('#include "SampleAnalyzer/User/Analyzer/sigmas.h"\n')
        if self.fastsim.reco.rules != {}:
            file.write('#include "SampleAnalyzer/User/Analyzer/reco.h"\n')
        if self.fastsim.scaling.rules != {}:
            file.write('#include "SampleAnalyzer/User/Analyzer/scaling.h"\n')
        file.write('using namespace MA5;\n')
        file.write('\n')
        if self.jet_smearing:
            self.WriteSmearingMethod(file,'Jet',['21', 'j', 'JES'])
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
        if self.track_smearing:
            self.WriteSmearingMethod(file,'Track',['track'])

    def WriteSmearingMethod(self,file,obj,reco_list):
        file.write('/// '+obj+' smearing method\n')
        file.write('MCParticleFormat NewSmearer::'+obj+'Smearer(const MCParticleFormat * part)\n')
        file.write('{\n')
        file.write('    MCParticleFormat *'+obj+' = &(output_);\n')
        file.write('    '+obj+'->Reset();\n')

        # Dont use reco eff for constituents
        if obj != 'Constituent':
            self.PrintReco(reco_list,file,'part')

        #if not eliminated set hadron momentum
        file.write('    SetDefaultOutput(part, output_);\n')

        # If constituents method is in use, jet smearing is only done for constituents
        if (obj != 'Jet') or (obj=='Jet' and self.fastsim.jetrecomode == 'jets'):
            observable_list = ['PT','ETA','PHI','E','PX','PY','PZ']
            if obj in ['Electron','Muon','Photon','Tau','Track']:
                observable_list += ['D0','DZ']
            self.PrintSmearer(reco_list, observable_list, file, obj)

        # Observable re-scaling
        self.PrintScaling(reco_list,['PT','ETA','PHI','E','PX','PY','PZ'], file,obj)

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
                          obj+'->eta(), '+obj+'->phi(), smeared_object*cosh('+obj+'->eta()));\n')
                elif val['obs'] == 'ETA':
                    file.write('      '+obj+\
                               '->momentum().SetPtEtaPhiE('+obj+'->pt(), '+\
                               'smeared_object, '+obj+'->phi(), '+ obj+'->e());\n')
                elif val['obs'] == 'PHI':
                    file.write('      '+obj+\
                               '->momentum().SetPtEtaPhiE('+obj+'->pt(), '+\
                               obj+'->eta(), smeared_object, '+ obj+'->e());\n')
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
                    file.write('      '+obj+'->momentum().SetPtEtaPhiE(smeared_object/cosh('+\
                           obj+'->eta()), '+obj+'->eta(), '+obj+'->phi(), smeared_object);\n')
                elif val['obs'] in ['D0','DZ']:
                    file.write('      '+obj+'->set'+val['obs']+'(smeared_object);\n')
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


    def PrintScaling(self, true_list, list_obs, file, obj):
        check_initializer = 0
        for key, val in self.fastsim.scaling.rules.items():
            if obj == 'Jet' and self.fastsim.jetrecomode == 'constituents' and val['id_true'] != 'JES':
                continue
            if val['id_true'] in true_list and val['obs'] in list_obs:
                eff_str = []
                initializer = 'MAdouble64 '
                if check_initializer > 0:
                    initializer = ''
                for eff_key, eff_val in val['efficiencies'].items():
                    my_eff_str = eff_val['bounds'].tocpp_call(obj,\
                                 'scale_bnd_'+str(val['id_true'])+'_'+str(val['obs'])+\
                                 '_'+str(eff_key))
                    my_eff_str +=' * '
                    my_eff_str += eff_val['function'].tocpp_call(obj,\
                                  'scale_'+str(val['id_true'])+\
                                  '_'+str(val['obs'])+'_'+str(eff_key))
                    eff_str.append(my_eff_str)
                if check_initializer > 0:
                    file.write('    scale = 1.;\n')
                file.write('    '+initializer+'scale = ' + ' + '.join(eff_str) +';\n')
                file.write('    '+initializer+'scaled_object = scale * '+obj+'->'+\
                               observable_list.__dict__[val['obs']].code_reco+';\n')
                # we dont want momentum and energy to be negative
                if val['obs'] == 'PT':
                    file.write('    if (scaled_object < 0.) scaled_object = 0.;\n')
                    file.write('    '+obj+'->momentum().SetPtEtaPhiE(scaled_object, '+\
                          obj+'->eta(), '+obj+'->phi(), scaled_object*cosh('+obj+'->eta()));\n')
                elif val['obs'] == 'ETA':
                    file.write('    '+obj+\
                               '->momentum().SetPtEtaPhiE('+obj+'->pt(), '+\
                               'scaled_object, '+obj+'->phi(), '+ obj+'->e());\n')
                elif val['obs'] == 'PHI':
                    file.write('    '+obj+\
                               '->momentum().SetPtEtaPhiE('+obj+'->pt(), '+\
                               obj+'->eta(), scaled_object, '+ obj+'->e());\n')
                elif val['obs'] == 'PX':
                    file.write('    '+obj+\
                               '->momentum().SetPxPyPzE(scaled_object,'+\
                               obj+'->py(), '+obj+'->pz(), '+obj+'->e());\n')
                elif val['obs'] == 'PY':
                    file.write('    '+obj+\
                               '->momentum().SetPxPyPzE('+obj+'->px(),'+\
                               'scaled_object, '+obj+'->pz(), '+obj+'->e());\n')
                elif val['obs'] == 'PZ':
                    file.write('    '+obj+\
                               '->momentum().SetPxPyPzE('+obj+'->px(),'+\
                               obj+'->py(), scaled_object, '+obj+'->e());\n')
                elif val['obs'] == 'E':
                    file.write('    if (scaled_object < 0.) scaled_object = 0.;\n')
                    file.write('    '+obj+'->momentum().SetPtEtaPhiE(scaled_object/cosh('+\
                           obj+'->eta()), '+obj+'->eta(), '+obj+'->phi(), scaled_object);\n')
                elif val['obs'] in ['D0','DZ']:
                    file.write('    '+obj+'->set'+val['obs']+'(scaled_object);\n')
                check_initializer+=1
