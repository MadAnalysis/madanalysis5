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

#ifndef HISTO_H
#define HISTO_H

// STL headers
#include <map>
#include <cmath>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/PlotBase.h"
#include "SampleAnalyzer/Process/RegionSelection/RegionSelection.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"

namespace MA5
{

    class Histo : public PlotBase
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    protected:
        /// Each variable is defined with WEIGHTS object which includes positive and negative accessors
        /// these are for positive and negative bins. std::map<MAint32,WEIGHTS> contains a map of
        /// different PDF and their corresponding positive and negative weights.

        /// Histogram arrays
        std::vector<std::map<MAint32, WEIGHTS>> histo_;
        std::map<MAint32, WEIGHTS> underflow_;
        std::map<MAint32, WEIGHTS> overflow_;

        /// Histogram description
        MAuint32 nbins_;
        MAfloat64 xmin_;
        MAfloat64 xmax_;
        MAfloat64 step_;

        /// Sum of event-weights over entries
        std::map<MAint32, WEIGHTS> sum_w_;

        /// Sum of squared weights
        std::map<MAint32, WEIGHTS> sum_ww_;

        /// Sum of value * weight
        std::map<MAint32, WEIGHTS> sum_xw_;

        /// Sum of value * value * weight
        std::map<MAint32, WEIGHTS> sum_xxw_;

        /// RegionSelections attached to the histo
        std::vector<RegionSelection *> regions_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public:
        /// Constructor without argument
        Histo() : PlotBase()
        {
            nbins_ = 100;
            xmin_ = 0;
            xmax_ = 100;
            step_ = (xmax_ - xmin_) / static_cast<MAfloat64>(nbins_);
        }

        /// Constructor with argument
        Histo(const std::string &name) : PlotBase(name) {}

        /// Constructor with argument
        Histo(const std::string &name, MAuint32 nbins, MAfloat64 xmin, MAfloat64 xmax) : PlotBase(name)
        {
            // Setting the description: nbins
            try
            {
                if (nbins == 0)
                    throw EXCEPTION_WARNING("nbins cannot be equal to 0. 100 bins will be used.", "", 0);
                nbins_ = nbins;
            }
            catch (const std::exception &e)
            {
                MANAGE_EXCEPTION(e);
                nbins_ = 100;
            }

            // Setting the description: min & max
            try
            {
                if (xmin >= xmax)
                    throw EXCEPTION_WARNING("xmin cannot be equal to or greater than xmax. Setting xmin to 0 and xmax to 100.", "", 0);
                xmin_ = xmin;
                xmax_ = xmax;
            }
            catch (const std::exception &e)
            {
                MANAGE_EXCEPTION(e);
                xmin_ = 0.;
                xmax_ = 100.;
            }

            step_ = (xmax_ - xmin_) / static_cast<MAfloat64>(nbins_);

            /// resize takes care of initialisation
            histo_.resize(nbins_);
        }

        /// Destructor
        virtual ~Histo() {}

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

        /// Initialise the class
        /// @brief Initialise the containers
        /// @param weights weight collection
        virtual void _initialize(const WeightCollection &multiweight);

        /// Filling histogram
        void Fill(MAfloat64 value, const WeightCollection &weights);

        /// Write the plot in a text file
        virtual void Write_TextFormat(std::ostream *output);

    protected:
        /// Write the plot in a text file
        virtual void Write_TextFormatBody(std::ostream *output);
    };

}

#endif
