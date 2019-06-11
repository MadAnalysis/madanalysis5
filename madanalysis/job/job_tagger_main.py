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
        # tau-tagging
        file.write('  // Storing the IDs of tau-tagged jets\n')
        file.write('  std::vector<MAuint32> toRemove;\n\n')
        # b/c-tagging + tau mistagging
        file.write('  // b/c-tagging + tau-mistagging\n')
        file.write('  for (MAuint32 i=0; i<event.rec()->jets().size(); i++)\n')
        file.write('  {\n')
        file.write('    // We have a true b-jet: is it b-tagged?\n')
        file.write('    if (event.rec()->jets()[i].true_btag())\n')
        file.write('    {\n')
        self.PrintTagger(['5', 'b'], ['5', 'b'],file,'(&event.rec()->jets()[i])','Btag')
        file.write('    }\n\n')
        file.write('    // We have a true c-jet: is it b-tagged?\n')
        file.write('    else if (event.rec()->jets()[i].true_ctag())\n')
        file.write('    {\n')
        self.PrintTagger(['4', 'c'], ['5', 'b'],file,'(&event.rec()->jets()[i])','Btag')
        file.write('    }\n\n')
        file.write('    // We have a true light jet: is it b-tagged?\n')
        file.write('    else\n')
        file.write('    {\n')
        self.PrintTagger(['21', 'j'], ['5', 'b'],file,'(&event.rec()->jets()[i])','Btag')
        file.write('    }\n\n')
        file.write('    // ------------------------------------------------------------------------ //\n\n')
        file.write('    // We have a b-tagged jet -> moving on with the next jet\n')
        file.write('    if (event.rec()->jets()[i].btag()) { continue; }\n\n')
        file.write('    // ------------------------------------------------------------------------ //\n\n')
        file.write('    // We have a true c-jet -> is it c-tagged?\n')
        file.write('    if (event.rec()->jets()[i].true_ctag())\n')
        file.write('    {\n')
        self.PrintTagger(['4', 'c'], ['4', 'c'],file,'(&event.rec()->jets()[i])','Ctag')
        file.write('    }\n\n')
        file.write('    // We have a true (non-b-tagged) b-jet -> is it c-tagged?\n')
        file.write('    else if (event.rec()->jets()[i].true_btag())\n')
        file.write('    {\n')
        self.PrintTagger(['5', 'b'], ['4', 'c'],file,'(&event.rec()->jets()[i])','Ctag')
        file.write('    }\n\n')
        file.write('    // We have a true light jet -> is it c-tagged?\n')
        file.write('    else\n')
        file.write('    {\n')
        self.PrintTagger(['21', 'j'], ['4', 'c'],file,'(&event.rec()->jets()[i])','Ctag')
        file.write('    }\n')
        file.write('    // ------------------------------------------------------------------------ //\n\n')
        file.write('    // We have a c-tagged jet -> moving on with the next jet\n')
        file.write('    if (event.rec()->jets()[i].ctag()) { continue; }\n\n')
        file.write('    // ------------------------------------------------------------------------ //\n\n')
        file.write('    // We have a true b/c-jet -> not tau-tagged\n')
        file.write('    if (event.rec()->jets()[i].true_btag() || event.rec()->jets()[i].true_ctag()) { continue; }\n\n')
        file.write('    // if not, is it tau-tagged?\n')
        file.write('    else\n')
        file.write('    {\n')
        self.PrintTagger(['21', 'j'], ['15', 'ta'],file,'(&event.rec()->jets()[i])','TauMistag')
        file.write('    }\n')
        file.write('  }\n\n')
        file.write('  for (MAuint32 i=toRemove.size();i>0;i--)\n')
        file.write('    event.rec()->jets().erase(event.rec()->jets().begin() + toRemove[i-1]);\n\n')

        # tau-tagging
        file.write('  // tau-tagging\n')
        file.write('  toRemove.clear();\n')
        file.write('  for (MAuint32 i=0; i<event.rec()->taus().size(); i++)\n')
        file.write('  {\n')
        self.PrintTagger(['15', 'ta'], ['15', 'ta'],file,'(&event.rec()->taus()[i])','Tautag')
        file.write('  }\n\n')
        file.write('  for (MAuint32 i=toRemove.size();i>0;i--)\n')
        file.write('    event.rec()->taus().erase(event.rec()->taus().begin() + toRemove[i-1]);\n\n')
        # End
        file.write('}\n\n')


    def PrintTagger(self, true_list, reco_list, file, obj, prop):
        for key, val in self.fastsim.tagger.rules.items():
            if val['id_true'] in true_list and val['id_reco'] in reco_list:
                eff_str = []
                for eff_key, eff_val in val['efficiencies'].items():
                    my_eff_str = eff_val['bounds'].tocpp_call(obj,\
                      'bnd_'+str(val['id_true'])+'_'+str(val['id_reco'])+'_'+str(eff_key))
                    my_eff_str +=' * '
                    my_eff_str += eff_val['function'].tocpp_call(obj,\
                      'eff_'+str(val['id_true'])+'_'+str(val['id_reco'])+'_'+str(eff_key))
                    eff_str.append(my_eff_str)
                file.write('      MAdouble64 efficiency = ' + ' + '.join(eff_str) +';\n')
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
                elif prop=='Tautag':
                    file.write('      if (RANDOM->flat() > efficiency)\n')
                    file.write('      {\n')
                    file.write('        RecJetFormat* newJet = event.rec()->GetNewJet();\n')
                    file.write('        newJet->setMomentum('+obj+'->momentum());\n')
                    file.write('        newJet->setNtracks(' + obj + '->ntracks());\n')
                    file.write('        newJet->setMc(' + obj + '->mc());\n')
                    file.write('        toRemove.push_back(i);\n')
                    file.write('      }\n')
                else:
                    file.write('      if (RANDOM->flat() < efficiency)')
                    file.write(' { ' + obj+'->set'+prop+'(true); }\n')
