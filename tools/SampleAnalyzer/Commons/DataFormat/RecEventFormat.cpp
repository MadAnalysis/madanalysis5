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

// Fastjet headers
#ifdef MA5_FASTJET_MODE
#include "fastjet/PseudoJet.hh"
#endif

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/RecEventFormat.h"

namespace MA5 {
    /// Clearing all information
    void RecEventFormat::Reset()
    {
        PrimaryJetID_ = "Ma5Jet";
        photons_.clear();
        electrons_.clear();
        muons_.clear();
        taus_.clear();
        jetcollection_.clear();
        emptyjet_.clear();
#ifdef MA5_FASTJET_MODE
        // pseudojet : only available when fastjet is in use (code efficiency)
        input_hadrons_.clear();
#endif
        towers_ok_=false;
        towers_.clear();
        tracks_ok_=false;
        tracks_.clear();
        vertices_ok_=false;
        vertices_.clear();
        EFlowTracks_ok_=false;
        EFlowTracks_.clear();
        EFlowPhotons_ok_=false;
        EFlowPhotons_.clear();
        EFlowNeutralHadrons_ok_=false;
        EFlowNeutralHadrons_.clear();
        MET_.Reset();
        MHT_.Reset();
        TET_  = 0.;
        THT_  = 0.;
        Meff_ = 0.;
        MCHadronicTaus_.clear();
        MCMuonicTaus_.clear();
        MCElectronicTaus_.clear();
        MCBquarks_.clear();
        MCCquarks_.clear();
    }

    //======================//
    //===== Jet Tools ======//
    //======================//

    // Remove an element from jet collection
    void RecEventFormat::Remove_Collection(std::string id)
    {
        if (hasJetID(id)) jetcollection_.erase(id);
        else ERROR << "Remove_Collection:: '" << id << "' does not exist." << endmsg;
    }

    /// Change Jet ID
    void RecEventFormat::ChangeJetID(std::string previous_id, std::string new_id)
    {
        if (!hasJetID(new_id) && hasJetID(previous_id))
        {
            auto it = jetcollection_.find(previous_id);
            std::swap(jetcollection_[new_id],it->second);
            jetcollection_.erase(it);
        }
        else
        {
            if (hasJetID(new_id))
                ERROR << "ChangeJetID:: '" << new_id << "' already exists." << endmsg;
            if (!hasJetID(previous_id))
                ERROR << "ChangeJetID:: '" << previous_id << "' does not exist." << endmsg;
        }
    }

    // Get the list of jet collection IDs
    const std::vector<std::string> RecEventFormat::GetJetIDs() const
    {
        std::vector<std::string> keys;
        keys.reserve(jetcollection_.size());
        for (auto &key: jetcollection_)
            keys.emplace_back(key.first);
        return keys;
    }


    // Add a new hadron to be clustered. (for code efficiency)
    void RecEventFormat::AddHadron(MCParticleFormat& v, MAuint32& idx)
    {
#ifdef MA5_FASTJET_MODE
        fastjet::PseudoJet input;
        input.reset(v.px(), v.py(), v.pz(), v.e());
        input.set_user_index(idx);
        input_hadrons_.push_back(input);
#endif
    }

#ifdef MA5_FASTJET_MODE
    // Get hadrons to cluster (code efficiency)
    std::vector<fastjet::PseudoJet>& RecEventFormat::cluster_inputs() {return input_hadrons_;}
#endif

    //======================//
    //=== Get New Object ===//
    //======================//

    /// Giving a new photon entry
    RecPhotonFormat* RecEventFormat::GetNewPhoton()
    {
        photons_.push_back(RecPhotonFormat());
        return &photons_.back();
    }

    /// Giving a new electron entry
    RecLeptonFormat* RecEventFormat::GetNewElectron()
    {
        electrons_.push_back(RecLeptonFormat());
        (&electrons_.back())->setElectronId();
        return &electrons_.back();
    }

    /// Giving a new muon entry
    RecLeptonFormat* RecEventFormat::GetNewMuon()
    {
        muons_.push_back(RecLeptonFormat());
        (&muons_.back())->setMuonId();
        return &muons_.back();
    }

