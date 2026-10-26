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
 * @file MCSampleFormat.h
 * @brief Generator-level information of a sample (beams, processes, cross section, weights).
 */

#ifndef MCSAMPLE_DATAFORMAT_H
#define MCSAMPLE_DATAFORMAT_H

// STL headers
#include <map>
#include <iostream>
#include <vector>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/GeneratorInfo.h"
#include "SampleAnalyzer/Commons/DataFormat/MCProcessFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/WeightDefinition.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

namespace MA5
{

    class LHEReader;
    class LHCOReader;
    class HEPMCReader;
    class STDHEPReader;
    class STDHEPreader;
    class ROOTReader;
    class LHEWriter;
    class SampleAnalyzer;

    /**
     * @brief Generator-level information of a sample.
     *
     * It stores the beam properties, the list of processes, the cross section, the sums
     * of positive and negative event weights, the names of the weights (multiweight
     * samples) and the length/energy units of the file.
     */
    class MCSampleFormat
    {
        friend class LHEReader;
        friend class LHCOReader;
        friend class HEPMCReader;
        friend class ROOTReader;
        friend class SampleAnalyzer;
        friend class LHEWriter;
        friend class STDHEPReader;
        friend class STDHEPreader;

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    private:
        // ---------------------- physics info -------------------------
        /** @brief Beam PDG codes, beam energies, PDF authors and PDF identifiers (one value per beam). */
        std::pair<MAint32, MAint32> beamPDGID_;
        std::pair<MAfloat64, MAfloat64> beamE_;
        std::pair<MAuint32, MAuint32> beamPDFauthor_;
        std::pair<MAuint32, MAuint32> beamPDFID_;
        /** @brief Weighting strategy (IDWTUP in the LHE <init> block). */
        MAint32 weightMode_;
        /** @brief Processes of the sample. */
        std::vector<ProcessFormat> processes_;
        /** @brief Generator of the sample (points to the member of the owning SampleFormat). */
        const MA5GEN::GeneratorType *sample_generator_;
        /** @brief Length unit (mm = 1, cm = 0.1). */
        MAfloat32 length_unit_; /// Length unit: mm=1 cm=0.1
        /** @brief Energy unit (GeV = 1, MeV = 0.001, keV = 0.000001). */
        MAfloat32 energy_unit_; /// Energy unit: GeV=1 MeV=0.001 keV=0.000001

        // ----------------------- multiweights ------------------------
        /** @brief Names of the weights, by identifier. */
        std::map<int, std::string> weight_names_;
        /** @brief Position of each weight identifier in the event weight vector. */
        std::map<int, std::size_t> weight_index_;

        // ----------------------- file info ---------------------------
        /** @brief Cross section and its uncertainty [pb]. */
        MAfloat64 xsection_;
        MAfloat64 xsection_error_;
        /** @brief Sum of the positive event weights. */
        MAfloat64 sumweight_positive_; // all events with positive weights
        /** @brief Sum of the absolute values of the negative event weights. */
        MAfloat64 sumweight_negative_; // all events with negative weights

        // -------------------------------------------------------------
        //                      method members
        // -------------------------------------------------------------
    public:
        /**
         * @brief Constructor.
         *
         * @param gen pointer to the generator type of the owning sample.
         */
        MCSampleFormat(const MA5GEN::GeneratorType *gen)
        {
            sample_generator_ = gen;
            Reset();
        }

        /** @brief Destructor. */
        ~MCSampleFormat() {}

        /** @brief Reset all the members (the generator pointer is kept). */
        void Reset()
        {
            // Physics info
            beamPDGID_ = std::make_pair(0, 0);
            beamE_ = std::make_pair(0, 0);
            beamPDFauthor_ = std::make_pair(0, 0);
            beamPDFID_ = std::make_pair(0, 0);
            weightMode_ = 0;
            processes_.clear();
            length_unit_ = 1.0;
            energy_unit_ = 1.0;

            weight_names_.clear();
            weight_index_.clear();

            // File info
            xsection_ = 0.;
            xsection_error_ = 0.;
            sumweight_positive_ = 0.;
            sumweight_negative_ = 0.;
        }

