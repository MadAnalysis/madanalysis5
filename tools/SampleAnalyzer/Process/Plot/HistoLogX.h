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

#ifndef HISTO_LOGX_H
#define HISTO_LOGX_H

// STL headers
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/Histo.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"

namespace MA5
{

    class HistoLogX : public Histo
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    protected:
        // Histogram boundaries in Log scale
        MAfloat64 log_xmin_;
        MAfloat64 log_xmax_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public:
        /// Constructor withtout argument
        HistoLogX() { initialised_ = false; }

        /// Constructor with argument
        HistoLogX(const std::string &name, MAuint32 nbins,
                  MAfloat64 xmin, MAfloat64 xmax) : Histo(name)
        {
            // Setting the description: nbins
            try
            {
                if (nbins == 0)
                    throw EXCEPTION_WARNING("nbins cannot be equal to 0. Using 100 bins.", "", 0);
                nbins_ = nbins;
            }
            catch (const std::exception &e)
            {
                MANAGE_EXCEPTION(e);
                nbins_ = 100;
            }

            // Setting the description: min
            try
            {
                if (xmin <= 0)
                    throw EXCEPTION_WARNING("xmin cannot be less than or equal to zero. Setting xmin to 0.1", "", 0);
                xmin_ = xmin;
            }
            catch (const std::exception &e)
            {
                MANAGE_EXCEPTION(e);
                xmin_ = .1;
            }

            // Setting the description: max
            try
            {
                if (xmin >= xmax)
                    throw EXCEPTION_WARNING("xmin cannot be equal to or greater than xmax. Setting xmin to 0.1 and xmax to 100.", "", 0);
                xmax_ = xmax;
            }
            catch (const std::exception &e)
            {
                MANAGE_EXCEPTION(e);
                xmin_ = .1;
                xmax_ = 100.;
            }

            log_xmin_ = std::log10(xmin_);
            log_xmax_ = std::log10(xmax_);
            step_ = (log_xmax_ - log_xmin_) / static_cast<MAfloat64>(nbins_);

            // Reseting the histogram array
            histo_.resize(nbins_);
            initialised_ = false;
        }

        /// Destructor
        virtual ~HistoLogX() {}

        /// Filling histogram
        void Fill(MAfloat64 value, const WeightCollection &weights);

        /// Write the plot in a Text file
        virtual void Write_TextFormat(std::ostream *output);

        /// @brief initialise the class
        virtual void _initialize(const WeightCollection &weights) {}
    };

}

#endif
