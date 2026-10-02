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
 * @file RecEventFormat.h
 * @brief Reconstructed objects of an event.
 */

#ifndef RecEventFormat_h
#define RecEventFormat_h

// STL headers
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/RecLeptonFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecTowerFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecTauFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecPhotonFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecTrackFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecVertexFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

#ifdef MA5_FASTJET_MODE
namespace fastjet
{
    class PseudoJet;
}
#endif

namespace MA5
{

    namespace Substructure
    {
        class ClusterBase;
    }
    class LHEReader;
    class LHCOReader;
    class ROOTReader;
    class TauTagger;
    class bTagger;
    class SFSTaggerBase;
    class JetClusterer;
    class ClusterAlgoBase;
    class ClusterAlgoFastJet;
    class DelphesTreeReader;
    class DelphesMA5tuneTreeReader;
    class DelphesMemoryInterface;

    /**
     * @brief Reconstructed objects of an event.
     *
     * Jets are stored in a dictionary of collections indexed by a jet identifier; the
     * primary collection (PrimaryJetID_, `Ma5Jet` by default) is returned by jets().
     * Additional collections are defined with `define jet_algorithm`; `fatjet` and
     * `genjet` are reserved identifiers. The class also stores the Monte Carlo B/C
     * hadrons and taus used by the truth-level taggers.
     */
    class RecEventFormat
    {
        friend class LHEReader;
        friend class LHCOReader;
        friend class ROOTReader;
        friend class TauTagger;
        friend class bTagger;
        friend class SFSTaggerBase;
        friend class JetClusterer;
        friend class ClusterAlgoBase;
        friend class ClusterAlgoFastJet;
        friend class DelphesTreeReader;
        friend class DelphesMA5tuneTreeReader;
        friend class DelphesMemoryInterface;
        friend class Substructure::ClusterBase;

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    private:
        /** @brief Reconstructed photons. */
        std::vector<RecPhotonFormat> photons_;

        /** @brief Reconstructed electrons. */
        std::vector<RecLeptonFormat> electrons_;

        /** @brief Reconstructed muons. */
        std::vector<RecLeptonFormat> muons_;

        /** @brief Reconstructed hadronic taus. */
        std::vector<RecTauFormat> taus_;

        /** @brief Identifier of the primary jet collection. */
        std::string PrimaryJetID_;

        /** @brief Jet collections, indexed by jet identifier. */
        std::map<std::string, std::vector<RecJetFormat>> jetcollection_;

#ifdef MA5_FASTJET_MODE
        /** @brief Hadrons to be clustered by FastJet (filled during the event reading). */
        std::vector<fastjet::PseudoJet> input_hadrons_;
#endif

        /** @brief Empty jet collection (unused). */
        std::vector<RecJetFormat> emptyjet_;

        /** @brief Reconstructed tracks (and availability flag). */
        MAbool tracks_ok_;
        std::vector<RecTrackFormat> tracks_;

        /** @brief Reconstructed vertices (and availability flag). */
        MAbool vertices_ok_;
        std::vector<RecVertexFormat> vertices_;

        /** @brief Calorimeter towers (and availability flag). */
        MAbool towers_ok_;
        std::vector<RecTowerFormat> towers_;

        /** @brief Energy-flow tracks (and availability flag). */
        MAbool EFlowTracks_ok_;
        std::vector<RecTrackFormat> EFlowTracks_;

        /** @brief Energy-flow photons (and availability flag). */
        MAbool EFlowPhotons_ok_;
        std::vector<RecParticleFormat> EFlowPhotons_;

        /** @brief Energy-flow neutral hadrons (and availability flag). */
        MAbool EFlowNeutralHadrons_ok_;
        std::vector<RecParticleFormat> EFlowNeutralHadrons_;

        /** @brief Missing transverse momentum. */
        RecParticleFormat MET_;

        /** @brief Missing hadronic transverse momentum. */
        RecParticleFormat MHT_;

        /** @brief Scalar sum of the transverse energies. */
        MAfloat64 TET_;

        /** @brief Scalar sum of the hadronic transverse energies. */
        MAfloat64 THT_;

        /** @brief Effective mass (THT + MET). */
        MAfloat64 Meff_;

        /** @brief Monte Carlo taus decaying hadronically (pointers into the Monte Carlo record). */
        std::vector<const MCParticleFormat *> MCHadronicTaus_;

        /** @brief Monte Carlo taus decaying into a muon. */
        std::vector<const MCParticleFormat *> MCMuonicTaus_;

