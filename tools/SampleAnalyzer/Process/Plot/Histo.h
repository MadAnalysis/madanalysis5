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
 * @file Histo.h
 * @brief Histogram with a linear binning.
 */

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

    /**
     * @brief Histogram with a linear binning, attached to signal regions.
     *
     * Each bin, the underflow and the overflow store, for each event weight, the sums of
     * positive and of absolute negative weights.
     */
    class Histo : public PlotBase
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    protected:
        /** @brief Bin contents, underflow and overflow. */
        std::vector<std::vector<WEIGHTS>> histo_;
        std::vector<WEIGHTS> underflow_;
        std::vector<WEIGHTS> overflow_;

        /** @brief Number of bins, bounds and bin width. */
        MAuint32 nbins_;
        MAfloat64 xmin_;
        MAfloat64 xmax_;
        MAfloat64 step_;

        /** @brief Sum of the weights of the entries. */
        std::vector<WEIGHTS> sum_w_;

        /** @brief Sum of the squared weights. */
        std::vector<WEIGHTS> sum_ww_;

        /** @brief Sum of value * weight. */
        std::vector<WEIGHTS> sum_xw_;

        /** @brief Sum of value^2 * weight. */
        std::vector<WEIGHTS> sum_xxw_;

        /** @brief Regions to which the histogram is attached. */
        std::vector<RegionSelection *> regions_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public:
        /** @brief Constructor (100 bins between 0 and 100; the bin containers are not sized). */
        Histo() : PlotBase()
        {
            nbins_ = 100;
            xmin_ = 0;
            xmax_ = 100;
            step_ = (xmax_ - xmin_) / static_cast<MAfloat64>(nbins_);
        }

        /**
         * @brief Constructor (the binning is left uninitialised: used by derived classes).
         *
         * @param name name.
         */
        Histo(const std::string &name) : PlotBase(name) {}

        /**
         * @brief Constructor.
         *
         * @param name name.
         * @param nbins number of bins (100 if 0).
         * @param xmin lower bound.
         * @param xmax upper bound (0-100 if xmin >= xmax).
         */
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

        /** @brief Destructor. */
        virtual ~Histo() {}

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
         * @brief Size the containers for the event weights.
         *
         * @param multiweight weights of an event (only the size is used).
         */
        virtual void _initialize(const WeightCollection &multiweight);

        /**
         * @brief Fill the histogram.
         *
         * @param value value of the observable.
         * @param weights weights of the event.
         */
        void Fill(MAfloat64 value, const WeightCollection &weights);

        /**
         * @brief Write the histogram in the SAF format.
         *
         * @param output output stream.
         */
        virtual void Write_TextFormat(std::ostream *output);

    protected:
        /**
         * @brief Write the description, statistics and data blocks of the histogram.
         *
         * @param output output stream.
         */
        virtual void Write_TextFormatBody(std::ostream *output);
    };

}

#endif
