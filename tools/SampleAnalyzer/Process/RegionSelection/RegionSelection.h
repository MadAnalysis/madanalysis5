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
 * @file RegionSelection.h
 * @brief Signal region: survival status and cut-flow.
 */

#ifndef __REGIONSELECTION_H
#define __REGIONSELECTION_H

// STL headers
#include <string>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Counter/CounterManager.h"
#include "SampleAnalyzer/Process/Writer/SAFWriter.h"

namespace MA5
{

    /** @brief Signal region: name, survival status for the current event and cut-flow. */
    class RegionSelection
    {
        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    private:
        /** @brief Name of the region. */
        std::string name_;
        /** @brief Does the current event survive all the cuts applied so far? */
        MAbool surviving_;
        /** @brief Number of cuts applied so far to the current event (index of the next cut). */
        MAuint32 NumberOfCutsAppliedSoFar_;

        /** @brief Cut-flow of the region. */
        CounterManager cutflow_;

        // -------------------------------------------------------------
        //                      method members
        // -------------------------------------------------------------
    public:
        /** @brief Constructor (unnamed region). */
        RegionSelection() { name_ = ""; };

        /**
         * @brief Constructor.
         *
         * @param name name of the region.
         */
        RegionSelection(const std::string &name) { name_ = name; };

        /** @brief Destructor. */
        ~RegionSelection() {};

        /**
         * @brief Accessor to the name.
         *
         * @return the name.
         */
        std::string GetName() { return name_; }

        /**
         * @brief Does the current event survive the cuts applied so far?
         *
         * @return the survival status.
         */
        MAbool IsSurviving() { return surviving_; }

        /**
         * @brief Number of cuts applied so far to the current event.
         *
         * @return the number of cuts.
         */
        MAuint32 GetNumberOfCutsAppliedSoFar() { return NumberOfCutsAppliedSoFar_; }

        /**
         * @brief Write the name of the region in the SAF format.
         *
         * @param output SAF writer.
         */
        void WriteDefinition(SAFWriter &output);

        /**
         * @brief Write the cut-flow in the SAF format.
         *
         * @param output SAF writer.
         */
        void WriteCutflow(SAFWriter &output) { cutflow_.Write_TextFormat(output); }

        /**
         * @brief Set the name.
         *
         * @param name name.
         */
        void SetName(std::string name) { name_ = name; }

        /**
         * @brief Set the survival status.
         *
         * @param surviving status.
         */
        void SetSurvivingTest(MAbool surviving) { surviving_ = surviving; }

        /**
         * @brief Set the number of cuts applied so far.
         *
         * @param NumberOfCutsAppliedSoFar number of cuts.
         */
        void SetNumberOfCutsAppliedSoFar(MAuint32 NumberOfCutsAppliedSoFar)
        {
            NumberOfCutsAppliedSoFar_ = NumberOfCutsAppliedSoFar;
        }

        /**
         * @brief Count the event in the counter of the current cut and move to the next cut.
         *
         * @param weight weights of the event (or of the region).
         */
        void IncrementCutFlow(const WeightCollection &weight)
        {
            cutflow_[NumberOfCutsAppliedSoFar_].Increment(weight);
            NumberOfCutsAppliedSoFar_++;
        }

        /**
         * @brief Add a cut to the cut-flow.
         *
         * @param CutName name of the cut.
         */
        void AddCut(std::string const &CutName) { cutflow_.InitCut(CutName); }

        /**
         * @brief Prepare the region for a new event (surviving, no cut applied, initial counter incremented).
         *
         * @param weights weights of the event.
         */
        void InitializeForNewEvent(const WeightCollection &weights)
        {
            SetSurvivingTest(true);
            SetNumberOfCutsAppliedSoFar(0);
            cutflow_.IncrementNInitial(weights);
        }
    };

}

#endif
