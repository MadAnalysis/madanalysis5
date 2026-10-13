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
 * @file SampleFormat.h
 * @brief Container of the information of a sample (file).
 */

#ifndef SAMPLE_DATAFORMAT_H
#define SAMPLE_DATAFORMAT_H


// STL headers
#include <map>
#include <iostream>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/GeneratorInfo.h"
#include "SampleAnalyzer/Commons/DataFormat/MCSampleFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecSampleFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


namespace MA5
{

    class LHEReader;
    class LHCOReader;
    class HEPMCReader;
    class SampleAnalyzer;


    /**
     * @brief Sample (event file): name, number of events, generator, format and header.
     *
     * The generator-level and reconstruction-level information are allocated on demand
     * with InitializeMC() and InitializeRec().
     */
    class SampleFormat
    {
        friend class LHEReader;
        friend class LHCOReader;
        friend class SampleAnalyzer;

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    private:

        /** @brief Name of the file. */
        std::string                 name_;      /// file name
        /** @brief Number of events in the file. */
        MAuint64                    nevents_;   /// number of events in the file
        /** @brief Generator-level information (null if not initialised). */
        MCSampleFormat  *           mc_;
        /** @brief Reconstruction-level information (null if not initialised). */
        RecSampleFormat *           rec_;
        /** @brief Generator and format of the sample. */
        MA5GEN::GeneratorType       sample_generator_;
        MA5FORMAT::SampleFormatType sample_format_;
        /** @brief Header lines of the file. */
        std::vector<std::string>    header_;    /// file header


        // -------------------------------------------------------------
        //                      method members
        // -------------------------------------------------------------
    public :

        /** @brief Constructor. */
        SampleFormat()
        {
            rec_=0;
            mc_=0;
            nevents_=0;
            sample_generator_  = MA5GEN::UNKNOWN;
            sample_format_     = MA5FORMAT::UNKNOWN;
            header_.clear();
        }

        /** @brief Destructor (the MC/Rec parts are not freed: see Delete()). */
        ~SampleFormat()
        {
        }

        /**
         * @brief Accessor to the generator-level information (read-only).
         *
         * @return the information, or 0 if not initialised.
         */
        const MCSampleFormat * mc() const
        { return mc_; }

        /**
         * @brief Accessor to the reconstruction-level information (read-only).
         *
         * @return the information, or 0 if not initialised.
         */
        const RecSampleFormat * rec() const
        { return rec_; }

        /**
         * @brief Accessor to the generator-level information.
         *
         * @return the information, or 0 if not initialised.
         */
        MCSampleFormat * mc()
        { return mc_; }

        /**
         * @brief Accessor to the reconstruction-level information.
         *
         * @return the information, or 0 if not initialised.
         */
        RecSampleFormat * rec()
        { return rec_; }

        /**
         * @brief Accessor to the file name.
         *
         * @return the name.
         */
        const std::string& name() const
        { return name_; }

        /**
         * @brief Accessor to the number of events.
         *
         * @return the number of events.
         */
        const MAuint64& nevents() const
        { return nevents_; }

        /**
         * @brief Set the file name.
         *
         * @param name file name.
         */
        void setName(const std::string& name)
        {name_=name;}

        /**
         * @brief Set the number of events.
         *
         * @param v number of events.
         */
        void setNEvents(MAuint64 v)
        {nevents_=v;}

        /** @brief Allocate the generator-level information (warning if already done). */
        void InitializeMC()
        {
            try
            {
                if (mc_!=0) throw EXCEPTION_WARNING("MC part of the SampleFormat is already initialized.","",0);
                // NOTE: the MC part keeps a pointer to sample_generator_: copying a SampleFormat leaves it pointing
                // to the original object.
                mc_=new MCSampleFormat(&sample_generator_);
            }
            catch(const std::exception& e)
            {
                MANAGE_EXCEPTION(e);
            }
        }

