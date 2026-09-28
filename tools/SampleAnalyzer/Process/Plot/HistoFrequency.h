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
 * @file HistoFrequency.h
 * @brief Histogram of the frequencies of integer values (e.g. PDG codes).
 */

#ifndef HISTO_FREQUENCY_H
#define HISTO_FREQUENCY_H

// STL headers
#include <map>
#include <string>
#include <sstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/PlotBase.h"
#include "SampleAnalyzer/Process/RegionSelection/RegionSelection.h"

namespace MA5
{

    /** @brief Histogram counting the occurrences of integer values (used for NPID/NAPID). */
    class HistoFrequency : public PlotBase
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    protected:
        /** @brief Sum of the weights for each value. */
        std::map<int, std::vector<WEIGHTS>> stack_;

        /** @brief Sum of the weights of the entries. */
        std::vector<WEIGHTS> sum_w_;

        /** @brief Regions to which the histogram is attached. */
        std::vector<RegionSelection *> regions_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public:
        /**
         * @brief Constructor.
         *
         * @param name name.
         */
        HistoFrequency(const std::string &name) : PlotBase(name) { initialised_ = false; }

        /** @brief Destructor. */
        virtual ~HistoFrequency() {}

        /**
         * @brief Size the containers for the event weights.
         *
         * @param multiweight weights of an event (only the size is used).
         */
        void _initialize(const WeightCollection &multiweight) { sum_w_.resize(multiweight.size()); }

        /**
         * @brief Attach the histogram to regions.
         *
         * @param myregions regions.
         */
        void SetSelectionRegions(std::vector<RegionSelection *> myregions)
        {
            regions_.insert(regions_.end(), myregions.begin(), myregions.end());
        }

        /**
         * @brief Check the status of the attached regions.
         *
         * @return 1 if all the regions survive, 0 if all fail (or no region), -1 otherwise.
         */
        MAint32 AllSurviving()
        {
            if (regions_.size() == 0)
                return 0;
            MAbool FirstRegionSurvival = regions_[0]->IsSurviving();
            for (MAuint32 ii = 1; ii < regions_.size(); ii++)
                if (regions_[ii]->IsSurviving() != FirstRegionSurvival)
                    return -1;
            if (FirstRegionSurvival)
                return 1;
            else
                return 0;
        }

        /**
         * @brief Count an occurrence of a value.
         *
         * @param obs value.
         * @param weights weights of the event.
         */
        void Fill(const MAint32 &obs, WeightCollection &weights);

        /**
         * @brief Write the histogram in the SAF format.
         *
         * @param output output stream.
         */
        virtual void Write_TextFormat(std::ostream *output);
    };

}

#endif
