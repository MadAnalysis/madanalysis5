////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2020 Jack Araz, Eric Conte & Benjamin Fuks
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
    namespace fastjet {
        class PseudoJet;
    }
#endif


namespace MA5
{

    namespace Substructure {
        class ClusterBase;
    }
    class LHEReader;
    class LHCOReader;
    class ROOTReader;
    class TauTagger;
    class bTagger;
    class JetClusterer;
    class ClusterAlgoBase;
    class ClusterAlgoFastJet;
    class DelphesTreeReader;
    class DelphesMA5tuneTreeReader;
    class DelphesMemoryInterface;

    class RecEventFormat
    {
        friend class LHEReader;
        friend class LHCOReader;
        friend class ROOTReader;
        friend class TauTagger;
        friend class bTagger;
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
    private :

        /// Collection of reconstructed photons
        std::vector<RecPhotonFormat> photons_;

        /// Collection of reconstructed electrons
        std::vector<RecLeptonFormat> electrons_;

        /// Collection of reconstructed muons
        std::vector<RecLeptonFormat> muons_;

        /// Collection of reconstructed taus
        std::vector<RecTauFormat>    taus_;

        // Identification of the primary jet. Corresponds to content of the jets_
        std::string PrimaryJetID_;

        /// Collection of reconstructed fat jets
        std::vector<RecJetFormat>    fatjets_;

        /// Collection of reconstructed jet dictionary
        // @JACK note for the future: if lots of jet input is used change map
        // to unordered map for efficiency UI is the same!
        std::map<std::string, std::vector<RecJetFormat> > jetcollection_;

#ifdef MA5_FASTJET_MODE
        // hadrons to be clustered (code efficiency)
    std::vector<fastjet::PseudoJet> input_hadrons_;
#endif

        /// Create an empty jet
        std::vector<RecJetFormat>    emptyjet_;

        /// Collection of generated jets
        std::vector<RecJetFormat>    genjets_;

        /// Collection of reconstructed tracks
        MAbool tracks_ok_;
        std::vector<RecTrackFormat>  tracks_;

        /// Collection of reconstructed vertices
        MAbool vertices_ok_;
        std::vector<RecVertexFormat>  vertices_;

        /// Reconstructed towers
        MAbool towers_ok_;
        std::vector<RecTowerFormat> towers_;

        /// Collection of reconstructed EFlow tracks
        MAbool EFlowTracks_ok_;
        std::vector<RecTrackFormat> EFlowTracks_;

        /// Collection of reconstructed EFlow tracks
        MAbool EFlowPhotons_ok_;
        std::vector<RecParticleFormat> EFlowPhotons_;

        /// Collection of reconstructed EFlow tracks
        MAbool EFlowNeutralHadrons_ok_;
        std::vector<RecParticleFormat> EFlowNeutralHadrons_;

        /// Reconstructed Missing Transverse Energy
        RecParticleFormat MET_;

        /// Reconstructed Missing Hadronic Transverse Energy
        RecParticleFormat MHT_;

        /// Reconstructed Scalar sum of transverse energy
        MAfloat64 TET_;

        /// Reconstructed Scalar sum of hadronic transverse energy
        MAfloat64 THT_;

        /// Computed total effective mass (sum of jet's PT + MET
        MAfloat64 Meff_;

        /// Monte Carlo taus decaying hadronically
        std::vector<const MCParticleFormat*> MCHadronicTaus_;

        /// Monte Carlo taus decaying into muon
        std::vector<const MCParticleFormat*> MCMuonicTaus_;

        /// Monte Carlo taus decaying into electron
        std::vector<const MCParticleFormat*> MCElectronicTaus_;

        /// Monte Carlo b-quarks
        std::vector<const MCParticleFormat*> MCBquarks_;

        /// Monte Carlo c-quarks
        std::vector<const MCParticleFormat*> MCCquarks_;


        // -------------------------------------------------------------
        //                      method members
        // -------------------------------------------------------------
    public :

        /// Constructor withtout arguments
        RecEventFormat()
        { Reset(); }

        /// Destructor
        ~RecEventFormat()
        { }

        /// Accessor to the photon collection (read-only)
        const std::vector<RecPhotonFormat>& photons() const {return photons_;}

        /// Accessor to the electron collection (read-only)
        const std::vector<RecLeptonFormat>& electrons() const {return electrons_;}

        /// Accessor to the muon collection (read-only)
        const std::vector<RecLeptonFormat>& muons() const {return muons_;}

        /// Accessor to the tau collection (read-only)
        const std::vector<RecTauFormat>& taus() const {return taus_;}

        /// Accessor to the fat jet collection (read-only)
        const std::vector<RecJetFormat>& fatjets() const {return fatjets_;}

