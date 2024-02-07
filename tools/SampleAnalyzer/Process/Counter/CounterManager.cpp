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
#include "SampleAnalyzer/Process/Counter/CounterManager.h"

using namespace MA5;

/// Write the counters in a TEXT file
void CounterManager::Write_TextFormat(SAFWriter &output) const
{
    // header
    *output.GetStream() << "<InitialCounter>" << std::endl;

    // name
    *output.GetStream() << "\"Initial number of events\"      #" << std::endl;

    // nentries
    for (auto &event : initial_.nentries_)
    {
        output.GetStream()->width(15);
        *output.GetStream() << std::left << std::scientific << event.second.positive;
        *output.GetStream() << " ";
        output.GetStream()->width(15);
        *output.GetStream() << std::left << std::scientific << event.second.negative;
    }
    *output.GetStream() << " # nentries" << std::endl;

    // sum of weights
    for (auto &event : initial_.sumweights_)
    {
        output.GetStream()->width(15);
        *output.GetStream() << std::left << std::scientific << event.second.positive;
        *output.GetStream() << " ";
        output.GetStream()->width(15);
        *output.GetStream() << std::left << std::scientific << event.second.negative;
    }
    *output.GetStream() << " # sum of weights" << std::endl;

    // sum of weights^2
    for (auto &event : initial_.sumweights2_)
    {
        output.GetStream()->width(15);
        *output.GetStream() << std::left << std::scientific << event.second.positive;
        *output.GetStream() << " ";
        output.GetStream()->width(15);
        *output.GetStream() << std::left << std::scientific << event.second.negative;
    }
    *output.GetStream() << " # sum of weights^2" << std::endl;

    // foot
    *output.GetStream() << "</InitialCounter>" << std::endl;
    *output.GetStream() << std::endl;

    // Loop over the counters
    for (MAuint32 i = 0; i < counters_.size(); i++)
    {
        // header
        *output.GetStream() << "<Counter>" << std::endl;

        // name
        MAint32 nsp = 30 - counters_[i].name_.size();
        if (nsp < 0)
            nsp = 0;
        *output.GetStream() << "\"" << counters_[i].name_ << "\"";
        for (MAuint32 jj = 0; jj < static_cast<MAuint32>(nsp); jj++)
            *output.GetStream() << " ";
        *output.GetStream() << "# " << i + 1 << "st cut" << std::endl;

        // nentries
        for (auto &event : counters_[i].nentries_)
        {
            output.GetStream()->width(15);
            *output.GetStream() << std::left << std::scientific << event.second.positive;
            *output.GetStream() << " ";
            output.GetStream()->width(15);
            *output.GetStream() << std::left << std::scientific << event.second.negative;
        }
        *output.GetStream() << " # nentries" << std::endl;

        // sum of weights
        for (auto &event : counters_[i].sumweights_)
        {
            output.GetStream()->width(15);
            *output.GetStream() << std::left << std::scientific << event.second.positive;
            *output.GetStream() << " ";
            output.GetStream()->width(15);
            *output.GetStream() << std::left << std::scientific << event.second.negative;
        }
        *output.GetStream() << " # sum of weights" << std::endl;

        // sum of weights^2
        for (auto &event : counters_[i].sumweights2_)
        {
            output.GetStream()->width(15);
            *output.GetStream() << std::left << std::scientific << event.second.positive;
            *output.GetStream() << " ";
            output.GetStream()->width(15);
            *output.GetStream() << std::left << std::scientific << event.second.negative;
        }
        *output.GetStream() << " # sum of weights^2" << std::endl;

        // foot
        *output.GetStream() << "</Counter>" << std::endl;
        *output.GetStream() << std::endl;
    }
}
