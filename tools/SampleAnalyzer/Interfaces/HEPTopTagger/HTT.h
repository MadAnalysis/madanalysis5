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

/**
 * @file HTT.h
 * @brief Interface to the HEPTopTagger (v2) top tagger.
 */

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
        /**
         * @brief Wrapper of the HEPTopTagger (Plehn, Spannowsky, Takeuchi and Zerwas, arXiv:1006.2833; HEPTopTagger2, arXiv:1503.05921).
         *
         * Configure it with an InputParameters structure, run it on a (fat) jet with Execute()
         * and query the tagging result and the top candidate with the accessors.
         */
        class HTT {

        protected:
            /** @brief HEPTopTagger instance (owned). */
            fastjet::HEPTopTagger::HEPTopTagger* _tagger;

        public:

            /** @brief Selection of the top candidate among the triplets of subjets. */
            enum Mode {
                EARLY_MASSRATIO_SORT_MASS,     // applies 2D mass plane requirements then select the candidate which minimizes |m_cand-mt|
                LATE_MASSRATIO_SORT_MASS,      // selects the candidate which minimizes |m_cand-mt|
                EARLY_MASSRATIO_SORT_MODDJADE, // applies the 2D mass plane requirements then select the candidate with highest jade distance
                LATE_MASSRATIO_SORT_MODDJADE,  // selects the candidate with highest modified jade distance
                TWO_STEP_FILTER                // only analyzes the candidate built with the highest pT(t) after unclustering
            };

            /** @brief Configuration of the tagger (the default values are those of HEPTopTagger). */
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
                // NOTE: the mass-ratio range is computed from the default values of fw, W_mass and top_mass when the
                //   structure is created; changing these members afterwards does not update it.
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

            /** @brief Constructor without argument (call Initialize() before use). */
            // FIXME: _tagger is left uninitialised, while the destructor deletes it.
            HTT() {}

            /** @brief Destructor (deletes the tagger). */
            ~HTT();

            //============================//
            //        Initialization      //
            //============================//

            /**
             * @brief Constructor with arguments (calls Initialize()).
             *
             * @param param tagger configuration.
             */
            HTT(HTT::InputParameters& param) { Initialize(param); }

            /**
             * @brief Create and configure the tagger.
             *
             * @param param tagger configuration.
             */
            void Initialize(HTT::InputParameters& param);

            //====================//
            //       Execute      //
            //====================//

            /**
             * @brief Run the tagger on a jet.
             *
             * @param jet jet to tag (its constituents are used).
             */
            void Execute(const RecJetFormat *jet);

            //======================//
            //       Accessors      //
            //======================//

            /**
             * @brief Accessor to the top candidate.
             *
             * The returned jet(s) are allocated on the heap and owned by the caller.
             *
             * @return the top candidate.
             */
            const RecJetFormat * top() const;

            /**
             * @brief Accessor to the b subjet of the top candidate.
             *
             * The returned jet(s) are allocated on the heap and owned by the caller.
             *
             * @return the b subjet of the top candidate.
             */
            const RecJetFormat * b() const;

            /**
             * @brief Accessor to the W candidate.
             *
             * The returned jet(s) are allocated on the heap and owned by the caller.
             *
             * @return the W candidate.
             */
            const RecJetFormat * W() const;

            /**
             * @brief Accessor to the leading subjet of the W candidate.
             *
             * The returned jet(s) are allocated on the heap and owned by the caller.
             *
             * @return the leading subjet of the W candidate.
             */
            const RecJetFormat * W1() const;

            /**
             * @brief Accessor to the second leading subjet of the W candidate.
             *
             * The returned jet(s) are allocated on the heap and owned by the caller.
             *
             * @return the second leading subjet of the W candidate.
             */
            const RecJetFormat * W2() const;

            /**
             * @brief Accessor to the three pT-ordered subjets of the top candidate.
             *
             * The returned jet(s) are allocated on the heap and owned by the caller.
             *
             * @return the subjets.
             */
            std::vector<const RecJetFormat *> subjets() const;

            /** @brief Print information on the last tagging. */
            void get_info() const;

            /** @brief Print the tagger settings. */
            void get_settings() const;

            /**
             * @brief Accessor to the pruned mass.
             *
             * @return the pruned mass.
             */
            MAfloat32 pruned_mass() const;

            /**
             * @brief Accessor to the unfiltered mass.
             *
             * @return the unfiltered mass.
             */
            MAfloat32 unfiltered_mass() const;

            /**
             * @brief Accessor to the difference between the reconstructed and the true top mass.
             *
             * @return the mass difference.
             */
            MAfloat32 delta_top() const;

            /**
             * @brief Whether the jet is top-tagged.
             *
             * @return true if tagged.
             */
            MAbool is_tagged() const;

            /**
             * @brief Whether the top mass window requirement is satisfied.
             *
             * @return true if satisfied.
             */
            MAbool is_maybe_top() const;

            /**
             * @brief Whether the 2D mass-plane requirements are satisfied.
             *
             * @return true if satisfied.
             */
            MAbool is_masscut_passed() const;
        };
    }
}

#endif //MADANALYSIS5_HTT_H
