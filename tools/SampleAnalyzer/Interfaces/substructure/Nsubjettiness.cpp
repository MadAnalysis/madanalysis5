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

// FastJet headers
#include "fastjet/contrib/Nsubjettiness.hh"

#include "SampleAnalyzer/Interfaces/substructure/Nsubjettiness.h"

namespace MA5 {
    namespace Substructure {

        Nsubjettiness::~Nsubjettiness()
        {
            // clean heap allocation
            delete axesdef_;
            delete measuredef_;
        }

        //============================//
        //        Initialization      //
        //============================//
        // Initialize the parameters of the algorithm. Initialization includes multiple if conditions
        // Hence it would be optimum execution to initialize the algorithm during the initialisation
        // of the analysis

        void Nsubjettiness::Initialize(
                MAint32 order,
                AxesDef axesdef,
                MeasureDef measuredef,
                MAfloat32 beta,
                MAfloat32 R0,
                MAfloat32 Rcutoff
        )
        {
            if (axesdef == Substructure::Nsubjettiness::KT_Axes)
                axesdef_ = new fastjet::contrib::KT_Axes();
            else if (axesdef == Substructure::Nsubjettiness::CA_Axes)
                axesdef_ = new fastjet::contrib::CA_Axes();
            else if (axesdef == Substructure::Nsubjettiness::AntiKT_Axes)
                axesdef_ = new fastjet::contrib::AntiKT_Axes(R0);
            else if (axesdef == Substructure::Nsubjettiness::WTA_KT_Axes)
                axesdef_ = new fastjet::contrib::WTA_KT_Axes();
            else if (axesdef == Substructure::Nsubjettiness::WTA_CA_Axes)
                axesdef_ = new fastjet::contrib::WTA_CA_Axes();
//                    else if (axesdef == Substructure::Nsubjettiness::GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::GenKT_Axes();
//                    else if (axesdef == Substructure::Nsubjettiness::WTA_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::WTA_GenKT_Axes();
//                    else if (axesdef == Substructure::Nsubjettiness::GenET_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::GenET_GenKT_Axes();
            else if (axesdef == Substructure::Nsubjettiness::Manual_Axes)
                axesdef_ = new fastjet::contrib::Manual_Axes();
            else if (axesdef == Substructure::Nsubjettiness::OnePass_KT_Axes)
                axesdef_ = new fastjet::contrib::OnePass_KT_Axes();
            else if (axesdef == Substructure::Nsubjettiness::OnePass_CA_Axes)
                axesdef_ = new fastjet::contrib::OnePass_CA_Axes();
            else if (axesdef == Substructure::Nsubjettiness::OnePass_AntiKT_Axes)
                axesdef_ = new fastjet::contrib::OnePass_AntiKT_Axes(R0);
            else if (axesdef == Substructure::Nsubjettiness::OnePass_WTA_KT_Axes)
                axesdef_ = new fastjet::contrib::OnePass_WTA_KT_Axes();
            else if (axesdef == Substructure::Nsubjettiness::OnePass_WTA_CA_Axes)
                axesdef_ = new fastjet::contrib::OnePass_WTA_CA_Axes();
//                    else if (axesdef == Substructure::Nsubjettiness::OnePass_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::OnePass_GenKT_Axes();
//                    else if (axesdef == Substructure::Nsubjettiness::OnePass_WTA_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::OnePass_WTA_GenKT_Axes();
//                    else if (axesdef == Substructure::Nsubjettiness::OnePass_GenET_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::OnePass_GenET_GenKT_Axes();
//                    else if (axesdef == Substructure::Nsubjettiness::OnePass_Manual_Axes)
//                        axesdef_ = new fastjet::contrib::OnePass_Manual_Axes();
//                    else if (axesdef == Substructure::Nsubjettiness::MultiPass_Axes)
//                        axesdef_ = new fastjet::contrib::MultiPass_Axes();
//                    else if (axesdef == Substructure::Nsubjettiness::MultiPass_Manual_Axes)
//                        axesdef_ = new fastjet::contrib::MultiPass_Manual_Axes();
//                    else if (axesdef == Substructure::Nsubjettiness::Comb_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::Comb_GenKT_Axes();
//                    else if (axesdef == Substructure::Nsubjettiness::Comb_WTA_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::Comb_WTA_GenKT_Axes();
//                    else if (axesdef == Substructure::Nsubjettiness::Comb_GenET_GenKT_Axes)
//                        axesdef_ = new fastjet::contrib::Comb_GenET_GenKT_Axes();

            if (measuredef == Substructure::Nsubjettiness::NormalizedCutoffMeasure)
                measuredef_ = new fastjet::contrib::NormalizedCutoffMeasure(beta, R0, Rcutoff);
            else if (measuredef == Substructure::Nsubjettiness::NormalizedMeasure)
                measuredef_ = new fastjet::contrib::NormalizedMeasure(beta, R0);
            else if (measuredef == Substructure::Nsubjettiness::UnnormalizedMeasure)
                measuredef_ = new fastjet::contrib::UnnormalizedMeasure(beta);
            else if (measuredef == Substructure::Nsubjettiness::UnnormalizedCutoffMeasure)
                measuredef_ = new fastjet::contrib::UnnormalizedCutoffMeasure(beta, Rcutoff);

            order_ = order;
        }

        //=======================//
        //        Execution      //
        //=======================//

        // Method to calculate nsub for a given jet with respect to initialization parameters
        MAdouble64 Nsubjettiness::Execute(const RecJetFormat *jet) const
        {
            fastjet::contrib::Nsubjettiness nsubjettiness(order_, *axesdef_, *measuredef_);
            return nsubjettiness(jet->pseudojet());
        }
    }
}