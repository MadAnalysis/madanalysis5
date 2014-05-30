////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
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


// STL headers
#include <sstream>

// SampleHeader headers
#include "SampleAnalyzer/Interfaces/delphes/DelphesTreeReader.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

// ROOT headers
#include <TROOT.h>
#include <TClonesArray.h>

// Delphes headers
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#include "classes/DelphesClasses.h"


using namespace MA5;

// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------
bool DelphesTreeReader::Initialize()
{
  // Create object of class ExRootTreeReader
  treeReader_    = new ExRootTreeReader(tree_);
  total_nevents_ = treeReader_->GetEntries();
  read_nevents_  = 0;

  // Get pointers to branches used in this analysis
  branchJet_       = treeReader_->UseBranch("Jet");
  if (branchJet_==0)
  {
    WARNING << "Jet collection branch is not found" << endmsg;
  }
  branchElectron_  = treeReader_->UseBranch("Electron");
  if (branchElectron_==0)
  {
    WARNING << "Electron collection branch is not found" << endmsg;
  }
  branchPhoton_    = treeReader_->UseBranch("Photon");
  if (branchPhoton_==0)
  {
    WARNING << "Photon collection branch is not found" << endmsg;
  }
  branchMuon_      = treeReader_->UseBranch("Muon");
  if (branchMuon_==0)
  {
    WARNING << "Muon collection branch is not found" << endmsg;
  }
  branchMissingET_ = treeReader_->UseBranch("MissingET");
  if (branchMissingET_==0)
  {
    WARNING << "MissingEt branch is not found" << endmsg;
  }
  branchScalarHT_ = treeReader_->UseBranch("ScalarHT");
  if (branchScalarHT_==0)
  {
    WARNING << "ScalarHT branch is not found" << endmsg;
  }
  branchGenParticle_ = treeReader_->UseBranch("Particle");
  if (branchGenParticle_==0)
  {
    WARNING << "GenParticle branch is not found" << endmsg;
  }
  branchTrack_ = treeReader_->UseBranch("Track");
  if (branchTrack_==0)
  {
    WARNING << "Track branch is not found" << endmsg;
  }

  return true;
}


// -----------------------------------------------------------------------------
// ReadHeader
// -----------------------------------------------------------------------------
bool DelphesTreeReader::ReadHeader(SampleFormat& mySample)
{
  mySample.InitializeRec();
  mySample.SetSampleFormat(MA5FORMAT::DELPHES);
  mySample.SetSampleGenerator(MA5GEN::DELPHES);
  return true;
}


// -----------------------------------------------------------------------------
// ReadEvent
// -----------------------------------------------------------------------------
StatusCode::Type DelphesTreeReader::ReadEvent(EventFormat& myEvent, SampleFormat& mySample)
{
  // Initiliaze MC
  myEvent.InitializeRec();
  myEvent.InitializeMC();

  // (expected) end of the file ?
  if (read_nevents_ >= total_nevents_) return StatusCode::FAILURE;

  // read the next event
  if (!treeReader_->ReadEntry(read_nevents_))
  {
    ERROR << "Unexpected end of the file !" << endmsg;
    return StatusCode::FAILURE;
  }

  read_nevents_++;

  FillEvent(myEvent,mySample);

  return StatusCode::KEEP;
}


// -----------------------------------------------------------------------------
// FinalizeEvent
// -----------------------------------------------------------------------------
bool DelphesTreeReader::FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)
{
  // MHT & THT
  for (unsigned int i=0; i<myEvent.rec()->jets_.size();i++)
  {
    myEvent.rec()->MHT_ -= myEvent.rec()->jets_[i].momentum();
    if (branchScalarHT_==0) myEvent.rec()->THT_ += myEvent.rec()->jets_[i].pt();
    myEvent.rec()->TET_ += myEvent.rec()->jets_[i].pt();
  }

  // TET
  for (unsigned int i=0; i<myEvent.rec()->muons_.size();i++)
  {
    myEvent.rec()->TET_ += myEvent.rec()->muons_[i].pt();
  }
  for (unsigned int i=0; i<myEvent.rec()->electrons_.size();i++)
  {
    myEvent.rec()->TET_ += myEvent.rec()->electrons_[i].pt();
  }
  for (unsigned int i=0; i<myEvent.rec()->taus_.size();i++)
  {
    myEvent.rec()->TET_ += myEvent.rec()->taus_[i].pt();
  }
  for (unsigned int i=0; i<myEvent.rec()->photons_.size();i++)
  {
    myEvent.rec()->TET_ += myEvent.rec()->photons_[i].pt();
  }

  // Finalize MHT
  myEvent.rec()->MHT_.momentum().SetPz(0.);
  myEvent.rec()->MHT_.momentum().SetE(myEvent.rec()->MHT_.momentum().Pt());


  // Mother pointer assignment
  for (unsigned int i=0; i<myEvent.mc()->particles_.size();i++)
  {
    MCParticleFormat& part = myEvent.mc()->particles_[i];

    // MET, MHT, TET, THT
    if (part.statuscode()==1 && !PHYSICS->Id->IsInvisible(part))
    {
      myEvent.mc()->MET_ -= part.momentum();
      myEvent.mc()->TET_ += part.pt();
      if (PHYSICS->Id->IsHadronic(part))
      {
        myEvent.mc()->MHT_ -= part.momentum();
        myEvent.mc()->THT_ += part.pt(); 
      }
    }
    
    /*    unsigned int index1=myEvent.mc()->particles_[i].mothup1_;
    unsigned int index2=myEvent.mc()->particles_[i].mothup2_;
    if (index1!=0 && index2!=0)
    {
      if (index1>=myEvent.mc()->particles_.size() ||
          index2>=myEvent.mc()->particles_.size())
      {
        WARNING << "mother index is greater to nb of particles" << endmsg;
        WARNING << " - index1 = " << index1 << endmsg;
        WARNING << " - index2 = " << index2 << endmsg;
        WARNING << " - particles.size() " << myEvent.mc()->particles_.size() << endmsg;
        WARNING << "This event is skipped." << endmsg;
        return false;
      }

      myEvent.mc()->particles_[i].mother1_ = &myEvent.mc()->particles_[index1-1];
      myEvent.mc()->particles_[index1-1].daughters_.push_back(&myEvent.mc()->particles_[i]);
      myEvent.mc()->particles_[i].mother2_ = &myEvent.mc()->particles_[index2-1];
      myEvent.mc()->particles_[index2-1].daughters_.push_back(&myEvent.mc()->particles_[i]);
    }
    */
  }

  // Finalize event
  myEvent.mc()->MET_.momentum().SetPz(0.);
  myEvent.mc()->MET_.momentum().SetE(myEvent.mc()->MET_.momentum().Pt());
  myEvent.mc()->MHT_.momentum().SetPz(0.);
  myEvent.mc()->MHT_.momentum().SetE(myEvent.mc()->MHT_.momentum().Pt());

  // Normal end
  return true; 
}




