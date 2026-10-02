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
 * @file Nsubjettiness.h
 * @brief N-subjettiness (FastJet contrib Nsubjettiness).
 */

#ifndef MADANALYSIS5_NSUBJETTINESS_H
#define MADANALYSIS5_NSUBJETTINESS_H

// STL headers
#include <algorithm>

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"

using namespace std;

namespace fastjet {
    namespace contrib {
        class AxesDefinition;
        class MeasureDefinition;
    }
}

namespace MA5 {
    namespace Substructure {
        /** @brief N-subjettiness tau_N (Thaler and Van Tilburg, arXiv:1011.2268). */
        class Nsubjettiness {
            //---------------------------------------------------------------------------------
            //                                 data members
            //---------------------------------------------------------------------------------
            protected:
                /** @brief Order N of tau_N. */
                MAint32 order_;
                /** @brief Axes definition (owned). */
                fastjet::contrib::AxesDefinition* axesdef_;
                /** @brief Measure definition (owned). */
                fastjet::contrib::MeasureDefinition* measuredef_;

            public:

                /** @brief Axes definitions (the commented ones are not supported). */
                enum AxesDef {
                    KT_Axes,
                    CA_Axes,
                    AntiKT_Axes,              // (R0)
                    WTA_KT_Axes,
                    WTA_CA_Axes,
    //            GenKT_Axes,               // (p, R0 = infinity)
    //            WTA_GenKT_Axes,           // (p, R0 = infinity)
    //            GenET_GenKT_Axes,         // (delta, p, R0 = infinity)
                    Manual_Axes,
                    OnePass_KT_Axes,
                    OnePass_CA_Axes,
                    OnePass_AntiKT_Axes,       // (R0)
                    OnePass_WTA_KT_Axes,
                    OnePass_WTA_CA_Axes,
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

                /** @brief Measure definitions (the parameters used by each measure are given in the comments). */
                enum MeasureDef {
                    NormalizedMeasure,            // (beta,R0)
                    UnnormalizedMeasure,          // (beta)
                    NormalizedCutoffMeasure,      // (beta,R0,Rcutoff)
                    UnnormalizedCutoffMeasure,    // (beta,Rcutoff)
                };

                /** @brief Constructor without argument (call Initialize() before use). */
                // FIXME: the default constructor leaves the pointer(s) uninitialised, but the destructor deletes them
                //   (undefined behaviour if Initialize() is never called).
                Nsubjettiness() {}

                /** @brief Destructor (deletes the axes and measure definitions). */
                ~Nsubjettiness();

                //============================//
                //        Initialization      //
                //============================//
                // Initialize the parameters of the algorithm. Initialization includes multiple if conditions
                // Hence it would be optimum execution to initialize the algorithm during the initialisation
                // of the analysis

                /**
                 * @brief Constructor with arguments (calls Initialize()).
                 *
                 * @param order order N.
                 * @param axesdef axes definition.
                 * @param measuredef measure definition.
                 * @param beta angular exponent.
                 * @param R0 characteristic jet radius (normalised measures and anti-kt axes).
                 * @param Rcutoff cutoff radius (cutoff measures).
                 */
                Nsubjettiness(
                    MAint32 order,
                    AxesDef axesdef,
                    MeasureDef measuredef,
                    MAfloat32 beta,
                    MAfloat32 R0,
                    MAfloat32 Rcutoff=std::numeric_limits<double>::max()
                )
                { Initialize(order, axesdef, measuredef, beta, R0, Rcutoff); }

                /**
                 * @brief Create the axes and measure definitions.
                 *
                 * @param order order N.
                 * @param axesdef axes definition.
                 * @param measuredef measure definition.
                 * @param beta angular exponent.
                 * @param R0 characteristic jet radius (normalised measures and anti-kt axes).
                 * @param Rcutoff cutoff radius (cutoff measures).
                 */
                void Initialize(
                    MAint32 order,
                    AxesDef axesdef,
                    MeasureDef measuredef,
                    MAfloat32 beta,
                    MAfloat32 R0,
                    MAfloat32 Rcutoff=std::numeric_limits<double>::max()
                );

                //=======================//
                //        Execution      //
                //=======================//

                /**
                 * @brief Compute tau_N for a jet.
                 *
                 * @param jet jet to process (its constituents are used).
                 * @return tau_N.
                 */
                MAdouble64 Execute(const RecJetFormat *jet) const;
        };
    }
}

#endif //MADANALYSIS5_NSUBJETTINESS_H
