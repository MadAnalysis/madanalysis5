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
        // @jackaraz: these may not be necessary:
        // typedef std::map<int, std::pair<MAfloat64, MAfloat64>>::iterator iterator;
        // typedef std::map<int, std::pair<MAfloat64, MAfloat64>>::const_iterator const_iterator;
        // typedef std::map<int, std::pair<MAfloat64, MAfloat64>>::size_type size_type;

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
        void Fill(const MAint32 &obs, WeightCollection &weights)
        {
            for (auto &weight : weights.GetWeights())
            {
                MAint32 idx = weight.first;
                MAdouble64 w = weight.second;
                // Value not found
                if (stack_.find(obs) == stack_.end())
                    stack_[obs][idx] = WEIGHTS();

                // Value found
                else
                {
                    if (w >= 0)
                    {
                        nentries_[idx].positive++;
                        sum_w_[idx].positive += w;
                        stack_[obs][idx].positive += w;
                    }
                    else
                    {
                        nentries_[idx].negative++;
                        sum_w_[idx].negative += std::fabs(w);
                        stack_[obs][idx].negative += std::fabs(w);
                    }
                }
            }
        }

        /// Write the plot in a ROOT file
        virtual void Write_TextFormat(std::ostream *output)
        {
            // Header
            *output << "<HistoFrequency>" << std::endl;

            // Description
            *output << "  <Description>" << std::endl;
            *output << "    \"" << name_ << "\"" << std::endl;

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
            *output << "  </Statistics>" << std::endl;

            // Data
            *output << "  <Data>" << std::endl;
            MAuint32 i = 0;
            for (auto &it : stack_)
            {
                *output << "      ";
                output->width(15);
                *output << std::left << std::fixed << it.first;

                for (auto &weight : it.second)
                {
                    output->width(15);
                    *output << std::left << std::scientific << weight.second.positive;
                    output->width(15);
                    *output << std::left << std::scientific << weight.second.negative;
                }
                if (i < 2 || i >= (stack_.size() - 2))
                    *output << " # bin " << i + 1 << " / " << stack_.size();
                *output << std::endl;
                i++;
            }
            *output << "  </Data>" << std::endl;

            // Footer
            *output << "</HistoFrequency>" << std::endl;
            *output << std::endl;
        }
    };

}

#endif
