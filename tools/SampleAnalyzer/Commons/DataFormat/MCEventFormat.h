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
 * @file MCEventFormat.h
 * @brief Monte Carlo event record.
 */

#ifndef MCEventFormat_h
#define MCEventFormat_h

// STL headers
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/MCParticleFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/WeightCollection.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

namespace MA5
{

    class LHEReader;
    class LHCOReader;
    class STDHEPreader;
    class HEPMCReader;
    class LHEWriter;
    class ROOTReader;
    class DelphesTreeReader;
    class DelphesMA5tuneTreeReader;

    /**
     * @brief Monte Carlo event: generated particles, event weights and global observables.
     *
     * The global observables (MET, MHT, TET, THT, Meff) are computed from the final-state
     * particles when the event is finalised by the reader.
     */
    class MCEventFormat
    {
        friend class LHEReader;
        friend class LHCOReader;
        friend class STDHEPreader;
        friend class HEPMCReader;
        friend class ROOTReader;
        friend class LHEWriter;
        friend class DelphesTreeReader;
        friend class DelphesMA5tuneTreeReader;

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    private:
        /** @brief Process identifier, scale, couplings, PDF information (the PDF members are unused). */
        MAuint32 processId_; /// identity of the current process
        // mutable MAfloat64 weight_;             /// event weight
        MAfloat64 scale_;                      /// scale Q of the event
        MAfloat64 alphaQED_;                   /// ALPHA_em value used
        MAfloat64 alphaQCD_;                   /// ALPHA_s value used
        // NOTE: PDFscale_, x_ and xpdf_ are never initialised nor accessible.
        MAfloat64 PDFscale_;                   /// scale for PDF
        std::pair<MAfloat64, MAfloat64> x_;    /// x values
        std::pair<MAfloat64, MAfloat64> xpdf_; /// xpdf values

        /** @brief Generated particles. */
        std::vector<MCParticleFormat> particles_;

        /** @brief Missing transverse momentum. */
        MCParticleFormat MET_;

        /** @brief Missing hadronic transverse momentum. */
        MCParticleFormat MHT_;

        /** @brief Scalar sum of the transverse energies. */
        MAfloat64 TET_;

        /** @brief Scalar sum of the hadronic transverse energies. */
        MAfloat64 THT_;

        /** @brief Effective mass (THT + MET). */
        MAfloat64 Meff_;

        /** @brief Event weights (index 0 is the nominal weight). */
        WeightCollection multiweights_;

        // -------------------------------------------------------------
        //                      method members
        // -------------------------------------------------------------
    public:
        /** @brief Constructor. */
        MCEventFormat()
        {
            processId_ = 0;
            scale_ = 0.;
            alphaQED_ = 0.;
            alphaQCD_ = 0.;
            TET_ = 0.;
            THT_ = 0.;
            Meff_ = 0.;
            multiweights_.clear();
        }

        /** @brief Destructor. */
        ~MCEventFormat() {}

        /**
         * @brief Accessor to the missing transverse momentum (read-only).
         *
         * @return the MET object.
         */
        const MCParticleFormat &MET() const { return MET_; }

        /**
         * @brief Accessor to the missing hadronic transverse momentum (read-only).
         *
         * @return the MHT object.
         */
        const MCParticleFormat &MHT() const { return MHT_; }

        /**
         * @brief Accessor to the scalar sum of the transverse energies (read-only).
         *
         * @return TET.
         */
        const MAfloat64 &TET() const { return TET_; }

        /**
         * @brief Accessor to the scalar sum of the hadronic transverse energies (read-only).
         *
         * @return THT.
         */
        const MAfloat64 &THT() const { return THT_; }

        /**
         * @brief Accessor to the effective mass (read-only).
         *
         * @return Meff.
         */
        const MAfloat64 &Meff() const { return Meff_; }

        /**
         * @brief Accessor to the missing transverse momentum.
         *
         * @return the MET object.
         */
        MCParticleFormat &MET() { return MET_; }

        /**
         * @brief Accessor to the missing hadronic transverse momentum.
         *
         * @return the MHT object.
         */
        MCParticleFormat &MHT() { return MHT_; }

        /**
         * @brief Accessor to the scalar sum of the transverse energies.
         *
         * @return TET.
         */
        MAfloat64 &TET() { return TET_; }

        /**
         * @brief Accessor to the scalar sum of the hadronic transverse energies.
         *
         * @return THT.
         */
        MAfloat64 &THT() { return THT_; }

