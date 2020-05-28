////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://launchpad.net/madanalysis5>
//  
//  MadAnalysis 5 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  MadAnalysis 5 is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
//  
////////////////////////////////////////////////////////////////////////////////


#ifndef SMEARERBASE_H
#define SMEARERBASE_H

// STL headers
#include <algorithm>
#include <cmath>

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Service/RandomService.h"

namespace MA5
{

    class SmearerBase
    {
        //---------------------------------------------------------------------------------
        //                                 data members
        //---------------------------------------------------------------------------------
        private:

            // Creating a container for the smeared output
            MCParticleFormat output_;

        //---------------------------------------------------------------------------------
        //                                method members
        //---------------------------------------------------------------------------------
        public:
            /// Constructor without argument
            SmearerBase()
            {}
            /// Destructor
            virtual ~SmearerBase()
            {}

            /// Matching general method
            MCParticleFormat Execute(const MCParticleFormat * part, MAuint32 absid)
            {
                output_.Reset();
                if      (absid == 21) output_ = JetSmearer(part);
                else if (absid == 15) output_ = TauSmearer(part);
                else if (absid == 13) output_ = MuonSmearer(part);
                else if (absid == 11) output_ = ElectronSmearer(part);
                else if (absid == 22) output_ = PhotonSmearer(part);
                else if (absid == 0)  output_ = ConstituentSmearer(part);
                else
                {
                    WARNING << "Unknown smearing method" << endmsg;
                    WARNING << "Smearing skipped for PDGID : "<< absid << endmsg;
                    output_.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());
                    return output_;
                }
                return output_;
            }

            // For all methods below, the only relevant part of the output object is the momentum
            // The reset allows to clear the left-over from the previous object

            // Electron smearing method
            virtual MCParticleFormat ElectronSmearer(const MCParticleFormat * part)
            {
                output_.Reset();
                output_.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());
                return output_;
            }
            // Check whether electron smearing is on (code-efficiency-related)
            virtual MAbool isElectronSmearerOn() {return false;}

            // Muon smearing method
            virtual MCParticleFormat MuonSmearer(const MCParticleFormat * part)
            {
                output_.Reset();
                output_.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());
                return output_;
            }
            // Check whether muon smearing is on (code-efficiency-related)
            virtual MAbool isMuonSmearerOn() {return false;}

            // Hadronic Tau smearing method
            virtual MCParticleFormat TauSmearer(const MCParticleFormat * part)
            {
                output_.Reset();
                output_.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());
                return output_;
            }
            // Check whether tau smearing is on (code-efficiency-related)
            virtual MAbool isTauSmearerOn() {return false;}

            // Photon smearing method
            virtual MCParticleFormat PhotonSmearer(const MCParticleFormat * part)
            {
                output_.Reset();
                output_.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());
                return output_;
            }
            // Check whether photon smearing is on (code-efficiency-related)
            virtual MAbool isPhotonSmearerOn() {return false;}

            // Jet smearing method
            virtual MCParticleFormat JetSmearer(const MCParticleFormat * part)
            {
                output_.Reset();
                output_.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());
                return output_;
            }

            // Check whether jet smearing is on (code-efficiency-related)
            virtual MAbool isJetSmearerOn() {return false;}

            // Jet Constituent smearing method
            virtual MCParticleFormat ConstituentSmearer(const MCParticleFormat * part)
            {
                output_.Reset();
                output_.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());
                return output_;
            }

            /// Smearer Gaussian function 
            /// (one smears the quantity 'property' with a Gaussian of variance 'sigma')
            MAdouble64 Gaussian(MAdouble64 sigma, MAdouble64 property)
            {
                MAdouble64 PI = 3.141592653589793;
                MAdouble64 N  = 1.0 / (sigma * sqrt(2.0 * PI));
                if (N > 1e20)
                {
                    WARNING << "Infinite normalization found in a smearing function" << endmsg;
                    WARNING << "Smearing ignored." << endmsg;
                    return property;
                }
                MAdouble64 gaussian = N * exp( -pow( property / sigma, 2.0) * 0.5 );
                MAdouble64 r        = RANDOM->flat();
                MAdouble64 sign     = (r >= 0.5) * 1.0 + (r < 0.5) * (-1.0);
                return property + sign * RANDOM->flat() * gaussian * 0.5;
            }

            /// Smearer Cumulative distribution function TO BE TESTED
//            MAdouble64 Sigmoid(MAdouble64 sigma, MAdouble64 property)
//            {
//                MAdouble64 err = erf(property/(sigma*sqrt(2.)));
//                MAdouble64 r        = RANDOM->flat();
//                MAdouble64 sign     = (r >= 0.5) * 1.0 + (r < 0.5) * (-1.0);
//                return property + sign * RANDOM->flat() * (1. + err) * 0.5;
//            }

            virtual void Print()
            {
                DEBUG << "Default Smearer" << endmsg;
            }
    };
}

#endif
