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
 * @file RegionSelectionManager.h
 * @brief Manager of the signal regions, cuts and histograms of an analysis (Manager()).
 */

#ifndef __REGIONSELECTIONMANAGER_H
#define __REGIONSELECTIONMANAGER_H

// STL headers
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>

/** @brief assert with a message. */
#define assertm(exp, msg) assert(((void)msg, exp))

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Counter/MultiRegionCounterManager.h"
#include "SampleAnalyzer/Process/Plot/PlotManager.h"
#include "SampleAnalyzer/Process/RegionSelection/RegionSelection.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Process/Writer/SAFWriter.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"

namespace MA5
{

    /**
     * @brief Manager of the signal regions, cuts and histograms of an analysis.
     *
     * Typical usage in an analysis:
     * - Initialize(): AddRegionSelection(), AddCut(), AddHisto();
     * - Execute(): ApplyCut() for each cut (false once all the regions fail),
     *   FillHisto(), IsSurviving().
     * The event weights are set by SampleAnalyzer::PrepareForExecution(); they can be
     * modified with SetCurrentEventWeight() or per region with SetRegionWeight().
     */
    class RegionSelectionManager
    {
        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    private:
        /** @brief Signal regions (owned). */
        std::vector<RegionSelection *> regions_;

        /** @brief Histograms. */
        PlotManager plotmanager_;

        /** @brief Cuts. */
        MultiRegionCounterManager cutmanager_;

        /** @brief Number of regions still surviving for the current event. */
        MAuint32 NumberOfSurvivingRegions_;

        /** @brief Weights of the current event. */
        WeightCollection weight_;

        /** @brief Weights of the current event specific to some regions (cut-flows only). */
        std::map<std::string, WeightCollection> region_weight_;

        // -------------------------------------------------------------
        //                      method members
        // -------------------------------------------------------------
    public:
        /** @brief Constructor. */
        RegionSelectionManager() {};

        /** @brief Destructor (deletes the regions). */
        ~RegionSelectionManager()
        {
            for (auto &region_pointer : regions_)
                delete region_pointer;
        };

        /** @brief Clear the regions (without deleting them), the cuts, the histograms and the weights. */
        void Reset()
        {
            NumberOfSurvivingRegions_ = 0;
            // NOTE: the regions are not deleted here (memory leak).
            regions_.clear();
            cutmanager_.Finalize();
            plotmanager_.Finalize();
            weight_.clear();
            region_weight_.clear();
        }

        /** @brief Clear the regions, the cuts, the histograms and the weights. */
        void Finalize() { Reset(); }

        /**
         * @brief Accessor to the regions.
         *
         * @return a copy of the collection.
         */
        std::vector<RegionSelection *> Regions() { return regions_; }

        /**
         * @brief Accessor to the cut manager.
         *
         * @return a pointer to the cut manager.
         */
        MultiRegionCounterManager *GetCutManager() { return &cutmanager_; }

        /**
         * @brief Accessor to the histogram manager.
         *
         * @return a pointer to the histogram manager.
         */
        PlotManager *GetPlotManager() { return &plotmanager_; }

        /**
         * @brief Accessor to the weights of the current event.
         *
         * @return a copy of the weights.
         */
        const WeightCollection GetCurrentEventWeights() const { return weight_; }

        /**
         * @brief Accessor to the nominal weight of the current event (backward compatibility).
         *
         * @return the nominal weight.
         */
        const MAdouble64 GetCurrentEventWeight() const { return weight_[0]; }


        /**
         * @brief Build a weight collection from a nominal weight, keeping the ratios of the variations to the nominal weight.
         *
         * Required for backward compatibility with older PAD analyses.
         *
         * @param scalar_w new nominal weight.
         * @return the rescaled weights.
         */
        WeightCollection BuildScalarWeights(MAfloat64 scalar_w) const
        {
            // With only one weight, no rescaling is needed.
            if (weight_.size()==1)
                return WeightCollection(1, scalar_w);

            // Safety - no correction can be inferred from a zero nominal weight.
            const MAfloat64 nominal = weight_[0];
            if (nominal==0.0)
            {
                if (scalar_w==0.0)
                    return weight_;
                // NOTE: throws std::invalid_argument (not a SampleAnalyzer exception) when the current nominal weight
                //   is 0.
                throw std::invalid_argument("BuildScalarWeights: cannot rescale multiple weights from a zero nominal to a nonzero nominal");
            }

            // Creates a new collection with all weights scaled accordingly
            // Stores the provided nominal weight exactly to avoid rounding from rescaling
            const MAfloat64 factor = scalar_w/nominal;
            WeightCollection result(weight_);
            result *= factor;
            result.Add(0, scalar_w);

            // Output
            return result;
        }