        /**
         * @brief Accessor to the effective mass.
         *
         * @return Meff.
         */
        MAfloat64 &Meff() { return Meff_; }

        /**
         * @brief Accessor to the process identifier.
         *
         * @return the identifier.
         */
        const MAuint32 &processId() const { return processId_; }

        /**
         * @brief Accessor to the nominal event weight (index 0).
         *
         * @return the weight.
         */
        const MAfloat64 &weight() const { return multiweights_.Get(0); }

        /**
         * @brief Accessor to the scale of the event.
         *
         * @return the scale.
         */
        const MAfloat64 &scale() const { return scale_; }

        /**
         * @brief Accessor to the QED coupling.
         *
         * @return alpha_QED.
         */
        const MAfloat64 &alphaQED() const { return alphaQED_; }

        /**
         * @brief Accessor to the QCD coupling.
         *
         * @return alpha_s.
         */
        const MAfloat64 &alphaQCD() const { return alphaQCD_; }

        /**
         * @brief Accessor to the event weights.
         *
         * @return the weights.
         */
        WeightCollection &weights() { return multiweights_; }

        /**
         * @brief Accessor to the event weights (read-only).
         *
         * @return the weights.
         */
        const WeightCollection &weights() const { return multiweights_; }

        /**
         * @brief Accessor to one event weight.
         *
         * @param id index of the weight.
         * @return the weight (0 with an error if the index is not defined).
         */
        const MAfloat64 &GetWeight(MAuint32 id) const { return multiweights_.Get(id); }

        /**
         * @brief Accessor to the generated particles (read-only).
         *
         * @return the particles.
         */
        const std::vector<MCParticleFormat> &particles() const { return particles_; }

        /**
         * @brief Accessor to the generated particles.
         *
         * @return the particles.
         */
        std::vector<MCParticleFormat> &particles() { return particles_; }

        /**
         * @brief Set the process identifier.
         *
         * @param v identifier.
         */
        void setProcessId(MAuint32 v) { processId_ = v; }

        /**
         * @brief Set one event weight (the index must exist).
         *
         * @param id index of the weight.
         * @param value weight.
         */
        void setWeight(MAuint32 id, MAfloat64 value) { multiweights_.Add(id, value); }

        /**
         * @brief Set all the event weights.
         *
         * @param v weights.
         */
        void setWeights(std::vector<MAfloat64> &v) { multiweights_.SetWeights(v); }

        /**
         * @brief Set the scale of the event.
         *
         * @param v scale.
         */
        void setScale(MAfloat64 v) { scale_ = v; }

        /**
         * @brief Set the QED coupling.
         *
         * @param v alpha_QED.
         */
        void setAlphaQED(MAfloat64 v) { alphaQED_ = v; }

        /**
         * @brief Set the QCD coupling.
         *
         * @param v alpha_s.
         */
        void setAlphaQCD(MAfloat64 v) { alphaQCD_ = v; }

        /** @brief Reset the event. */
        void Reset()
        {
            processId_ = 0;
            scale_ = 0.;
            alphaQED_ = 0.;
            alphaQCD_ = 0.;
            particles_.clear();
            multiweights_.Reset();
            MET_.Reset();
            MHT_.Reset();
            TET_ = 0.;
            THT_ = 0.;
            Meff_ = 0.;
        }

        /** @brief Print the event properties. */
        void Print() const
        {
            INFO << "nparts=" << particles_.size()
                 << " - processId=" << processId_
                 << " - weight=" << multiweights_.Get(0)
                 << " - scale=" << scale_
                 << " - alphaQED=" << alphaQED_
                 << " - alphaQCD=" << alphaQCD_ << endmsg;
            INFO << "nweights=" << multiweights_.size() << endmsg;
        }

        /** @brief Print the decay vertices (reconstructed from the mother links) on the standard output. */
        void PrintVertices() const;

        /** @brief Print the mothers of each particle on the standard output. */
        void PrintMothers() const;

        /** @brief Print the daughters of each particle on the standard output. */
        void PrintDaughters() const;

        /**
         * @brief Append a new (empty) particle to the event.
         *
         * @return a pointer to the new particle.
         */
        MCParticleFormat *GetNewParticle()
        {
            // NOTE: push_back may reallocate the vector: pointers to the particles (e.g. mother/daughter links)
            // are invalidated if the capacity is exceeded.
            particles_.push_back(MCParticleFormat());
            return &particles_.back();
        }
    };

}

#endif
