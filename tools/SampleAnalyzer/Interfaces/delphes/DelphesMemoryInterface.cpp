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

// SampleHeader headers
/**
 * @file DelphesMemoryInterface.cpp
 * @brief Implementation of MA5::DelphesMemoryInterface.
 */

#include "SampleAnalyzer/Interfaces/delphes/DelphesMemoryInterface.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"

// Delphes headers
#include "classes/DelphesClasses.h"
#include "modules/Delphes.h"

// ROOT headers
#include <TObjArray.h>
#include <TFile.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>
#include <TClonesArray.h>

// Exceptions
#include <stdexcept>

using namespace MA5;

// -----------------------------------------------------------------------------
// Constructor without arguments
// -----------------------------------------------------------------------------
DelphesMemoryInterface::DelphesMemoryInterface()
{
    // FIXME: Vertex_ is neither initialised here nor set in Initialize().
    Jet_ = 0;
    FatJet_ = 0;
    Electron_ = 0;
    Photon_ = 0;
    Muon_ = 0;
    MET_ = 0;
    HT_ = 0;
    GenParticle_ = 0;
    Track_ = 0;
    Tower_ = 0;
    EFlowTrack_ = 0;
    EFlowPhoton_ = 0;
    EFlowNeutral_ = 0;
    Event_ = 0;
    delphesMA5card_ = false;
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
DelphesMemoryInterface::~DelphesMemoryInterface() {}

// -----------------------------------------------------------------------------
// GetCollection
// -----------------------------------------------------------------------------
TObjArray* DelphesMemoryInterface::GetCollection(Delphes* delphes, const std::map<std::string, std::string>& table, const std::string& name)
{
    std::map<std::string, std::string>::const_iterator it = table.find(name);

    if (it == table.end())
        return 0;

    return delphes->ImportArray(it->second.c_str());
}

// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------
void DelphesMemoryInterface::Initialize(Delphes *delphes, const std::map<std::string, std::string> &table, MAbool MA5card)
{
    // DelphesMA5 card ?
    delphesMA5card_ = MA5card;

    // Official Delphes collections
    //  GenJet_       = GetCollection(delphes,table,"GenJet");
    MET_ = GetCollection(delphes, table, "MissingET");
    Tower_ = GetCollection(delphes, table, "Tower");
    Track_ = GetCollection(delphes, table, "Track");
    HT_ = GetCollection(delphes, table, "ScalarHT");
    EFlowTrack_ = GetCollection(delphes, table, "EFlowTrack");
    EFlowPhoton_ = GetCollection(delphes, table, "EFlowPhoton");
    EFlowNeutral_ = GetCollection(delphes, table, "EFlowNeutralHadron");
    FatJet_ = GetCollection(delphes, table, "FatJet");

    // MA5 Delphes collections
    if (MA5card)
    {
        Jet_ = GetCollection(delphes, table, "JetMA5");
        Electron_ = GetCollection(delphes, table, "ElectronMA5");
        Muon_ = GetCollection(delphes, table, "MuonMA5");
        Photon_ = GetCollection(delphes, table, "PhotonMA5");
    }
    else
    {
        Jet_ = GetCollection(delphes, table, "Jet");
        Electron_ = GetCollection(delphes, table, "Electron");
        Muon_ = GetCollection(delphes, table, "Muon");
        Photon_ = GetCollection(delphes, table, "Photon");
    }

    // Display warning to main branches
    try
    {
        if (MET_ == 0)
            throw EXCEPTION_WARNING("Delphes output: MET is not found", "", 0);
        if (Electron_ == 0)
            throw EXCEPTION_WARNING("Delphes output: Electron collection is not found", "", 0);
        if (Muon_ == 0)
            throw EXCEPTION_WARNING("Delphes output: Muon collection is not found", "", 0);
        if (Photon_ == 0)
            throw EXCEPTION_WARNING("Delphes output: Photon collection is not found", "", 0);
        if (Jet_ == 0)
            throw EXCEPTION_WARNING("Delphes output: Jet collection is not found", "", 0);
    }
    catch (const std::exception &e)
    {
        MANAGE_EXCEPTION(e);
    }
}

// -----------------------------------------------------------------------------
// TransfertDELPHEStoMA5
// -----------------------------------------------------------------------------
MAbool DelphesMemoryInterface::TransfertDELPHEStoMA5(SampleFormat &mySample, EventFormat &myEvent)
{
  // Use the same generator association as Delphes TreeWriter for tracks
  // and leptons. Cloning preserves the original candidate at position zero.
  const auto associateMC = [&](RecParticleFormat *particle, Candidate *candidate)
  {
    TObjArray *sources = candidate->GetCandidates();
    const Candidate *source = sources->GetEntriesFast() == 0 ? 0 : dynamic_cast<const Candidate *>(sources->At(0));

    if (source == 0)
        throw std::runtime_error("DelphesMemoryInterface: missing track/lepton source candidate");

    // Event-local identity shared by a track and its reconstructed lepton,
    // including pileup particles absent from the input MC collection.
    particle->delphesTags_.push_back(static_cast<MAuint64>(source->GetUniqueID()));
    particle->mc_ = 0;

    // Delphes generates pileup internally: these particles have no counterpart
    // in myEvent.mc(), so their MC association remains null.
    const auto found = MCParticleIndices_.find(source);
    if (found != MCParticleIndices_.end())
        particle->mc_ = &myEvent.mc()->particles()[found->second];
    else if (!source->IsPU)
        throw std::runtime_error("DelphesMemoryInterface: non-pileup source is not an input MC particle");
  };

    // --------------Jet collection
    if (Jet_ != 0)
    {
        MAint32 njet = 0;
        for (MAuint32 i = 0; i < static_cast<MAuint32>(Jet_->GetEntries()); i++)
        {
            Candidate *cand = dynamic_cast<Candidate *>(Jet_->At(i));
            if (cand == 0)
            {
                ERROR << "impossible to access the " << i + 1 << "th jet" << endmsg;
                continue;
            }
            if (cand->TauTag == 1)
            {
                RecTauFormat *tau = myEvent.rec()->GetNewTau();
                tau->momentum_.SetPxPyPzE(
                    cand->Momentum.Px(),
                    cand->Momentum.Py(),
                    cand->Momentum.Pz(),
                    cand->Momentum.E());
                if (cand->Charge > 0)
                    tau->charge_ = true;
                else
                    tau->charge_ = false;
                if (cand->Eem != 0)
                    tau->HEoverEE_ = cand->Ehad / cand->Eem;
                else
                    tau->HEoverEE_ = 999.;
                tau->ntracks_ = 0; // To fix later
            }
            else
            {
                RecJetFormat *jet = myEvent.rec()->GetNewJet();
                jet->setMomentum(
                    MALorentzVector(
                        cand->Momentum.Px(),
                        cand->Momentum.Py(),
                        cand->Momentum.Pz(),
                        cand->Momentum.E()));
                jet->loose_btag_ = cand->BTag;
                if (cand->Eem != 0)
                    jet->HEoverEE_ = cand->Ehad / cand->Eem;
                else
                    jet->HEoverEE_ = 999.;
                jet->ntracks_ = 0; // To fix later
                njet++;
            }
        }
        if (njet == 0)
            myEvent.rec()->CreateEmptyJetAccesor();
    }
    else
    {
        myEvent.rec()->CreateEmptyJetAccesor();
    }

    if (FatJet_ != 0)
    {
        for (MAuint32 i = 0; i < static_cast<MAuint32>(FatJet_->GetEntries()); i++)
        {
            Candidate *cand = dynamic_cast<Candidate *>(FatJet_->At(i));
            if (cand == 0)
            {
                ERROR << "impossible to access the " << i + 1 << "th jet" << endmsg;
                continue;
            }

            RecJetFormat *jet = myEvent.rec()->GetNewFatJet();
            jet->setMomentum(
                MALorentzVector(
                    cand->Momentum.Px(),
                    cand->Momentum.Py(),
                    cand->Momentum.Pz(),
                    cand->Momentum.E()));
            jet->loose_btag_ = cand->BTag;
            if (cand->Eem != 0)
                jet->HEoverEE_ = cand->Ehad / cand->Eem;
            else
                jet->HEoverEE_ = 999.;
            jet->ntracks_ = 0; // To fix later
        }
    }
    else
    {
        myEvent.rec()->CreateEmptyJetAccesor("fatjet");
    }

    // --------------GenJet collection
    /*  if (genjetsArray!=0)
    {
      for (MAuint32 i=0;i<static_cast<MAuint32>(genjetsArray->GetEntries());i++)
      {
        Candidate* cand = dynamic_cast<Candidate*>(genjetsArray->At(i));
        if (cand==0)
        {
          ERROR << "impossible to access the " << i+1 << "th genjet" << endmsg;
          continue;
        }
        RecJetFormat* genjet = myEvent.rec()->GetNewGenJet();
        genjet->momentum_ = cand->Momentum;
        genjet->loose_btag_ = cand->BTag;
      }
      }*/

    // --------------Muon collection
    if (Muon_ != 0)
    {
        for (MAuint32 i = 0; i < static_cast<MAuint32>(Muon_->GetEntries()); i++)
        {
            Candidate *cand = dynamic_cast<Candidate *>(Muon_->At(i));
            if (cand == 0)
            {
                ERROR << "impossible to access the " << i + 1 << "th muon" << endmsg;
                continue;
            }
            RecLeptonFormat *muon = myEvent.rec()->GetNewMuon();
            associateMC(muon, cand);
            muon->d0_ = cand->D0;
            muon->d0error_ = cand->ErrorD0;
            muon->dz_ = cand->DZ;
            muon->dzerror_ = cand->ErrorDZ;
            muon->momentum_.SetPxPyPzE(cand->Momentum.Px(), cand->Momentum.Py(), cand->Momentum.Pz(), cand->Momentum.E());
            muon->SetCharge(cand->Charge);
        }
    }

    // --------------Electron collection
    if (Electron_ != 0)
    {
        for (MAuint32 i = 0; i < static_cast<MAuint32>(Electron_->GetEntries()); i++)
        {
            Candidate *cand = dynamic_cast<Candidate *>(Electron_->At(i));
            if (cand == 0)
            {
                ERROR << "impossible to access the " << i + 1 << "th electron" << endmsg;
                continue;
            }
            RecLeptonFormat *elec = myEvent.rec()->GetNewElectron();
            associateMC(elec, cand);
            elec->d0_ = cand->D0;
            elec->d0error_ = cand->ErrorD0;
            elec->dz_ = cand->DZ;
            elec->dzerror_ = cand->ErrorDZ;
            elec->momentum_.SetPxPyPzE(cand->Momentum.Px(), cand->Momentum.Py(), cand->Momentum.Pz(), cand->Momentum.E());
            elec->SetCharge(cand->Charge);
        }
    }

    // --------------Photon collection
    if (Photon_ != 0)
    {
        for (MAuint32 i = 0; i < static_cast<MAuint32>(Photon_->GetEntries()); i++)
        {
            Candidate *cand = dynamic_cast<Candidate *>(Photon_->At(i));
            if (cand == 0)
            {
                ERROR << "impossible to access the " << i + 1 << "th photon" << endmsg;
                continue;
            }
            else
            {
                RecPhotonFormat *photon = myEvent.rec()->GetNewPhoton();
                photon->momentum_.SetPxPyPzE(cand->Momentum.Px(), cand->Momentum.Py(), cand->Momentum.Pz(), cand->Momentum.E());
                if (cand->Eem != 0)
                    photon->HEoverEE_ = cand->Ehad / cand->Eem;
                else
                    photon->HEoverEE_ = 999.;
            }
        }
    }

    // --------------Track collection
    if (Track_ != 0)
    {
        for (MAuint32 i = 0; i < static_cast<MAuint32>(Track_->GetEntries()); i++)
        {
            Candidate *cand = dynamic_cast<Candidate *>(Track_->At(i));
            if (cand == 0)
            {
                ERROR << "impossible to access the " << i + 1 << "th track" << endmsg;
                continue;
            }
            RecTrackFormat *track = myEvent.rec()->GetNewTrack();
            associateMC(track, cand);
            track->pdgid_ = cand->PID;
            if (cand->Charge > 0)
                track->charge_ = true;
            else
                track->charge_ = false;
            track->momentum_.SetPxPyPzE(cand->Momentum.Px(), cand->Momentum.Py(), cand->Momentum.Pz(), cand->Momentum.E());
            track->etaOuter_ = cand->Position.Eta();
            track->phiOuter_ = cand->Position.Phi();
        }
    }

    // --------------MET
    if (MET_ != 0)
    {
        Candidate *metCand = dynamic_cast<Candidate *>(MET_->At(0));
        if (metCand == 0)
        {
            ERROR << "impossible to access the MET" << endmsg;
        }
        else
        {
            MAfloat64 pt = metCand->Momentum.Pt();
            MAfloat64 px = metCand->Momentum.Px();
            MAfloat64 py = metCand->Momentum.Py();
            myEvent.rec()->MET().momentum_.SetPxPyPzE(px, py, 0, pt);
        }
    }

    // --------------HT
    if (HT_ != 0)
    {
        Candidate *Cand = dynamic_cast<Candidate *>(HT_->At(0));
        if (Cand == 0)
        {
            ERROR << "impossible to access the HT" << endmsg;
        }
        else
        {
            myEvent.rec()->THT_ = Cand->Momentum.Pt();
        }
    }

    // --------------Tower collection
    if (Tower_ != 0)
    {
        for (MAuint32 i = 0; i < static_cast<MAuint32>(Tower_->GetEntries()); i++)
        {
            Candidate *cand = dynamic_cast<Candidate *>(Tower_->At(i));
            if (cand == 0)
            {
                ERROR << "impossible to access the " << i + 1 << "th track" << endmsg;
                continue;
            }
            RecTowerFormat *tower = myEvent.rec()->GetNewTower();
            tower->momentum_.SetPxPyPzE(cand->Momentum.Px(), cand->Momentum.Py(), cand->Momentum.Pz(), cand->Momentum.E());
        }
    }

    // --------------EFlowTrack collection
    if (EFlowTrack_ != 0)
    {
        for (MAuint32 i = 0; i < static_cast<MAuint32>(EFlowTrack_->GetEntries()); i++)
        {
            Candidate *cand = dynamic_cast<Candidate *>(EFlowTrack_->At(i));
            if (cand == 0)
            {
                ERROR << "impossible to access the " << i + 1 << "th track" << endmsg;
                continue;
            }
            RecTrackFormat *track = myEvent.rec()->GetNewEFlowTrack();
            track->momentum_.SetPxPyPzE(cand->Momentum.Px(), cand->Momentum.Py(), cand->Momentum.Pz(), cand->Momentum.E());
        }
    }

    // --------------EFlowPhoton collection
    if (EFlowPhoton_ != 0)
    {
        for (MAuint32 i = 0; i < static_cast<MAuint32>(EFlowPhoton_->GetEntries()); i++)
        {
            Candidate *cand = dynamic_cast<Candidate *>(EFlowPhoton_->At(i));
            if (cand == 0)
            {
                ERROR << "impossible to access the " << i + 1 << "th track" << endmsg;
                continue;
            }
            RecParticleFormat *tower = myEvent.rec()->GetNewEFlowPhoton();
            tower->momentum_.SetPxPyPzE(cand->Momentum.Px(), cand->Momentum.Py(), cand->Momentum.Pz(), cand->Momentum.E());
        }
    }

    // --------------EFlowNeutral collection
    if (EFlowNeutral_ != 0)
    {
        for (MAuint32 i = 0; i < static_cast<MAuint32>(EFlowNeutral_->GetEntries()); i++)
        {
            Candidate *cand = dynamic_cast<Candidate *>(EFlowNeutral_->At(i));
            if (cand == 0)
            {
                ERROR << "impossible to access the " << i + 1 << "th track" << endmsg;
                continue;
            }
            RecParticleFormat *tower = myEvent.rec()->GetNewEFlowNeutralHadron();
            tower->momentum_.SetPxPyPzE(cand->Momentum.Px(), cand->Momentum.Py(), cand->Momentum.Pz(), cand->Momentum.E());
        }
    }

    return true;
}
