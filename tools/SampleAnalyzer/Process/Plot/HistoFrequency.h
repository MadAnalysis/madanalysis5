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

    class HistoFrequency : public PlotBase
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    protected:
        /// Collection of observables
        std::map<int, std::map<MAint32, WEIGHTS>> stack_;

        /// Sum of event-weights over entries
        std::map<MAint32, WEIGHTS> sum_w_;

        /// RegionSelections attached to the histo
        std::vector<RegionSelection *> regions_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public:
        /// Constructor with argument
        HistoFrequency(const std::string &name) : PlotBase(name) { initialised_ = false; }

        /// Destructor
        virtual ~HistoFrequency() {}

        void _initialize(const WeightCollection &multiweight)
        {
            for (auto &weight : multiweight.GetWeights())
                sum_w_[weight.first] = WEIGHTS();
        }

        /// Setting the linked regions
        void SetSelectionRegions(std::vector<RegionSelection *> myregions)
        {
            regions_.insert(regions_.end(), myregions.begin(), myregions.end());
        }

        /// Checking that all regions of the histo are surviving
        /// Returns 0 if all regions are failing (includes te case with 0 SR)
        /// Returns 1 if all regions are passing
        // returns -1 otherwise
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

        /// Adding an entry for a given observable
        void Fill(const MAint32 &obs, WeightCollection &weights);

        /// Write the plot in a text file
        virtual void Write_TextFormat(std::ostream *output);
    };

}

#endif
