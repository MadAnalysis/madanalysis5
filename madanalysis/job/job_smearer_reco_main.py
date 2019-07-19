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

import madanalysis.observable.observable_list as obs_list

class JobSmearerRecoMain:

    ## Initialization
    def __init__(self, fastsim):
        self.fastsim = fastsim


    ## Writing NewTagger.h
    def WriteNewSmearerRecoSource(self, file):
        # header
        file.write('#include "SampleAnalyzer/User/Analyzer/new_smearer_reco.h"\n')
        if self.fastsim.smearer.rules != {}:
            file.write('#include "SampleAnalyzer/User/Analyzer/sigmas.h"\n')
        file.write('#include "SampleAnalyzer/Commons/Service/RandomService.h"\n')
        if self.fastsim.reco.rules != {}:
            file.write('#include "SampleAnalyzer/User/Analyzer/reco.h"\n')
        file.write('using namespace MA5;\n')
        file.write('\n')
        file.write('void NewSmearer::Execute(SampleFormat& sample, ' +\
            'EventFormat& event)\n{\n')

        # Objects to remove if not reconstructed
        file.write('  // Container to store the IDs of the objects to be removed (no reco).\n')
        file.write('  std::vector<MAuint32> toRemove;\n\n')

        # MET, Meff, TET & THT
        file.write('  // New Lorentz Vector for recalculating the missing energy after smearing\n')
        file.write('  MALorentzVector pTmiss;\n\n')
        file.write('  // shortcut for Meff, TET & THT\n')
        file.write('  MAfloat64 & TET  = event.rec()->TET();\n')
        file.write('  MAfloat64 & THT  = event.rec()->THT();\n')
        file.write('  MAfloat64 & Meff = event.rec()->Meff();\n')
        file.write('  TET = 0.; THT = 0.; Meff = 0.;\n\n')

        # Jet smearing and reconstruction
        file.write('  // Jet smearing in the \''+self.fastsim.jetrecomode+'\' mode\n')
        file.write('  for (MAuint32 i=0; i<event.rec()->jets().size(); i++)\n')
        file.write('  {\n')
        if self.fastsim.jetrecomode == 'jets':
            self.PrintReco(['21', 'j'],file,'(&event.rec()->jets()[i])')
            self.PrintSmearer(['21', 'j'], ['PT','ETA','PHI','E','PX','PY','PZ'],file,'(&event.rec()->jets()[i])')
        elif self.fastsim.jetrecomode == 'constituents':
            file.write('    MAuint32 Ntracks = 0;\n')
            file.write('    MALorentzVector JetMomentum;\n')
            file.write('    for (MAuint32 icnst=0;icnst<(&event.rec()->jets()[i])->constituents().size();icnst++)\n')
            file.write('    {\n')
            file.write('      MCParticleFormat* particle = &event.mc()->particles()[event.rec()->jets()[i].constituents()[icnst]];\n')
            self.PrintReco(['21', 'j'],file,'particle', set2Remove=False)
            self.PrintSmearer(['21', 'j'], ['PT','ETA','PHI','E','PX','PY','PZ'],file,'particle')
            file.write('      JetMomentum += particle->momentum();\n')
            file.write('      Ntracks++;\n')
            file.write('    }\n')
            file.write('    if (Ntracks == 0)\n')
            file.write('    {\n')
            file.write('      toRemove.push_back(i);\n')
            file.write('      continue;\n')
            file.write('    }\n')
            file.write('    (&event.rec()->jets()[i])->momentum().SetPxPyPzE(JetMomentum.Px(), '+\
                               'JetMomentum.Py(), JetMomentum.Pz(), JetMomentum.E());\n')
            file.write('    (&event.rec()->jets()[i])->setNtracks(Ntracks);\n')
        file.write('    pTmiss -= event.rec()->jets()[i].momentum();\n')
        file.write('    THT    += event.rec()->jets()[i].pt();\n')
        file.write('    TET    += event.rec()->jets()[i].pt();\n')
        file.write('    Meff   += event.rec()->jets()[i].pt();\n')
        file.write('  }\n')
        file.write('  // Removal of the non-reconstructed jets\n')
        file.write('  for (MAuint32 i=toRemove.size();i>0;i--)\n')
        file.write('    event.rec()->jets().erase(event.rec()->jets().begin() + toRemove[i-1]);\n')
        file.write('  toRemove.clear();\n\n')

        # Tau smearing
        file.write('  // Tau smearing\n')
        file.write('  for (MAuint32 i=0; i<event.rec()->taus().size(); i++)\n')
        file.write('  {\n')
        self.PrintReco(['15', 'ta'],file,'(&event.rec()->taus()[i])')
        self.PrintSmearer(['15', 'ta'], ['PT','ETA','PHI','E','PX','PY','PZ'],file,'(&event.rec()->taus()[i])')
        file.write('    pTmiss -= event.rec()->taus()[i].momentum();\n')
        file.write('    THT    += event.rec()->jets()[i].pt();\n')
        file.write('    TET    += event.rec()->taus()[i].pt();\n')
        file.write('  }\n')
        file.write('  // Removal of the non-reconstructed taus\n')
        file.write('  for (MAuint32 i=toRemove.size();i>0;i--)\n')
        file.write('    event.rec()->taus().erase(event.rec()->taus().begin() + toRemove[i-1]);\n')
        file.write('  toRemove.clear();\n\n')

        # Updating the missing HT
        file.write('  // Updating MHT\n')
        file.write('  (&event.rec()->MHT().momentum())->SetPxPyPzE(pTmiss.Px(), pTmiss.Py(), 0., pTmiss.E());\n')

        # Electron smearing
        file.write('  // Electron smearing \n')
        file.write('  for (MAuint32 i=0; i<event.rec()->electrons().size(); i++)\n')
        file.write('  {\n')
        self.PrintReco(['11', 'e'],file,'(&event.rec()->electrons()[i])')
        self.PrintSmearer(['11', 'e'], ['PT','ETA','PHI','E','PX','PY','PZ'],file,'(&event.rec()->electrons()[i])')
        file.write('    pTmiss -= event.rec()->electrons()[i].momentum();\n')
        file.write('    TET    += event.rec()->electrons()[i].pt();\n')
        file.write('  }\n')
        file.write('  // Removal of the non-reconstructed electrons\n')
        file.write('  for (MAuint32 i=toRemove.size();i>0;i--)\n')
        file.write('    event.rec()->electrons().erase(event.rec()->electrons().begin() + toRemove[i-1]);\n')
        file.write('  toRemove.clear();\n\n')

        # Muon smearing
        file.write('  // Muon smearing \n')
        file.write('  for (MAuint32 i=0; i<event.rec()->muons().size(); i++)\n')
        file.write('  {\n')
        self.PrintReco(['13', 'mu'],file,'(&event.rec()->muons()[i])')
        self.PrintSmearer(['13', 'mu'], ['PT','ETA','PHI','E','PX','PY','PZ'],file,'(&event.rec()->muons()[i])')
        file.write('    pTmiss -= event.rec()->muons()[i].momentum();\n')
        file.write('    TET    += event.rec()->muons()[i].pt();\n')
        file.write('  }\n')
        file.write('  // Removal of the non-reconstructed muons\n')
        file.write('  for (MAuint32 i=toRemove.size();i>0;i--)\n')
        file.write('    event.rec()->muons().erase(event.rec()->muons().begin() + toRemove[i-1]);\n')
        file.write('  toRemove.clear();\n\n')

        # Photon smearing
        file.write('  // Photon smearing\n')
        file.write('  for (MAuint32 i=0; i<event.rec()->photons().size(); i++)\n')
        file.write('  {\n')
        self.PrintReco(['22', 'a'],file,'(&event.rec()->photons()[i])')
        self.PrintSmearer(['22', 'a'], ['PT','ETA', 'PHI', 'E','PX','PY','PZ'],file,'(&event.rec()->photons()[i])')
        file.write('    pTmiss -= event.rec()->photons()[i].momentum();\n')
        file.write('    TET    += event.rec()->photons()[i].pt();\n')
        file.write('  }\n')
        file.write('  // Removal of the non-reconstructed photons\n')
        file.write('  for (MAuint32 i=toRemove.size();i>0;i--)\n')
        file.write('    event.rec()->photons().erase(event.rec()->photons().begin() + toRemove[i-1]);\n')
        file.write('  toRemove.clear();\n\n')

        # Set missing transverse energy
        file.write('  // New MET (and Meff)\n')
        file.write('  (&event.rec()->MET().momentum())->SetPxPyPzE(pTmiss.Px(), pTmiss.Py(), 0., pTmiss.E());\n')
        file.write('  Meff += event.rec()->MET().pt();\n')
        file.write('}\n\n')

        # Gaussian
        file.write('MAdouble64 NewSmearer::Gaussian(MAdouble64 sigma, MAdouble64 object)\n{\n')
        file.write('  MAdouble64 PI = 3.141592653589793;\n')
        file.write('  MAdouble64 N  = 1.0 / (sigma * sqrt(2.0 * PI));\n')
        file.write('  if (N > 1e20)\n') ## N can become infinity
        file.write('  {\n')
        file.write('    WARNING << "Infinite normalization found in a smearing function" << endmsg;\n')
        file.write('    WARNING << "Smearing ignored." << endmsg;\n')
        file.write('    return object;\n')
        file.write('  }\n')
        file.write('  MAdouble64 gaussian = N * exp( -pow( object / sigma, 2.0) * 0.5 );\n')
        file.write('  MAdouble64 r        = RANDOM->flat();\n')
        file.write('  MAdouble64 sign     = (r >= 0.5) * 1.0 + (r < 0.5) * (-1.0);\n')
        file.write('  return object + sign * RANDOM->flat() * gaussian/2.;\n')
        file.write('}\n\n')


    def PrintSmearer(self, true_list, list_obs, file, obj):
        import madanalysis.observable.observable_list as observable_list
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
                               obj+'->eta(), '+obj+'->phi(), '+\
                               obj+'->e());\n')
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


    def PrintReco(self, reco_list, file, obj, set2Remove=True):
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
                if set2Remove:
                    file.write('        toRemove.push_back(i);\n')
                file.write('        continue;\n')
                file.write('      }\n')

