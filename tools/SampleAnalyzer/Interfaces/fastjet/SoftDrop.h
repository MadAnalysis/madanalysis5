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

// FastJet headers
#include "fastjet/contrib/SoftDrop.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/fastjet/ClusterBase.h"

using namespace std;

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

                // SoftDrop input variables
                MAfloat32 beta_;         // the value of the beta parameter
                MAfloat32 symmetry_cut_; // the value of the cut on the symmetry measure
                MAfloat32 R0_;           // the angular distance normalisation [1 by default]

            // -------------------------------------------------------------
            //                       method members
            // -------------------------------------------------------------
            public:

                // Constructor without argument
                SoftDrop() {}

                // Destructor
                virtual ~SoftDrop() {}

                //============================//
                //        Initialization      //
                //============================//
                
                // Constructor with arguments
                SoftDrop(MAfloat32 beta, MAfloat32 symmetry_cut, MAfloat32 R0=1.)
                { Initialize(beta, symmetry_cut, R0); }

                void Initialize(MAfloat32 beta, MAfloat32 symmetry_cut, MAfloat32 R0=1.)
                { beta_ = beta; symmetry_cut_ = symmetry_cut; R0_ = R0;}

                //=======================//
                //        Execution      //
                //=======================//
                
                // Execute with a single jet
                const RecJetFormat *Execute(const RecJetFormat *jet)
                {
                    fastjet::contrib::SoftDrop sd(beta_, symmetry_cut_, R0_);
                    fastjet::PseudoJet sd_jet = sd(jet->pseudojet());
                    return ClusterBase().__transform_jet(sd_jet);
                }

                // Execute with a list of jets
                std::vector<const RecJetFormat *> Execute(std::vector<const RecJetFormat *> &jets)
                {
                    std::vector<const RecJetFormat *> output_jets;
                    for (auto &jet: jets)
                        output_jets.push_back(Execute(jet));

                    // Sort with respect to jet pT
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

        };
    }
}
#endif //MADANALYSIS5_SOFTDROP_H