        /** @brief Allocate the reconstruction-level information (warning if already done). */
        void InitializeRec()
        {
            try
            {
                if (rec_!=0) throw EXCEPTION_WARNING("REC part of the SampleFormat is already initialized.","",0);
                rec_=new RecSampleFormat();
            }
            catch(const std::exception& e)
            {
                MANAGE_EXCEPTION(e);
            }
        }

        /** @brief Free the MC and Rec parts. */
        void Delete()
        {
            // NOTE: the pointers are not reset to 0.
            if (rec_!=0) delete rec_;
            if (mc_!=0)  delete mc_;
        }

        /**
         * @brief Set the generator of the sample.
         *
         * @param value generator.
         */
        void SetSampleGenerator(MA5GEN::GeneratorType value)
        { sample_generator_ = value; }

        /**
         * @brief Set the format of the sample.
         *
         * @param value format.
         */
        void SetSampleFormat(MA5FORMAT::SampleFormatType value)
        { sample_format_ = value; }

        /**
         * @brief Accessor to the generator of the sample.
         *
         * @return the generator.
         */
        const MA5GEN::GeneratorType& sampleGenerator() const
        { return sample_generator_; }

        /**
         * @brief Accessor to the format of the sample.
         *
         * @return the format.
         */
        const MA5FORMAT::SampleFormatType& sampleFormat() const
        { return sample_format_; }

        /** @brief Print the format and the generator of the sample. */
        void printSubtitle() const
        {
            // Sample format
            INFO << "        => sample format: ";
            if (sample_format_==MA5FORMAT::UNKNOWN) INFO << "unknown-format";
            else if (sample_format_==MA5FORMAT::LHE) INFO << "LHE";
            else if (sample_format_==MA5FORMAT::SIMPLIFIED_LHE) INFO << "simplified LHE";
            else if (sample_format_==MA5FORMAT::STDHEP) INFO << "STDHEP";
            else if (sample_format_==MA5FORMAT::HEPMC) INFO << "HEPMC";
            else if (sample_format_==MA5FORMAT::LHCO) INFO << "LHCO";
            else if (sample_format_==MA5FORMAT::DELPHES) INFO << "Delphes-ROOT";
            else if (sample_format_==MA5FORMAT::DELPHESMA5TUNE) INFO << "Delphes-MA5tune ROOT";
            else if (sample_format_==MA5FORMAT::DELPHESMA5CARD) INFO << "Delphes-ROOT";
            INFO << " file produced by ";

            // Generator
            if (sample_generator_==MA5GEN::UNKNOWN)
                INFO << "an unknown generator "
                     << "(cross section assumed in pb)";
            else if (sample_generator_==MA5GEN::MG5) INFO << "MadGraph5";
            // NOTE: typo in the message ('MadAnalysi5').
            else if (sample_generator_==MA5GEN::MA5) INFO << "MadAnalysi5";
            else if (sample_generator_==MA5GEN::PYTHIA6) INFO << "Pythia6";
            else if (sample_generator_==MA5GEN::PYTHIA8) INFO << "Pythia8";
            else if (sample_generator_==MA5GEN::HERWIG6) INFO << "Herwig6";
            else if (sample_generator_==MA5GEN::HERWIGPP) INFO << "Herwig++";
            else if (sample_generator_==MA5GEN::DELPHES) INFO << "Delphes";
            else if (sample_generator_==MA5GEN::DELPHESMA5TUNE) INFO << "Delphes-MA5tune";
            else if (sample_generator_==MA5GEN::DELPHESMA5CARD) INFO << "Delphes + MA5tuned-cards";
            else if (sample_generator_==MA5GEN::CALCHEP) INFO << "CalcHEP";
            INFO << "." << endmsg;
        }

        /**
         * @brief Accessor to the header lines.
         *
         * @return the lines.
         */
        const std::vector<std::string>& header() const
        { return header_; }

        /**
         * @brief Append a header line.
         *
         * @param line line.
         */
        void AddHeader(const std::string& line)
        { header_.push_back(line); }

    };

}

#endif
