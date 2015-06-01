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
  branchElectronMA5_  = treeReader_->UseBranch("ElectronMA5");
  if (branchElectronMA5_==0)
  {
    WARNING << "ElectronMA5 collection branch is not found" << endmsg;
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
  branchMuonMA5_      = treeReader_->UseBranch("MuonMA5");
  if (branchMuonMA5_==0)
  {
    WARNING << "MuonMA5 collection branch is not found" << endmsg;
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
  branchTower_ = treeReader_->UseBranch("Tower");
  if (branchTower_==0)
  {
    WARNING << "Tower branch is not found" << endmsg;
  }
  branchEFlowTracks_ = treeReader_->UseBranch("EFlowTrack");
  if (branchEFlowTracks_==0)
  {
    WARNING << "EFlowTracks branch is not found" << endmsg;
  }
  branchEFlowPhotons_ = treeReader_->UseBranch("EFlowPhoton");
  if (branchEFlowPhotons_==0)
  {
    WARNING << "EFlowPhotons branch is not found" << endmsg;
  }
  branchEFlowNeutralHadrons_ = treeReader_->UseBranch("EFlowNeutralHadron");
  if (branchEFlowNeutralHadrons_==0)
  {
    WARNING << "EFlowNeutralHadrons branch is not found" << endmsg;
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
  branchEvent_ = treeReader_->UseBranch("Event");
  if (branchEvent_==0)
  {
    WARNING << "Event branch is not found" << endmsg;
  }

  // DelphesMA5 tune mode
  if (branchMuonMA5_!=0 && branchElectronMA5_!=0)
  {
    INFO << "MA5-Tune root file found" << endmsg;
  }
  else
  {
    INFO << "Traditionnal Delphes root file found" << endmsg;
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
  // ---------------------------------------------------------------------------
  // GenParticle collection
  // ---------------------------------------------------------------------------
  std::map<const GenParticle*,unsigned int> gentable;
  std::map<const GenParticle*,unsigned int>::const_iterator genit;
  if (branchGenParticle_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchGenParticle_->GetEntries());i++)
  {
    // getting the i-th particle
    GenParticle* part = dynamic_cast<GenParticle*>(branchGenParticle_->At(i));
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
  if (branchElectron_!=0 && branchElectronMA5_==0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchElectron_->GetEntries());i++)
  {
    // getting the i-th particle
    Electron* part = dynamic_cast<Electron*>(branchElectron_->At(i));
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
  // Fill electrons MA5
  // ---------------------------------------------------------------------------
  if (branchElectronMA5_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchElectronMA5_->GetEntries());i++)
  {
    // getting the i-th particle
    Electron* part = dynamic_cast<Electron*>(branchElectronMA5_->At(i));
    if (part==0) continue;

    // creating new particle and filling particle info
    RecLeptonFormat * electron = myEvent.rec()->GetNewElectron();
    electron->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
    if (part->Charge>0) electron->charge_=true; else electron->charge_=false;

    // setting corresponding gen particle
    const GenParticle* mc = dynamic_cast<const GenParticle*>(part->Particle.GetObject());
    if (mc!=0)
    {
      genit = gentable.find(mc);
      if (genit!=gentable.end()) electron->mc_=&(myEvent.mc()->particles()[genit->second]);
      else WARNING << "GenParticle corresponding to a electron is not found in the gen table" << endmsg;
    }
    electron->delphesTags_.push_back(reinterpret_cast<ULong64_t>(mc));
  }


  // ---------------------------------------------------------------------------
  // Fill photons
  // ---------------------------------------------------------------------------
  if (branchPhoton_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchPhoton_->GetEntries());i++)
  {
    // getting the i-th particle
    Photon* part = dynamic_cast<Photon*>(branchPhoton_->At(i));
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
  if (branchEvent_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchEvent_->GetEntries());i++)
  {
    // Get the header 
    LHEFEvent* header1 =  dynamic_cast<LHEFEvent*>(branchEvent_->At(i));
    if (header1!=0)
    {
      // Set event-weight
      myEvent.mc()->setWeight(header1->Weight);
    }
    else
    {
      HepMCEvent* header2 = dynamic_cast<HepMCEvent*>(branchEvent_->At(i));
      if (header2==0) continue;
      // Set event-weight
      myEvent.mc()->setWeight(header2->Weight);
    }
  }



  // ---------------------------------------------------------------------------
  // Fill muons
  // ---------------------------------------------------------------------------
  if (branchMuon_!=0 && branchMuonMA5_==0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchMuon_->GetEntries());i++)
  {
    // getting the i-th particle
    Muon* part = dynamic_cast<Muon*>(branchMuon_->At(i));
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
  // Fill muons MA5
  // ---------------------------------------------------------------------------
  if (branchMuonMA5_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchMuonMA5_->GetEntries());i++)
  {
    // getting the i-th particle
    Muon* part = dynamic_cast<Muon*>(branchMuonMA5_->At(i));
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
  if (branchTower_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchTower_->GetEntries());i++)
  {
    // getting the i-th particle
    Tower* tower = dynamic_cast<Tower*>(branchTower_->At(i));
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
  if (branchEFlowTracks_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchEFlowTracks_->GetEntries());i++)
  {
    // getting the i-th particle
    Track* track = dynamic_cast<Track*>(branchEFlowTracks_->At(i));
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
  if (branchEFlowPhotons_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchEFlowPhotons_->GetEntries());i++)
  {
    // getting the i-th particle
    Tower* tower = dynamic_cast<Tower*>(branchEFlowPhotons_->At(i));
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
  if (branchEFlowNeutralHadrons_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchEFlowNeutralHadrons_->GetEntries());i++)
  {
    // getting the i-th particle
    Tower* tower = dynamic_cast<Tower*>(branchEFlowNeutralHadrons_->At(i));
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
  if (branchJet_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchJet_->GetEntries());i++)
  {
    // getting the i-th particle
    Jet* part = dynamic_cast<Jet*>(branchJet_->At(i));
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
  if (branchMissingET_!=0)
  if (branchMissingET_->GetEntries()>0)
  {
    // getting the first particle
    MissingET* part = dynamic_cast<MissingET*>(branchMissingET_->At(0));

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
  if (branchScalarHT_!=0)
  if (branchScalarHT_->GetEntries()>0)
  {
    // getting the first particle
    ScalarHT* part = dynamic_cast<ScalarHT*>(branchScalarHT_->At(0));

    // filling THT info
    if (part!=0)
    {
      myEvent.rec()->THT_=part->HT;
    }
  }


  // ---------------------------------------------------------------------------
  // Track collection
  // ---------------------------------------------------------------------------
  if (branchTrack_!=0)
  for (unsigned int i=0;i<static_cast<UInt_t>(branchTrack_->GetEntries());i++)
  {
    // getting the i-th track
    Track* ref = dynamic_cast<Track*>(branchTrack_->At(i));
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

