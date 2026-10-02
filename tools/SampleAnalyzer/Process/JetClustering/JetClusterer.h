////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//
//  This file is part of MadAnalysis 5.
//  Official website: <https://github.com/MadAnalysis/madanalysis5>
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
 * @file JetClusterer.h
 * @brief Reconstruction of an event from the Monte Carlo record (SFS/FastJet mode).
 */

#ifndef JET_CLUSTERER_H
#define JET_CLUSTERER_H

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Commons/Base/SmearerBase.h"
#include "SampleAnalyzer/Commons/Base/SFSTaggerBase.h"

#ifdef MA5_FASTJET_MODE
#include "SampleAnalyzer/Interfaces/substructure/ClusterBase.h"
#endif

// STL headers
#include <locale>

namespace MA5
{

    class ClusterAlgoBase;

    /**
     * @brief Reconstruction of an event from its Monte Carlo record.
     *
     * From the final-state particles, the JetClusterer builds the tracks, electrons, muons,
     * photons and hadronic taus (with the SFS smearing), clusters the remaining hadrons
     * into jets (primary and additional jet collections), computes MET/MHT/TET, applies
     * the b/c/tau taggers and fills the isolation cones.
     */
    class JetClusterer
    {
        //--------------------------------------------------------------------------
        //                              data members
        //--------------------------------------------------------------------------
    protected:
        /** @brief Clustering algorithm of the primary jets (owned). */
        ClusterAlgoBase *algo_;
        /** @brief SFS smearer (owned). */
        SmearerBase *mySmearer_;
        /** @brief SFS tagger (owned). */
        SFSTaggerBase *myTagger_;

        /** @brief Options of the tagger (owned). */
        SFSTaggerBaseOptions *myTaggerOptions_;

        /**
         * @brief Exclusive identification.
         *
         * true: final-state leptons (photons) coming from hadron decays are not included in the
         * lepton (photon) collections and the identified objects are removed from the jet
         * inputs; false: all final-state leptons (photons) are kept and only the muons are
         * removed from the jet inputs.
         */
        MAbool ExclusiveId_;

        /** @brief Identifier of the primary jet collection. */
        std::string JetID_;

#ifdef MA5_FASTJET_MODE
        /** @brief Additional jet collections (standard FastJet algorithms), by identifier. */
        std::map<std::string, ClusterAlgoBase *> cluster_collection_;

        /** @brief Additional jet collections (variable-R), by identifier. */
        std::map<std::string, Substructure::ClusterBase *> substructure_collection_;
#endif

        /** @brief Radii of the track isolation cones. */
        std::vector<MAfloat64> isocone_track_radius_;

        /** @brief Radii of the electron isolation cones. */
        std::vector<MAfloat64> isocone_electron_radius_;

        /** @brief Radii of the muon isolation cones. */
        std::vector<MAfloat64> isocone_muon_radius_;

        /** @brief Radii of the photon isolation cones. */
        std::vector<MAfloat64> isocone_photon_radius_;

        //--------------------------------------------------------------------------
        //                              method members
        //--------------------------------------------------------------------------
    public:
        /**
         * @brief Constructor.
         *
         * @param algo clustering algorithm of the primary jets (owned).
         */
        JetClusterer(ClusterAlgoBase *algo);

        /** @brief Destructor (deletes the algorithms, the smearer and the tagger). */
        ~JetClusterer();

        /**
         * @brief Initialise the clusterer from the options of main.cpp.
         *
         * Recognised options: `exclusive_id`, `bjet_id.*`, `cjet_id.*`, `tau_id.*` (tagger),
         * `cluster.*` (clustering algorithm), `jetid` and `isolation.<object>.radius`.
         *
         * @param options options.
         * @return false if no algorithm is defined.
         */
        MAbool Initialize(const std::map<std::string, std::string> &options);

        /**
         * @brief Reconstruct an event.
         *
         * @param mySample current sample.
         * @param myEvent event (the reconstructed part is filled).
         * @return false if the Monte Carlo information is missing.
         */
        MAbool Execute(SampleFormat &mySample, EventFormat &myEvent);

        /** @brief Delete the algorithm, the smearer and the tagger. */
        void Finalize();

        /**
         * @brief Replace the default smearer (e.g. by the generated NewSmearer) and initialise it.
         *
         * @param smearer smearer (owned).
         */
        void LoadSmearer(SmearerBase *smearer)
        {
            mySmearer_ = smearer;
            mySmearer_->Initialize();
        }

        /**
         * @brief Replace the default tagger (e.g. by the generated NewTagger) and initialise it.
         *
         * @param tagger tagger (owned).
         */
        void LoadTagger(SFSTaggerBase *tagger)
        {
            // NOTE: the previous smearer/tagger created by Initialize() is not deleted (memory leak).
            myTagger_ = tagger;
            myTagger_->Initialize();
            myTagger_->SetOptions(*myTaggerOptions_);
        }

        /**
         * @brief Declare an additional jet collection (`define jet_algorithm`).
         *
         * @param options options: JetID, algorithm and cluster.* parameters.
         * @return false for an unknown parameter or algorithm type.
         */
        MAbool LoadJetConfiguration(std::map<std::string, std::string> options);

        /**
         * @brief Accessor to the name of the clustering algorithm.
         *
         * @return the name.
         */
        std::string GetName();

        /** @brief Print the parameters of the tagger. */
        void TaggerParameters();

        /** @brief Print the parameters of the clustering algorithm. */
        void PrintParam();

        /**
         * @brief Accessor to the parameters of the clustering algorithm.
         *
         * @return a printable summary.
         */
        std::string GetParameters();

    private:
        /**
         * @brief Is the particle the last one of its type in its decay chain?
         *
         * @param part particle.
         * @param myEvent event (unused).
         * @return true if no daughter has the same PDG code.
         */
        MAbool IsLast(const MCParticleFormat *part, EventFormat &myEvent);
        /**
         * @brief Collect the final-state descendants of a particle.
         *
         * @param part particle.
         * @param finalstates set to extend.
         */
        void GetFinalState(const MCParticleFormat *part, std::set<const MCParticleFormat *> &finalstates);
    };

}

#endif
