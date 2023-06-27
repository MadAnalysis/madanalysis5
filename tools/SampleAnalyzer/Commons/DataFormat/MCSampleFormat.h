////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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
        std::pair<MAint32, MAint32> beamPDGID_;
        std::pair<MAfloat64, MAfloat64> beamE_;
        std::pair<MAuint32, MAuint32> beamPDFauthor_;
        std::pair<MAuint32, MAuint32> beamPDFID_;
        MAint32 weightMode_;
        std::vector<ProcessFormat> processes_;
        const MA5GEN::GeneratorType *sample_generator_;
        MAfloat32 length_unit_; /// Length unit: mm=1 cm=0.1
        MAfloat32 energy_unit_; /// Energy unit: GeV=1 MeV=0.001 keV=0.000001

        // ----------------------- multiweights ------------------------
        /// @jackaraz: not sure if this is doing anything
        WeightDefinition weight_definition_;
        /// @brief store weight names
        std::map<int, std::string> weight_names_;

        // ----------------------- file info ---------------------------
        MAfloat64 xsection_;
        MAfloat64 xsection_error_;
        MAfloat64 sumweight_positive_; // all events with positive weights
        MAfloat64 sumweight_negative_; // all events with negative weights

        // -------------------------------------------------------------
        //                      method members
        // -------------------------------------------------------------
    public:
        /// Constructor withtout arguments
        MCSampleFormat(const MA5GEN::GeneratorType *gen)
        {
            sample_generator_ = gen;
            Reset();
        }

        /// Destructor
        ~MCSampleFormat() {}

        /// Clear all the content
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

            // WeightDefinition
            weight_definition_.Reset();
            weight_names_.clear();

            // File info
            xsection_ = 0.;
            xsection_error_ = 0.;
            sumweight_positive_ = 0.;
            sumweight_negative_ = 0.;
        }

        /// Accessoir to the generator type
        const MA5GEN::GeneratorType *GeneratorType() const
        {
            return sample_generator_;
        }

        /// Accessor to PDG ID of the intial partons
        const std::pair<MAint32, MAint32> &beamPDGID() const
        {
            return beamPDGID_;
        }

        /// Accessor to the beam energy
        const std::pair<MAfloat64, MAfloat64> &beamE() const
        {
            return beamE_;
        }

        /// Accessor to the PDF authors
        const std::pair<MAuint32, MAuint32> &beamPDFauthor() const
        {
            return beamPDFauthor_;
        }

        /// Accessor to the PDF identity
        const std::pair<MAuint32, MAuint32> &beamPDFID() const
        {
            return beamPDFID_;
        }

        /// Accessor to the weight mode
        const MAint32 &weightMode() const
        {
            return weightMode_;
        }

        /// Accessor to the xsection mean
        const MAfloat64 &xsection() const
        {
            return xsection_;
        }

        /// Accessor to the xsection mean
        const MAfloat64 &xsection_mean() const
        {
            return xsection_;
        }

        /// Accessor to the xsection error
        const MAfloat64 &xsection_error() const
        {
            return xsection_error_;
        }

        /// Accessor to the number of events with positive weight
        const MAfloat64 &sumweight_positive() const
        {
            return sumweight_positive_;
        }

        /// Accessor to the number of events with negative weight
        const MAfloat64 &sumweight_negative() const
        {
            return sumweight_negative_;
        }

        /// Accessor to the process collection (read-only)
        const std::vector<ProcessFormat> &processes() const
        {
            return processes_;
        }

        /// Accessor to the process collection
        std::vector<ProcessFormat> &processes()
        {
            return processes_;
        }

        /// Accessor to the weight definition (read-only)
        const WeightDefinition &weight_definition() const
        {
            return weight_definition_;
        }

        /// Accessor to the weight definition
        WeightDefinition &weight_definition()
        {
            return weight_definition_;
        }

        /// Set the PDG ID of the intial partons
        void setBeamPDGID(MAint32 a, MAint32 b)
        {
            beamPDGID_ = std::make_pair(a, b);
        }

        /// Set the beam energy
        void setBeamE(MAfloat64 a, MAfloat64 b)
        {
            beamE_ = std::make_pair(a, b);
        }

        /// Set the PDF authors
        void setBeamPDFauthor(MAuint32 a, MAuint32 b)
        {
            beamPDFauthor_ = std::make_pair(a, b);
        }

        /// Set the the PDF identity
        void setBeamPDFid(MAuint32 a, MAuint32 b)
        {
            beamPDFID_ = std::make_pair(a, b);
        }

        /// Set the weight mode
        void setWeightMode(MAint32 v)
        {
            weightMode_ = v;
        }

        /// Set the cross section mean
        // BENJ: the normalization in the pythia lhe output by madgraph has been changed
        //       the 1e9 factor is not needed anymore
        void setXsection(MAfloat64 value)
        //  { xsection_=value*getXsectionUnitFactor();}
        {
            xsection_ = value;
        }

        /// Set the cross section mean
        void setXsectionMean(MAfloat64 value)
        {
            xsection_ = value;
        }

        /// Set the cross section mean
        void setXsectionError(MAfloat64 value)
        {
            xsection_error_ = value;
        }

        /// @brief set weight names
        /// @param id location of the weight
        /// @param name name of the weight
        void SetWeightName(int id, std::string name) { weight_names_[id] = name; }

        /// @brief accessor to weight names
        const std::map<int, std::string> WeightNames() const { return weight_names_; }

        /// Adding a weight
        void addWeightedEvents(MAfloat64 weight)
        {
            if (weight >= 0)
                sumweight_positive_ += std::abs(weight);
            else
                sumweight_negative_ += std::abs(weight);
        }

        /// Accessor to the number of events with positive weight
        void setSumweight_positive(MAfloat64 sum)
        {
            sumweight_positive_ += sum;
        }

        /// Accessor to the number of events with negative weight
        void setSumweight_negative(MAfloat64 sum)
        {
            sumweight_negative_ += sum;
        }

        /// Giving a new process entry
        ProcessFormat *GetNewProcess()
        {
            processes_.push_back(ProcessFormat());
            return &processes_.back();
        }

        /// Get scale factor required to set the cross section in pb unit
        MAfloat64 getXsectionUnitFactor()
        {
            if (*sample_generator_ == MA5GEN::PYTHIA6)
                return 1e9;
            else
                return 1.;
        }

        /// Length unit setter
        void SetLengthUnit(MAfloat32 val) { length_unit_ = val; }

        /// Accessor to the length unit
        MAfloat32 LengthUnit() { return length_unit_; }

        /// Accessor to the length unit
        const MAfloat32 LengthUnit() const { return length_unit_; }

        /// Energy unit setter
        void SetEnergyUnit(MAfloat32 val) { energy_unit_ = val; }

        /// Accessor to the energy unit
        MAfloat32 EnergyUnit() { return energy_unit_; }

        /// Accessor to the energy unit
        const MAfloat32 EnergyUnit() const { return energy_unit_; }
    };

}

#endif
