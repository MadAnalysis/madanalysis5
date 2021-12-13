////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2020 Jack Araz, Eric Conte & Benjamin Fuks
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
#include "SampleAnalyzer/Commons/Service/PDGService.h"
#include "SampleAnalyzer/Commons/Service/RandomService.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"


namespace MA5
{

    class SmearerBase
    {
        //---------------------------------------------------------------------------------
        //                              private data members
        //---------------------------------------------------------------------------------
        private:
            // Set constants: speed of light & pi
            MAdouble64 c_;
            MAdouble64 pi_;

        //---------------------------------------------------------------------------------
        //                            protected data members
        //---------------------------------------------------------------------------------
        protected:

            // Creating a container for the smeared output
            MCParticleFormat output_;

            // Magnetic field along beam axis
            MAdouble64 Bz_;

            // Tracker cylinder radius
            MAdouble64 Radius_;

            // Tracker half length
            MAdouble64 HalfLength_;

            // To optimise the code running time
            MAbool MuonSmearer_;
            MAbool ElectronSmearer_;
            MAbool PhotonSmearer_;
            MAbool TauSmearer_;
            MAbool JetSmearer_;
            MAbool ParticlePropagator_;

        //---------------------------------------------------------------------------------
        //                              public data members
        //---------------------------------------------------------------------------------
        public:
            //---------------------------------------------------------------------------------
            //                                method members
            //---------------------------------------------------------------------------------
            /// Constructor without argument
            SmearerBase() { }

            /// Destructor
            virtual ~SmearerBase() {}


            /// Accessors
            const MAdouble64 Bz() const { return Bz_; }

            /// Initialisation
            void Initialize(MAbool base=false)
            {
                SetParameters();
                if (!base) { PrintHeader(); }
                PrintDebug();
                output_.Reset();
                c_  = 2.99792458E+8; // [m/s]
                pi_ = 3.14159265;
            }

            /// Matching general method
            MCParticleFormat Execute(const MCParticleFormat * part, MAint32 smearerID)
            {
                // Clearing the output vector
                output_.Reset();

                if      (smearerID == 21) output_ = JetSmearer(part);
                else if (smearerID == 15) output_ = TauSmearer(part);
                else if (smearerID == 13) output_ = MuonSmearer(part);
                else if (smearerID == 11) output_ = ElectronSmearer(part);
                else if (smearerID == 22) output_ = PhotonSmearer(part);
                else if (smearerID == 0)  output_ = ConstituentSmearer(part);
                else if (smearerID == -1) output_ = TrackSmearer(part);
                else
                {
                    WARNING << "Unknown smearing method" << endmsg;
                    WARNING << "Smearing skipped for PDG-ID : "<< smearerID << endmsg;
                    SetDefaultOutput(part,output_);
                }
                return output_;
            }

            // Copy part to output
            void SetDefaultOutput(const MCParticleFormat * part, MCParticleFormat & output)
            {
                output.Reset();
                output.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());
                output.setDecayVertex(part->decay_vertex());
                if (!isPropagatorOn() && part->mothers().size() > 0)
                  SetDisplacementObservables(part, output);
                else
                {
                    output.setClosestApproach(part->closest_approach());
                    output.setD0(part->d0());
                    output.setDZ(part->dz());
                    output.setD0Approx(part->d0_approx());
                    output.setDZApprox(part->dz_approx());
                }
            }

            // Calculate displacement observables without magnetic field
            void SetDisplacementObservables(const MCParticleFormat*, MCParticleFormat &);

            // Set parameters
            virtual void SetParameters()
            {
                Bz_                 = 1.0e-9;
                Radius_             = 1.0e+99;
                HalfLength_         = 1.0e+99;
                ParticlePropagator_ = false;
                MuonSmearer_        = false;
                ElectronSmearer_    = false;
                PhotonSmearer_      = false;
                TauSmearer_         = false;
                JetSmearer_         = false;
            }

            // For all methods below, the only relevant part of the output object is the momentum
            // The reset allows to clear the left-over from the previous object

            // Electron smearing method
            virtual MCParticleFormat ElectronSmearer(const MCParticleFormat * part)
            {
                SetDefaultOutput(part,output_);
                return output_;
            }
            // Check whether electron smearing is on (code-efficiency-related)
            MAbool isElectronSmearerOn() {return ElectronSmearer_;}

            // Muon smearing method
            virtual MCParticleFormat MuonSmearer(const MCParticleFormat * part)
            {
                SetDefaultOutput(part,output_);
                return output_;
            }
            // Check whether muon smearing is on (code-efficiency-related)
            MAbool isMuonSmearerOn() {return MuonSmearer_;}

