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
#include "SampleAnalyzer/Interfaces/HEPTopTagger/HTT.h"

namespace MA5 {
    namespace Substructure {

        HTT::~HTT() { delete _tagger; }

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

            fastjet::JetAlgorithm algo_ = fastjet::antikt_algorithm;
            if (param.filtering_algorithm == Substructure::cambridge) algo_ = fastjet::cambridge_algorithm;
            else if (param.filtering_algorithm == Substructure::kt)   algo_ = fastjet::kt_algorithm;
            _tagger->set_filtering_jetalgorithm(algo_);

            // Reclustering
            algo_ = fastjet::antikt_algorithm;
            if (param.reclustering_algorithm == Substructure::cambridge) algo_ = fastjet::cambridge_algorithm;
            else if (param.reclustering_algorithm == Substructure::kt)   algo_ = fastjet::kt_algorithm;
            _tagger->set_reclustering_jetalgorithm(algo_);

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
        const RecJetFormat * HTT::top() const
        {
            RecJetFormat * NewJet = new RecJetFormat(const_cast<fastjet::PseudoJet&>(_tagger->t()));
            return NewJet;
        }

        //accessor to bottom jet
        const RecJetFormat * HTT::b() const
        {
            RecJetFormat * NewJet = new RecJetFormat(const_cast<fastjet::PseudoJet&>(_tagger->b()));
            return NewJet;
        }

        //accessor to W jet
        const RecJetFormat * HTT::W() const
        {
            RecJetFormat * NewJet = new RecJetFormat(const_cast<fastjet::PseudoJet&>(_tagger->W()));
            return NewJet;
        }

        //accessor to leading subjet from W
        const RecJetFormat * HTT::W1() const
        {
            RecJetFormat * NewJet = new RecJetFormat(const_cast<fastjet::PseudoJet&>(_tagger->W1()));
            return NewJet;
        }

        //accessor to second leading subjet from W
        const RecJetFormat * HTT::W2() const
        {
            RecJetFormat * NewJet = new RecJetFormat(const_cast<fastjet::PseudoJet&>(_tagger->W2()));
            return NewJet;
        }

        // accessor to PT ordered subjets
        std::vector<const RecJetFormat *> HTT::subjets() const
        {
            std::vector<const RecJetFormat *> output;
            output.reserve(3);
            RecJetFormat * j1 = new RecJetFormat(const_cast<fastjet::PseudoJet&>(_tagger->j1()));
            RecJetFormat * j2 = new RecJetFormat(const_cast<fastjet::PseudoJet&>(_tagger->j2()));
            RecJetFormat * j3 = new RecJetFormat(const_cast<fastjet::PseudoJet&>(_tagger->j3()));
            output.push_back(j1);
            output.push_back(j2);
            output.push_back(j3);
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