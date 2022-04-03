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
        class Nsubjettiness {
            //---------------------------------------------------------------------------------
            //                                 data members
            //---------------------------------------------------------------------------------
            protected:
                MAint32 order_;
                fastjet::contrib::AxesDefinition* axesdef_;
                fastjet::contrib::MeasureDefinition* measuredef_;

            public:

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

                enum MeasureDef {
                    NormalizedMeasure,            // (beta,R0)
                    UnnormalizedMeasure,          // (beta)
                    NormalizedCutoffMeasure,      // (beta,R0,Rcutoff)
                    UnnormalizedCutoffMeasure,    // (beta,Rcutoff)
                };

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
                );

                //=======================//
                //        Execution      //
                //=======================//

                // Method to calculate nsub for a given jet with respect to initialization parameters
                MAdouble64 Execute(const RecJetFormat *jet) const;
        };
    }
}

#endif //MADANALYSIS5_NSUBJETTINESS_H
