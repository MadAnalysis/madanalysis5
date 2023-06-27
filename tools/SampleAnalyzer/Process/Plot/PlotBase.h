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

#ifndef PLOT_BASE_H
#define PLOT_BASE_H

// STL headers
#include <iostream>
#include <map>
#include <string>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Base/Basics.h"
#include "SampleAnalyzer/Commons/DataFormat/WeightCollection.h"

namespace MA5
{

    class PlotBase
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    protected:
        /// Name of the plots
        std::string name_;

        /// @brief number of events. entries object includes positive and negative accessors
        std::map<MAint32, ENTRIES> nevents_;

        /// @brief Number of entries
        std::map<MAint32, ENTRIES> nentries_;

        /// @brief Sum of event-weight over events
        std::map<MAint32, WEIGHTS> nevents_w_;

        /// Flag telling whether a given histo has already been modified for an event
        MAbool fresh_event_;

        /// @brief is the plot initialised
        MAbool initialised_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public:
        /// Constructor without argument
        PlotBase()
        {
            // Reseting statistical counters
            nevents_.insert(std::make_pair(0, ENTRIES()));
            nentries_.insert(std::make_pair(0, ENTRIES()));
            nevents_w_.insert(std::make_pair(0, WEIGHTS()));
            fresh_event_ = true;
        }

        /// Constructor with argument
        PlotBase(const std::string &name)
        {
            name_ = name;
            nevents_.insert(std::make_pair(0, ENTRIES()));
            nentries_.insert(std::make_pair(0, ENTRIES()));
            nevents_w_.insert(std::make_pair(0, WEIGHTS()));
            fresh_event_ = true;
        }

        /// Destructor
        virtual ~PlotBase() {}

        /// Accesor for fresh_event
        MAbool FreshEvent() { return fresh_event_; }

        /// Modifier for fresh_event
        void SetFreshEvent(MAbool tag) { fresh_event_ = tag; }

        /// Write the plot in a ROOT file
        virtual void Write_TextFormat(std::ostream *output) = 0;

        /// @brief Initialise the containers
        /// @param multiweight multiweight collection
        void Initialise(const WeightCollection &multiweight)
        {
            if (!initialised_)
            {
                for (auto &weight : multiweight.GetWeights())
                {

                    nevents_[weight.first] = ENTRIES();
                    nentries_[weight.first] = ENTRIES();
                    nevents_w_[weight.first] = WEIGHTS();
                }
            }
        }

        /// Increment number of events
        void IncrementNEvents(const WeightCollection &weights)
        {
            Initialise(weights);
            for (auto &weight : weights.GetWeights())
            {
                MAint32 idx = weight.first;
                MAdouble64 w = weight.second;
                if (w >= 0)
                {
                    nevents_[idx].positive++;
                    nevents_w_[idx].positive += w;
                }
                else
                {
                    nevents_[idx].negative++;
                    nevents_w_[idx].negative += std::fabs(w);
                }
            }
            SetFreshEvent(false);
        }

        /// Return Number of events
        const std::map<MAint32, ENTRIES> &GetNEvents() { return nevents_; }

        // Return the name
        std::string GetName() { return name_; }
    };

}

#endif