        /// Accessor to the jet collection (read-only)
        const std::vector<RecJetFormat>& jets() const
        {
            if (jetcollection_.find(PrimaryJetID_) != jetcollection_.end())
                return jetcollection_.at(PrimaryJetID_);
            else
            {
                return emptyjet_;
            }
        }

        /// Accessor to the jet collection dictionary (read-only)
        const std::map<std::string, std::vector<RecJetFormat> >& jetcollection() const {return jetcollection_;}

        // Accessor to a specific jet dictionary (read-only)
        const std::vector<RecJetFormat>& jets(std::string id) const
        {
            if (jetcollection_.find(id) != jetcollection_.end())
                return jetcollection_.at(id);
            else
            {
                return emptyjet_;
            }
        }

        /// Accessor to the genjet collection (read-only)
        const std::vector<RecJetFormat>& genjets() const {return genjets_;}

        /// Accessor to the track collection (read-only)
        const std::vector<RecTrackFormat>& tracks() const {return tracks_;}

        /// Accessor to the vertex collection (read-only)
        const std::vector<RecVertexFormat>& vertex() const {return vertices_;}

        /// Accessor to the tower collection (read-only)
        const std::vector<RecTowerFormat>& towers() const {return towers_;}
        const std::vector<RecTrackFormat>& EFlowTracks() const {return EFlowTracks_;}
        const std::vector<RecParticleFormat>& EFlowPhotons() const {return EFlowPhotons_;}
        const std::vector<RecParticleFormat>& EFlowNeutralHadrons() const {return EFlowNeutralHadrons_;}

        /// Accessor to the Missing Transverse Energy (read-only)
        const RecParticleFormat& MET() const {return MET_;}

        /// Accessor to the Missing Hadronic Transverse Energy (read-only)
        const RecParticleFormat& MHT() const {return MHT_;}

        /// Accessor to the Total Transverse Energy (read-only)
        const MAfloat64& TET() const {return TET_;}

        /// Accessor to the Total Hadronic Transverse Energy (read-only)
        const MAfloat64& THT() const {return THT_;}

        /// Accessor to the Total effective mass (read-only)
        const MAfloat64& Meff() const {return Meff_;}

        /// Accessor to the Monte Carlo taus decaying hadronically
        const std::vector<const MCParticleFormat*>& MCHadronicTaus() const
        {return MCHadronicTaus_;}

        /// Accessor to Monte Carlo taus decaying into muon
        const std::vector<const MCParticleFormat*>& MCMuonicTaus() const
        {return MCMuonicTaus_;}

        /// Accessor to Monte Carlo taus decaying into electron
        const std::vector<const MCParticleFormat*>& MCElectronicTaus() const
        {return MCElectronicTaus_;}

        /// Accessor to Monte Carlo b-quarks
        const std::vector<const MCParticleFormat*>& MCBquarks() const
        {return MCBquarks_;}

        /// Accessor to Monte Carlo c-quarks
        const std::vector<const MCParticleFormat*>& MCCquarks() const
        {return MCCquarks_;}

        /// Accessor to the electron collection
        std::vector<RecPhotonFormat>& photons() {return photons_;}

        /// Accessor to the electron collection
        std::vector<RecLeptonFormat>& electrons() {return electrons_;}

        /// Accessor to the muon collection
        std::vector<RecLeptonFormat>& muons() {return muons_;}

        /// Accessor to the tau collection
        std::vector<RecTauFormat>& taus() {return taus_;}

        /// Accessor to the jet collection
        std::vector<RecJetFormat>& jets()
        {
            if (jetcollection_.find(PrimaryJetID_) != jetcollection_.end())
                return jetcollection_.at(PrimaryJetID_);
            else
            {
                return emptyjet_;
            }
        }

        /// Accessor to the jet dictionary
        std::map<std::string, std::vector<RecJetFormat> >& jetcollection() {return jetcollection_;}

        /// Accessor to a specific jet in the dictionary
        std::vector<RecJetFormat>& jets(std::string id)
        {
            if (jetcollection_.find(id) != jetcollection_.end())
                return jetcollection_.at(id);
            else
            {
                return emptyjet_;
            }
        }

        /// Accessor to the fat jet collection
        std::vector<RecJetFormat>& fatjets() {return fatjets_;}

        /// Accessor to the towers collection
        std::vector<RecTowerFormat>& towers() {return towers_;}
        std::vector<RecTrackFormat>& EFlowTracks() {return EFlowTracks_;}
        std::vector<RecParticleFormat>& EFlowPhotons() {return EFlowPhotons_;}
        std::vector<RecParticleFormat>& EFlowNeutralHadrons() {return EFlowNeutralHadrons_;}

        /// Accessor to the jet collection
        std::vector<RecJetFormat>& genjets() {return genjets_;}

        /// Accessor to the track collection
        std::vector<RecTrackFormat>& tracks() {return tracks_;}

        /// Accessor to the Missing Transverse Energy
        RecParticleFormat& MET() {return MET_;}