        /** @brief Monte Carlo taus decaying into an electron. */
        std::vector<const MCParticleFormat *> MCElectronicTaus_;

        /** @brief Monte Carlo B hadrons used for b-tagging. */
        std::vector<const MCParticleFormat *> MCBquarks_;

        /** @brief Monte Carlo C hadrons used for c-tagging. */
        std::vector<const MCParticleFormat *> MCCquarks_;

        // -------------------------------------------------------------
        //                      method members
        // -------------------------------------------------------------
    public:
        /** @brief Constructor (members reset). */
        RecEventFormat()
        {
            Reset();
        }

        /** @brief Destructor. */
        ~RecEventFormat()
        {
            // FIXME: the Monte Carlo pointers below point into the particle vector of MCEventFormat (they are
            // not allocated with new, see JetClusterer.cpp): deleting them is undefined behaviour.
            for (auto &p : MCHadronicTaus_)
                if (p != 0)
                    delete p;
            for (auto &p : MCMuonicTaus_)
                if (p != 0)
                    delete p;
            for (auto &p : MCElectronicTaus_)
                if (p != 0)
                    delete p;
            for (auto &p : MCBquarks_)
                if (p != 0)
                    delete p;
            for (auto &p : MCCquarks_)
                if (p != 0)
                    delete p;
        }

        /**
         * @brief Accessor to the photons (read-only).
         *
         * @return the photons.
         */
        const std::vector<RecPhotonFormat> &photons() const { return photons_; }

        /**
         * @brief Accessor to the electrons (read-only).
         *
         * @return the electrons.
         */
        const std::vector<RecLeptonFormat> &electrons() const { return electrons_; }

        /**
         * @brief Accessor to the muons (read-only).
         *
         * @return the muons.
         */
        const std::vector<RecLeptonFormat> &muons() const { return muons_; }

        /**
         * @brief Accessor to the hadronic taus (read-only).
         *
         * @return the hadronic taus.
         */
        const std::vector<RecTauFormat> &taus() const { return taus_; }

        /**
         * @brief Accessor to a jet collection (read-only).
         *
         * @param id jet identifier.
         * @return the collection (empty if the identifier does not exist).
         */
        const std::vector<RecJetFormat> &jets(std::string id) const
        {
            auto it = jetcollection_.find(id);
            if (it != jetcollection_.end())
                return it->second;

            static const std::vector<RecJetFormat> empty_jet;
            return empty_jet;
        }

        /**
         * @brief Accessor to the fat jets (`fatjet` collection) (read-only).
         *
         * @return the fat jets.
         */
        const std::vector<RecJetFormat> &fatjets() const { return jets("fatjet"); }

        /**
         * @brief Accessor to the primary jets (read-only).
         *
         * @return the primary jets.
         */
        const std::vector<RecJetFormat> &jets() const { return jets(PrimaryJetID_); }

        /**
         * @brief Accessor to the jet collections (read-only).
         *
         * @return the jet collections.
         */
        const std::map<std::string, std::vector<RecJetFormat>> &jetcollection() const { return jetcollection_; }

        /**
         * @brief Accessor to the generator-level jets (`genjet` collection) (read-only).
         *
         * @return the generator-level jets.
         */
        const std::vector<RecJetFormat> &genjets() const { return jets("genjet"); }

        /**
         * @brief Accessor to the tracks (read-only).
         *
         * @return the tracks.
         */
        const std::vector<RecTrackFormat> &tracks() const { return tracks_; }

        /**
         * @brief Accessor to the vertices (read-only).
         *
         * @return the vertices.
         */
        const std::vector<RecVertexFormat> &vertex() const { return vertices_; }

        /**
         * @brief Accessor to the calorimeter towers (read-only).
         *
         * @return the calorimeter towers.
         */
        const std::vector<RecTowerFormat> &towers() const { return towers_; }
        /**
         * @brief Accessor to the energy-flow tracks (read-only).
         *
         * @return the energy-flow tracks.
         */
        const std::vector<RecTrackFormat> &EFlowTracks() const { return EFlowTracks_; }
        /**
         * @brief Accessor to the energy-flow photons (read-only).
         *
         * @return the energy-flow photons.
         */
        const std::vector<RecParticleFormat> &EFlowPhotons() const { return EFlowPhotons_; }
        /**
         * @brief Accessor to the energy-flow neutral hadrons (read-only).
         *
         * @return the energy-flow neutral hadrons.
         */
        const std::vector<RecParticleFormat> &EFlowNeutralHadrons() const { return EFlowNeutralHadrons_; }