            // Hadronic Tau smearing method
            virtual MCParticleFormat TauSmearer(const MCParticleFormat * part)
            {
                SetDefaultOutput(part,output_);
                return output_;
            }
            // Check whether tau smearing is on (code-efficiency-related)
            MAbool isTauSmearerOn() {return TauSmearer_;}

            // Photon smearing method
            virtual MCParticleFormat PhotonSmearer(const MCParticleFormat * part)
            {
                SetDefaultOutput(part,output_);
                return output_;
            }
            // Check whether photon smearing is on (code-efficiency-related)
            MAbool isPhotonSmearerOn() {return PhotonSmearer_;}

            // Jet smearing method
            virtual MCParticleFormat JetSmearer(const MCParticleFormat * part)
            {
                SetDefaultOutput(part,output_);
                return output_;
            }

            // Check whether jet smearing is on (code-efficiency-related)
            MAbool isJetSmearerOn() {return JetSmearer_;}

            // Jet Constituent smearing method
            virtual MCParticleFormat ConstituentSmearer(const MCParticleFormat * part)
            {
                SetDefaultOutput(part,output_);
                return output_;
            }

            // Track smearing method
            virtual MCParticleFormat TrackSmearer(const MCParticleFormat * part)
            {
                SetDefaultOutput(part,output_);
                return output_;
            }


            //================================//
            //   Particle Propagator Method   //
            //================================//

            // Check whether particle propagator is on (code-efficiency-related)
            MAbool isPropagatorOn() {return ParticlePropagator_;}

            // Particle propagator method
            void ParticlePropagator(MCParticleFormat * part);

            /// Smearer Gaussian function
            MAdouble64 Gaussian(MAdouble64, MAdouble64);

            /// Smearer Cumulative distribution function TO BE TESTED
//            MAdouble64 Sigmoid(MAdouble64 sigma, MAdouble64 property)
//            {
//                MAdouble64 err = erf(property/(sigma*sqrt(2.)));
//                MAdouble64 r        = RANDOM->flat();
//                MAdouble64 sign     = (r >= 0.5) * 1.0 + (r < 0.5) * (-1.0);
//                return property + sign * RANDOM->flat() * (1. + err) * 0.5;
//            }

            void PrintDebug()
            {
                DEBUG << "   -> Smearer Input Values:" << endmsg;
                DEBUG << "   * Magnetic field [T] = " << Bz_ << endmsg;
//                DEBUG << "   * Radius [m]         = " << Radius_ << endmsg;
//                DEBUG << "   * Half Length [m]    = " << HalfLength_ << endmsg;

                std::string module  = ParticlePropagator_ ? "on" : "off";
                DEBUG << "       * Propagator         = " << module << endmsg;

                module = MuonSmearer_ ? "on" : "off";
                DEBUG << "      * Muon Smearer       = " << module << endmsg;

                module = ElectronSmearer_ ? "on" : "off";
                DEBUG << "      * Electron Smearer   = " << module << endmsg;

                module = PhotonSmearer_ ? "on" : "off";
                DEBUG << "      * Photon Smearer     = " << module << endmsg;

                module = TauSmearer_ ? "on" : "off";
                DEBUG << "      * Tau Smearer        = " << module << endmsg;

                module = JetSmearer_ ? "on" : "off";
                DEBUG << "      * Jet Smearer        = " << module << endmsg;
            }


            void PrintHeader()
            {
                INFO << "   <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>" << endmsg;
                INFO << "   <>                                                              <>" << endmsg;
                INFO << "   <>     Simplified Fast Detector Simulation in MadAnalysis 5     <>" << endmsg;
                INFO << "   <>            Please cite arXiv:2006.09387 [hep-ph]             <>" << endmsg;
                if (isPropagatorOn()) // cite particle propagator module
                {
                    INFO << "   <>                                                              <>" << endmsg;
                    INFO << "   <>            Particle Propagation in MadAnalysis 5             <>" << endmsg;
                    INFO << "   <>            Please cite arXiv:2112.05163 [hep-ph]             <>" << endmsg;
                    INFO << "   <>                                                              <>" << endmsg;
                }
                INFO << "   <>         https://madanalysis.irmp.ucl.ac.be/wiki/SFS          <>" << endmsg;
                INFO << "   <>                                                              <>" << endmsg;
                INFO << "   <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>" << endmsg;
            }
    };
}

#endif
