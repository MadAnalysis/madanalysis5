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
 * @file MCParticleFormat.h
 * @brief Monte Carlo (generator-level) particle.
 */

#ifndef MCParticleFormat_h
#define MCParticleFormat_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/ParticleBaseFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"
#include "SampleAnalyzer/Commons/Vector/MABoost.h"


namespace MA5
{

    class LHEReader;
    class LHCOReader;
    class STDHEPreader;
    class HEPMCReader;
    class ROOTReader;
    class LHEWriter;
    class MergingPlots;
    class DelphesTreeReader;
    class DelphesMA5tuneTreeReader;
    class SmearerBase;

    /**
     * @brief Particle of the Monte Carlo event record.
     *
     * In addition to the kinematics, it stores the PDG code, the status code, the spin,
     * the mother/daughter links (pointers into MCEventFormat::particles()) and the
     * decay vertex used by the SFS particle propagator.
     */
    class MCParticleFormat : public ParticleBaseFormat
    {
        friend class LHEReader;
        friend class LHCOReader;
        friend class STDHEPreader;
        friend class HEPMCReader;
        friend class ROOTReader;
        friend class LHEWriter;
        friend class MergingPlots;
        friend class DelphesTreeReader;
        friend class DelphesMA5tuneTreeReader;
        friend class SmearerBase;

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    private:

        /** @brief PDG code of the particle. */
        MAint32 pdgid_;

        /** @brief Status code (LHE: -1 initial state, 2 intermediate state, 1 final state; generator-specific otherwise). */
        MAint16 statuscode_;

        /** @brief Cosine of the angle between the spin and the three-momentum, in the laboratory frame. */
        MAfloat32 spin_;

        /** @brief Is the particle coming from pile-up? */
        MAbool isPU_;

        /** @brief Daughter particles. */
        std::vector<MCParticleFormat*> daughters_;

        /** @brief Mother particles. */
        std::vector<MCParticleFormat*> mothers_;

        /** @brief Decay vertex (x, y, z [mm] and c*tau [mm] stored as the time component). */
        MALorentzVector decay_vertex_;

        /** @brief Rotation angle of the momentum due to the propagation in the magnetic field. */
        MAdouble64 momentum_rotation_;


        // -------------------------------------------------------------
        //                      method members
        // -------------------------------------------------------------
    public :

        /** @brief Constructor (all members reset). */
        MCParticleFormat()
        { Reset(); }

        /** @brief Destructor. */
        virtual ~MCParticleFormat()
        {}

        /**
         * @brief Constructor from the momentum components.
         *
         * @param px x component.
         * @param py y component.
         * @param pz z component.
         * @param e energy.
         */
        MCParticleFormat(MAfloat64 px, MAfloat64 py, MAfloat64 pz, MAfloat64 e)
        { Reset(); momentum_.SetPxPyPzE(px,py,pz,e); }

        /** @brief Reset all members. */
        virtual void Reset()
        {
            momentum_.SetPxPyPzE(0.,0.,0.,0.);
            spin_       = 0.;
            pdgid_      = 0;
            statuscode_ = 0;
            isPU_       = false;
            daughters_.clear();
            mothers_.clear();
            decay_vertex_.clear();
            // NOTE: d0_approx_ and dz_approx_ are not reset.
            closest_approach_.clear();
            d0_         = 0.;
            dz_         = 0.;
            momentum_rotation_ = 0.;
        }

        /** @brief Print the particle properties. */
        virtual void Print() const
        {
            INFO << "momentum=(" << /*set::setw(8)*/"" << std::left << momentum_.Px()
                 << ", "<</*set::setw(8)*/"" << std::left << momentum_.Py()
                 << ", "<</*set::setw(8)*/"" << std::left << momentum_.Pz()
                 << ", "<</*set::setw(8)*/"" << std::left << momentum_.E() << ") - " << endmsg;
            INFO << "ctau=" << /*set::setw(8)*/"" << std::left << decay_vertex_.T() << " - "
                 << "spin=" << /*set::setw(8)*/"" << std::left << spin_ << " - "
                 << "d0=" << /*set::setw(8)*/"" << std::left << d0_ << " - "
                 << "dz=" << /*set::setw(8)*/"" << std::left << dz_ << " - "
                 << "PDGID=" << /*set::setw(8)*/"" << std::left << pdgid_ << " - "
                 << "StatusCode=" << /*set::setw(3)*/"" << std::left
                 << static_cast<signed int>(statuscode_) << " - " << endmsg;
            INFO << "Number of mothers=" << mothers_.size() << " - "
                 << "Number of daughters=" << daughters_.size() << endmsg;
        }

