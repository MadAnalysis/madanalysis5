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
 * @file PlotBase.h
 * @brief Base class of the histograms.
 */

#ifndef PLOT_BASE_H
#define PLOT_BASE_H

// STL headers
#include <iostream>
#include <map>
#include <string>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Base/HistoStructures.h"
#include "SampleAnalyzer/Commons/DataFormat/WeightCollection.h"

namespace MA5
{

    /**
     * @brief Base class of the histograms (statistics common to all histogram types).
     *
     * All the containers have one entry per event weight (multiweight support), each
     * storing separately the contributions of positive and negative weights (the latter in
     * absolute value).
     */
    class PlotBase
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    protected:
        /** @brief Name of the histogram. */
        std::string name_;

        /** @brief Number of events (positive/negative weights), per weight. */
        std::vector<ENTRIES> nevents_;

        /** @brief Number of entries, per weight. */
        std::vector<ENTRIES> nentries_;

        /** @brief Sum of the event weights over the events, per weight. */
        std::vector<WEIGHTS> nevents_w_;

        /** @brief Has the histogram not been filled yet for the current event? */
        MAbool fresh_event_;

        /** @brief Have the containers been sized for the event weights? */
        MAbool initialised_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public:
        /** @brief Constructor. */
        PlotBase()
        {
            // Reseting statistical counters
            fresh_event_ = true;
            initialised_ = false;
        }

        /**
         * @brief Constructor.
         *
         * @param name name of the histogram.
         */
        PlotBase(const std::string &name)
        {
            name_ = name;
            fresh_event_ = true;
            initialised_ = false;
        }

        /** @brief Destructor. */
        virtual ~PlotBase() {}

        /**
         * @brief Has the histogram not been filled yet for the current event?
         *
         * @return the flag.
         */
        MAbool FreshEvent() { return fresh_event_; }

        /**
         * @brief Set the fresh-event flag (the containers are sized at the first call).
         *
         * @param tag flag.
         * @param EventWeight weights of the event.
         */
        void SetFreshEvent(MAbool tag, const WeightCollection &EventWeight)
        {
            Initialize(EventWeight);
            fresh_event_ = tag;
        }

        /**
         * @brief Write the histogram in the SAF format.
         *
         * @param output output stream.
         */
        virtual void Write_TextFormat(std::ostream *output) = 0;

        /**
         * @brief Size the containers specific to the derived class.
         *
         * @param multiweight weights of an event (only the size is used).
         */
        virtual void _initialize(const WeightCollection &multiweight) = 0;

        /**
         * @brief Size the containers (only once).
         *
         * @param multiweight weights of an event (only the size is used).
         */
        void Initialize(const WeightCollection &multiweight)
        {
            if (!initialised_)
            {
                MAuint32 n = multiweight.size();
                nevents_.resize(n);
                nentries_.resize(n);
                nevents_w_.resize(n);
                _initialize(multiweight);
                initialised_ = true;
            }
        }

        /**
         * @brief Count an event.
         *
         * @param weights weights of the event.
         */
        void IncrementNEvents(const WeightCollection &weights)
        {
            for (MAuint32 idx = 0; idx < weights.size(); idx++)
            {
                MAdouble64 w = weights[idx];
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
            fresh_event_ = false;
        }

        /**
         * @brief Accessor to the numbers of events.
         *
         * @return the numbers of events, per weight.
         */
        const std::vector<ENTRIES> &GetNEvents() { return nevents_; }

        /**
         * @brief Accessor to the name.
         *
         * @return the name.
         */
        std::string GetName() { return name_; }
    };

}

#endif
