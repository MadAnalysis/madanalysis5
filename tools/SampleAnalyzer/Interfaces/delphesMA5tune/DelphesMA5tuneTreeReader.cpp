////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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
#include "SampleAnalyzer/Interfaces/delphesMA5tune/DelphesMA5tuneTreeReader.h"
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
MAbool DelphesMA5tuneTreeReader::Initialize()
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
  branchElectron_  = treeReader_->UseBranch("DelphesMA5tuneElectron");
  if (branchElectron_==0)
  {
    WARNING << "Electron collection branch is not found" << endmsg;
  }
  branchPhoton_    = treeReader_->UseBranch("DelphesMA5tunePhoton");
  if (branchPhoton_==0)
  {
    WARNING << "Photon collection branch is not found" << endmsg;
  }
  branchMuon_      = treeReader_->UseBranch("DelphesMA5tuneMuon");
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
  branchTrack_ = treeReader_->UseBranch("DelphesMA5tuneTrack");
  if (branchTrack_==0)
  {
    WARNING << "Track branch is not found" << endmsg;
  }
  branchEvent_ = treeReader_->UseBranch("Event");
  if (branchEvent_==0)
  {
    WARNING << "Event branch is not found" << endmsg;
  }

  return true;
}


// -----------------------------------------------------------------------------
// ReadHeader
// -----------------------------------------------------------------------------
MAbool DelphesMA5tuneTreeReader::ReadHeader(SampleFormat& mySample)
{
  mySample.InitializeRec();
  mySample.SetSampleFormat(MA5FORMAT::DELPHESMA5TUNE);
  mySample.SetSampleGenerator(MA5GEN::DELPHESMA5TUNE);
  return true;
}


// -----------------------------------------------------------------------------
// ReadEvent
// -----------------------------------------------------------------------------
StatusCode::Type DelphesMA5tuneTreeReader::ReadEvent(EventFormat& myEvent, SampleFormat& mySample)
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
MAbool DelphesMA5tuneTreeReader::FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)
{
  // MHT & THT
  for (MAuint32 i=0; i<myEvent.rec()->jets_.size();i++)
  {
    myEvent.rec()->MHT_ -= myEvent.rec()->jets_[i].momentum();
    if (branchScalarHT_==0) myEvent.rec()->THT_ += myEvent.rec()->jets_[i].pt();
    myEvent.rec()->TET_ += myEvent.rec()->jets_[i].pt();
    myEvent.rec()->Meff_ += myEvent.rec()->jets_[i].pt();
  }

  // TET
  for (MAuint32 i=0; i<myEvent.rec()->muons_.size();i++)
  {
    myEvent.rec()->TET_ += myEvent.rec()->muons_[i].pt();
  }
  for (MAuint32 i=0; i<myEvent.rec()->electrons_.size();i++)
  {
    myEvent.rec()->TET_ += myEvent.rec()->electrons_[i].pt();
  }
  for (MAuint32 i=0; i<myEvent.rec()->taus_.size();i++)
  {
    myEvent.rec()->TET_ += myEvent.rec()->taus_[i].pt();
  }
  for (MAuint32 i=0; i<myEvent.rec()->photons_.size();i++)
  {
    myEvent.rec()->TET_ += myEvent.rec()->photons_[i].pt();
  }

  // Finalize MHT
  myEvent.rec()->MHT_.momentum().SetPz(0.);
  myEvent.rec()->MHT_.momentum().SetE(myEvent.rec()->MHT_.momentum().Pt());


  // Compute transverse observable
  for (MAuint32 i=0; i<myEvent.mc()->particles_.size();i++)
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
        myEvent.mc()->Meff_ += part.pt();
      }
    }

    /*  
    MAbool debug=false;
    MAint32 index1=myEvent.mc()->particles_[i].mothup1_;
    MAint32 index2=myEvent.mc()->particles_[i].mothup2_;
    if (debug)  std::cout << "index1=" << index1 << "\tindex2=" << index2 << std::endl;
    if (index1!=0)
    {
      if (MA5index.find(index1)!=MA5index.end())
      {
        myEvent.mc()->particles_[i].mother1_ = &myEvent.mc()->particles_[MA5index[index1]];
        myEvent.mc()->particles_[MA5index[index1]].daughters_.push_back(&myEvent.mc()->particles_[i]);
      }
      else
      {
        if (debug) std::cout << "NOT FOUND" << std::endl;
      }
    }
    if (index2!=0)
    {
      if (MA5index.find(index2)!=MA5index.end())
      {
        myEvent.mc()->particles_[i].mother2_ = &myEvent.mc()->particles_[MA5index[index2]];
        myEvent.mc()->particles_[MA5index[index2]].daughters_.push_back(&myEvent.mc()->particles_[i]);
      }
    }
    */
  }


  // Finalize event
  myEvent.mc()->MET_.momentum().SetPz(0.);
  myEvent.mc()->MET_.momentum().SetE(myEvent.mc()->MET_.momentum().Pt());
  myEvent.mc()->MHT_.momentum().SetPz(0.);
  myEvent.mc()->MHT_.momentum().SetE(myEvent.mc()->MHT_.momentum().Pt());

  // Effective mass
  myEvent.rec()->Meff_ += myEvent.rec()->MET_.pt();
  myEvent.mc()->Meff_  += myEvent.mc()->MET_.pt();


          /*
  // Link muons with MC particles
  for (MAuint32 i=0;i<myEvent.rec()->muons().size();i++)
  {
    RecLeptonFormat& part =myEvent.rec()->muons()[i];
    if (MuonIndex_[i]<0) continue;
    part.setMc(&myEvent.mc()->particles_[MA5index[MuonIndex_[i]]]);
  }

  // Link electrons with MC particles
  for (MAuint32 i=0;i<myEvent.rec()->electrons().size();i++)
  {
    RecLeptonFormat& part = myEvent.rec()->electrons()[i];
    if (ElectronIndex_[i]<0) continue;
    part.setMc(&myEvent.mc()->particles_[MA5index[ElectronIndex_[i]]]);
  }

  */
  // Normal end
  return true; 
}




