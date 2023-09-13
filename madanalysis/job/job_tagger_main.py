################################################################################
#  
#  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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

from madanalysis.job.job_tagger_header import JobTaggerHeader
from madanalysis.fastsim.tagger import TaggerStatus

class JobTaggerMain:

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
            if (int(rule["id_true"]), int(rule["id_reco"]), TaggerStatus.to_str(rule["tag"])) not in unique_rules:
                unique_rules.append((int(rule["id_true"]), int(rule["id_reco"]), TaggerStatus.to_str(rule["tag"])))

        for true_id, reco_id, tag in unique_rules:
            file.write("\n" + JobTaggerHeader.base[(true_id, reco_id)](tag) + " {\n")

            check_initializer = 0
            for key, val in self.fastsim.tagger.rules.items():
                if val['id_true'] in [str(true_id)] and val['id_reco'] in [str(reco_id)] and val["tag"] == TaggerStatus.get_status(tag):
                    eff_str = []
                    initializer = 'MAfloat32 '
                    if check_initializer > 0:
                        initializer = ''
                    for eff_key, eff_val in val['efficiencies'].items():
                        my_eff_str = eff_val['bounds'].tocpp_call(
                            "object",
                            f"bnd_{val['id_true']}_{val['id_reco']}_{eff_key}_{TaggerStatus.to_str(val['tag'])}",
                            pointer=".",
                        )
                        my_eff_str +=' * '
                        my_eff_str += eff_val['function'].tocpp_call(
                            "object",
                            f"eff_{val['id_true']}_{val['id_reco']}_{eff_key}_{TaggerStatus.to_str(val['tag'])}",
                            pointer=".",
                        )
                        eff_str.append(my_eff_str)
                    file.write('    ' + initializer  + ' efficiency = ' + ' + '.join(eff_str) +';\n')
                    check_initializer += 1
            file.write("    return efficiency;\n}\n")