        /**
         * @brief Set the weights of the current event.
         *
         * @param weight weights.
         */
        void SetCurrentEventWeight(WeightCollection &weight) { weight_ = WeightCollection(weight); }

        /**
         * @brief Set the weights of the current event.
         *
         * @param weight weights.
         */
        void SetCurrentEventWeight(const WeightCollection &weight) { weight_ = WeightCollection(weight); }


        /**
         * @brief Set the nominal weight of the current event; the variations are rescaled by the same factor
         * (backward compatibility with older PAD analyses).
         *
         * @param weight nominal weight.
         */
        void SetCurrentEventWeight(MAfloat64 weight) { weight_ = BuildScalarWeights(weight); }


        /**
         * @brief Set weights specific to a region (used for its cut-flow).
         *
         * @param name name of the region.
         * @param weight weights.
         */
        void SetRegionWeight(std::string name, WeightCollection &weight)
        {
            region_weight_[name] = WeightCollection(weight);
        }

        /**
         * @brief Set the nominal weight of a region (variations rescaled, see SetCurrentEventWeight()).
         *
         * @param name name of the region.
         * @param weight nominal weight.
         */
        void SetRegionWeight(std::string name, MAfloat64 weight)
        {
            const WeightCollection region_weights = BuildScalarWeights(weight);
            region_weight_[name] = region_weights;
        }


        /**
         * @brief Declare a signal region.
         *
         * @param name name of the region.
         */
        void AddRegionSelection(const std::string &name)
        {
            std::string myname = name;
            if (myname.compare("") == 0)
            {
                std::stringstream numstream;
                numstream << regions_.size();
                myname = "RegionSelection" + numstream.str();
            }
            // FIXME: 'name' is used instead of 'myname': the default name of an unnamed region is never applied.
            RegionSelection *myregion = new RegionSelection(name);
            regions_.push_back(myregion);
        }

        /**
         * @brief Deprecated: the event weights are now set by SampleAnalyzer::PrepareForExecution() (no-op).
         *
         * @param EventWeight ignored.
         */
        void InitializeForNewEvent(MAfloat64 EventWeight) {}

        /**
         * @brief Prepare all the regions and histograms for a new event.
         *
         * @param EventWeight weights of the event.
         */
        void InitializeForNewEvent(const WeightCollection &EventWeight)
        {
            weight_.SetWeights(EventWeight.GetWeights());
            region_weight_.clear();
            NumberOfSurvivingRegions_ = regions_.size();
            for (auto &reg : regions_)
                reg->InitializeForNewEvent(EventWeight);
            for (MAuint32 i = 0; i < plotmanager_.GetNplots(); i++)
                plotmanager_.GetHistos()[i]->SetFreshEvent(true, EventWeight);
        }

        /**
         * @brief Declare a cut applied to all the regions.
         *
         * @param name name of the cut (a default name is used if empty).
         */
        void AddCut(const std::string &name)
        {
            // The name of the cut
            std::string myname = name;
            if (myname.compare("") == 0)
            {
                std::stringstream numstream;
                numstream << cutmanager_.GetCuts().size();
                myname = "Cut" + numstream.str();
            }
            // Adding the cut to all the regions
            cutmanager_.AddCut(myname, regions_);
        }

        /**
         * @brief Declare a cut applied to one region.
         *
         * @param name name of the cut.
         * @param RSname name of the region.
         */
        void AddCut(const std::string &name, const std::string &RSname)
        {
            std::string RSnameA[] = {RSname};
            AddCut(name, RSnameA);
        }

