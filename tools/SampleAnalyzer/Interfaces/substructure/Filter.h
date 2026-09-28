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
 * @file Filter.h
 * @brief Jet filtering and trimming (FastJet Filter).
 */

#ifndef MADANALYSIS5_FILTER_H
#define MADANALYSIS5_FILTER_H

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/substructure/Commons.h"
#include "SampleAnalyzer/Interfaces/substructure/Selector.h"

namespace fastjet {
    class Filter;
    class JetDefinition;
}

namespace MA5 {
    namespace Substructure {
        /** @brief Jet filtering / trimming through the FastJet Filter, see the description below. */
        class Filter {

            /// Class that helps perform filtering (Butterworth, Davison, Rubin
            /// and Salam, arXiv:0802.2470) and trimming (Krohn, Thaler and Wang,
            /// arXiv:0912.1342) on jets, optionally in conjunction with
            /// subtraction (Cacciari and Salam, arXiv:0707.1378).

            /// For example, to apply filtering that reclusters a jet's
            /// constituents with the Cambridge/Aachen jet algorithm with R=0.3
            /// and then selects the 3 hardest subjets, one can use the following

            // NOTE: the code example announced above is missing.
            /// To obtain trimming, involving for example the selection of all
            /// subjets carrying at least 3% of the original jet's pt, the
            /// selector would be replaced by SelectorPtFractionMin(0.03).

            //---------------------------------------------------------------------------------
            //                                 data members
            //---------------------------------------------------------------------------------
            protected:

                /** @brief Jet definition used to obtain the subjets (owned, null if Rfilt_ is used). */
                fastjet::JetDefinition* JetDefinition_; // the jet definition applied to obtain the subjets

                MAfloat32 rho_; // if non-zero, backgruond-subtract each subjet befor selection
                MAfloat32 Rfilt_; // the filtering radius

                /** @brief FastJet filter (owned). */
                fastjet::Filter * JetFilter_;
            // -------------------------------------------------------------
            //                       method members
            // -------------------------------------------------------------
            public:

                /** @brief Constructor without argument (call Initialize() before use). */
                Filter() {
                    JetDefinition_ = 0;
                    JetFilter_ = 0;
                }

                /** @brief Destructor (deletes the jet definition and the filter). */
                ~Filter();

                //============================//
                //        Initialization      //
                //============================//

                /**
                 * @brief Constructor with a jet definition (calls Initialize()).
                 *
                 * @param algorithm algorithm used to obtain the subjets.
                 * @param radius subjet radius.
                 * @param selector selector applied to the subjets.
                 * @param rho if non-zero, the subjets are background-subtracted.
                 */
                Filter(Algorithm algorithm, MAfloat32 radius, Selector selector, MAfloat32 rho=0.)
                { Initialize(algorithm, radius, selector, rho); }

                /**
                 * @brief Constructor with a filtering radius (calls Initialize()).
                 *
                 * @param Rfilt filtering radius (C/A subjets).
                 * @param selector selector applied to the subjets.
                 * @param rho if non-zero, the subjets are background-subtracted.
                 */
                Filter(MAfloat32 Rfilt, Selector selector, MAfloat32 rho=0.)
                { Initialize(Rfilt, selector, rho); }

                /**
                 * @brief Initialise with a jet definition.
                 *
                 * @param algorithm algorithm used to obtain the subjets.
                 * @param radius subjet radius.
                 * @param selector selector applied to the subjets.
                 * @param rho if non-zero, the subjets are background-subtracted.
                 */
                void Initialize(Algorithm algorithm, MAfloat32 radius, Selector selector, MAfloat32 rho=0.);
                /**
                 * @brief Initialise with a filtering radius.
                 *
                 * @param Rfilt filtering radius (C/A subjets).
                 * @param selector selector applied to the subjets.
                 * @param rho if non-zero, the subjets are background-subtracted.
                 */
                void Initialize(MAfloat32 Rfilt, Selector selector, MAfloat32 rho=0.);

                //=======================//
                //        Execution      //
                //=======================//

                /**
                 * @brief Filter a jet.
                 *
                 * The returned jets are allocated on the heap and are owned by the caller.
                 *
                 * @param jet jet to process (its constituents are used).
                 * @return the filtered jet.
                 */
                const RecJetFormat* Execute(const RecJetFormat *jet) const;

                /**
                 * @brief Filter each jet of a collection.
                 *
                 * The returned jets are allocated on the heap and are owned by the caller.
                 *
                 * @param jets jets to process.
                 * @return the filtered jets, pT-ordered.
                 */
                std::vector<const RecJetFormat*> Execute(std::vector<const RecJetFormat *> &jets) const;

            private:

                /**
                 * @brief Create the FastJet filter.
                 *
                 * @param selector subjet selector.
                 * @param isJetDefined use JetDefinition_ (true) or Rfilt_ (false).
                 */
                void init_filter(Selector selector, MAbool isJetDefined);


        };
    }
}

#endif //MADANALYSIS5_FILTER_H