        /**
         * @brief Accessor to the missing transverse momentum (read-only).
         *
         * @return the missing transverse momentum.
         */
        const RecParticleFormat &MET() const { return MET_; }

        /**
         * @brief Accessor to the missing hadronic transverse momentum (read-only).
         *
         * @return the missing hadronic transverse momentum.
         */
        const RecParticleFormat &MHT() const { return MHT_; }

        /**
         * @brief Accessor to the scalar sum of the transverse energies (read-only).
         *
         * @return the scalar sum of the transverse energies.
         */
        const MAfloat64 &TET() const { return TET_; }

        /**
         * @brief Accessor to the scalar sum of the hadronic transverse energies (read-only).
         *
         * @return the scalar sum of the hadronic transverse energies.
         */
        const MAfloat64 &THT() const { return THT_; }

        /**
         * @brief Accessor to the effective mass (read-only).
         *
         * @return the effective mass.
         */
        const MAfloat64 &Meff() const { return Meff_; }

        /**
         * @brief Accessor to the Monte Carlo taus decaying hadronically (read-only).
         *
         * @return the Monte Carlo taus decaying hadronically.
         */
        const std::vector<const MCParticleFormat *> &MCHadronicTaus() const
        {
            return MCHadronicTaus_;
        }

        /**
         * @brief Accessor to the Monte Carlo taus decaying into a muon (read-only).
         *
         * @return the Monte Carlo taus decaying into a muon.
         */
        const std::vector<const MCParticleFormat *> &MCMuonicTaus() const
        {
            return MCMuonicTaus_;
        }

        /**
         * @brief Accessor to the Monte Carlo taus decaying into an electron (read-only).
         *
         * @return the Monte Carlo taus decaying into an electron.
         */
        const std::vector<const MCParticleFormat *> &MCElectronicTaus() const
        {
            return MCElectronicTaus_;
        }

        /**
         * @brief Accessor to the Monte Carlo B hadrons (read-only).
         *
         * @return the Monte Carlo B hadrons.
         */
        const std::vector<const MCParticleFormat *> &MCBquarks() const
        {
            return MCBquarks_;
        }

        /**
         * @brief Accessor to the Monte Carlo C hadrons (read-only).
         *
         * @return the Monte Carlo C hadrons.
         */
        const std::vector<const MCParticleFormat *> &MCCquarks() const
        {
            return MCCquarks_;
        }

        /**
         * @brief Accessor to the photons.
         *
         * @return the photons.
         */
        std::vector<RecPhotonFormat> &photons() { return photons_; }

        /**
         * @brief Accessor to the electrons.
         *
         * @return the electrons.
         */
        std::vector<RecLeptonFormat> &electrons() { return electrons_; }

        /**
         * @brief Accessor to the muons.
         *
         * @return the muons.
         */
        std::vector<RecLeptonFormat> &muons() { return muons_; }

        /**
         * @brief Accessor to the hadronic taus.
         *
         * @return the hadronic taus.
         */
        std::vector<RecTauFormat> &taus() { return taus_; }

        /**
         * @brief Accessor to a jet collection.
         *
         * @param id jet identifier.
         * @return the collection (a shared static empty vector if the identifier does not exist).
         */
        std::vector<RecJetFormat> &jets(std::string id)
        {
            auto it = jetcollection_.find(id);
            if (it != jetcollection_.end())
                return it->second;

            // NOTE: modifying the returned empty vector affects all later lookups of unknown identifiers.
            static std::vector<RecJetFormat> empty_jet;
            return empty_jet;
        }

        /**
         * @brief Accessor to the primary jets.
         *
         * @return the primary jets.
         */
        std::vector<RecJetFormat> &jets() { return jets(PrimaryJetID_); }

        /**
         * @brief Accessor to the jet collections.
         *
         * @return the jet collections.
         */
        std::map<std::string, std::vector<RecJetFormat>> &jetcollection() { return jetcollection_; }

        /**
         * @brief Accessor to the fat jets (`fatjet` collection).
         *
         * @return the fat jets.
         */
        std::vector<RecJetFormat> &fatjets() { return jets("fatjet"); }

