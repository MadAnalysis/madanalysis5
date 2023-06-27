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

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/HistoLogX.h"

using namespace MA5;

/// Write the plot in a Text file
void HistoLogX::Write_TextFormat(std::ostream *output)
{
    // Header
    *output << "<HistoLogX>" << std::endl;

    // Write the body
    Write_TextFormatBody(output);

    // Foot
    *output << "</HistoLogX>" << std::endl;
    *output << std::endl;
}

void HistoLogX::Fill(MAfloat64 value, const WeightCollection &weights)
{
    Initialise(weights);
    // Safety : nan or isinf
    try
    {
        if (std::isnan(value))
            throw EXCEPTION_WARNING("Skipping a NaN (Not a Number) value in an histogram.", "", 0);
        if (std::isinf(value))
            throw EXCEPTION_WARNING("Skipping a Infinity value in an histogram.", "", 0);
    }
    catch (const std::exception &e)
    {
        MANAGE_EXCEPTION(e);
    }

    for (auto &w : weights.GetWeights())
    {
        MAdouble64 weight = w.second;
        MAint32 idx = w.first;
        // Positive weight
        if (weight >= 0)
        {
            nentries_[idx].positive++;
            sum_w_[idx].positive += weight;
            sum_ww_[idx].positive += weight * weight;
            sum_xw_[idx].positive += value * weight;
            sum_xxw_[idx].positive += value * value * weight;
            if (value < xmin_)
                underflow_[idx].positive += weight;
            else if (value >= xmax_)
                overflow_[idx].positive += weight;
            else
                histo_[std::floor((std::log10(value) - log_xmin_) / step_)][idx].positive += weight;
        }

        // Negative weight
        else
        {
            nentries_[idx].negative++;
            weight = std::fabs(weight);
            sum_w_[idx].negative += weight;
            sum_ww_[idx].negative += weight * weight;
            sum_xw_[idx].negative += value * weight;
            sum_xxw_[idx].negative += value * value * weight;
            if (value < xmin_)
                underflow_[idx].negative += weight;
            else if (value >= xmax_)
                overflow_[idx].negative += weight;
            else
                histo_[std::floor((std::log10(value) - log_xmin_) / step_)][idx].negative += weight;
        }
    }
}
