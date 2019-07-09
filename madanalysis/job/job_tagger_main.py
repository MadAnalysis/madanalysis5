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


class JobTaggerMain:

    ## Initialization
    def __init__(self, fastsim):
        self.fastsim = fastsim


    ## Writing NewTagger.h
    def WriteNewTaggerSource(self, file):
        # header
        file.write('#include "SampleAnalyzer/User/Analyzer/new_tagger.h"\n')
        file.write('#include "SampleAnalyzer/User/Analyzer/efficiencies.h"\n')
        file.write('#include "SampleAnalyzer/Commons/Service/RandomService.h"\n')
        file.write('#include "SampleAnalyzer/Commons/Service/PDGService.h"\n')
        file.write('#include "SampleAnalyzer/Commons/Service/Physics.h"\n')
        file.write('using namespace MA5;\n')
        file.write('\n')
        file.write('void NewTagger::Execute(SampleFormat& sample, ' +\
            'EventFormat& event)\n{\n')

        # Removal container
        file.write('  // Storing the IDs of objects that need to leave a collection\n')
        file.write('  std::vector<MAuint32> toRemove;\n\n')

        # b/c-tagging + tau mistagging
        file.write('  // b/c-tagging + tau-mistagging\n')
        file.write('  unsigned int Ntaus = event.rec()->taus().size();\n')
        file.write('  for (MAuint32 i=0; i<event.rec()->jets().size(); i++)\n')
        file.write('  {\n')
        for reco_ID in [ ['5', 'b', 'Btag'], ['4', 'c', 'Ctag'] ]:
            pretag=''
            for true_ID in [ ['5', 'b', 'true_btag'], ['4', 'c', 'true_ctag'], ['21', 'j', '' ] ]:
                if self.HaveRules(true_ID[:-1], reco_ID[:-1]):
                    file.write('    // We have a true ' + true_ID[-2].replace('j','light') + '-jet: is it ' + reco_ID[-2] + '-tagged?\n')
                    if true_ID[-1]=='':
                        file.write('    ' + pretag + '\n')
                    else:
                        file.write('    ' + pretag + 'if (event.rec()->jets()[i].' + true_ID[-1]+ '())\n')
                    file.write('    {\n')
                    self.PrintTagger(true_ID[:-1], reco_ID[:-1], file,'(&event.rec()->jets()[i])',reco_ID[-1])
                    file.write('    }\n\n')
                    pretag='else '
            file.write('    // We have a '+reco_ID[-2]+'-tagged jet -> moving on with the next jet\n')
            file.write('    if (event.rec()->jets()[i].'+reco_ID[-1].lower()+'()) { continue; }\n\n')
        if self.HaveRules(['21', 'j'], ['15', 'ta']):
            file.write('    // We have a true b/c-jet -> not tau-tagged\n')
            file.write('    if (event.rec()->jets()[i].true_btag() || event.rec()->jets()[i].true_ctag()) { continue; }\n\n')
            file.write('    // if not, is it tau-tagged?\n')
            file.write('    else\n')
            file.write('    {\n')
            self.PrintTagger(['21', 'j'], ['15', 'ta'],file,'(&event.rec()->jets()[i])','TauMistag')
            file.write('    }\n')
        file.write('  }\n\n')
        file.write('  for (MAuint32 i=toRemove.size();i>0;i--)\n')
        file.write('    event.rec()->jets().erase(event.rec()->jets().begin() + toRemove[i-1]);\n')
        file.write('  toRemove.clear();\n\n')

        # tau-tagging
        if self.HaveRules(['15', 'ta'], ['15', 'ta']):
            file.write('  // tau-tagging\n')
            file.write('  for (MAuint32 i=0; i<Ntaus; i++)\n')
            file.write('  {\n')
            self.PrintTagger(['15', 'ta'], ['15', 'ta'],file,'(&event.rec()->taus()[i])','Jet')
            file.write('  }\n')
            file.write('  for (MAuint32 i=toRemove.size();i>0;i--)\n')
            file.write('    event.rec()->taus().erase(event.rec()->taus().begin() + toRemove[i-1]);\n')
            file.write('  toRemove.clear();\n\n')

        # global event variables
        file.write('  // Shortcut for global event variables\n')
        file.write('  MAfloat64 & THT  = event.rec()->THT();\n')
        file.write('  MAfloat64 & Meff = event.rec()->Meff();\n\n')

        # Muon and electron mis-tagging
        for true_ID in [  ['13', 'mu', 'muons'],  ['11', 'e', 'electrons'],  ['22', 'a', 'photons']  ]:
            if self.HaveRules(true_ID[:-1], ['11', 'e', '22', 'a', '21', 'j', '13', 'mu']):
                file.write('  // Mistagging of ' + true_ID[-1] + '\n')
                file.write('  for (MAuint32 i=0; i<event.rec()->' + true_ID[-1] + '().size(); i++)\n')
                file.write('  {\n')
                self.PrintTagger(true_ID[:-1], ['11', 'e', '22', 'a', '21', 'j', '13', 'mu'], file,
                   '(&event.rec()->' + true_ID[-1] + '()[i])','LeptonicMistag')
                file.write('  }\n')
                file.write('  for (MAuint32 i=toRemove.size();i>0;i--)\n')
                file.write('    event.rec()->' + true_ID[-1] + '().erase(event.rec()->' + true_ID[-1] + '().begin() + toRemove[i-1]);\n')
                file.write('  toRemove.clear();\n\n')

        # End
        file.write('}\n\n')


    def HaveRules(self, true_list, reco_list):
        for key, val in self.fastsim.tagger.rules.items():
            if val['id_true'] in true_list and val['id_reco'] in reco_list:
                return True
        return False


    def PrintTagger(self, true_list, reco_list, file, obj, prop):
        # To get information on the existence of a tagger for a given particle species
        check_initializer = 0
        for key, val in self.fastsim.tagger.rules.items():
            if val['id_true'] in true_list and val['id_reco'] in reco_list:
                eff_str = []
                initializer = 'MAdouble64 '
                if check_initializer > 0:
                    initializer = ''
                for eff_key, eff_val in val['efficiencies'].items():
                    my_eff_str = eff_val['bounds'].tocpp_call(obj,\
                      'bnd_'+str(val['id_true'])+'_'+str(val['id_reco'])+'_'+str(eff_key))
                    my_eff_str +=' * '
                    my_eff_str += eff_val['function'].tocpp_call(obj,\
                      'eff_'+str(val['id_true'])+'_'+str(val['id_reco'])+'_'+str(eff_key))
                    eff_str.append(my_eff_str)
                file.write('      ' + initializer  + ' efficiency = ' + ' + '.join(eff_str) +';\n')
                if prop=='TauMistag':
                    file.write('      if (RANDOM->flat() < efficiency)\n')
                    file.write('      {\n')
                    file.write('        RecTauFormat* newTau = event.rec()->GetNewTau();\n')
                    file.write('        newTau->setMomentum('+obj+'->momentum());\n')
                    file.write('        newTau->setNtracks(' + obj + '->ntracks());\n')
                    file.write('        newTau->setMc(' + obj + '->mc());\n')
                    file.write('        newTau->setDecayMode(PHYSICS->GetTauDecayMode(newTau->mc()));\n')
                    file.write('        MAint32 charge = 0;\n')
                    file.write('        for (MAuint32 icst=0;icst<'+obj+'->constituents().size();icst++)\n')
                    file.write('          charge += PDG->GetCharge(event.mc()->particles()['+obj+'->constituents()[icst]].pdgid());\n')
                    file.write('        newTau->setCharge(charge>0);\n')
                    file.write('        toRemove.push_back(i);\n')
                    file.write('      }\n')
                elif prop in ['Jet', 'LeptonicMistag']:
                    if 'tau' in obj and prop=='Jet':
                        file.write('      if (RANDOM->flat() > efficiency)\n')
                    else:
                        file.write('      if (RANDOM->flat() < efficiency)\n')
                    # Get the object for the reco object
                    newprop = prop
                    if val['id_reco'] in ['11', 'e']  :
                        newprop = 'Electron'
                    elif val['id_reco'] in ['13', 'mu'] :
                        newprop = 'Muon'
                    elif val['id_reco'] in ['22', 'a']  :
                        newprop = 'Photon'
                    elif val['id_reco'] in ['21', 'j']  :
                        newprop = 'Jet'
                    file.write('      {\n')
                    file.write('        Rec'+newprop.replace('Electron','Lepton').replace('Muon','Lepton')+ \
                         'Format* NewParticle = event.rec()->GetNew'+newprop+'();\n')
                    file.write('        NewParticle->setMomentum('+obj+'->momentum());\n')
                    file.write('        NewParticle->setMc(' + obj + '->mc());\n')
                    if newprop in ['Electron', 'Muon']:
                         if 'muon' in obj or 'electron' in obj:
                             file.write('        NewParticle->SetCharge(' + obj + '->charge());\n')
                         else:
                             file.write('        if(RANDOM->flat() > 0.5)\n')
                             file.write('            NewParticle->SetCharge(1.);\n')
                             file.write('        else)\n')
                             file.write('            NewParticle->SetCharge(-1.);\n')
                    elif newprop=='Jet':
                        if 'tau' in obj:
                            file.write('        NewParticle->setNtracks(' + obj + '->ntracks());\n')
                        else:
                            file.write('        NewParticle->setNtracks(1);\n')
                            file.write('        THT    += '+obj+'->pt();\n')
                            file.write('        Meff   += '+obj+'->pt();\n')
                            file.write('        MALorentzVector MissHT = event.rec()->MHT().momentum() - '+obj+'->momentum();\n')
                            file.write('        (&event.rec()->MHT().momentum())->SetPxPyPzE(MissHT.Px(), MissHT.Py(), 0., MissHT.E());\n')
                    file.write('        toRemove.push_back(i);\n')
                    file.write('        continue;\n')
                    file.write('      }\n')
                else:
                    if true_list[0] in reco_list:
                        file.write('      if (RANDOM->flat() > efficiency)')
                        file.write(' { ' + obj+'->set'+prop+'(false); }\n')
                    else:
                        file.write('      if (RANDOM->flat() < efficiency)')
                        file.write(' { ' + obj+'->set'+prop+'(true); }\n')
                check_initializer+=1