        /**
         * @brief Accessor to the calorimeter towers.
         *
         * @return the calorimeter towers.
         */
        std::vector<RecTowerFormat> &towers() { return towers_; }
        /**
         * @brief Accessor to the energy-flow tracks.
         *
         * @return the energy-flow tracks.
         */
        std::vector<RecTrackFormat> &EFlowTracks() { return EFlowTracks_; }
        /**
         * @brief Accessor to the energy-flow photons.
         *
         * @return the energy-flow photons.
         */
        std::vector<RecParticleFormat> &EFlowPhotons() { return EFlowPhotons_; }
        /**
         * @brief Accessor to the energy-flow neutral hadrons.
         *
         * @return the energy-flow neutral hadrons.
         */
        std::vector<RecParticleFormat> &EFlowNeutralHadrons() { return EFlowNeutralHadrons_; }

        /**
         * @brief Accessor to the generator-level jets (`genjet` collection).
         *
         * @return the generator-level jets.
         */
        std::vector<RecJetFormat> &genjets() { return jets("genjet"); }

        /**
         * @brief Accessor to the tracks.
         *
         * @return the tracks.
         */
        std::vector<RecTrackFormat> &tracks() { return tracks_; }

        /**
         * @brief Accessor to the missing transverse momentum.
         *
         * @return the missing transverse momentum.
         */
        RecParticleFormat &MET() { return MET_; }

        /**
         * @brief Accessor to the missing hadronic transverse momentum.
         *
         * @return the missing hadronic transverse momentum.
         */
        RecParticleFormat &MHT() { return MHT_; }

        /**
         * @brief Accessor to the scalar sum of the transverse energies.
         *
         * @return the scalar sum of the transverse energies.
         */
        MAfloat64 &TET() { return TET_; }

        /**
         * @brief Accessor to the scalar sum of the hadronic transverse energies.
         *
         * @return the scalar sum of the hadronic transverse energies.
         */
        MAfloat64 &THT() { return THT_; }

        /**
         * @brief Accessor to the effective mass.
         *
         * @return the effective mass.
         */
        MAfloat64 &Meff() { return Meff_; }

        /**
         * @brief Accessor to the Monte Carlo taus decaying hadronically.
         *
         * @return the Monte Carlo taus decaying hadronically.
         */
        std::vector<const MCParticleFormat *> &MCHadronicTaus()
        {
            return MCHadronicTaus_;
        }

        /**
         * @brief Accessor to the Monte Carlo taus decaying into a muon.
         *
         * @return the Monte Carlo taus decaying into a muon.
         */
        std::vector<const MCParticleFormat *> &MCMuonicTaus()
        {
            return MCMuonicTaus_;
        }

        /**
         * @brief Accessor to the Monte Carlo taus decaying into an electron.
         *
         * @return the Monte Carlo taus decaying into an electron.
         */
        std::vector<const MCParticleFormat *> &MCElectronicTaus()
        {
            return MCElectronicTaus_;
        }

        /**
         * @brief Accessor to the Monte Carlo B hadrons.
         *
         * @return the Monte Carlo B hadrons.
         */
        std::vector<const MCParticleFormat *> &MCBquarks()
        {
            return MCBquarks_;
        }

        /**
         * @brief Accessor to the Monte Carlo C hadrons.
         *
         * @return the Monte Carlo C hadrons.
         */
        std::vector<const MCParticleFormat *> &MCCquarks()
        {
            return MCCquarks_;
        }

        /** @brief Reset the event (the primary jet identifier is set back to `Ma5Jet`). */
        void Reset();

        /**
         * @brief Set the identifier of the primary jet collection.
         *
         * @param v jet identifier.
         */
        void SetPrimaryJetID(std::string v) { PrimaryJetID_ = v; }

        /**
         * @brief Remove a jet collection (error if it does not exist).
         *
         * @param id jet identifier.
         */
        void Remove_Collection(std::string id);

        /**
         * @brief Rename a jet collection.
         *
         * @param previous_id current identifier.
         * @param new_id new identifier (must not exist).
         */
        void ChangeJetID(std::string previous_id, std::string new_id);

        /**
         * @brief List the identifiers of the jet collections.
         *
         * @return the identifiers.
         */
        const std::vector<std::string> GetJetIDs() const;

        /**
         * @brief Does a jet collection exist?
         *
         * @param id jet identifier.
         * @return true if it exists.
         */
        MAbool hasJetID(std::string id) { return (jetcollection_.find(id) != jetcollection_.end()); }

        /**
         * @brief Add a hadron to the list of FastJet inputs (FastJet mode only).
         *
         * @param v hadron.
         * @param idx index of the hadron in the Monte Carlo record (FastJet user index).
         */
        void AddHadron(MCParticleFormat &v, MAuint32 &idx);

#ifdef MA5_FASTJET_MODE
        /**
         * @brief Accessor to the FastJet inputs.
         *
         * @return the pseudojets.
         */
        std::vector<fastjet::PseudoJet> &cluster_inputs();
#endif