        /**
         * @brief Declare a cut applied to several regions.
         *
         * @tparam NRS number of regions.
         * @param name name of the cut.
         * @param RSnames names of the regions.
         */
        template <int NRS>
        void AddCut(const std::string &name, std::string const (&RSnames)[NRS])
        {
            // The name of the cut
            std::string myname = name;
            if (myname.compare("") == 0)
            {
                std::stringstream numstream;
                numstream << cutmanager_.GetCuts().size();
                myname = "Cut" + numstream.str();
            }

            // Creating the vector of SR of interests
            std::vector<RegionSelection *> myregions;
            for (MAuint32 i = 0; i < NRS; i++)
            {
                for (MAuint32 j = 0; j < regions_.size(); j++)
                {
                    if (regions_[j]->GetName().compare(RSnames[i]) == 0)
                    {
                        myregions.push_back(regions_[j]);
                        break;
                    }
                }

                try
                {
                    if (myregions.size() == i)
                        throw EXCEPTION_WARNING("Assigning the cut \"" + name +
                                                    "\" to the non-existing signal region \"" + RSnames[i] +
                                                    "\"",
                                                "", 0);
                }
                catch (const std::exception &e)
                {
                    MANAGE_EXCEPTION(e);
                }
            }

            // Creating the cut
            cutmanager_.AddCut(myname, myregions);
        }

        /**
         * @brief Declare a cut applied to several regions.
         *
         * @param name name of the cut.
         * @param SRnames names of the regions.
         */
        void AddCut(const std::string &name, std::vector<std::string> SRnames)
        {
            // The name of the cut
            std::string myname = name;
            if (myname.compare("") == 0)
            {
                std::stringstream numstream;
                numstream << cutmanager_.GetCuts().size();
                myname = "Cut" + numstream.str();
            }

            // Creating the vector of SR of interests
            std::vector<RegionSelection *> myregions;
            for (MAuint32 i = 0; i < SRnames.size(); i++)
            {
                for (MAuint32 j = 0; j < regions_.size(); j++)
                {
                    if (regions_[j]->GetName().compare(SRnames[i]) == 0)
                    {
                        myregions.push_back(regions_[j]);
                        break;
                    }
                }

                try
                {
                    if (myregions.size() == i)
                        throw EXCEPTION_WARNING("Assigning the cut \"" + name +
                                                    "\" to the non-existing signal region \"" + SRnames[i] +
                                                    "\"",
                                                "", 0);
                }
                catch (const std::exception &e)
                {
                    MANAGE_EXCEPTION(e);
                }
            }

            // Creating the cut
            cutmanager_.AddCut(myname, myregions);
        }

        /**
         * @brief Apply a cut to the surviving regions attached to it.
         *
         * @param condition result of the cut for the current event.
         * @param cut name of the cut.
         * @return false if no region survives anymore (the event can be skipped), true otherwise (also for an undeclared cut).
         */
        MAbool ApplyCut(MAbool, std::string const &);

        /**
         * @brief Declare a frequency histogram attached to all the regions.
         *
         * @param name name of the histogram.
         */
        void AddHistoFrequency(const std::string &name)
        {
            // The name of the histo
            std::string myname = name;
            if (myname.compare("") == 0)
            {
                std::stringstream numstream;
                numstream << plotmanager_.GetHistos().size();
                myname = "Histo" + numstream.str();
            }
            // Adding the histo and linking all regions to the histo
            plotmanager_.Add_HistoFrequency(myname, regions_);
        }

        /**
         * @brief Declare a histogram attached to all the regions.
         *
         * @param name name.
         * @param nb number of bins.
         * @param xmin lower bound.
         * @param xmax upper bound.
         */
        void AddHisto(const std::string &name, MAuint32 nb, MAfloat64 xmin, MAfloat64 xmax)
        {
            // The name of the histo
            std::string myname = name;
            if (myname.compare("") == 0)
            {
                std::stringstream numstream;
                numstream << plotmanager_.GetHistos().size();
                myname = "Histo" + numstream.str();
            }
            // Adding the histo and linking all regions to the histo
            plotmanager_.Add_Histo(myname, nb, xmin, xmax, regions_);
        }

