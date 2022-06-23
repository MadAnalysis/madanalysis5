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

#ifndef MADANALYSIS5_HTT_H
#define MADANALYSIS5_HTT_H

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/substructure/Commons.h"

namespace fastjet {
    namespace HEPTopTagger {
        class HEPTopTagger;
    }
}

namespace MA5 {
    namespace Substructure {
        class HTT {

        protected:
            fastjet::HEPTopTagger::HEPTopTagger* _tagger;

        public:

            enum Mode {
                EARLY_MASSRATIO_SORT_MASS,     // applies 2D mass plane requirements then select the candidate which minimizes |m_cand-mt|
                LATE_MASSRATIO_SORT_MASS,      // selects the candidate which minimizes |m_cand-mt|
                EARLY_MASSRATIO_SORT_MODDJADE, // applies the 2D mass plane requirements then select the candidate with highest jade distance
                LATE_MASSRATIO_SORT_MODDJADE,  // selects the candidate with highest modified jade distance
                TWO_STEP_FILTER                // only analyzes the candidate built with the highest pT(t) after unclustering
            };

            struct InputParameters {
                Mode mode = EARLY_MASSRATIO_SORT_MASS; // execution mode

                MAbool do_optimalR = true; // initialize optimal R or set to fixed R
                // optimal R parameters
                MAfloat32 optimalR_min = 0.5; // min jet size
                MAfloat32 optimalR_step = 0.1; // step size
                MAfloat32 optimalR_threshold = 0.2; // step size

                // massdrop - unclustering
                MAfloat32 mass_drop = 0.8;
                MAfloat32 max_subjet = 30.; // set_max_subjet_mass

                // filtering
                MAuint32 filt_N = 5; // set_nfilt
                MAfloat32 filtering_R = 0.3; // max subjet distance for filtering
                MAfloat32 filtering_minpt = 0.; // min subjet pt for filtering
                // jet algorithm for filtering
                Algorithm filtering_algorithm = Algorithm::cambridge;

                // Reclustering
                // reclustering jet algorithm
                Algorithm reclustering_algorithm = Algorithm::cambridge;

                //top mass range
                MAfloat32 top_mass = 172.3;
                MAfloat32 W_mass = 80.4;
                MAfloat32 Mtop_min = 150.;
                MAfloat32 Mtop_max = 200.; //set_top_range(min,max)

                // set top mass ratio range
                MAfloat32 fw = 0.15;
                MAfloat32 mass_ratio_range_min = (1.-fw)*W_mass/top_mass;
                MAfloat32 mass_ratio_range_max = (1.+fw)*W_mass/top_mass;

                //mass ratio cuts
                MAfloat32 m23cut = 0.35;
                MAfloat32 m13cutmin = 0.2;
                MAfloat32 m13cutmax = 1.3;

                // pruning
                MAfloat32 prun_zcut = 0.1; // set_prun_zcut
                MAfloat32 prun_rcut = .5; // set_prun_rcut
            };

            // Constructor without arguments
            HTT() {}

            // Destructor
            ~HTT();

            //============================//
            //        Initialization      //
            //============================//

            HTT(HTT::InputParameters& param) { Initialize(param); }

            void Initialize(HTT::InputParameters& param);

            //====================//
            //       Execute      //
            //====================//

            // Method run top tagger.
            void Execute(const RecJetFormat *jet);

            //======================//
            //       Accessors      //
            //======================//

            // accessor to top jet
            const RecJetFormat * top() const;

            //accessor to bottom jet
            const RecJetFormat * b() const;

            //accessor to W jet
            const RecJetFormat * W() const;

            //accessor to leading subjet from W
            const RecJetFormat * W1() const;

            //accessor to second leading subjet from W
            const RecJetFormat * W2() const;

            // accessor to PT ordered subjets
            std::vector<const RecJetFormat *> subjets() const;

            // print tagger information
            void get_info() const;

            // print tagger settings
            void get_settings() const;

            // accessor to pruned mass
            MAfloat32 pruned_mass() const;

            // accessor to unfiltered mass
            MAfloat32 unfiltered_mass() const;

            // accessor to delta top
            MAfloat32 delta_top() const;

            // Is given jet tagged
            MAbool is_tagged() const;

            // top mass window requirement passed?
            MAbool is_maybe_top() const;

            // 2D mass plane requirements passed?
            MAbool is_masscut_passed() const;
        };
    }
}

#endif //MADANALYSIS5_HTT_H