    /// Giving a new tower entry
    RecTowerFormat* RecEventFormat::GetNewTower()
    {
        towers_.push_back(RecTowerFormat());
        return &towers_.back();
    }

    /// Giving a new EFlowTrack entry
    RecTrackFormat* RecEventFormat::GetNewEFlowTrack()
    {
        EFlowTracks_.push_back(RecTrackFormat());
        return &EFlowTracks_.back();
    }

    /// Giving a new EFlowPhoton entry
    RecParticleFormat* RecEventFormat::GetNewEFlowPhoton()
    {
        EFlowPhotons_.push_back(RecParticleFormat());
        return &EFlowPhotons_.back();
    }

    /// Giving a new EFlowNeutralHadron entry
    RecParticleFormat* RecEventFormat::GetNewEFlowNeutralHadron()
    {
        EFlowNeutralHadrons_.push_back(RecParticleFormat());
        return &EFlowNeutralHadrons_.back();
    }

    /// Giving a new tau entry
    RecTauFormat* RecEventFormat::GetNewTau()
    {
        taus_.push_back(RecTauFormat());
        return &taus_.back();
    }

    /// Giving a new primary jet entry
    RecJetFormat* RecEventFormat::GetNewJet()
    {
        std::pair< std::map<std::string, std::vector<RecJetFormat> >::iterator, bool> new_jet;
        new_jet = jetcollection_.insert(std::make_pair(PrimaryJetID_,std::vector<RecJetFormat>() ));
        new_jet.first->second.push_back(RecJetFormat());
        return &(new_jet.first->second.back());
    }

    /// Giving a new primary jet entry
    void RecEventFormat::CreateEmptyJetAccesor()
    {
        std::pair< std::map<std::string, std::vector<RecJetFormat> >::iterator, bool> new_jet;
        new_jet = jetcollection_.insert(std::make_pair(PrimaryJetID_,std::vector<RecJetFormat>() ));
    }

    // Get a new jet entry with specific ID
    RecJetFormat* RecEventFormat::GetNewJet(std::string id)
    {
        std::pair< std::map<std::string, std::vector<RecJetFormat> >::iterator,bool> new_jet;
        new_jet = jetcollection_.insert(std::make_pair(id,std::vector<RecJetFormat>() ));
        new_jet.first->second.push_back(RecJetFormat());
        return &(new_jet.first->second.back());
    }

    // Create an empty jet accessor with specific id
    void RecEventFormat::CreateEmptyJetAccesor(std::string id)
    {
        std::pair< std::map<std::string, std::vector<RecJetFormat> >::iterator,bool> new_jet;
        new_jet = jetcollection_.insert(std::make_pair(id,std::vector<RecJetFormat>() ));
    }

    /// Giving a new fat jet entry
    RecJetFormat* RecEventFormat::GetNewFatJet()
    {
        std::string id = "fatjet";
        std::pair< std::map<std::string, std::vector<RecJetFormat> >::iterator,bool> new_jet;
        new_jet = jetcollection_.insert(std::make_pair(id,std::vector<RecJetFormat>() ));
        new_jet.first->second.push_back(RecJetFormat());
        return &(new_jet.first->second.back());
    }

    /// Giving a new gen jet entry
    RecJetFormat* RecEventFormat::GetNewGenJet()
    {
        std::string id = "genjet";
        std::pair< std::map<std::string, std::vector<RecJetFormat> >::iterator,bool> new_jet;
        new_jet = jetcollection_.insert(std::make_pair(id,std::vector<RecJetFormat>() ));
        new_jet.first->second.push_back(RecJetFormat());
        return &(new_jet.first->second.back());
    }

    /// Giving a new track entry
    RecTrackFormat* RecEventFormat::GetNewTrack()
    {
        tracks_.push_back(RecTrackFormat());
        return &tracks_.back();
    }

    /// Giving a new vertex entry
    RecVertexFormat* RecEventFormat::GetNewVertex()
    {
        vertices_.push_back(RecVertexFormat());
        return &vertices_.back();
    }

    /// Giving a pointer to the Missing Transverse Energy
    RecParticleFormat* RecEventFormat::GetNewMet() { return &MET_; }

    /// Giving a pointer to the Missing Transverse Energy
    RecParticleFormat* RecEventFormat::GetNewMht() { return &MHT_; }


}
