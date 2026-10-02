////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//
//  This file is part of MadAnalysis 5.
//  Official website: <https://github.com/MadAnalysis/madanalysis5>
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

/**
 * @file SmearerBase.h
 * @brief Base class of the SFS smearers and of the particle propagator.
 */

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

    /**
     * @brief Base class of the Simplified Fast Simulation (SFS) smearers.
     *
     * The default methods return the input particle unchanged. MadAnalysis 5 generates
     * the class NewSmearer (new_smearer_reco.h/cpp) from the `define smearer`,
     * `define reco_efficiency` and `define jes|scaling` commands; it overrides
     * SetParameters() and the object-specific smearing methods.
     *
     * Lengths are in mm, the magnetic field in tesla.
     */
    class SmearerBase
    {
        //---------------------------------------------------------------------------------
        //                              private data members
        //---------------------------------------------------------------------------------
    private:
        /** @brief Speed of light [m/s] (c_) and pi (pi_), set by Initialize(). */
        MAdouble64 c_;
        MAdouble64 pi_;

        /** @brief Length unit (mm = 1, cm = 0.1). */
        MAfloat32 length_unit_;

        //---------------------------------------------------------------------------------
        //                            protected data members
        //---------------------------------------------------------------------------------
    protected:
        /** @brief Output of the last smearing (reset for each object). */
        MCParticleFormat output_;

        /** @brief Magnetic field along the beam axis [T]. */
        MAdouble64 Bz_;

        /** @brief Radius of the tracker cylinder (currently unused). */
        MAdouble64 Radius_;

        /** @brief Half-length of the tracker cylinder (currently unused). */
        MAdouble64 HalfLength_;

        /** @brief Flags switching on the smearing of each object type and the particle propagator. */
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
        /** @brief Constructor. */
        SmearerBase() {}

        /** @brief Destructor. */
        virtual ~SmearerBase() {}

        /**
         * @brief Accessor to the magnetic field.
         *
         * @return the magnetic field along the beam axis [T].
         */
        const MAdouble64 Bz() const { return Bz_; }

        /**
         * @brief Set the length unit.
         *
         * @param val length unit (mm = 1, cm = 0.1).
         */
        void SetLengthUnit(MAfloat32 val) { length_unit_ = val; }

        /**
         * @brief Initialise the smearer (parameters, banner, constants).
         *
         * @param base true for the default (no-smearing) smearer: the SFS banner is not printed.
         */
        void Initialize(MAbool base = false)
        {
            SetParameters();
            // NOTE: any length unit set before with SetLengthUnit() is overwritten here.
            length_unit_ = 1.0;
            if (!base)
            {
                PrintHeader();
            }
            PrintDebug();
            output_.Reset();
            c_ = 2.99792458E+8; // [m/s]
            pi_ = 3.14159265;
        }

        /**
         * @brief Smear a particle with the method corresponding to its type.
         *
         * @param part particle to smear.
         * @param smearerID type of smearing: 21 jet, 15 hadronic tau, 13 muon, 11 electron, 22 photon,
         *        0 jet constituent, -1 track (any other value: no smearing).
         * @return the smeared particle.
         */
        MCParticleFormat Execute(const MCParticleFormat *part, MAint32 smearerID)
        {
            // Clearing the output vector
            output_.Reset();

            if (smearerID == 21)
                output_ = JetSmearer(part);
            else if (smearerID == 15)
                output_ = TauSmearer(part);
            else if (smearerID == 13)
                output_ = MuonSmearer(part);
            else if (smearerID == 11)
                output_ = ElectronSmearer(part);
            else if (smearerID == 22)
                output_ = PhotonSmearer(part);
            else if (smearerID == 0)
                output_ = ConstituentSmearer(part);
            else if (smearerID == -1)
                output_ = TrackSmearer(part);
            else
            {
                WARNING << "Unknown smearing method" << endmsg;
                WARNING << "Smearing skipped for PDG-ID : " << smearerID << endmsg;
                SetDefaultOutput(part, output_);
            }
            return output_;
        }

        /**
         * @brief Copy a particle (momentum, decay vertex and displacement observables) to an output object.
         *
         * If the propagator is off and the particle has a mother, the displacement
         * observables are recomputed with SetDisplacementObservables().
         *
         * @param part input particle.
         * @param output output object (reset first).
         */
        void SetDefaultOutput(const MCParticleFormat *part, MCParticleFormat &output)
        {
            output.Reset();
            output.momentum().SetPxPyPzE(part->px(), part->py(), part->pz(), part->e());
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

        /**
         * @brief Compute the displacement observables (d0, dz, closest approach) for a straight-line trajectory.
         *
         * @param part input particle (its first mother gives the production vertex).
         * @param output output object to fill.
         */
        void SetDisplacementObservables(const MCParticleFormat *, MCParticleFormat &);

        /** @brief Set the parameters of the smearer (overridden by NewSmearer; by default nothing is smeared). */
        virtual void SetParameters()
        {
            Bz_ = 1.0e-9;
            Radius_ = 1.0e+99;
            HalfLength_ = 1.0e+99;
            ParticlePropagator_ = false;
            MuonSmearer_ = false;
            ElectronSmearer_ = false;
            PhotonSmearer_ = false;
            TauSmearer_ = false;
            JetSmearer_ = false;
        }

        // For all methods below, the only relevant part of the output object is the momentum
        // The reset allows to clear the left-over from the previous object

        /**
         * @brief Smear an electron (default: no smearing).
         *
         * @param part electron to smear.
         * @return the smeared electron (only the kinematics and displacement are relevant).
         */
        virtual MCParticleFormat ElectronSmearer(const MCParticleFormat *part)
        {
            SetDefaultOutput(part, output_);
            return output_;
        }
        /**
         * @brief Is the electron smearing switched on?
         *
         * @return true if it is on.
         */
        MAbool isElectronSmearerOn() { return ElectronSmearer_; }

        /**
         * @brief Smear a muon (default: no smearing).
         *
         * @param part muon to smear.
         * @return the smeared muon (only the kinematics and displacement are relevant).
         */
        virtual MCParticleFormat MuonSmearer(const MCParticleFormat *part)
        {
            SetDefaultOutput(part, output_);
            return output_;
        }
        /**
         * @brief Is the muon smearing switched on?
         *
         * @return true if it is on.
         */
        MAbool isMuonSmearerOn() { return MuonSmearer_; }

        /**
         * @brief Smear a hadronic tau (default: no smearing).
         *
         * @param part hadronic tau to smear.
         * @return the smeared hadronic tau (only the kinematics and displacement are relevant).
         */
        virtual MCParticleFormat TauSmearer(const MCParticleFormat *part)
        {
            SetDefaultOutput(part, output_);
            return output_;
        }
        /**
         * @brief Is the hadronic tau smearing switched on?
         *
         * @return true if it is on.
         */
        MAbool isTauSmearerOn() { return TauSmearer_; }

        /**
         * @brief Smear a photon (default: no smearing).
         *
         * @param part photon to smear.
         * @return the smeared photon (only the kinematics and displacement are relevant).
         */
        virtual MCParticleFormat PhotonSmearer(const MCParticleFormat *part)
        {
            SetDefaultOutput(part, output_);
            return output_;
        }
        /**
         * @brief Is the photon smearing switched on?
         *
         * @return true if it is on.
         */
        MAbool isPhotonSmearerOn() { return PhotonSmearer_; }

        /**
         * @brief Smear a jet (default: no smearing).
         *
         * @param part jet to smear.
         * @return the smeared jet (only the kinematics and displacement are relevant).
         */
        virtual MCParticleFormat JetSmearer(const MCParticleFormat *part)
        {
            SetDefaultOutput(part, output_);
            return output_;
        }

        /**
         * @brief Is the jet smearing switched on?
         *
         * @return true if it is on.
         */
        MAbool isJetSmearerOn() { return JetSmearer_; }

        /**
         * @brief Smear a jet constituent (default: no smearing).
         *
         * @param part constituent to smear.
         * @return the smeared constituent.
         */
        virtual MCParticleFormat ConstituentSmearer(const MCParticleFormat *part)
        {
            SetDefaultOutput(part, output_);
            return output_;
        }

        /**
         * @brief Smear a track (default: no smearing).
         *
         * @param part track to smear.
         * @return the smeared track.
         */
        virtual MCParticleFormat TrackSmearer(const MCParticleFormat *part)
        {
            SetDefaultOutput(part, output_);
            return output_;
        }

        //================================//
        //   Particle Propagator Method   //
        //================================//

        /**
         * @brief Is the particle propagator switched on?
         *
         * @return true if it is on.
         */
        MAbool isPropagatorOn() { return ParticlePropagator_; }

        /**
         * @brief Propagate a particle from the decay vertex of its mother (helix in the magnetic field).
         *
         * The momentum is rotated like the mother's, and the displacement observables and the
         * decay vertex are updated.
         *
         * @param part particle to propagate (modified in place).
         */
        void ParticlePropagator(MCParticleFormat *part);

        /**
         * @brief Smear a value with a Gaussian distribution.
         *
         * @param sigma standard deviation (the comment in the source says 'variance').
         * @param property central value.
         * @return the smeared value.
         */
        MAdouble64 Gaussian(MAdouble64, MAdouble64);

        /** @brief Print the parameters of the smearer (debug level). */
        void PrintDebug()
        {
            DEBUG << "   -> Smearer Input Values:" << endmsg;
            DEBUG << "   * Magnetic field [T] = " << Bz_ << endmsg;
            //                DEBUG << "   * Radius [m]         = " << Radius_ << endmsg;
            //                DEBUG << "   * Half Length [m]    = " << HalfLength_ << endmsg;

            std::string module = ParticlePropagator_ ? "on" : "off";
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

        /** @brief Print the SFS banner with the references to cite. */
        void PrintHeader()
        {
            INFO << "   <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>" << endmsg;
            INFO << "   <>                                                              <>" << endmsg;
            INFO << "   <>     Simplified Fast Detector Simulation in MadAnalysis 5     <>" << endmsg;
            INFO << "   <>            Please cite arXiv:2006.09387 [hep-ph]             <>" << endmsg;
            INFO << "   <>                    and arXiv:2303.03427 [hep-ph]             <>" << endmsg;
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
