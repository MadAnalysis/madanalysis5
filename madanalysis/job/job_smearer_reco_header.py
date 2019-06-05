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


class JobSmearerRecoHeader:

    ## Initialization
    def __init__(self, fastsim):
        self.fastsim = fastsim


    ## Writing NewTagger.h
    def WriteNewSmearerRecoHeader(self, file):
        file.write('#ifndef NEW_SMEARER_H\n')
        file.write('#define NEW_SMEARER_H\n')
        file.write('// SampleAnalyzer headers\n')
        file.write('#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"\n')
        file.write('#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"\n')
        file.write('namespace MA5\n')
        file.write('{\n')
        file.write('  class NewSmearer\n')
        file.write('  {\n')
        file.write('    public :\n')
        file.write('      /// Constructor without argument\n')
        file.write('      NewSmearer() \n')
        file.write('      {\n')
        file.write('      }\n\n')
        file.write('      /// Destructor\n')
        file.write('      virtual ~NewSmearer() {}\n\n')
        file.write('      /// Smearer execution\n')
        file.write('      void Execute(SampleFormat &mySample, EventFormat &myEvent);\n\n')
        file.write('      /// Smearer Gaussian function\n')
        file.write('      MAdouble64 Gaussian(MAdouble64 sigma, MAdouble64 object);\n\n')
        file.write('  };\n')
        file.write('}\n')
        file.write('#endif')


    ## efficiencies and bounds
    def WriteNewSmearerEfficiencies(self,file):
        file.write('#ifndef SIG_H_INCLUDED\n')
        file.write('#define SIG_H_INCLUDED\n')
        file.write('#include <cmath>\n')
        file.write('#include <math.h>\n')
        file.write('#include <iostream>\n')
        for key, value in self.fastsim.smearer.rules.items():
            for eff_key in value['efficiencies'].keys():
                 eff_fnc = value['efficiencies'][eff_key]['function']
                 eff_bnd = value['efficiencies'][eff_key]['bounds'  ]
                 file.write(eff_fnc.tocpp('MAdouble64', \
                     'eff_'+str(value['id_true']) + '_' + str(value['obs'])+'_'+\
                       str(eff_key))+'\n')
                 file.write(eff_bnd.tocpp('MAbool', \
                     'bnd_'+str(value['id_true']) + '_' + str(value['obs'])+'_'+\
                       str(eff_key)) + '\n')
        file.write('#endif')

    ## Reconstruction efficiencies and bounds
    def WriteNewRecoEfficiencies(self,file):
        file.write('#ifndef RECO_H_INCLUDED\n')
        file.write('#define RECO_H_INCLUDED\n')
        file.write('#include <cmath>\n')
        file.write('#include <math.h>\n')
        file.write('#include <iostream>\n')
        for key, value in self.fastsim.reco.rules.items():
            for eff_key in value['efficiencies'].keys():
                 eff_fnc = value['efficiencies'][eff_key]['function']
                 eff_bnd = value['efficiencies'][eff_key]['bounds'  ]
                 file.write(eff_fnc.tocpp('MAdouble64', \
                     'reco_'+str(value['id_reco']) + '_' + str(eff_key))+'\n')
                 file.write(eff_bnd.tocpp('MAbool', \
                     'reco_bnd_'+str(value['id_reco']) +'_'+str(eff_key)) + '\n')
        file.write('#endif')