        /**
         * @brief Is the particle coming from pile-up?
         *
         * @return true for a pile-up particle.
         */
        const MAbool& isPU()  const {return isPU_;}
        /**
         * @brief Accessor to the proper decay length (time component of the decay vertex).
         *
         * @return c*tau [mm].
         */
        const MAfloat64& ctau() const {return decay_vertex_.T();}
        /**
         * @brief Accessor to the spin (cosine of the angle with the momentum).
         *
         * @return the spin.
         */
        const MAfloat32& spin() const {return spin_;}
        /**
         * @brief Accessor to the PDG code.
         *
         * @return the PDG code.
         */
        const MAint32& pdgid()  const {return pdgid_;}
        /**
         * @brief Accessor to the status code.
         *
         * @return the status code.
         */
        const MAint16& statuscode() const {return statuscode_;}

        /**
         * @brief Accessor to the daughters (read-only).
         *
         * @return the daughters.
         */
        const std::vector<MCParticleFormat*>& daughters() const {return daughters_;}

        /**
         * @brief Accessor to the daughters.
         *
         * @return the daughters.
         */
        std::vector<MCParticleFormat*>& daughters() {return daughters_;}

        /**
         * @brief Accessor to the mothers (read-only).
         *
         * @return the mothers.
         */
        const std::vector<MCParticleFormat*>& mothers() const {return mothers_;}

        /**
         * @brief Accessor to the mothers.
         *
         * @return the mothers.
         */
        std::vector<MCParticleFormat*>& mothers() {return mothers_;}

        /**
         * @brief Accessor to the decay vertex.
         *
         * @return the decay vertex (x, y, z, c*tau) [mm].
         */
        const MALorentzVector& decay_vertex() const {return decay_vertex_;}

        /**
         * @brief Accessor to the rotation angle of the momentum.
         *
         * @return the angle [rad].
         */
        const MAdouble64& momentum_rotation() const {return momentum_rotation_;}


        /**
         * @brief Set the decay vertex.
         *
         * @param v decay vertex (x, y, z, c*tau) [mm].
         */
        void setDecayVertex(const MALorentzVector& v) {decay_vertex_=v;}
        /**
         * @brief Set the pile-up flag.
         *
         * @param v flag.
         */
        void setIsPU(MAbool v)   {isPU_=v;}
        /**
         * @brief Set the spin.
         *
         * @param v cosine of the angle between the spin and the momentum.
         */
        void setSpin(MAfloat32 v)  {spin_=v;}
        /**
         * @brief Set the PDG code.
         *
         * @param v PDG code.
         */
        void setPdgid(MAint32 v)   {pdgid_=v;}
        /**
         * @brief Set the status code.
         *
         * @param v status code.
         */
        void setStatuscode(MAint16 v)  {statuscode_=v;}
        /**
         * @brief Set the four-momentum.
         *
         * @param v four-momentum.
         */
        void setMomentum(const MALorentzVector& v)  {momentum_=v;}
        /**
         * @brief Set the rotation angle of the momentum.
         *
         * @param v angle [rad].
         */
        void setMomentumRotation(MAdouble64 v) {momentum_rotation_=v;}

        /**
         * @brief Boost the four-momentum to the rest frame of another particle.
         *
         * @param boost reference particle (nothing is done if null).
         */
        void ToRestFrame(const MCParticleFormat* boost)
        {
            if (boost==0) return;
            ToRestFrame(*boost);
        }

        /**
         * @brief Boost the four-momentum to the rest frame of another particle.
         *
         * @param boost reference particle.
         */
        void ToRestFrame(const MCParticleFormat& boost)
        {
            MALorentzVector momentum = boost.momentum();
            momentum.SetPx(-momentum.X());
            momentum.SetPy(-momentum.Y());
            momentum.SetPz(-momentum.Z());

            MABoost convertor;
            convertor.setBoostVector(momentum);
            convertor.boost(momentum_);
        }

    };

}

#endif
