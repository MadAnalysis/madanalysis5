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
#include <TError.h>
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
  total_nevents_ = tree_->GetEntries();
  read_nevents_  = 0;

  // Iniializing branches
  data_.InitializeBranch(tree_);

  // Iniializing data
  data_.InitializeData();

  return true;
}


// -----------------------------------------------------------------------------
// ReadHeader
// -----------------------------------------------------------------------------
bool DelphesTreeReader::ReadHeader(SampleFormat& mySample)
{
  mySample.InitializeRec();
  if (data_.delphesMA5card_)
  {
    mySample.SetSampleFormat(MA5FORMAT::DELPHESMA5CARD);
    mySample.SetSampleGenerator(MA5GEN::DELPHESMA5CARD);
  }
  else
  {
    mySample.SetSampleFormat(MA5FORMAT::DELPHES);
    mySample.SetSampleGenerator(MA5GEN::DELPHES);
  }
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
  Int_t treeEntry = tree_->LoadTree(read_nevents_);
  if (treeEntry<0)
  {
    ERROR << "Unexpected end of the file !" << endmsg;
    return StatusCode::FAILURE;
  }
  read_nevents_++;

  // load Delphes data
  data_.GetEntry(treeEntry);

  // Fill MA5 dataformat
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
    if (data_.branchHT_==0) myEvent.rec()->THT_ += myEvent.rec()->jets_[i].pt();
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
  // ---------------------------------------------------------------------------
  // GenParticle collection
  // ---------------------------------------------------------------------------
  std::map<const GenParticle*,unsigned int> gentable;
  std::map<const GenParticle*,unsigned int>::const_iterator genit;
  if (data_.GenParticle_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(data_.GenParticle_->GetEntries());i++)
  {
    // getting the i-th particle
    GenParticle* part = dynamic_cast<GenParticle*>(data_.GenParticle_->At(i));
    if (part==0) continue;

    // filling the mapping table: pointer address <-> i-th
    gentable[part]=i;

    // creating new particle and filling particle info
    MCParticleFormat * gen = myEvent.mc()->GetNewParticle();
    gen->pdgid_      = part->PID;
    gen->statuscode_ = part->Status;
    gen->mothup1_    = part->M1;
    gen->mothup2_    = part->M2;
    gen->isPU_      = part->IsPU;
    gen->momentum_.SetPxPyPzE(part->Px,part->Py, part->Pz, part->E);
  }


  // ---------------------------------------------------------------------------
  // Fill electrons
  // ---------------------------------------------------------------------------
  if (data_.Electron_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(data_.Electron_->GetEntries());i++)
  {
    // getting the i-th particle
    Electron* part = dynamic_cast<Electron*>(data_.Electron_->At(i));
    if (part==0) continue;

    // creating new particle and filling particle info
    RecLeptonFormat * electron = myEvent.rec()->GetNewElectron();
    electron->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
    if (part->Charge>0) electron->charge_=true; else electron->charge_=false;
    electron->HEoverEE_ = part->EhadOverEem;

    // setting corresponding gen particle
    const GenParticle* mc = dynamic_cast<const GenParticle*>(part->Particle.GetObject());
    if (mc!=0)
    {
      genit = gentable.find(mc);
      if (genit!=gentable.end()) electron->mc_=&(myEvent.mc()->particles()[genit->second]);
      else WARNING << "GenParticle corresponding to an electron is not found in the gen table" << endmsg;
    }
    electron->delphesTags_.push_back(reinterpret_cast<ULong64_t>(mc));
  }


  // ---------------------------------------------------------------------------
  // Fill photons
  // ---------------------------------------------------------------------------
  if (data_.Photon_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(data_.Photon_->GetEntries());i++)
  {
    // getting the i-th particle
    Photon* part = dynamic_cast<Photon*>(data_.Photon_->At(i));
    if (part==0) continue;

    // creating new particle and filling particle info
    RecPhotonFormat * photon = myEvent.rec()->GetNewPhoton();
    photon->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
    photon->HEoverEE_ = part->EhadOverEem;

    // setting corresponding gen particle
    GenParticle* mc=0;
    for (unsigned int j=0;j<static_cast<UInt_t>(part->Particles.GetEntries());j++)
    {
      GenParticle* ref = dynamic_cast<GenParticle*>(part->Particles.At(j));
      if (ref==0) continue;
      if (mc==0) mc=ref;
      else if (mc->PT<ref->PT) mc=ref;
    }
    if (mc!=0)
    {
      photon->delphesTags_.push_back(reinterpret_cast<ULong64_t>(mc));
      genit = gentable.find(mc);
      if (genit!=gentable.end()) photon->mc_=&(myEvent.mc()->particles()[genit->second]);
      else WARNING << "GenParticle corresponding to a photon is not found in the gen table" << endmsg;
    }
  }


  // ---------------------------------------------------------------------------
  // Fill Event
  // ---------------------------------------------------------------------------
  if (data_.Event_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(data_.Event_->GetEntries());i++)
  {
    // Get the header 
    LHEFEvent* header1 =  dynamic_cast<LHEFEvent*>(data_.Event_->At(i));
    if (header1!=0)
    {
      // Set event-weight
      myEvent.mc()->setWeight(header1->Weight);
    }
    else
    {
      HepMCEvent* header2 = dynamic_cast<HepMCEvent*>(data_.Event_->At(i));
      if (header2==0) continue;
      // Set event-weight
      myEvent.mc()->setWeight(header2->Weight);
    }
  }



  // ---------------------------------------------------------------------------
  // Fill muons
  // ---------------------------------------------------------------------------
  if (data_.Muon_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(data_.Muon_->GetEntries());i++)
  {
    // getting the i-th particle
    Muon* part = dynamic_cast<Muon*>(data_.Muon_->At(i));
    if (part==0) continue;

    // creating new particle and filling particle info
    RecLeptonFormat * muon = myEvent.rec()->GetNewMuon();
    muon->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
    if (part->Charge>0) muon->charge_=true; else muon->charge_=false;

    // setting corresponding gen particle
    const GenParticle* mc = dynamic_cast<const GenParticle*>(part->Particle.GetObject());
    if (mc!=0)
    {
      genit = gentable.find(mc);
      if (genit!=gentable.end()) muon->mc_=&(myEvent.mc()->particles()[genit->second]);
      else WARNING << "GenParticle corresponding to a muon is not found in the gen table" << endmsg;
    }
    muon->delphesTags_.push_back(reinterpret_cast<ULong64_t>(mc));
  }


  // ---------------------------------------------------------------------------
  // Fill Tower
  // ---------------------------------------------------------------------------
  if (data_.Tower_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(data_.Tower_->GetEntries());i++)
  {
    // getting the i-th particle
    Tower* tower = dynamic_cast<Tower*>(data_.Tower_->At(i));
    if (tower==0) continue;

    // creating new tower and filling particle info
    RecTowerFormat * part = myEvent.rec()->GetNewTower();
    part->momentum_.SetPtEtaPhiM(tower->ET,tower->Eta,tower->Phi,0.0);

    // setting corresponding gen particle
    for (unsigned int j=0;j<static_cast<UInt_t>(tower->Particles.GetEntries());j++)
    {
       const GenParticle* mc = dynamic_cast<const GenParticle*>(tower->Particles[j]);
       //       if (mc!=0)
       //       {
       //         genit = gentable.find(mc);
       //         if (genit!=gentable.end()) track->mc_=&(myEvent.mc()->particles()[genit->second]);
       //         else WARNING << "GenParticle corresponding to a track is not found in the gen table" << endmsg;
       //       }

       // setting 
       part->delphesTags_.push_back(reinterpret_cast<ULong64_t>(mc));
    }
  }


  // ---------------------------------------------------------------------------
  // Fill EFlowTrack
  // ---------------------------------------------------------------------------
  if (data_.EFlowTrack_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(data_.EFlowTrack_->GetEntries());i++)
  {
    // getting the i-th particle
    Track* track = dynamic_cast<Track*>(data_.EFlowTrack_->At(i));
    if (track==0) continue;

    // creating new track and filling particle info
    RecTrackFormat * part = myEvent.rec()->GetNewEFlowTrack();
    part->momentum_.SetPtEtaPhiM(track->PT,track->Eta,track->Phi,0.0);

    // setting corresponding gen particle
    const GenParticle* mc = dynamic_cast<const GenParticle*>(track->Particle.GetObject());
    part->delphesTags_.push_back(reinterpret_cast<ULong64_t>(mc));

  }


  // ---------------------------------------------------------------------------
  // Fill EFlowPhotons
  // ---------------------------------------------------------------------------
  if (data_.EFlowPhoton_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(data_.EFlowPhoton_->GetEntries());i++)
  {
    // getting the i-th particle
    Tower* tower = dynamic_cast<Tower*>(data_.EFlowPhoton_->At(i));
    if (tower==0) continue;

    // creating new tower and filling particle info
    RecParticleFormat * part = myEvent.rec()->GetNewEFlowPhoton();
    part->momentum_.SetPtEtaPhiM(tower->ET,tower->Eta,tower->Phi,0.0);

    // setting corresponding gen particle
    /*    std::cout << "number of igen : " << tower->Particles.GetEntries() << std::endl;
    for (unsigned igen=0;igen<tower->Particles.GetEntries();igen++)
    {
      std::cout << tower->Particles.At(igen) << std::endl;
    }
    */
  }


  // ---------------------------------------------------------------------------
  // Fill EFlowNeutralHadrons
  // ---------------------------------------------------------------------------
  if (data_.EFlowNeutral_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(data_.EFlowNeutral_->GetEntries());i++)
  {
    // getting the i-th particle
    Tower* tower = dynamic_cast<Tower*>(data_.EFlowNeutral_->At(i));
    if (tower==0) continue;

    // creating new tower and filling particle info
    RecParticleFormat * part = myEvent.rec()->GetNewEFlowNeutralHadron();
    part->momentum_.SetPtEtaPhiM(tower->ET,tower->Eta,tower->Phi,0.0);

    // setting corresponding gen particle
    /*    std::cout << "number of igen : " << tower->Particles.GetEntries() << std::endl;
    for (unsigned igen=0;igen<tower->Particles.GetEntries();igen++)
    {
      std::cout << tower->Particles.At(igen) << std::endl;
    }
    */
  }


  // ---------------------------------------------------------------------------
  // Fill jets and taus
  // ---------------------------------------------------------------------------
  if (data_.Jet_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(data_.Jet_->GetEntries());i++)
  {
    // getting the i-th particle
    Jet* part = dynamic_cast<Jet*>(data_.Jet_->At(i));
    if (part==0) continue;

    // creating new tau
    if(part->TauTag==1)
    {
      RecTauFormat * tau = myEvent.rec()->GetNewTau();
      tau->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
      tau->ntracks_  = 0; // To fix later
      if (part->Charge>0) tau->charge_=true; else tau->charge_=false;
      tau->HEoverEE_ = part->EhadOverEem;
    }

    // creating new jet
    else
    {
      // Creating new jet
      RecJetFormat * jet = myEvent.rec()->GetNewJet();

      // Setting jet info
      jet->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
      jet->ntracks_  = 0; // To fix later
      jet->btag_     = part->BTag;
      jet->HEoverEE_ = part->EhadOverEem;

      // Setting corresponding gen particle
      /*      for (unsigned int j=0;j<static_cast<UInt_t>(part->Particles.GetEntries());j++)
      {
        GenParticle* ref = dynamic_cast<GenParticle*>(part->Particles.At(j));
        if (ref!=0)
        {
          genit = gentable.find(ref);
          if (genit!=gentable.end()) jet->Constituents_.push_back(genit->second);
          else WARNING << "GenParticle corresponding to a jet constituent is not found in the gen table" << endmsg;
        }
        }*/
    }
  }


  // ---------------------------------------------------------------------------
  // MET
  // ---------------------------------------------------------------------------
  if (data_.MET_!=0)
  if (data_.MET_->GetEntries()>0)
  {
    // getting the first particle
    MissingET* part = dynamic_cast<MissingET*>(data_.MET_->At(0));

    // filling MET info
    if (part!=0)
    {
      myEvent.rec()->MET_.momentum_.SetPx(part->MET*cos(part->Phi));
      myEvent.rec()->MET_.momentum_.SetPy(part->MET*sin(part->Phi));
      myEvent.rec()->MET_.momentum_.SetE (part->MET);
    }
  }


  // ---------------------------------------------------------------------------
  // THT
  // ---------------------------------------------------------------------------
  if (data_.HT_!=0)
  if (data_.HT_->GetEntries()>0)
  {
    // getting the first particle
    ScalarHT* part = dynamic_cast<ScalarHT*>(data_.HT_->At(0));

    // filling THT info
    if (part!=0)
    {
      myEvent.rec()->THT_=part->HT;
    }
  }


  // ---------------------------------------------------------------------------
  // Track collection
  // ---------------------------------------------------------------------------
  if (data_.Track_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(data_.Track_->GetEntries());i++)
  {
    // getting the i-th track
    Track* ref = dynamic_cast<Track*>(data_.Track_->At(i));
    if (ref==0) continue;

    // creating new track
    RecTrackFormat * track = myEvent.rec()->GetNewTrack();

    // filling track info
    track->pdgid_ = ref->PID;
    if (ref->Charge>0) track->charge_=true; else track->charge_=false;
    track->momentum_.SetPtEtaPhiE(ref->PT,ref->Eta,ref->Phi,ref->PT);
    track->etaOuter_ = ref->EtaOuter;
    track->phiOuter_ = ref->PhiOuter;

    // setting corresponding gen particle
    const GenParticle* mc = dynamic_cast<const GenParticle*>(ref->Particle.GetObject());
    if (mc!=0)
    {
      genit = gentable.find(mc);
      if (genit!=gentable.end()) track->mc_=&(myEvent.mc()->particles()[genit->second]);
      else WARNING << "GenParticle corresponding to a track is not found in the gen table" << endmsg;
    }

    // setting 
    track->delphesTags_.push_back(reinterpret_cast<ULong64_t>(mc));
  }

}