        /**
         * @brief Accessor to the generator type.
         *
         * @return a pointer to the generator type.
         */
        const MA5GEN::GeneratorType *GeneratorType() const { return sample_generator_; }

        /**
         * @brief Accessor to the PDG codes of the beams.
         *
         * @return the PDG codes of the beams.
         */
        const std::pair<MAint32, MAint32> &beamPDGID() const { return beamPDGID_; }

        /**
         * @brief Accessor to the beam energies.
         *
         * @return the beam energies.
         */
        const std::pair<MAfloat64, MAfloat64> &beamE() const { return beamE_; }

        /**
         * @brief Accessor to the PDF authors (one per beam).
         *
         * @return the PDF authors.
         */
        const std::pair<MAuint32, MAuint32> &beamPDFauthor() const { return beamPDFauthor_; }

        /**
         * @brief Accessor to the PDF identifiers (one per beam).
         *
         * @return the PDF identifiers.
         */
        const std::pair<MAuint32, MAuint32> &beamPDFID() const { return beamPDFID_; }

        /**
         * @brief Accessor to the weighting strategy.
         *
         * @return the weighting strategy.
         */
        const MAint32 &weightMode() const { return weightMode_; }

        /**
         * @brief Accessor to the cross section [pb].
         *
         * @return the cross section [pb].
         */
        const MAfloat64 &xsection() const { return xsection_; }

        /**
         * @brief Accessor to the cross section [pb].
         *
         * @return the cross section [pb].
         */
        const MAfloat64 &xsection_mean() const { return xsection_; }

        /**
         * @brief Accessor to the uncertainty on the cross section [pb].
         *
         * @return the uncertainty on the cross section [pb].
         */
        const MAfloat64 &xsection_error() const { return xsection_error_; }

        /**
         * @brief Accessor to the sum of the positive event weights.
         *
         * @return the sum of the positive event weights.
         */
        const MAfloat64 &sumweight_positive() const { return sumweight_positive_; }

        /**
         * @brief Accessor to the sum of the absolute values of the negative event weights.
         *
         * @return the sum of the absolute values of the negative event weights.
         */
        const MAfloat64 &sumweight_negative() const { return sumweight_negative_; }

        /**
         * @brief Accessor to the processes (read-only).
         *
         * @return the processes.
         */
        const std::vector<ProcessFormat> &processes() const { return processes_; }

        /**
         * @brief Accessor to the processes.
         *
         * @return the processes.
         */
        std::vector<ProcessFormat> &processes() { return processes_; }

        /**
         * @brief Set the PDG codes of the beams.
         *
         * @param a first beam.
         * @param b second beam.
         */
        void setBeamPDGID(MAint32 a, MAint32 b) { beamPDGID_ = std::make_pair(a, b); }

        /**
         * @brief Set the beam energies.
         *
         * @param a first beam.
         * @param b second beam.
         */
        void setBeamE(MAfloat64 a, MAfloat64 b) { beamE_ = std::make_pair(a, b); }

        /**
         * @brief Set the PDF authors.
         *
         * @param a first beam.
         * @param b second beam.
         */
        void setBeamPDFauthor(MAuint32 a, MAuint32 b) { beamPDFauthor_ = std::make_pair(a, b); }

        /**
         * @brief Set the PDF identifiers.
         *
         * @param a first beam.
         * @param b second beam.
         */
        void setBeamPDFid(MAuint32 a, MAuint32 b) { beamPDFID_ = std::make_pair(a, b); }

        /**
         * @brief Set the weighting strategy.
         *
         * @param v IDWTUP value.
         */
        void setWeightMode(MAint32 v) { weightMode_ = v; }

        /**
         * @brief Set the cross section.
         *
         * @param value cross section [pb].
         */
        // BENJ: the normalization in the pythia lhe output by madgraph has been changed
        //       the 1e9 factor is not needed anymore
        void setXsection(MAfloat64 value) { xsection_ = value; }