        /**
         * @brief Declare a histogram with a logarithmic binning attached to all the regions.
         *
         * @param name name.
         * @param nb number of bins.
         * @param xmin lower bound.
         * @param xmax upper bound.
         */
        void AddHistoLogX(const std::string &name, MAuint32 nb, MAfloat64 xmin, MAfloat64 xmax)
        {
            // The name of the histo
            std::string myname = name;
            if (myname.compare("") == 0)
            {
                std::stringstream numstream;
                numstream << plotmanager_.GetHistos().size();
                myname = "Histo" + numstream.str();
            }
            // Adding the histo and linking all regions to the histo
            plotmanager_.Add_HistoLogX(myname, nb, xmin, xmax, regions_);
        }

        /**
         * @brief Declare a histogram attached to one region.
         *
         * @param name name.
         * @param nb number of bins.
         * @param xmin lower bound.
         * @param xmax upper bound.
         * @param RSname name of the region.
         */
        void AddHisto(const std::string &name, MAuint32 nb, MAfloat64 xmin, MAfloat64 xmax,
                      const std::string &RSname)
        {
            std::string RSnameA[] = {RSname};
            AddHisto(name, nb, xmin, xmax, RSnameA);
        }

        /**
         * @brief Declare a histogram with a logarithmic binning attached to one region.
         *
         * @param name name.
         * @param nb number of bins.
         * @param xmin lower bound.
         * @param xmax upper bound.
         * @param RSname name of the region.
         */
        void AddHistoLogX(const std::string &name, MAuint32 nb, MAfloat64 xmin, MAfloat64 xmax,
                          const std::string &RSname)
        {
            std::string RSnameA[] = {RSname};
            AddHistoLogX(name, nb, xmin, xmax, RSnameA);
        }
        /**
         * @brief Declare a frequency histogram attached to one region.
         *
         * @param name name.
         * @param RSname name of the region.
         */
        void AddHistoFrequency(const std::string &name, const std::string &RSname)
        {
            std::string RSnameA[] = {RSname};
            AddHistoFrequency(name, RSnameA);
        }

        /**
         * @brief Declare a histogram attached to several regions.
         *
         * @tparam NRS number of regions.
         * @param name name.
         * @param nb number of bins.
         * @param xmin lower bound.
         * @param xmax upper bound.
         * @param RSnames names of the regions.
         */
        template <int NRS>
        void AddHisto(const std::string &name, MAuint32 nb,
                      MAfloat64 xmin, MAfloat64 xmax, std::string const (&RSnames)[NRS])
        {
            // The name of the histo
            std::string myname = name;
            if (myname.compare("") == 0)
            {
                std::stringstream numstream;
                numstream << plotmanager_.GetNplots();
                myname = "Histo" + numstream.str();
            }
            // Creating the vector of SR of interests
            std::vector<RegionSelection *> myregions;
            for (MAuint32 i = 0; i < NRS; i++)
            {
                for (MAuint32 j = 0; j < regions_.size(); j++)
                {
                    if (regions_[j]->GetName().compare(RSnames[i]) == 0)
                    {
                        myregions.push_back(regions_[j]);
                        break;
                    }
                }
                try
                {
                    if (myregions.size() == i)
                        throw EXCEPTION_WARNING("Assigning the histo \"" + name +
                                                    "\" to the non-existing signal region \"" + RSnames[i] +
                                                    "\"",
                                                "", 0);
                }
                catch (const std::exception &e)
                {
                    MANAGE_EXCEPTION(e);
                }
            }

            // Creating the histo
            plotmanager_.Add_Histo(myname, nb, xmin, xmax, myregions);
        }