// -----------------------------------------------------------------------------
// FillEventParticleLine
// -----------------------------------------------------------------------------
void DelphesTreeReader::FillEvent(EventFormat& myEvent, SampleFormat& mySample)
{
  // Fill electrons
  if (branchElectron_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchElectron_->GetEntries());i++)
  {
    Electron* part = dynamic_cast<Electron*>(branchElectron_->At(i));
    RecLeptonFormat * electron = myEvent.rec()->GetNewElectron();
    electron->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
    if (part->Charge>0) electron->charge_=true; else electron->charge_=false;
    electron->HEoverEE_ = part->EhadOverEem;
  }

  // Fill photons
  if (branchPhoton_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchPhoton_->GetEntries());i++)
  {
    Photon* part = dynamic_cast<Photon*>(branchPhoton_->At(i));
    RecPhotonFormat * photon = myEvent.rec()->GetNewPhoton();
    photon->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
    photon->HEoverEE_ = part->EhadOverEem;
  }

  // Fill muons
  if (branchMuon_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchMuon_->GetEntries());i++)
  {
    Muon* part = dynamic_cast<Muon*>(branchMuon_->At(i));
    RecLeptonFormat * muon = myEvent.rec()->GetNewMuon();
    muon->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
    if (part->Charge>0) muon->charge_=true; else muon->charge_=false;
  }

  // Fill jets and taus
  if (branchJet_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchJet_->GetEntries());i++)
  {
    Jet* part = dynamic_cast<Jet*>(branchJet_->At(i));
    if(part->TauTag==1)
    {
      RecTauFormat * tau = myEvent.rec()->GetNewTau();
      tau->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
      tau->ntracks_  = 0; // To fix later
      if (part->Charge>0) tau->charge_=true; else tau->charge_=false;
      tau->HEoverEE_ = part->EhadOverEem;
    }
    else
    {
      RecJetFormat * jet = myEvent.rec()->GetNewJet();
      jet->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
      jet->ntracks_  = 0; // To fix later
      jet->btag_     = part->BTag;
      jet->HEoverEE_ = part->EhadOverEem;
    }
  }

  // MET
  if (branchMissingET_!=0)
  if (branchMissingET_->GetEntries()>0)
  {
    MissingET* part = dynamic_cast<MissingET*>(branchMissingET_->At(0));
    myEvent.rec()->MET_.momentum_.SetPx(part->MET*cos(part->Phi));
    myEvent.rec()->MET_.momentum_.SetPy(part->MET*sin(part->Phi));
    myEvent.rec()->MET_.momentum_.SetE(part->MET);
  }

  // THT
  if (branchScalarHT_!=0)
  if (branchScalarHT_->GetEntries()>0)
  {
    ScalarHT* part = dynamic_cast<ScalarHT*>(branchScalarHT_->At(0));
    myEvent.rec()->THT_=part->HT;
  }

  // GenParticle collection
  if (branchGenParticle_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchGenParticle_->GetEntries());i++)
  {
    GenParticle* part = dynamic_cast<GenParticle*>(branchGenParticle_->At(i));
    MCParticleFormat * gen = myEvent.mc()->GetNewParticle();
    gen->pdgid_      = part->PID;
    gen->statuscode_ = part->Status;
    gen->mothup1_    = part->M1;
    gen->mothup2_    = part->M2;
    gen->momentum_.SetPxPyPzE(part->Px,part->Py, part->Pz, part->E);
  }

  // Track collection
  if (branchTrack_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchTrack_->GetEntries());i++)
  {
    Track* ref = dynamic_cast<Track*>(branchTrack_->At(i));
    RecTrackFormat * track = myEvent.rec()->GetNewTrack();
    track->pdgid_ = ref->PID;
    if (ref->Charge>0) track->charge_=true; else track->charge_=false;
    track->momentum_.SetPtEtaPhiE(ref->PT,ref->Eta,ref->Phi,ref->PT);
    track->etaOuter_ = ref->EtaOuter;
    track->phiOuter_ = ref->PhiOuter;
  }

}

