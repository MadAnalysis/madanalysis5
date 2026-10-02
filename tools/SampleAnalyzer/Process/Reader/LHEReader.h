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
 * @file LHEReader.h
 * @brief Reader of Les Houches Event (LHE) files.
 */

#ifndef LHE_READER_h
#define LHE_READER_h

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Reader/ReaderTextBase.h"

namespace MA5
{

    /**
     * @brief Reader of Les Houches Event files (LHE and the simplified LHE format of MadAnalysis 5).
     *
     * The generator is guessed from the header tags; multiweights are read from the
     * <initrwgt>/<rwgt> blocks (numerical weight identifiers only).
     */
    class LHEReader : public ReaderTextBase
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    protected:
        /** @brief Has the <event> tag of the first event already been read by ReadHeader()? */
        MAbool firstevent_;
        /** @brief Mother indices (MOTHUP1, MOTHUP2) of the particles of the current event. */
        std::vector<std::pair<MAint32, MAint32>> mothers_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public:
        /** @brief Constructor. */
        LHEReader() { firstevent_ = false; }

        /** @brief Destructor. */
        virtual ~LHEReader() {}

        /**
         * @brief Open the file.
         *
         * @param rawfilename file name.
         * @param cfg run configuration.
         * @return false if the file cannot be opened.
         */
        virtual MAbool Initialize(const std::string &rawfilename,
                                  const Configuration &cfg)
        {
            firstevent_ = false;
            return ReaderTextBase::Initialize(rawfilename, cfg);
        }

        /**
         * @brief Close the file.
         *
         * @return true.
         */
        virtual MAbool Finalize() { return ReaderTextBase::Finalize(); }

        /**
         * @brief Read the header, the <init> block and the weight definitions; guess the generator.
         *
         * @param mySample sample to fill.
         * @return false if the file ends before the first event.
         */
        virtual MAbool ReadHeader(SampleFormat &mySample);

        /**
         * @brief Compute the cross section of the sample (sum over the processes).
         *
         * @param mySample sample.
         * @return true.
         */
        virtual MAbool FinalizeHeader(SampleFormat &mySample);

        /**
         * @brief Read the next <event> block.
         *
         * @param myEvent event to fill.
         * @param mySample sample.
         * @return KEEP, or FAILURE at the end of the file.
         */
        virtual StatusCode::Type ReadEvent(EventFormat &myEvent, SampleFormat &mySample);

        /**
         * @brief Build the mother-daughter links and compute MET, MHT, TET, THT and Meff.
         *
         * @param mySample sample.
         * @param myEvent event.
         * @return true.
         */
        virtual MAbool FinalizeEvent(SampleFormat &mySample, EventFormat &myEvent);

    private:
        /**
         * @brief Read a process line of the <init> block.
         *
         * @param line line.
         * @param mySample sample.
         */
        void FillHeaderProcessLine(const std::string &line, SampleFormat &mySample);
        /**
         * @brief Read the first line of the <init> block (beams, PDFs, weighting strategy).
         *
         * @param line line.
         * @param mySample sample.
         */
        void FillHeaderInitLine(const std::string &line, SampleFormat &mySample);

        /**
         * @brief Read the first line of an event (number of particles, process, weight, scale, couplings).
         *
         * @param line line.
         * @param mySample sample.
         * @param myFormat event.
         */
        void FillEventInitLine(const std::string &line, SampleFormat &mySample, EventFormat &myFormat);
        /**
         * @brief Read a particle line.
         *
         * @param line line.
         * @param myFormat event.
         */
        void FillEventParticleLine(const std::string &line, EventFormat &myFormat);
        /**
         * @brief Read a weight definition of the <initrwgt> block.
         *
         * @param line line.
         * @param mySample sample.
         */
        void FillWeightNames(const std::string &line, SampleFormat &mySample);
        /**
         * @brief Read a weight of the <rwgt> block of an event.
         *
         * @param line line.
         * @param mySample sample.
         * @param myEvent event.
         */
        void FillWeightLine(const std::string &line, SampleFormat &mySample, EventFormat &myEvent);
    };

}

#endif
