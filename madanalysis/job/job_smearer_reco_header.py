################################################################################
#  
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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
        self.electron_smearing    = False
        self.muon_smearing        = False
        self.photon_smearing      = False
        self.tau_smearing         = False
        self.jet_smearing         = False
        self.constituent_smearing = False
        self.track_smearing       = False
        self.propagator           = fastsim.propagator
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


    ## Writing NewSmearer.h
    def WriteNewSmearerRecoHeader(self, file):
        file.write('#ifndef NEW_SMEARER_H\n')
        file.write('#define NEW_SMEARER_H\n')
        file.write('// SampleAnalyzer headers\n')
        file.write('#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"\n')
        file.write('#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"\n')
        file.write('#include "SampleAnalyzer/Commons/Base/SmearerBase.h"\n')
        file.write('namespace MA5\n')
        file.write('{\n')
        file.write('  class NewSmearer: public SmearerBase\n')
        file.write('  {\n')
        file.write('    public :\n')
        file.write('      /// Constructor without argument\n')
        file.write('      NewSmearer()\n')
        file.write('      {\n')
        file.write('          output_.Reset();\n')
        file.write('      }\n')
        file.write('      /// Destructor\n')
        file.write('      ~NewSmearer() {}\n\n')
        file.write('      /// Smearer methods: \n')
        file.write('      /// Set parameters\n')
        file.write('      void SetParameters()\n')
        file.write('      {\n')
        file.write('          // Magnetic field along beam axis\n')
        file.write('          Bz_                 = {:.6E};\n'.format(self.fastsim.mag_field))
        # Not in use at the moment
        # file.write('          // Tracker cylinder radius\n')
        # file.write('          Radius_             = {:.6E};\n'.format(self.fastsim.radius))
        # file.write('          // Tracker half length\n')
        # file.write('          HalfLength_         = {:.6E};\n'.format(self.fastsim.half_length))
        file.write('          // Code-efficiency parameters\n')
        file.write('          ParticlePropagator_ = '+(self.propagator*'true'       + (not self.propagator)*'false')+';\n')
        file.write('          MuonSmearer_        = '+(self.muon_smearing*'true'    + (not self.muon_smearing)*'false')+';\n')
        file.write('          ElectronSmearer_    = '+(self.electron_smearing*'true'+ (not self.electron_smearing)*'false')+';\n')
        file.write('          PhotonSmearer_      = '+(self.photon_smearing*'true'  + (not self.photon_smearing)*'false')+';\n')
        file.write('          TauSmearer_         = '+(self.tau_smearing*'true'     + (not self.tau_smearing)*'false')+';\n')
        file.write('          JetSmearer_         = '+(self.jet_smearing*'true'     + (not self.jet_smearing)*'false')+';\n')
        file.write('      }\n\n')
        if self.electron_smearing:
            file.write('      /// Electron smearing method\n')
            file.write('      MCParticleFormat ElectronSmearer(const MCParticleFormat * part);\n\n')
        if self.muon_smearing:
            file.write('      /// Muon smearing method\n')
            file.write('      MCParticleFormat MuonSmearer(const MCParticleFormat * part);\n\n')
        if self.photon_smearing:
            file.write('      /// Photon smearing method\n')
            file.write('      MCParticleFormat PhotonSmearer(const MCParticleFormat * part);\n\n')
        if self.tau_smearing:
            file.write('      /// Hadronic Tau smearing method\n')
            file.write('      MCParticleFormat TauSmearer(const MCParticleFormat * part);\n\n')
        if self.jet_smearing:
            file.write('      /// Jet smearing method\n')
            file.write('      MCParticleFormat JetSmearer(const MCParticleFormat * part);\n\n')
        if self.constituent_smearing:
            file.write('      /// Jet Constituent smearing method\n')
            file.write('      MCParticleFormat ConstituentSmearer(const MCParticleFormat * part);\n\n')
        if self.track_smearing:
            file.write('      /// Track smearing method\n')
            file.write('      MCParticleFormat TrackSmearer(const MCParticleFormat * part);\n\n')
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
    def WriteNewRecoEfficiencies(self,file, constituents=False):
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


    ## scales and bounds
    def WriteNewScales(self,file):
        file.write('#ifndef SC_H_INCLUDED\n')
        file.write('#define SC_H_INCLUDED\n')
        file.write('#include <cmath>\n')
        file.write('#include <math.h>\n')
        file.write('#include <iostream>\n')
        for key, value in self.fastsim.scaling.rules.items():
            for eff_key in value['efficiencies'].keys():
                 eff_fnc = value['efficiencies'][eff_key]['function']
                 eff_bnd = value['efficiencies'][eff_key]['bounds'  ]
                 file.write(eff_fnc.tocpp('MAdouble64', 'scale_'+\
                                          str(value['id_true']) + '_' + str(value['obs'])+'_'+\
                                          str(eff_key))+'\n')
                 file.write(eff_bnd.tocpp('MAbool', \
                                          'scale_bnd_'+str(value['id_true']) +\
                                          '_' + str(value['obs'])+'_'+\
                                          str(eff_key)) + '\n')
        file.write('#endif')

