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

#ifndef MADANALYSIS5_NSUBJETTINESS_H
#define MADANALYSIS5_NSUBJETTINESS_H

// STL headers
#include <vector>
#include <algorithm>

// FastJet headers
#include "fastjet/contrib/Nsubjettiness.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"

using namespace MA5;
using namespace std;

namespace MA5 {
    namespace Substructure {

        enum AxesDef {
            KT_Axes,
            CA_Axes,
            AntiKT_Axes,              // (R0)
            WTA_KT_Axes,
            WTA_CA_Axes,
//            GenKT_Axes,               // (p, R0 = infinity)
//            WTA_GenKT_Axes,           // (p, R0 = infinity)
//            GenET_GenKT_Axes,         // (delta, p, R0 = infinity)
//            Manual_Axes,
//            OnePass_KT_Axes,
//            OnePass_CA_Axes,
//            OnePass_AntiKT_Axes,       // (R0)
//            OnePass_WTA_KT_Axes,
//            OnePass_WTA_CA_Axes,
//            OnePass_GenKT_Axes,        // (p, R0 = infinity)
//            OnePass_WTA_GenKT_Axes,    // (p, R0 = infinity)
//            OnePass_GenET_GenKT_Axes,  // (delta, p, R0 = infinity)
//            OnePass_Manual_Axes,
//            MultiPass_Axes,            // (NPass) (currently only defined for KT_Axes)
//            MultiPass_Manual_Axes,     // (NPass)
//            Comb_GenKT_Axes,           // (nExtra, p, R0 = infinity)
//            Comb_WTA_GenKT_Axes,       // (nExtra, p, R0 = infinity)
//            Comb_GenET_GenKT_Axes,     // (nExtra, delta, p, R0 = infinity)
        };

        enum MeasureDef {
            NormalizedMeasure,            // (beta,R0)
            UnnormalizedMeasure,          // (beta)
            NormalizedCutoffMeasure,      // (beta,R0,Rcutoff)
            UnnormalizedCutoffMeasure,    // (beta,Rcutoff)
        };

        class Nsubjettiness {
            //---------------------------------------------------------------------------------
            //                                 data members
            //---------------------------------------------------------------------------------
            protected:
                MAint32 order_;
                fastjet::contrib::AxesDefinition* axesdef_;
                fastjet::contrib::MeasureDefinition* measuredef_;

            public:

                /// Constructor without argument
                Nsubjettiness() {}

                /// Destructor
                virtual ~Nsubjettiness() {}

                //============================//
                //        Initialization      //
                //============================//
                // Initialize the parameters of the algorithm. Initialization includes multiple if conditions
                // Hence it would be optimum execution to initialize the algorithm during the initialisation
                // of the analysis

                // Constructor with arguments
                Nsubjettiness(
                    MAint32 order,
                    AxesDef axesdef,
                    MeasureDef measuredef,
                    MAfloat32 beta,
                    MAfloat32 R0,
                    MAfloat32 Rcutoff=std::numeric_limits<double>::max()
                )
                { Initialize(order, axesdef, measuredef, beta, R0, Rcutoff); }

                void Initialize(
                    MAint32 order,
                    AxesDef axesdef,
                    MeasureDef measuredef,
                    MAfloat32 beta,
                    MAfloat32 R0,
                    MAfloat32 Rcutoff=std::numeric_limits<double>::max()
                )
                {
                    if (axesdef == Substructure::KT_Axes)
                        axesdef_ = new fastjet::contrib::KT_Axes();
                    else if (axesdef == Substructure::CA_Axes)
                        axesdef_ = new fastjet::contrib::CA_Axes();
                    else if (axesdef == Substructure::AntiKT_Axes)
                        axesdef_ = new fastjet::contrib::AntiKT_Axes(R0);
                    else if (axesdef == Substructure::WTA_KT_Axes)
                        axesdef_ = new fastjet::contrib::WTA_KT_Axes();
                    else if (axesdef == Substructure::WTA_CA_Axes)
                        axesdef_ = new fastjet::contrib::WTA_CA_Axes();
//                    else if (axesdef == Substructure::GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::GenKT_Axes();
//                    else if (axesdef == Substructure::WTA_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::WTA_GenKT_Axes();
//                    else if (axesdef == Substructure::GenET_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::GenET_GenKT_Axes();
//                    else if (axesdef == Substructure::Manual_Axes)
//                        axesdef_ = new fastjet::contrib::Manual_Axes();
//                    else if (axesdef == Substructure::OnePass_KT_Axes)
//                        axesdef_ = new fastjet::contrib::OnePass_KT_Axes();
//                    else if (axesdef == Substructure::OnePass_CA_Axes)
//                        axesdef_ = new fastjet::contrib::OnePass_CA_Axes();
//                    else if (axesdef == Substructure::OnePass_AntiKT_Axes)
//                        axesdef_ = new fastjet::contrib::OnePass_AntiKT_Axes();
//                    else if (axesdef == Substructure::OnePass_WTA_KT_Axes)
//                        axesdef_ = new fastjet::contrib::OnePass_WTA_KT_Axes();
//                    else if (axesdef == Substructure::OnePass_WTA_CA_Axes)
//                        axesdef_ = new fastjet::contrib::OnePass_WTA_CA_Axes();
//                    else if (axesdef == Substructure::OnePass_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::OnePass_GenKT_Axes();
//                    else if (axesdef == Substructure::OnePass_WTA_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::OnePass_WTA_GenKT_Axes();
//                    else if (axesdef == Substructure::OnePass_GenET_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::OnePass_GenET_GenKT_Axes();
//                    else if (axesdef == Substructure::OnePass_Manual_Axes)
//                        axesdef_ = new fastjet::contrib::OnePass_Manual_Axes();
//                    else if (axesdef == Substructure::MultiPass_Axes)
//                        axesdef_ = new fastjet::contrib::MultiPass_Axes();
//                    else if (axesdef == Substructure::MultiPass_Manual_Axes)
//                        axesdef_ = new fastjet::contrib::MultiPass_Manual_Axes();
//                    else if (axesdef == Substructure::Comb_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::Comb_GenKT_Axes();
//                    else if (axesdef == Substructure::Comb_WTA_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::Comb_WTA_GenKT_Axes();
//                    else if (axesdef == Substructure::Comb_GenET_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::Comb_GenET_GenKT_Axes();

                    if (measuredef == Substructure::NormalizedCutoffMeasure)
                        measuredef_ = new fastjet::contrib::NormalizedCutoffMeasure(beta, R0, Rcutoff);
                    else if (measuredef == Substructure::NormalizedMeasure)
                        measuredef_ = new fastjet::contrib::NormalizedMeasure(beta, R0);
                    else if (measuredef == Substructure::UnnormalizedMeasure)
                        measuredef_ = new fastjet::contrib::UnnormalizedMeasure(beta);
                    else if (measuredef == Substructure::UnnormalizedCutoffMeasure)
                        measuredef_ = new fastjet::contrib::UnnormalizedCutoffMeasure(beta, Rcutoff);

                    order_ = order;
                }

                //=======================//
                //        Execution      //
                //=======================//

                // Method to calculate nsub for a given jet with respect to initialization parameters
                MAdouble64 Execute(const RecJetFormat *jet)
                {
                    fastjet::contrib::Nsubjettiness nsubjettiness(order_, *axesdef_, *measuredef_);
                    return nsubjettiness(jet->pseudojet());
                }


        };
    }
}

#endif //MADANALYSIS5_NSUBJETTINESS_H
