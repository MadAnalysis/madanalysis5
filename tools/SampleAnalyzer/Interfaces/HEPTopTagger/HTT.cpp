////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//
//  This file is part of MadAnalysis 5.
//  Official website: <https://launchpad.net/madanalysis5>
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

#include "HEPTopTagger/HEPTopTagger.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/substructure/ClusterBase.h"
#include "SampleAnalyzer/Interfaces/HEPTopTagger/HTT.h"

namespace MA5 {
    namespace Substructure {

        //============================//
        //        Initialization      //
        //============================//

        void HTT::Initialize(InputParameters& param)
        {
            _tagger = new fastjet::HEPTopTagger::HEPTopTagger();

            // Optimal R
            _tagger->do_optimalR(param.do_optimalR);
            _tagger->set_optimalR_min(param.optimalR_min);
            _tagger->set_optimalR_step(param.optimalR_step);
            _tagger->set_optimalR_threshold(param.optimalR_threshold);

            // Candidate selection
            fastjet::HEPTopTagger::Mode mode;
            if (param.mode == HTT::EARLY_MASSRATIO_SORT_MASS)
                mode = fastjet::HEPTopTagger::EARLY_MASSRATIO_SORT_MASS;
            else if (param.mode == HTT::LATE_MASSRATIO_SORT_MASS)
                mode = fastjet::HEPTopTagger::LATE_MASSRATIO_SORT_MASS;
            else if (param.mode == HTT::EARLY_MASSRATIO_SORT_MODDJADE)
                mode = fastjet::HEPTopTagger::EARLY_MASSRATIO_SORT_MODDJADE;
            else if (param.mode == HTT::LATE_MASSRATIO_SORT_MODDJADE)
                mode = fastjet::HEPTopTagger::LATE_MASSRATIO_SORT_MODDJADE;
            else
                mode = fastjet::HEPTopTagger::TWO_STEP_FILTER;
            _tagger->set_mode(mode);
            _tagger->set_mt(param.top_mass);
            _tagger->set_mw(param.W_mass);
            _tagger->set_top_mass_range(param.Mtop_min, param.Mtop_max);
            _tagger->set_fw(param.fw);
            _tagger->set_mass_ratio_range(param.mass_ratio_range_min, param.mass_ratio_range_max);
            _tagger->set_mass_ratio_cut(param.m23cut, param.m13cutmin, param.m13cutmax);

            // Filtering
            _tagger->set_filtering_n(param.filt_N);
            _tagger->set_filtering_R(param.filtering_R);
            _tagger->set_filtering_minpt_subjet(param.filtering_minpt);
            _tagger->set_filtering_jetalgorithm(
                ClusterBase().__get_clustering_algorithm(param.filtering_algorithm)
            );

            // MassDrop
            _tagger->set_mass_drop_threshold(param.mass_drop);
            _tagger->set_mass_drop_threshold(param.mass_drop);

            // Pruning
            _tagger->set_pruning_rcut_factor(param.prun_rcut);
            _tagger->set_pruning_zcut(param.prun_zcut);
        }

        //====================//
        //       Execute      //
        //====================//

        // Method to run top tagger
        void HTT::Execute(const RecJetFormat *jet) { _tagger->run(jet->pseudojet()); }

        //======================//
        //       Accessors      //
        //======================//

        // accessor to top jet
        const RecJetFormat * HTT::top() const { return ClusterBase().__transform_jet(_tagger->t());}

        //accessor to bottom jet
        const RecJetFormat * HTT::b() const { return ClusterBase().__transform_jet(_tagger->b());}

        //accessor to W jet
        const RecJetFormat * HTT::W() const { return ClusterBase().__transform_jet(_tagger->W());}

        //accessor to leading subjet from W
        const RecJetFormat * HTT::W1() const { return ClusterBase().__transform_jet(_tagger->W1());}

        //accessor to second leading subjet from W
        const RecJetFormat * HTT::W2() const { return ClusterBase().__transform_jet(_tagger->W2());}

        // accessor to PT ordered subjets
        std::vector<const RecJetFormat *> HTT::subjets() const
        {
            std::vector<const RecJetFormat *> output;
            output.reserve(3);
            output.push_back(ClusterBase().__transform_jet(_tagger->j1()));
            output.push_back(ClusterBase().__transform_jet(_tagger->j2()));
            output.push_back(ClusterBase().__transform_jet(_tagger->j3()));
            return output;
        }

        // print tagger information
        void HTT::get_info() const { _tagger->get_info(); }

        // print tagger settings
        void HTT::get_settings() const { _tagger->get_setting(); }

        // accessor to pruned mass
        MAfloat32 HTT::pruned_mass() const { return _tagger->pruned_mass(); }

        // accessor to unfiltered mass
        MAfloat32 HTT::unfiltered_mass() const { return _tagger->unfiltered_mass(); }

        // accessor to delta top
        MAfloat32 HTT::delta_top() const { return _tagger->delta_top(); }

        // Is given jet tagged
        MAbool HTT::is_tagged() const { return _tagger->is_tagged(); }

        // top mass window requirement passed?
        MAbool HTT::is_maybe_top() const { return _tagger->is_maybe_top(); }

        // 2D mass plane requirements passed?
        MAbool HTT::is_masscut_passed() const { return _tagger->is_masscut_passed(); }
    }
}