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
 * @file SAFWriter.h
 * @brief Writer of the SAF (Simple Analysis Format) output files.
 */

#ifndef WRITER_SAF_h
#define WRITER_SAF_h

// STL headers
#include <fstream>
#include <iostream>
#include <sstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Writer/WriterTextBase.h"

namespace MA5
{

    /**
     * @brief Writer of the SAF files (sample summaries, histograms, cut-flows).
     *
     * The histograms and cut-flows write themselves through GetStream().
     */
    class SAFWriter : public WriterTextBase
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    protected:
        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public:
        /** @brief Constructor. */
        SAFWriter() {}

        /** @brief Destructor. */
        virtual ~SAFWriter() {}

        /**
         * @brief Write the SAF header and the global information of a sample (cross section, sums of weights, weight names).
         *
         * @param mySample sample.
         * @return true.
         */
        virtual MAbool WriteHeader(const SampleFormat &mySample);
        /**
         * @brief Write the SAF header.
         *
         * @return true.
         */
        virtual MAbool WriteHeader();

        /**
         * @brief Write the list of files and their detailed information.
         *
         * @param mySample samples.
         * @return true.
         */
        MAbool WriteFiles(const std::vector<SampleFormat> &mySample);

        /**
         * @brief Write an event (nothing is written).
         *
         * @param myEvent event.
         * @param mySample sample.
         * @return true.
         */
        virtual MAbool WriteEvent(const EventFormat &myEvent,
                                  const SampleFormat &mySample);

        /**
         * @brief Write the SAF footer.
         *
         * @param mySample sample.
         * @return true.
         */
        virtual MAbool WriteFoot(const SampleFormat &mySample);
        /**
         * @brief Write the SAF footer.
         *
         * @return true.
         */
        virtual MAbool WriteFoot();

        /**
         * @brief Accessor to the output stream.
         *
         * @return the stream.
         */
        std::ostream *GetStream() { return output_; }
    };

}

#endif
