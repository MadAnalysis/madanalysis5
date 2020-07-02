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
            ///
            /// REFERENCE:  - W. Hoermann and G. Derflinger (1990):
            ///              The ACR Method for generating normal random variables,
            ///              OR Spektrum 12 (1990), 181-185.
            ///
            /// Implementation taken from
            /// UNURAN (c) 2000  W. Hoermann & J. Leydold, Institut f. Statistik, WU Wien
            /// Taken from ROOT: https://root.cern.ch/doc/master/TRandom_8cxx_source.html#l00263
            MAdouble64 Gaussian(MAdouble64 sigma, MAdouble64 property)
            {
              MAdouble64 kC1 = 1.448242853;          MAdouble64 kAs = 0.8853395638;
              MAdouble64 kC2 = 3.307147487;          MAdouble64 kBs = 0.2452635696;
              MAdouble64 kC3 = 1.46754004;           MAdouble64 kCs = 0.2770276848;
              MAdouble64 kD1 = 1.036467755;          MAdouble64 kB  = 0.5029324303;
              MAdouble64 kD2 = 5.295844968;          MAdouble64 kX0 = 0.4571828819;
              MAdouble64 kD3 = 3.631288474;          MAdouble64 kYm = 0.187308492 ;
              MAdouble64 kHm = 0.483941449;          MAdouble64 kS  = 0.7270572718 ;
              MAdouble64 kZm = 0.107981933;          MAdouble64 kT  = 0.03895759111;
              MAdouble64 kHp = 4.132731354;          MAdouble64 kHp1 = 3.132731354;
              MAdouble64 kZp = 18.52161694;          MAdouble64 kHzm = 0.375959516;
              MAdouble64 kPhln = 0.4515827053;       MAdouble64 kHzmp = 0.591923442;
              MAdouble64 kHm1 = 0.516058551;

              MAdouble64 result, rn,x,y,z;
              do
              {
                y = RANDOM->flat();
                if (y>kHm1) { result = kHp*y-kHp1; break; }
                else if (y<kZm) { rn = kZp*y-1; result = (rn>0) ? (1+rn) : (-1+rn); break; }
                else if (y<kHm)
                {
                  rn = RANDOM->flat();
                  rn = rn-1+rn;
                  z = (rn>0) ? 2-rn : -2-rn;
                  if ((kC1-y)*(kC3+fabs(z))<kC2) { result = z; break; }
                  else
                  {
                    x = rn*rn;
                    if ((y+kD1)*(kD3+x)<kD2) { result = rn; break; }
                    else if (kHzmp-y<exp(-(z*z+kPhln)/2)) { result = z; break; }
                    else if (y+kHzm<exp(-(x+kPhln)/2)) { result = rn; break; }
                  }
                }
                while (1)
                {
                  x = RANDOM->flat();
                  y = kYm * RANDOM->flat();
                  z = kX0 - kS*x - y;
                  if (z>0) rn = 2+y/x;
                  else { x = 1-x;y = kYm-y;rn = -(2+y/x); }
                  if ((y-kAs+x)*(kCs+x)+kBs<0) { result = rn; break; }
                  else if (y<x+kT)
                    if (rn*rn<4*(kB-log(x))) { result = rn; break; }
                }
              } while(0);
              return property + sigma * result;
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
