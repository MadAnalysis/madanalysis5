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

#ifndef MADANALYSIS5_ENERGYCORRELATOR_H
#define MADANALYSIS5_ENERGYCORRELATOR_H

// STL headers
#include <vector>
#include <algorithm>

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"

using namespace std;

namespace fastjet {
    namespace contrib {
        class EnergyCorrelator;
    }
}

namespace MA5 {
    namespace Substructure {
        class EnergyCorrelator {

        //---------------------------------------------------------------------------------
        //                                 data members
        //---------------------------------------------------------------------------------
        protected:
            fastjet::contrib::EnergyCorrelator * _EC;

        public:
            enum Measure {
                pt_R,     ///< use transverse momenta and boost-invariant angles,
                ///< eg \f$\mathrm{ECF}(2,\beta) = \sum_{i<j} p_{ti} p_{tj} \Delta R_{ij}^{\beta} \f$
                E_theta,   ///  use energies and angles,
                ///  eg \f$\mathrm{ECF}(2,\beta) = \sum_{i<j} E_{i} E_{j}   \theta_{ij}^{\beta} \f$
                E_inv     ///  use energies and invariant mass,
                ///  eg \f$\mathrm{ECF}(2,\beta) = \sum_{i<j} E_{i} E_{j}
                /// (\frac{2 p_{i} \cdot p_{j}}{E_{i} E_{j}})^{\beta/2} \f$
            };

            enum Strategy {
                slow,          ///< interparticle angles are not cached.
                ///< For N>=3 this leads to many expensive recomputations,
                ///< but has only O(n) memory usage for n particles
                storage_array  /// the interparticle angles are cached. This gives a significant speed
                /// improvement for N>=3, but has a memory requirement of (4n^2) bytes.
            };

            /// Constructor without argument
            EnergyCorrelator() {}

            /// Destructor
            virtual ~EnergyCorrelator() {}

            /// constructs an N-point correlator with angular exponent beta,
            /// using the specified choice of energy and angular measure as well
            /// one of two possible underlying computational Strategy
            EnergyCorrelator(
                    MAuint32 N,
                    MAfloat32 beta,
                    EnergyCorrelator::Measure measure = EnergyCorrelator::Measure::pt_R,
                    EnergyCorrelator::Strategy strategy = EnergyCorrelator::Strategy::storage_array
            ) { Initialize(N, beta, measure, strategy); }

            void Initialize(
                    MAuint32 N,
                    MAfloat32 beta,
                    EnergyCorrelator::Measure measure = EnergyCorrelator::Measure::pt_R,
                    EnergyCorrelator::Strategy strategy = EnergyCorrelator::Strategy::storage_array
            );

            // Method to execute with a single jet
            MAdouble64 Execute(const RecJetFormat* jet) const;
        };
    }
}

#endif //MADANALYSIS5_ENERGYCORRELATOR_H
