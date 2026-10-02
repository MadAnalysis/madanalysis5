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
 * @file CounterManager.h
 * @brief Cut-flow of a signal region (initial counter and one counter per cut).
 */

#ifndef COUNTER_MANAGER_H
#define COUNTER_MANAGER_H

// STL headers
#include <iostream>
#include <ostream>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Counter/Counter.h"
#include "SampleAnalyzer/Process/Writer/SAFWriter.h"

namespace MA5
{

    /** @brief Cut-flow of a signal region. */
    class CounterManager
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    private:
        /** @brief Have the counters been sized for the event weights? */
        MAbool initialised_;

        /** @brief Counters of the cuts. */
        std::vector<Counter> counters_;

        /** @brief Counter of the initial number of events. */
        Counter initial_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public:
        /** @brief Constructor. */
        CounterManager() { initialised_ = false; }

        /** @brief Destructor. */
        ~CounterManager() {}

        /**
         * @brief Resize the collection of counters.
         *
         * @param n number of cuts.
         */
        void Initialize(const MAuint32 &n) { counters_.resize(n); }

        /**
         * @brief Add a counter for a new cut.
         *
         * @param myname name of the cut.
         */
        void InitCut(const std::string myname)
        {
            Counter tmpcnt(myname);
            counters_.push_back(tmpcnt);
        }

        /** @brief Remove all the cut counters. */
        void Reset() { counters_.clear(); }

        /**
         * @brief Access a cut counter (read-only).
         *
         * @param index index of the cut.
         * @return the counter.
         */
        const Counter &operator[](const MAuint32 &index) const { return counters_[index]; }
        /**
         * @brief Access a cut counter.
         *
         * @param index index of the cut.
         * @return the counter.
         */
        Counter &operator[](const MAuint32 &index) { return counters_[index]; }

        /**
         * @brief Count an event in the initial counter (the counters are sized at the first call).
         *
         * @param weight weights of the event.
         */
        void IncrementNInitial(const WeightCollection &weight)
        {
            if (!initialised_)
            {
                for (auto &counter : counters_)
                    counter.Initialise(weight);
                initial_.Initialise(weight);
                initialised_ = true;
            }
            initial_.Increment(weight);
        }

        /**
         * @brief Accessor to the initial counter.
         *
         * @return the counter.
         */
        Counter &GetInitial() { return initial_; }

        /**
         * @brief Accessor to the initial counter (read-only).
         *
         * @return the counter.
         */
        const Counter &GetInitial() const { return initial_; }

        /**
         * @brief Write the cut-flow in the SAF format.
         *
         * @param output SAF writer.
         */
        void Write_TextFormat(SAFWriter &output) const;

        /** @brief Remove all the cut counters. */
        void Finalize() { Reset(); }
    };

}

#endif
