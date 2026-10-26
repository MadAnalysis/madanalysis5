//////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////

/**
 * @file SoftDrop.h
 * @brief Soft-drop grooming (FastJet contrib RecursiveTools).
 */

#ifndef MADANALYSIS5_SOFTDROP_H
#define MADANALYSIS5_SOFTDROP_H

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"

namespace fastjet {
    namespace contrib {
        class SoftDrop;
    }
}

namespace MA5 {
    namespace Substructure {
        /** @brief Soft-drop jet grooming (arXiv:1402.2657), see the description below. */
        class SoftDrop {

            // SoftDrop wrapper arXiv:1402.2657.
            //
            // For the basic functionalities, we refer the reader to the
            // documentation of the RecursiveSymmetryCutBase from which SoftDrop
            // inherits. Here, we mostly put the emphasis on things specific to
            // SoftDrop:
            //
            //  - the cut applied recursively is
            //     \f[
            //        z > z_{\rm cut} (\theta/R0)^\beta
            //     \f]
            //    with z the asymmetry measure and \f$\theta\f$ the geometrical
            //    distance between the two subjets. R0 is set to 1 by default.
            //
            //  - by default, we work in "grooming mode" i.s. if no substructure
            //    is found, we return a jet made of a single parton. Note that
            //    this behaviour differs from the mMDT (and can be a source of
            //    differences when running SoftDrop with beta=0.)
            //

            //---------------------------------------------------------------------------------
            //                                 data members
            //---------------------------------------------------------------------------------
            protected :
                /** @brief FastJet soft-drop tool (owned). */
                fastjet::contrib::SoftDrop * softDrop_;

            // -------------------------------------------------------------
            //                       method members
            // -------------------------------------------------------------
            public:

                /** @brief Constructor without argument (call Initialize() before use). */
                // FIXME: the default constructor leaves the pointer(s) uninitialised, but the destructor deletes them
                //   (undefined behaviour if Initialize() is never called).
                SoftDrop() {}

                /** @brief Destructor (deletes the soft-drop tool). */
                ~SoftDrop();

                //============================//
                //        Initialization      //
                //============================//
                
                /**
                 * @brief Constructor with arguments (calls Initialize()).
                 *
                 * @param beta angular exponent beta.
                 * @param symmetry_cut value of z_cut.
                 * @param R0 angular normalisation.
                 */
                SoftDrop(
                    MAfloat32 beta,             // the value of the beta parameter
                    MAfloat32 symmetry_cut,     // the value of the cut on the symmetry measure
                    MAfloat32 R0=1.              // the angular distance normalisation [1 by default]
                 )
                { Initialize(beta, symmetry_cut, R0); }

                /**
                 * @brief Create the soft-drop tool.
                 *
                 * @param beta angular exponent beta.
                 * @param symmetry_cut value of z_cut.
                 * @param R0 angular normalisation.
                 */
                void Initialize(MAfloat32 beta, MAfloat32 symmetry_cut, MAfloat32 R0=1.);

                //=======================//
                //        Execution      //
                //=======================//
                
                /**
                 * @brief Groom a jet.
                 *
                 * The returned jets are allocated on the heap and are owned by the caller.
                 *
                 * @param jet jet to process (its constituents are used).
                 * @return the groomed jet.
                 */
                const RecJetFormat * Execute(const RecJetFormat *jet) const;

                /**
                 * @brief Groom each jet of a collection.
                 *
                 * The returned jets are allocated on the heap and are owned by the caller.
                 *
                 * @param jets jets to process.
                 * @return the groomed jets, pT-ordered.
                 */
                std::vector<const RecJetFormat *> Execute(std::vector<const RecJetFormat *> &jets) const;
        };
    }
}
#endif //MADANALYSIS5_SOFTDROP_H