        /**
         * @brief Set the cross section.
         *
         * @param value cross section [pb].
         */
        void setXsectionMean(MAfloat64 value) { xsection_ = value; }

        /**
         * @brief Set the uncertainty on the cross section.
         *
         * @param value uncertainty [pb].
         */
        void setXsectionError(MAfloat64 value) { xsection_error_ = value; }

        /**
         * @brief Set the name of a weight and register its position in the event weight vector.
         *
         * @param id identifier of the weight.
         * @param name name of the weight.
         */
        void SetWeightName(int id, std::string name)
        {
          weight_names_[id] = name;
          if (weight_index_.find(id)==weight_index_.end())
          {
            std::size_t idx = weight_index_.size();
            weight_index_[id] = idx;
          }
        }

        /**
         * @brief Position of a weight identifier in the event weight vector.
         *
         * @throw an EXCEPTION_ERROR if the identifier is unknown.
         *
         * @param id identifier of the weight.
         * @return the position.
         */
        std::size_t GetWeightIndex(int id) const
        {
            auto it = weight_index_.find(id);
            if (it == weight_index_.end())
            {
                std::stringstream str;
                str << id;
                throw EXCEPTION_ERROR("Unknown weight ID '" + str.str() + "'", "", 0);
            }
            return it->second;
        }

        /**
         * @brief Accessor to the names of the weights.
         *
         * @return the names, by identifier.
         */
        const std::map<int, std::string> &WeightNames() const { return weight_names_; }

        /**
         * @brief Add an event weight to the sums of positive/negative weights.
         *
         * @param weight event weight.
         */
        void addWeightedEvents(MAfloat64 weight)
        {
            if (weight >= 0)
                sumweight_positive_ += std::abs(weight);
            else
                sumweight_negative_ += std::abs(weight);
        }

        /**
         * @brief Add a value to the sum of positive weights (despite its name, it does not overwrite).
         *
         * @param sum value to add.
         */
        void setSumweight_positive(MAfloat64 sum)
        {
            sumweight_positive_ += sum;
        }

        /**
         * @brief Add a value to the sum of negative weights (despite its name, it does not overwrite).
         *
         * @param sum value to add.
         */
        void setSumweight_negative(MAfloat64 sum) { sumweight_negative_ += sum; }

        /**
         * @brief Append a new process.
         *
         * @return a pointer to the new process.
         */
        ProcessFormat *GetNewProcess()
        {
            processes_.push_back(ProcessFormat());
            return &processes_.back();
        }

        /**
         * @brief Factor converting the cross section to pb (1e9 for Pythia 6 samples).
         *
         * @return the factor.
         */
        MAfloat64 getXsectionUnitFactor()
        {
            if (*sample_generator_ == MA5GEN::PYTHIA6)
                return 1e9;
            else
                return 1.;
        }

        /**
         * @brief Set the length unit.
         *
         * @param val unit (mm = 1, cm = 0.1).
         */
        void SetLengthUnit(MAfloat32 val) { length_unit_ = val; }

        /**
         * @brief Accessor to the length unit.
         *
         * @return the unit.
         */
        MAfloat32 LengthUnit() { return length_unit_; }

        /**
         * @brief Accessor to the length unit.
         *
         * @return the unit.
         */
        const MAfloat32 LengthUnit() const { return length_unit_; }

        /**
         * @brief Set the energy unit.
         *
         * @param val unit (GeV = 1).
         */
        void SetEnergyUnit(MAfloat32 val) { energy_unit_ = val; }

        /**
         * @brief Accessor to the energy unit.
         *
         * @return the unit.
         */
        MAfloat32 EnergyUnit() { return energy_unit_; }

        /**
         * @brief Accessor to the energy unit.
         *
         * @return the unit.
         */
        const MAfloat32 EnergyUnit() const { return energy_unit_; }
    };

}

#endif
// MCSAMPLE_DATAFORMAT_H