        /// Accessor to the Missing Hadronic Transverse Energy
        RecParticleFormat& MHT() {return MHT_;}

        /// Accessor to the Total Transverse Energy
        MAfloat64& TET() {return TET_;}

        /// Accessor to the Total Hadronic Transverse Energy
        MAfloat64& THT() {return THT_;}

        /// Accessor to the Total effective mass
        MAfloat64& Meff() {return Meff_;}

        /// Accessor to the Monte Carlo taus decaying hadronically
        std::vector<const MCParticleFormat*>& MCHadronicTaus()
        {return MCHadronicTaus_;}

        /// Accessor to Monte Carlo taus decaying into muon
        std::vector<const MCParticleFormat*>& MCMuonicTaus()
        {return MCMuonicTaus_;}

        /// Accessor to Monte Carlo taus decaying into electron
        std::vector<const MCParticleFormat*>& MCElectronicTaus()
        {return MCElectronicTaus_;}

        /// Accessor to Monte Carlo b-quarks
        std::vector<const MCParticleFormat*>& MCBquarks()
        {return MCBquarks_;}

        /// Accessor to Monte Carlo c-quarks
        std::vector<const MCParticleFormat*>& MCCquarks()
        {return MCCquarks_;}

        /// Clearing all information
        void Reset();

        // Initialize PrimaryJetID
        void SetPrimaryJetID(std::string v) {PrimaryJetID_ = v;}

        // Remove an element from jet collection
        void Remove_Collection(std::string id);

        /// Change Jet ID
        void ChangeJetID(std::string previous_id, std::string new_id);

        // Get the list of jet collection IDs
        const std::vector<std::string> GetJetIDs() const;

        // Check if collection ID exists
        MAbool hasJetID(std::string id) { return (jetcollection_.find(id) != jetcollection_.end()); }

#ifdef MA5_FASTJET_MODE
        // Add a new hadron to be clustered. (for code efficiency)
    void AddHadron(fastjet::PseudoJet v);

    // Get hadrons to cluster (code efficiency)
    std::vector<fastjet::PseudoJet>& cluster_inputs();
#endif

        /// Displaying data member values.
        /// @JACK: added for debugging purposes. Does not need to be included in the original code.
        void Print() const
        {
            INFO << "   -------------------------------------------" << endmsg;
            INFO << "   Event Content : " << endmsg;
            INFO << "      * Jet Content : " << endmsg;
            for (std::map<std::string, std::vector<RecJetFormat> >::const_iterator
                    it=jetcollection_.begin();it!=jetcollection_.end();it++)
            {
                std::string tag = it->first==PrimaryJetID_ ? " (Primary)" : " ";
                INFO << "         - Jet ID = " << it->first << ", Number of Jets = "
                     << it->second.size() << tag << endmsg;
            }
            INFO << "      * Number of Taus      = " << taus_.size() << endmsg;
            INFO << "      * Number of Electrons = " << electrons_.size() << endmsg;
            INFO << "      * Number of Muons     = " << muons_.size() << endmsg;
            INFO << "      * Number of Photons   = " << photons_.size() << endmsg;
            INFO << "   -------------------------------------------" << endmsg;
        }

        /// Giving a new photon entry
        RecPhotonFormat* GetNewPhoton();

        /// Giving a new electron entry
        RecLeptonFormat* GetNewElectron();

        /// Giving a new muon entry
        RecLeptonFormat* GetNewMuon();

        /// Giving a new tower entry
        RecTowerFormat* GetNewTower();

        /// Giving a new EFlowTrack entry
        RecTrackFormat* GetNewEFlowTrack();

        /// Giving a new EFlowPhoton entry
        RecParticleFormat* GetNewEFlowPhoton();

        /// Giving a new EFlowNeutralHadron entry
        RecParticleFormat* GetNewEFlowNeutralHadron();

        /// Giving a new tau entry
        RecTauFormat* GetNewTau();

        /// Giving a new primary jet entry
        RecJetFormat* GetNewJet();

        /// Giving a new primary jet entry
        void CreateEmptyJetAccesor();

        // Get a new jet entry with specific ID
        RecJetFormat* GetNewJet(std::string id);

        // Create an empty jet accessor with specific id
        void CreateEmptyJetAccesor(std::string id);

        /// Giving a new fat jet entry
        RecJetFormat* GetNewFatJet();

        /// Giving a new gen jet entry
        RecJetFormat* GetNewGenJet();

        /// Giving a new track entry
        RecTrackFormat* GetNewTrack();

        /// Giving a new vertex entry
        RecVertexFormat* GetNewVertex();

        /// Giving a pointer to the Missing Transverse Energy
        RecParticleFormat* GetNewMet();

        /// Giving a pointer to the Missing Transverse Energy
        RecParticleFormat* GetNewMht();

    };

}

#endif