// -----------------------------------------------------------------------------
// FillEventParticleLine
// -----------------------------------------------------------------------------
void DelphesMA5tuneTreeReader::FillEvent(EventFormat& myEvent, SampleFormat& mySample)
{
  MuonIndex_.clear();
  ElectronIndex_.clear();

  // Fill electrons
  if (branchElectron_!=0)
  for (MAuint32 i=0;i<static_cast<MAuint32>(branchElectron_->GetEntries());i++)
  {
    Electron* part = dynamic_cast<Electron*>(branchElectron_->At(i));
    RecLeptonFormat * electron = myEvent.rec()->GetNewElectron();
    electron->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
    if (part->Charge>0) electron->charge_=true; else electron->charge_=false;
    electron->HEoverEE_ = part->EhadOverEem;

    IsolationConeType* isolcone05 = electron->GetNewIsolCone();
    isolcone05->sumPT_   = part->sumPT05;
    isolcone05->sumET_   = part->sumET05;
    isolcone05->ntracks_ = part->nTrack05;
    isolcone05->eflow_sumPT_ = part->sumPTeflow05;
    isolcone05->deltaR_  = 0.5;

    IsolationConeType* isolcone04 = electron->GetNewIsolCone();
    isolcone04->sumPT_   = part->sumPT04;
    isolcone04->sumET_   = part->sumET04;
    isolcone04->ntracks_ = part->nTrack04;
    isolcone04->eflow_sumPT_ = part->sumPTeflow04;
    isolcone04->deltaR_  = 0.4;

    IsolationConeType* isolcone03 = electron->GetNewIsolCone();
    isolcone03->sumPT_   = part->sumPT03;
    isolcone03->sumET_   = part->sumET03;
    isolcone03->ntracks_ = part->nTrack03;
    isolcone03->eflow_sumPT_ = part->sumPTeflow03;
    isolcone03->deltaR_  = 0.3;

    IsolationConeType* isolcone02 = electron->GetNewIsolCone();
    isolcone02->sumPT_   = part->sumPT02;
    isolcone02->sumET_   = part->sumET02;
    isolcone02->ntracks_ = part->nTrack02;
    isolcone02->eflow_sumPT_ = part->sumPTeflow02;
    isolcone02->deltaR_  = 0.2;

    MAint32 index=0;
    if (part->MA5index>=0) index=part->MA5index+1;
    ElectronIndex_.push_back(index);
  }

  // Fill photons
  if (branchPhoton_!=0)
  for (MAuint32 i=0;i<static_cast<MAuint32>(branchPhoton_->GetEntries());i++)
  {
    Photon* part = dynamic_cast<Photon*>(branchPhoton_->At(i));
    RecPhotonFormat * photon = myEvent.rec()->GetNewPhoton();
    photon->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
    photon->HEoverEE_ = part->EhadOverEem;

    IsolationConeType* isolcone05 = photon->GetNewIsolCone();
    isolcone05->sumPT_   = part->sumPT05;
    isolcone05->sumET_   = part->sumET05;
    isolcone05->ntracks_ = part->nTrack05;
    isolcone05->eflow_sumPT_ = part->sumPTeflow05;
    isolcone05->deltaR_  = 0.5;

    IsolationConeType* isolcone04 = photon->GetNewIsolCone();
    isolcone04->sumPT_   = part->sumPT04;
    isolcone04->sumET_   = part->sumET04;
    isolcone04->ntracks_ = part->nTrack04;
    isolcone04->eflow_sumPT_ = part->sumPTeflow04;
    isolcone04->deltaR_  = 0.4;

    IsolationConeType* isolcone03 = photon->GetNewIsolCone();
    isolcone03->sumPT_   = part->sumPT03;
    isolcone03->sumET_   = part->sumET03;
    isolcone03->ntracks_ = part->nTrack03;
    isolcone03->eflow_sumPT_ = part->sumPTeflow03;
    isolcone03->deltaR_  = 0.3;

    IsolationConeType* isolcone02 = photon->GetNewIsolCone();
    isolcone02->sumPT_   = part->sumPT02;
    isolcone02->sumET_   = part->sumET02;
    isolcone02->ntracks_ = part->nTrack02;
    isolcone02->eflow_sumPT_ = part->sumPTeflow02;
    isolcone02->deltaR_  = 0.2;

  }

  if (branchMuon_!=0)
  for (MAuint32 i=0;i<static_cast<MAuint32>(branchMuon_->GetEntries());i++)
  {
    Muon* part = dynamic_cast<Muon*>(branchMuon_->At(i));
    RecLeptonFormat * muon = myEvent.rec()->GetNewMuon();
    muon->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
    if (part->Charge>0) muon->charge_=true; else muon->charge_=false;

    IsolationConeType* isolcone05 = muon->GetNewIsolCone();
    isolcone05->sumPT_   = part->sumPT05;
    isolcone05->sumET_   = part->sumET05;
    isolcone05->ntracks_ = part->nTrack05;
    isolcone05->eflow_sumPT_ = part->sumPTeflow05;
    isolcone05->deltaR_  = 0.5;

    IsolationConeType* isolcone04 = muon->GetNewIsolCone();
    isolcone04->sumPT_   = part->sumPT04;
    isolcone04->sumET_   = part->sumET04;
    isolcone04->ntracks_ = part->nTrack04;
    isolcone04->eflow_sumPT_ = part->sumPTeflow04;
    isolcone04->deltaR_  = 0.4;

    IsolationConeType* isolcone03 = muon->GetNewIsolCone();
    isolcone03->sumPT_   = part->sumPT03;
    isolcone03->sumET_   = part->sumET03;
    isolcone03->ntracks_ = part->nTrack03;
    isolcone03->eflow_sumPT_ = part->sumPTeflow03;
    isolcone03->deltaR_  = 0.3;

    IsolationConeType* isolcone02 = muon->GetNewIsolCone();
    isolcone02->sumPT_   = part->sumPT02;
    isolcone02->sumET_   = part->sumET02;
    isolcone02->ntracks_ = part->nTrack02;
    isolcone02->eflow_sumPT_ = part->sumPTeflow02;
    isolcone02->deltaR_  = 0.2;

    MAint32 index=0;
    if (part->MA5index>=0) index=part->MA5index+1;
    MuonIndex_.push_back(index);
  }

  // ---------------------------------------------------------------------------
  // Fill Event
  // ---------------------------------------------------------------------------
  if (branchEvent_!=0)
  {
    for (MAuint32 i=0;i<static_cast<MAuint32>(branchEvent_->GetEntries());i++)
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
  }

  // Fill jets and taus
  if (branchJet_!=0)
  for (MAuint32 i=0;i<static_cast<MAuint32>(branchJet_->GetEntries());i++)
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
  {
    // Number of generated particles
    MAuint32 nparts = static_cast<MAuint32>(branchGenParticle_->GetEntries());
    myEvent.mc()->particles_.reserve(nparts);

    // Temporary vector for mother-daughter relations
    std::vector<std::pair<MAint32,MAint32> > mothers(nparts);

    for (MAuint32 i=0;i<nparts;i++)
    {
      GenParticle* part = dynamic_cast<GenParticle*>(branchGenParticle_->At(i));
      MCParticleFormat * gen = myEvent.mc()->GetNewParticle();
      gen->pdgid_      = part->PID;
      gen->statuscode_ = part->Status;
      gen->momentum_.SetPxPyPzE(part->Px,part->Py, part->Pz, part->E);

      // Filling mother-daughter relations
      mothers[i]=std::make_pair(part->M1,part->M2);
      /*
      // conventions: part->M1=0,1,2,3,...
      // 0 -> no mother
      // 1 -> first line
      if (part->M1<0) gen->mothup1_ = 0;
      else gen->mothup1_ = static_cast<MAuint32>(part->M1)+1;
      if (part->M2<0) gen->mothup2_ = 0;
      else gen->mothup2_ = static_cast<MAuint32>(part->M2)+1;
      if (part->MA5index<0) gen->extra1_ = 0;
      else gen->extra1_ = part->MA5index+1;

      MAbool debug=false;
      if (debug)
      {
        std::cout << "DEBUG i=" << i+1 << "\t" << gen->pdgid_ 
                  << "[" << gen->statuscode_ << "]\tMA5index=" << gen->extra1_ 
                  << "\tM1=" << gen->mothup1_ << "\tM2=" << gen->mothup2_ << std::endl;
      }
      */
    }

    // Setting the mother-daughter relations
    for (MAuint32 i=0;i<myEvent.mc()->particles_.size();i++)
    {
      MCParticleFormat* part = &(myEvent.mc()->particles_[i]);
      std::vector<MAint32> mother_indices(2,0);
      mother_indices[0]=mothers[i].first;
      mother_indices[1]=mothers[i].second;
      for (MAuint32 index=0;index<mother_indices.size();index++)
      {
        // No mother case : index=-1
        if (mother_indices[index]<0) continue;

        // Check if the mother has been registered
        MAbool redundant=false;
        for (MAuint32 index2=0;index2<index;index2++)
        {
          if (mother_indices[index]==mother_indices[index2])
          {
            redundant=true;
            break;
          }
        }
        if (redundant) continue;

        // Set the mothers and the daughters
        MAuint32 newindex = static_cast<MAuint32>(mother_indices[index]);
        if (newindex < myEvent.mc()->particles().size())
        {
          MCParticleFormat* mum = &(myEvent.mc()->particles()[newindex]);
          if (part!=mum)
          {
            mum->daughters().push_back(part);
            part->mothers().push_back(mum);
          }
        } 
        else
        {
          WARNING << "A mother particle is defined after the given particle. ";
          WARNING << " This daughter-mother link will be skipped" << endmsg;
        }
      }
    }
  }

  // Track collection
  if (branchTrack_!=0)
  for (MAuint32 i=0;i<static_cast<MAuint32>(branchTrack_->GetEntries());i++)
  {
    Track* ref = dynamic_cast<Track*>(branchTrack_->At(i));
    RecTrackFormat * track = myEvent.rec()->GetNewTrack();
    track->pdgid_ = ref->PID;
    if (ref->Charge>0) track->charge_=true; else track->charge_=false;
    track->momentum_.SetPtEtaPhiE(ref->PT,ref->Eta,ref->Phi,ref->PT);
    track->etaOuter_ = ref->EtaOuter;
    track->phiOuter_ = ref->PhiOuter;

    IsolationConeType* isolcone05 = track->GetNewIsolCone();
    isolcone05->sumPT_   = ref->sumPT05;
    isolcone05->sumET_   = ref->sumET05;
    isolcone05->ntracks_ = ref->nTrack05;
    isolcone05->eflow_sumPT_ = ref->sumPTeflow05;
    isolcone05->deltaR_  = 0.5;

    IsolationConeType* isolcone04 = track->GetNewIsolCone();
    isolcone04->sumPT_   = ref->sumPT04;
    isolcone04->sumET_   = ref->sumET04;
    isolcone04->ntracks_ = ref->nTrack04;
    isolcone04->eflow_sumPT_ = ref->sumPTeflow04;
    isolcone04->deltaR_  = 0.4;

    IsolationConeType* isolcone03 = track->GetNewIsolCone();
    isolcone03->sumPT_   = ref->sumPT03;
    isolcone03->sumET_   = ref->sumET03;
    isolcone03->ntracks_ = ref->nTrack03;
    isolcone03->eflow_sumPT_ = ref->sumPTeflow03;
    isolcone03->deltaR_  = 0.3;

    IsolationConeType* isolcone02 = track->GetNewIsolCone();
    isolcone02->sumPT_   = ref->sumPT02;
    isolcone02->sumET_   = ref->sumET02;
    isolcone02->ntracks_ = ref->nTrack02;
    isolcone02->eflow_sumPT_ = ref->sumPTeflow02;
    isolcone02->deltaR_  = 0.2;
  }
}


