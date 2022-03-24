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

#ifndef MADANALYSIS5_FILTER_H
#define MADANALYSIS5_FILTER_H

// FastJet headers
#include "fastjet/tools/Filter.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/fastjet/ClusterBase.h"
#include "SampleAnalyzer/Interfaces/fastjet/Selector.h"

using namespace std;

namespace MA5 {
    namespace Substructure {
        class Filter {

            /// Class that helps perform filtering (Butterworth, Davison, Rubin
            /// and Salam, arXiv:0802.2470) and trimming (Krohn, Thaler and Wang,
            /// arXiv:0912.1342) on jets, optionally in conjunction with
            /// subtraction (Cacciari and Salam, arXiv:0707.1378).

            /// For example, to apply filtering that reclusters a jet's
            /// constituents with the Cambridge/Aachen jet algorithm with R=0.3
            /// and then selects the 3 hardest subjets, one can use the following

            /// To obtain trimming, involving for example the selection of all
            /// subjets carrying at least 3% of the original jet's pt, the
            /// selector would be replaced by SelectorPtFractionMin(0.03).

            //---------------------------------------------------------------------------------
            //                                 data members
            //---------------------------------------------------------------------------------
            protected:
                fastjet::Selector selector_; // the Selector applied to compute the kept subjets

                /// Jet definition
                fastjet::JetDefinition* JetDefinition_; // the jet definition applied to obtain the subjets
                MAbool isJetDefined_;

                MAfloat32 rho_; // if non-zero, backgruond-subtract each subjet befor selection
                MAfloat32 Rfilt_; // the filtering radius

                fastjet::Filter * JetFilter_;
            // -------------------------------------------------------------
            //                       method members
            // -------------------------------------------------------------
            public:

                /// Constructor without argument
                Filter() {}

                /// Destructor
                virtual ~Filter() {}

                //============================//
                //        Initialization      //
                //============================//

                // Constructor with arguments
                Filter(Algorithm algorithm, MAfloat32 radius, Selector selector, MAfloat32 rho=0.)
                { Initialize(algorithm, radius, selector, rho); }

                Filter(MAfloat32 Rfilt, Selector selector, MAfloat32 rho=0.)
                { Initialize(Rfilt, selector, rho); }

                void Initialize(Algorithm algorithm, MAfloat32 radius, Selector selector, MAfloat32 rho=0.)
                {
                    JetDefinition_ = new fastjet::JetDefinition(
                            ClusterBase().__get_clustering_algorithm(algorithm), radius
                    );
                    selector_ = selector.__get(); Rfilt_=-1.; rho_=rho; isJetDefined_=true;
                    init_filter();
                }

                void Initialize(MAfloat32 Rfilt, Selector selector, MAfloat32 rho=0.)
                {
                    selector_ = selector.__get(); Rfilt_=Rfilt; rho_=rho; isJetDefined_=false;
                    init_filter();
                }

                //=======================//
                //        Execution      //
                //=======================//

                // Method to filter a given jet.
                const RecJetFormat* Execute(const RecJetFormat *jet)
                {
                    fastjet::PseudoJet filtered_jet = (*JetFilter_)(jet->pseudojet());
                    return ClusterBase().__transform_jet(filtered_jet);
                }

                // Method to filter all the jets in a vector
                std::vector<const RecJetFormat*> Execute(std::vector<const RecJetFormat *> &jets)
                {
                    std::vector<const RecJetFormat *> output_jets;
                    for (auto &jet: jets)
                        output_jets.push_back(Execute(jet));

                    std::sort(
                        output_jets.begin(),
                        output_jets.end(),
                        [](const RecJetFormat *j1, const RecJetFormat *j2)
                        {
                            return (j1->pt() > j2->pt());
                        }
                    );

                    return output_jets;
                }

            private:

                void init_filter()
                {
                    if (isJetDefined_)
                        JetFilter_ = new fastjet::Filter(*JetDefinition_, selector_, rho_);
                    else
                        JetFilter_ = new fastjet::Filter(Rfilt_, selector_, rho_);
                }


        };
    }
}

#endif //MADANALYSIS5_FILTER_H
