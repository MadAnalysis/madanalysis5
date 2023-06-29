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

    class CounterManager
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    private:
        /// @brief intialisation indicator
        MAbool initialised_;

        // Collection of counters
        std::vector<Counter> counters_;

        // Initial number of events
        Counter initial_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public:
        /// Constructor without argument
        CounterManager() { initialised_ = false; }

        /// Destructor
        ~CounterManager() {}

        /// Initialize
        void Initialize(const MAuint32 &n) { counters_.resize(n); }

        // Specifying a cut name
        void InitCut(const std::string myname)
        {
            Counter tmpcnt(myname);
            counters_.push_back(tmpcnt);
        }

        /// Reset
        void Reset() { counters_.clear(); }

        /// Overloading operator []
        const Counter &operator[](const MAuint32 &index) const { return counters_[index]; }
        Counter &operator[](const MAuint32 &index) { return counters_[index]; }

        /// Incrementing the initial number of events
        void IncrementNInitial(const WeightCollection &weight)
        {
            if (!initialised_)
            {
                for (auto &counter : counters_)
                    counter.Initialise(weight);
                initialised_ = true;
            }
            initial_.Increment(weight);
        }

        /// Incrementing the initial number of events
        Counter &GetInitial() { return initial_; }

        const Counter &GetInitial() const { return initial_; }

        /// Write the counters in a Text file
        void Write_TextFormat(SAFWriter &output) const;

        /// Finalizing
        void Finalize() { Reset(); }
    };

}

#endif
