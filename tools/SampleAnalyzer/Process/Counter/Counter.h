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

#ifndef COUNTER_h
#define COUNTER_h

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Base/Basics.h"
#include "SampleAnalyzer/Commons/DataFormat/WeightCollection.h"

// STL headers
#include <iostream>
#include <string>
#include <map>

namespace MA5
{
    class CounterManager;

    class Counter
    {
        friend class CounterManager;

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    public:
        /// name of the analysis
        std::string name_;

        /// number of times the function Increment is called
        /// first = positive weight ; second = negative weight
        std::map<MAint32, ENTRIES> nentries_;

        /// sum of weights
        /// first = positive weight ; second = negative weight
        std::map<MAint32, WEIGHTS> sumweights_;

        /// sum of squared weights
        /// first = positive weight ; second = negative weight
        std::map<MAint32, WEIGHTS> sumweights2_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public:
        /// Constructor without argument
        Counter(const std::string &name = "unkwown")
        {
            name_ = name;
            Reset();
        }

        /// Destructor
        ~Counter() {}

        /// Reset
        void Reset()
        {
            nentries_.clear();
            sumweights_.clear();
            sumweights2_.clear();
        }

        MAint32 size() { return nentries_.size(); }

        void Initialise(const WeightCollection &multiweight)
        {
            Reset();
            for (auto &weight : multiweight.GetWeights())
            {

                nentries_[weight.first] = ENTRIES();
                sumweights_[weight.first] = WEIGHTS();
                sumweights2_[weight.first] = WEIGHTS();
            }
        }

        std::map<MAint32, ENTRIES> nentries() { return nentries_; }
        std::map<MAint32, WEIGHTS> sumW() { return sumweights_; }
        std::map<MAint32, WEIGHTS> sumW2() { return sumweights2_; }

        /// Increment the counter
        void Increment(const WeightCollection &multiweight)
        {
            for (auto &weight : multiweight.GetWeights())
            {
                MAint32 idx = weight.first;
                MAdouble64 w = weight.second;
                if (w >= 0)
                {
                    nentries_[idx].positive++;
                    sumweights_[idx].positive += w;
                    sumweights2_[idx].positive += w * w;
                }
                else
                {
                    nentries_[idx].negative++;
                    sumweights_[idx].negative += w;
                    sumweights2_[idx].negative += w * w;
                }
            }
        }
    };

}

#endif
