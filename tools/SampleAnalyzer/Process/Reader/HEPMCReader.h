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
 * @file HEPMCReader.h
 * @brief Reader of HepMC2 ASCII files (IO_GenEvent).
 */

#ifndef HEPMC_READER_h
#define HEPMC_READER_h

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Reader/ReaderTextBase.h"

namespace MA5
{

    /**
     * @brief Reader of HepMC2 ASCII event files.
     *
     * The E, N, U, C, H, F, V and P lines are decoded; the mother-daughter links are built
     * from the vertices when the event is finalised.
     */
    class HEPMCReader : public ReaderTextBase
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    protected:
        /** @brief Is the first event being read? */
        MAbool firstevent_;
        /** @brief Has the end of the event been reached? */
        MAbool endevent_;
        /** @brief Has a line been saved for the next event? */
        MAbool saved_;
        /** @brief Has the end of the file been reached? */
        MAbool EndOfFile_;
        /** @brief Should a warning be issued about the mothers? */
        MAbool warnmother_;
        /** @brief Current particle and vertex codes. */
        MAint32 partcode_;
        MAint32 vertcode_;
        /** @brief Energy unit (GeV = 1). */
        MAfloat32 energy_unit_;
        /** @brief Length unit (mm = 1). */
        MAfloat32 length_unit_;
        /** @brief Line saved for the next event (first line of the event). */
        std::string savedline_; // last saved line
        /** @brief Is the warning about heavy-ion blocks still to be issued? */
        MAbool firstHeavyIons_;
        /** @brief Maximum numbers of particles and vertices seen so far (for memory reservation). */
        MAuint64 nparts_max_;
        MAuint64 nvertices_max_;

        /** @brief Vertex of the HepMC record: position, c*tau, barcode, incoming and outgoing particles. */
        struct HEPVertex
        {
            MAfloat64 ctau_;
            MAfloat64 id_;
            MAfloat64 x_;
            MAfloat64 y_;
            MAfloat64 z_;
            MAint32 barcode_;
            std::vector<MAuint32> in_;
            std::vector<MAuint32> out_;
            HEPVertex()
            {
                ctau_ = 0;
                id_ = 0;
                x_ = 0;
                y_ = 0;
                z_ = 0;
                barcode_ = 0;
            }
        };

        /** @brief Vertices of the current event, by barcode. */
        std::map<MAint32, HEPVertex> vertices_;
        /** @brief Barcode of the current vertex (production vertex of the next particles). */
        MAint32 currentvertex_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public:
        /** @brief Constructor. */
        HEPMCReader()
        {
            firstevent_ = false;
            firstHeavyIons_ = true;
            nparts_max_ = 0;
            nvertices_max_ = 0;
            energy_unit_ = 1.0;
            length_unit_ = 1.0;
        }

        /** @brief Destructor. */
        virtual ~HEPMCReader()
        {
        }

        /**
         * @brief Skip the header until the first event line.
         *
         * @param mySample sample.
         * @return false if no event is found.
         */
        virtual MAbool ReadHeader(SampleFormat &mySample);

        /**
         * @brief Finalise the header (nothing to do).
         *
         * @param mySample sample.
         * @return true.
         */
        virtual MAbool FinalizeHeader(SampleFormat &mySample);

        /**
         * @brief Read the lines of the next event.
         *
         * @param myEvent event to fill.
         * @param mySample sample.
         * @return KEEP, or FAILURE at the end of the file.
         */
        virtual StatusCode::Type ReadEvent(EventFormat &myEvent, SampleFormat &mySample);

        /**
         * @brief Build the mother-daughter links and the decay vertices, and compute MET, MHT, TET, THT and Meff.
         *
         * @param mySample sample.
         * @param myEvent event.
         * @return true.
         */
        virtual MAbool FinalizeEvent(SampleFormat &mySample, EventFormat &myEvent);

    private:
        /**
         * @brief Decode a line of the event record.
         *
         * @param line line.
         * @param myEvent event.
         * @param mySample sample.
         * @return false at the end of the event listing.
         */
        MAbool FillEvent(const std::string &line, EventFormat &myEvent, SampleFormat &mySample);
        /**
         * @brief Decode an E line (event number, scale, couplings, process, weights).
         *
         * @param line line.
         * @param myEvent event.
         */
        void FillEventInformations(const std::string &line, EventFormat &myEvent);
        /**
         * @brief Decode a C line (cross section and uncertainty).
         *
         * @param line line.
         * @param mySample sample.
         */
        void FillCrossSection(const std::string &line, SampleFormat &mySample);
        /**
         * @brief Decode a U line (energy and length units).
         *
         * @param line line.
         * @param mySample sample.
         */
        void FillUnits(const std::string &line, SampleFormat &mySample);
        /**
         * @brief Decode an F line (PDF information).
         *
         * @param line line.
         * @param mySample sample.
         * @param myEvent event.
         */
        void FillEventPDFInfo(const std::string &line, SampleFormat &mySample, EventFormat &myEvent);
        /**
         * @brief Decode a P line (particle).
         *
         * @param line line.
         * @param myEvent event.
         */
        void FillEventParticleLine(const std::string &line, EventFormat &myEvent);
        /**
         * @brief Decode a V line (vertex).
         *
         * @param line line.
         * @param myEvent event.
         */
        void FillEventVertexLine(const std::string &line, EventFormat &myEvent);
        /**
         * @brief Decode an N line (weight names).
         *
         * @param line line.
         * @param mySample sample.
         * @return true.
         */
        MAbool FillWeightNames(const std::string &line, SampleFormat &mySample);
        /**
         * @brief Decode an H line (heavy-ion information, ignored with a warning).
         *
         * @param line line.
         * @return false.
         */
        MAbool FillHeavyIons(const std::string &line);
    };

}

#endif