        /** @brief Print the content of the event (debugging). */
        void Print() const
        {
            INFO << "   -------------------------------------------" << endmsg;
            INFO << "   Event Content : " << endmsg;
            INFO << "      * Jet Content : " << endmsg;
            for (auto &col : jetcollection_)
            {
                std::string tag = col.first == PrimaryJetID_ ? " (Primary)" : " ";
                INFO << "         - Jet ID = " << col.first << ", Number of Jets = "
                     << col.second.size() << tag << endmsg;
            }
            INFO << "      * Number of Taus      = " << taus_.size() << endmsg;
            INFO << "      * Number of Electrons = " << electrons_.size() << endmsg;
            INFO << "      * Number of Muons     = " << muons_.size() << endmsg;
            INFO << "      * Number of Photons   = " << photons_.size() << endmsg;
            INFO << "   -------------------------------------------" << endmsg;
        }

        /**
         * @brief Append a new photon.
         *
         * @return a pointer to the new object (invalidated by the next insertion).
         */
        RecPhotonFormat *GetNewPhoton();

        /**
         * @brief Append a new electron.
         *
         * @return a pointer to the new object (invalidated by the next insertion).
         */
        RecLeptonFormat *GetNewElectron();

        /**
         * @brief Append a new muon.
         *
         * @return a pointer to the new object (invalidated by the next insertion).
         */
        RecLeptonFormat *GetNewMuon();

        /**
         * @brief Append a new calorimeter tower.
         *
         * @return a pointer to the new object (invalidated by the next insertion).
         */
        RecTowerFormat *GetNewTower();

        /**
         * @brief Append a new energy-flow track.
         *
         * @return a pointer to the new object (invalidated by the next insertion).
         */
        RecTrackFormat *GetNewEFlowTrack();

        /**
         * @brief Append a new energy-flow photon.
         *
         * @return a pointer to the new object (invalidated by the next insertion).
         */
        RecParticleFormat *GetNewEFlowPhoton();

        /**
         * @brief Append a new energy-flow neutral hadron.
         *
         * @return a pointer to the new object (invalidated by the next insertion).
         */
        RecParticleFormat *GetNewEFlowNeutralHadron();

        /**
         * @brief Append a new hadronic tau.
         *
         * @return a pointer to the new object (invalidated by the next insertion).
         */
        RecTauFormat *GetNewTau();

        /**
         * @brief Append a new jet to a collection (created if needed).
         *
         * @param id jet identifier.
         * @return a pointer to the new jet.
         */
        RecJetFormat *GetNewJet(std::string id);

        /**
         * @brief Append a new primary jet.
         *
         * @return a pointer to the new object (invalidated by the next insertion).
         */
        RecJetFormat *GetNewJet() { return GetNewJet(PrimaryJetID_); }

        /**
         * @brief Append a new fat jet.
         *
         * @return a pointer to the new object (invalidated by the next insertion).
         */
        RecJetFormat *GetNewFatJet() { return GetNewJet("fatjet"); }

        /**
         * @brief Append a new generator-level jet.
         *
         * @return a pointer to the new object (invalidated by the next insertion).
         */
        RecJetFormat *GetNewGenJet() { return GetNewJet("genjet"); }

        /**
         * @brief Create an empty jet collection (nothing is done if it exists).
         *
         * @param id jet identifier.
         */
        void CreateEmptyJetAccesor(std::string id);

        /** @brief Create an empty primary jet collection. */
        void CreateEmptyJetAccesor() { return CreateEmptyJetAccesor(PrimaryJetID_); }

        /**
         * @brief Append a new track.
         *
         * @return a pointer to the new object (invalidated by the next insertion).
         */
        RecTrackFormat *GetNewTrack();

        /**
         * @brief Append a new vertex.
         *
         * @return a pointer to the new object (invalidated by the next insertion).
         */
        RecVertexFormat *GetNewVertex();

        /**
         * @brief Accessor to the missing transverse momentum object.
         *
         * @return a pointer to MET.
         */
        RecParticleFormat *GetNewMet() { return &MET_; }

        /**
         * @brief Accessor to the missing hadronic transverse momentum object.
         *
         * @return a pointer to MHT.
         */
        RecParticleFormat *GetNewMht() { return &MHT_; }
    };

}

#endif
