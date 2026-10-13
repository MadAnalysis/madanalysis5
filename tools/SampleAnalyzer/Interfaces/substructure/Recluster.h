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
 * @file Recluster.h
 * @brief Reclustering of jets (FastJet contrib Recluster).
 */

#ifndef MADANALYSIS5_RECLUSTER_H
#define MADANALYSIS5_RECLUSTER_H

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/substructure/ClusterBase.h"

using namespace std;

namespace MA5 {
    namespace Substructure {
        /** @brief Recluster jets with another algorithm or radius (FastJet contrib Recluster). */
        class Recluster : public ClusterBase{

            // -------------------------------------------------------------
            //                       method members
            // -------------------------------------------------------------
            public:

                /** @brief Constructor without argument (call Initialize() before use). */
                Recluster() {}

                /** @brief Destructor. */
                ~Recluster() {}

                //============================//
                //        Initialization      //
                //============================//
                // Initialize the parameters of the algorithm. Initialization includes multiple if conditions
                // Hence it would be optimum execution to initialize the algorithm during the initialisation
                // of the analysis

                /**
                 * @brief Constructor with arguments (calls Initialize()).
                 *
                 * @param algorithm reclustering algorithm.
                 * @param radius jet radius.
                 */
                Recluster(Algorithm algorithm, MAfloat32 radius) { Initialize(algorithm, radius); }

                /**
                 * @brief Set the jet definition used for the reclustering.
                 *
                 * @param algorithm reclustering algorithm.
                 * @param radius jet radius.
                 */
                void Initialize(Algorithm algorithm, MAfloat32 radius) { SetJetDef(algorithm, radius); }

                //=======================//
                //        Execution      //
                //=======================//

                /**
                 * @brief Recluster a jet and return the hardest reclustered jet.
                 *
                 * The returned jets are allocated on the heap and are owned by the caller.
                 *
                 * @param jet jet to process (its constituents are used).
                 * @return the reclustered jet.
                 */
                // NOTE: this hides ClusterBase::Execute(const RecJetFormat*), which returns all the reclustered jets.
                const RecJetFormat* Execute(const RecJetFormat *jet);

                /**
                 * @brief Recluster each jet of a collection individually.
                 *
                 * The returned jets are allocated on the heap and are owned by the caller.
                 *
                 * @param jets jets to process.
                 * @return the reclustered jets, pT-ordered.
                 */
                std::vector<const RecJetFormat *> Execute(std::vector<const RecJetFormat *> &jets) override;

                //=============================//
                //        NOT IMPLEMENTED      //
                //=============================//

                /**
                 * @brief Not implemented (logs an error).
                 *
                 * @param event event.
                 * @param JetID jet collection identifier.
                 */
                void Execute(const EventFormat& event, std::string JetID) override;

                /**
                 * @brief Not implemented (logs an error).
                 *
                 * @tparam Func filter type.
                 * @param jet jet to process (its constituents are used).
                 * @param func filter.
                 * @return an empty vector.
                 */
                template<class Func>
                std::vector<const RecJetFormat *> Execute(const RecJetFormat *jet, Func func);
        };
    }
}

#endif //MADANALYSIS5_RECLUSTER_H
