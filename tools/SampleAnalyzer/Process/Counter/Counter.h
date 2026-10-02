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
 * @file Counter.h
 * @brief Counter of the events passing a cut, for each event weight.
 */

#ifndef COUNTER_h
#define COUNTER_h

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Base/HistoStructures.h"
#include "SampleAnalyzer/Commons/DataFormat/WeightCollection.h"

// STL headers
#include <iostream>
#include <string>
#include <map>

namespace MA5
{
    class CounterManager;

    /**
     * @brief Counter of the events passing a cut.
     *
     * For each weight of the event (multiweight support), the number of entries, the sum of
     * weights and the sum of squared weights are accumulated separately for positive and
     * negative weights.
     */
    class Counter
    {
        friend class CounterManager;

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    public:
        /** @brief Name of the cut. */
        std::string name_;

        /** @brief Number of entries, per weight (positive/negative weights). */
        std::vector<ENTRIES> nentries_;

        /** @brief Sum of the weights, per weight (positive/negative weights). */
        std::vector<WEIGHTS> sumweights_;

        /** @brief Sum of the squared weights, per weight (positive/negative weights). */
        std::vector<WEIGHTS> sumweights2_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public:
        /**
         * @brief Constructor.
         *
         * @param name name of the cut.
         */
        Counter(const std::string &name = "unkwown")
        {
            name_ = name;
            Reset();
        }

        /** @brief Destructor. */
        ~Counter() {}

        /** @brief Remove all the entries. */
        void Reset()
        {
            nentries_.clear();
            sumweights_.clear();
            sumweights2_.clear();
        }

        /**
         * @brief Number of weights.
         *
         * @return the number of weights.
         */
        MAint32 size() { return nentries_.size(); }

        /**
         * @brief Reset and size the counter for a given set of weights.
         *
         * @param multiweight weights of an event (only the size is used).
         */
        void Initialise(const WeightCollection &multiweight)
        {
            Reset();
            MAuint32 n = multiweight.GetWeights().size();
            nentries_.resize(n);
            sumweights_.resize(n);
            sumweights2_.resize(n);
        }

        /**
         * @brief Accessor to the numbers of entries.
         *
         * @return a copy of the numbers of entries.
         */
        std::vector<ENTRIES> nentries() { return nentries_; }
        /**
         * @brief Accessor to the sums of weights.
         *
         * @return a copy of the sums.
         */
        std::vector<WEIGHTS> sumW() { return sumweights_; }
        /**
         * @brief Accessor to the sums of squared weights.
         *
         * @return a copy of the sums.
         */
        std::vector<WEIGHTS> sumW2() { return sumweights2_; }

        /**
         * @brief Count an event.
         *
         * @param multiweight weights of the event.
         */
        void Increment(const WeightCollection &multiweight)
        {
            for (MAuint32 idx = 0; idx < multiweight.size(); idx++)
            {
                MAfloat64 w = multiweight[idx];
                if (w >= 0.0)
                {
                    nentries_[idx].positive++;
                    sumweights_[idx].positive += w;
                    sumweights2_[idx].positive += w * w;
                }
                else
                {
                    nentries_[idx].negative++;
                    // FIXME: the negative weights are summed with their sign, whereas Histo stores their absolute value
                    // and the Python cut-flow (CutFlowForDataset) computes positive minus negative sums: events with
                    // negative weights are then added instead of subtracted.
                    sumweights_[idx].negative += w;
                    sumweights2_[idx].negative += w * w;
                }
            }
        }
    };

}

#endif
