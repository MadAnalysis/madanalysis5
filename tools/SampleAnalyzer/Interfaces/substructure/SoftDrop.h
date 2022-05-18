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
                fastjet::contrib::SoftDrop * softDrop_;

            // -------------------------------------------------------------
            //                       method members
            // -------------------------------------------------------------
            public:

                // Constructor without argument
                SoftDrop() {}

                // Destructor
                ~SoftDrop();

                //============================//
                //        Initialization      //
                //============================//
                
                // Constructor with arguments
                SoftDrop(
                    MAfloat32 beta,             // the value of the beta parameter
                    MAfloat32 symmetry_cut,     // the value of the cut on the symmetry measure
                    MAfloat32 R0=1.              // the angular distance normalisation [1 by default]
                 )
                { Initialize(beta, symmetry_cut, R0); }

                void Initialize(MAfloat32 beta, MAfloat32 symmetry_cut, MAfloat32 R0=1.);

                //=======================//
                //        Execution      //
                //=======================//
                
                // Execute with a single jet
                const RecJetFormat * Execute(const RecJetFormat *jet) const;

                // Execute with a list of jets
                std::vector<const RecJetFormat *> Execute(std::vector<const RecJetFormat *> &jets) const;
        };
    }
}
#endif //MADANALYSIS5_SOFTDROP_H
