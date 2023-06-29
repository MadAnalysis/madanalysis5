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
#include "SampleAnalyzer/Process/Plot/Histo.h"

using namespace MA5;

/// Write the plot in a Text file
void Histo::Write_TextFormat(std::ostream *output)
{
    // Header
    *output << "<Histo>" << std::endl;

    // Write the body
    Write_TextFormatBody(output);

    // Foot
    *output << "</Histo>" << std::endl;
    *output << std::endl;
}

/// Write the plot in a Text file
void Histo::Write_TextFormatBody(std::ostream *output)
{
    // Description
    *output << "  <Description>" << std::endl;

    // Name
    *output << "    \"" << name_ << "\"" << std::endl;

    // Title
    *output << "    ";
    output->width(10);
    *output << std::left << "# nbins";
    output->width(15);
    *output << std::left << "xmin";
    output->width(15);
    *output << std::left << "xmax" << std::endl;

    // Data
    *output << "      ";
    output->width(8);
    *output << std::left << nbins_;
    output->width(15);
    *output << std::left << std::scientific << xmin_;
    output->width(15);
    *output << std::left << std::scientific << xmax_ << std::endl;

    // SelectionRegions
    if (regions_.size() != 0)
    {
        MAuint32 maxlength = 0;
        for (MAuint32 i = 0; i < regions_.size(); i++)
            if (regions_[i]->GetName().size() > maxlength)
                maxlength = regions_[i]->GetName().size();
        *output << std::left << "    # Defined regions" << std::endl;
        for (MAuint32 i = 0; i < regions_.size(); i++)
        {
            *output << "      " << std::setw(maxlength) << std::left << regions_[i]->GetName();
            *output << "    # Region nr. " << std::fixed << i + 1 << std::endl;
        }
    }

    // End description
    *output << "  </Description>" << std::endl;

    // Statistics
    *output << "  <Statistics>" << std::endl;

    *output << "      ";
    for (auto &event : nevents_)
    {
        output->width(15);
        *output << std::fixed << event.second.positive;
        output->width(15);
        *output << std::fixed << event.second.negative;
    }
    *output << " # nevents" << std::endl;
    *output << "      ";
    for (auto &event : nevents_w_)
    {
        output->width(15);
        *output << std::scientific << event.second.positive;
        output->width(15);
        *output << std::scientific << event.second.negative;
    }
    *output << " # sum of event-weights over events" << std::endl;
    *output << "      ";
    for (auto &event : nentries_)
    {
        output->width(15);
        *output << std::fixed << event.second.positive;
        output->width(15);
        *output << std::fixed << event.second.negative;
    }
    *output << " # nentries" << std::endl;
    *output << "      ";
    for (auto &event : sum_w_)
    {
        output->width(15);
        *output << std::scientific << event.second.positive;
        output->width(15);
        *output << std::scientific << event.second.negative;
    }
    *output << " # sum of event-weights over entries" << std::endl;
    *output << "      ";
    for (auto &event : sum_ww_)
    {
        output->width(15);
        *output << std::scientific << event.second.positive;
        output->width(15);
        *output << std::scientific << event.second.negative;
    }
    *output << " # sum weights^2" << std::endl;
    *output << "      ";
    for (auto &event : sum_xw_)
    {
        output->width(15);
        *output << std::scientific << event.second.positive;
        output->width(15);
        *output << std::scientific << event.second.negative;
    }
    *output << " # sum value*weight" << std::endl;
    *output << "      ";
    for (auto &event : sum_xxw_)
    {
        output->width(15);
        *output << std::scientific << event.second.positive;
        output->width(15);
        *output << std::scientific << event.second.negative;
    }
    *output << " # sum value^2*weight" << std::endl;
    *output << "  </Statistics>" << std::endl;

    // Data
    *output << "  <Data>" << std::endl;
    *output << "      ";
    for (auto &event : underflow_)
    {
        output->width(15);
        *output << std::scientific << event.second.positive;
        output->width(15);
        *output << std::scientific << event.second.negative;
    }
    *output << " # underflow" << std::endl;
    for (MAuint32 ibin = 0; ibin < histo_.size(); ibin++)
    {
        *output << "      ";
        for (auto &bin : histo_[ibin])
        {
            output->width(15);
            *output << std::scientific << bin.second.positive;
            output->width(15);
            *output << std::scientific << bin.second.negative;
        }
        if (ibin < 2 || ibin >= (histo_.size() - 2))
            *output << " # bin " << ibin + 1 << " / " << histo_.size();
        *output << std::endl;
    }
    *output << "      ";
    for (auto &event : overflow_)
    {
        output->width(15);
        *output << std::scientific << event.second.positive;
        output->width(15);
        *output << std::scientific << event.second.negative;
    }
    *output << " # overflow" << std::endl;
    *output << "  </Data>" << std::endl;
}

/// @brief Initialise the containers
/// @param weights weight collection
void Histo::_initialize(const WeightCollection &weights)
{
    std::map<MAint32, WEIGHTS> current;
    for (auto &w : weights.GetWeights())
    {
        MAint32 idx = w.first;
        underflow_[idx] = WEIGHTS();
        overflow_[idx] = WEIGHTS();
        sum_w_[idx] = WEIGHTS();
        sum_ww_[idx] = WEIGHTS();
        sum_xw_[idx] = WEIGHTS();
        sum_xxw_[idx] = WEIGHTS();
    }
}

/// Filling histogram
void Histo::Fill(MAfloat64 value, const WeightCollection &weights)
{
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
                histo_[std::floor((value - xmin_) / step_)][idx].positive += weight;
        }

        // Negative weight
        else
        {
            MAdouble64 pw = std::fabs(weight);
            nentries_[idx].negative++;
            sum_w_[idx].negative += pw;
            sum_ww_[idx].negative += pw * pw;
            sum_xw_[idx].negative += value * pw;
            sum_xxw_[idx].negative += value * value * pw;
            if (value < xmin_)
                underflow_[idx].negative += pw;
            else if (value >= xmax_)
                overflow_[idx].negative += pw;
            else
                histo_[std::floor((value - xmin_) / step_)][idx].negative += pw;
        }
    }
}