        /**
         * @brief Declare a histogram with a logarithmic binning attached to several regions.
         *
         * @tparam NRS number of regions.
         * @param name name.
         * @param nb number of bins.
         * @param xmin lower bound.
         * @param xmax upper bound.
         * @param RSnames names of the regions.
         */
        template <int NRS>
        void AddHistoLogX(const std::string &name, MAuint32 nb,
                          MAfloat64 xmin, MAfloat64 xmax, std::string const (&RSnames)[NRS])
        {
            // The name of the histo
            std::string myname = name;
            if (myname.compare("") == 0)
            {
                std::stringstream numstream;
                numstream << plotmanager_.GetNplots();
                myname = "Histo" + numstream.str();
            }
            // Creating the vector of SR of interests
            std::vector<RegionSelection *> myregions;
            for (MAuint32 i = 0; i < NRS; i++)
            {
                for (MAuint32 j = 0; j < regions_.size(); j++)
                {
                    if (regions_[j]->GetName().compare(RSnames[i]) == 0)
                    {
                        myregions.push_back(regions_[j]);
                        break;
                    }
                }
                try
                {
                    if (myregions.size() == i)
                        throw EXCEPTION_WARNING("Assigning the histo \"" + name +
                                                    "\" to the non-existing signal region \"" + RSnames[i] +
                                                    "\"",
                                                "", 0);
                }
                catch (const std::exception &e)
                {
                    MANAGE_EXCEPTION(e);
                }
            }

            // Creating the histo
            plotmanager_.Add_HistoLogX(myname, nb, xmin, xmax, myregions);
        }

        /**
         * @brief Declare a frequency histogram attached to several regions.
         *
         * @tparam NRS number of regions.
         * @param name name.
         * @param RSnames names of the regions.
         */
        template <int NRS>
        void AddHistoFrequency(const std::string &name, std::string const (&RSnames)[NRS])
        {
            // The name of the histo
            std::string myname = name;
            if (myname.compare("") == 0)
            {
                std::stringstream numstream;
                numstream << plotmanager_.GetNplots();
                myname = "Histo" + numstream.str();
            }
            // Creating the vector of SR of interests
            std::vector<RegionSelection *> myregions;
            for (MAuint32 i = 0; i < NRS; i++)
            {
                for (MAuint32 j = 0; j < regions_.size(); j++)
                {
                    if (regions_[j]->GetName().compare(RSnames[i]) == 0)
                    {
                        myregions.push_back(regions_[j]);
                        break;
                    }
                }
                try
                {
                    if (myregions.size() == i)
                        throw EXCEPTION_WARNING("Assigning the histo \"" + name +
                                                    "\" to the non-existing signal region \"" + RSnames[i] +
                                                    "\"",
                                                "", 0);
                }
                catch (const std::exception &e)
                {
                    MANAGE_EXCEPTION(e);
                }
            }

            // Creating the histo
            plotmanager_.Add_HistoFrequency(myname, myregions);
        }

        /**
         * @brief Fill a histogram if all its regions survive (warning if only some of them survive).
         *
         * @param histname name of the histogram.
         * @param val value.
         */
        void FillHisto(std::string const &, MAfloat64 val);

        /**
         * @brief Write the list of regions in the SAF format.
         *
         * @param output SAF writer.
         */
        void WriteHistoDefinition(SAFWriter &output);

        /**
         * @brief Does the current event survive the cuts of a region?
         *
         * @param RSname name of the region.
         * @return the survival status (false with a warning for an unknown region).
         */
        MAbool IsSurviving(const std::string &RSname)
        {
            // Looking for the region and checking its status
            for (MAuint32 i = 0; i < regions_.size(); i++)
            {
                if (regions_[i]->GetName().compare(RSname) == 0)
                    return regions_[i]->IsSurviving();
            }

            // The region has not been found
            try
            {
                throw EXCEPTION_WARNING("Checking whether the non-declared region \"" +
                                            RSname + "\" is surviving the applied cuts.",
                                        "", 0);
            }
            catch (const std::exception &e)
            {
                MANAGE_EXCEPTION(e);
            }

            return false;
        }

        /**
         * @brief Write the names of the regions (as `<analysis>::<region>`, comma-separated).
         *
         * @param outwriter output stream.
         * @param ananame name of the analysis.
         * @param is_first is this the first analysis written?
         */
        void HeadSR(std::ostream &, const std::string &, MAbool &);
        /**
         * @brief Write the survival status of the regions (comma-separated).
         *
         * @param outwriter output stream.
         * @param is_first is this the first analysis written?
         */
        void DumpSR(std::ostream &, MAbool &);
    };

}
#endif
