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
MAbool DelphesTreeReader::Initialize()
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
MAbool DelphesTreeReader::ReadHeader(SampleFormat& mySample)
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
  MAint32 treeEntry = tree_->LoadTree(read_nevents_);
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
MAbool DelphesTreeReader::FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)
{
  // MHT & THT
  for (MAuint32 i=0; i<myEvent.rec()->jets_.size();i++)
  {
    myEvent.rec()->MHT_ -= myEvent.rec()->jets_[i].momentum();
    if (data_.branchHT_==0) myEvent.rec()->THT_ += myEvent.rec()->jets_[i].pt();
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

    /*    MAuint32 index1=myEvent.mc()->particles_[i].mothup1_;
    MAuint32 index2=myEvent.mc()->particles_[i].mothup2_;
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

  // Effective mass
  myEvent.rec()->Meff_ += myEvent.rec()->MET_.pt();
  myEvent.mc()->Meff_  += myEvent.mc()->MET_.pt();

  // Normal end
  return true; 
}




// -----------------------------------------------------------------------------
// FillEventParticleLine
// -----------------------------------------------------------------------------
void DelphesTreeReader::FillEvent(EventFormat& myEvent, SampleFormat& mySample)
{

  // ---------------------------------------------------------------------------
  // Weight
  // ---------------------------------------------------------------------------
  if (data_.Weight_!=0)
  {
    // Number of generated particles
    MAuint32 nweights = static_cast<MAuint32>(data_.Weight_->GetEntries());

    for (MAuint32 i=0;i<nweights;i++)
    {
      // getting the i-th particle
      LHEFWeight* weight = dynamic_cast<LHEFWeight*>(data_.Weight_->At(i));
      if (weight==0) continue;

      // creating new particle and filling particle info
      myEvent.mc()->multiweights().Add(weight->ID,weight->Weight);
    }
  }

  // ---------------------------------------------------------------------------
  // GenParticle collection
  // ---------------------------------------------------------------------------
  std::map<const GenParticle*,MAuint32> gentable;
  std::map<const GenParticle*,MAuint32>::const_iterator genit;
  if (data_.GenParticle_!=0)
  {
    // Number of generated particles
    MAuint32 nparts = static_cast<MAuint32>(data_.GenParticle_->GetEntries());
    myEvent.mc()->particles_.reserve(nparts);

    // Temporary vector for mother-daughter relations
    std::vector<std::pair<MAint32,MAint32> > mothers(nparts);

    // Loop over the generated particles
    for (MAuint32 i=0;i<nparts;i++)
    {
      // Getting the i-th particle
      GenParticle* part = dynamic_cast<GenParticle*>(data_.GenParticle_->At(i));
      if (part==0) continue;

      // Filling the mapping table: pointer address <-> i-th
      gentable[part]=i;

      // Creating a new particle
      MCParticleFormat * gen = myEvent.mc()->GetNewParticle();

      // Filling main infos
      gen->momentum_.SetPxPyPzE(part->Px,part->Py, part->Pz, part->E);
      gen->pdgid_      = part->PID;
      gen->statuscode_ = part->Status;
      gen->isPU_       = part->IsPU;

      // Filling mother-daughter relations
      mothers[i]=std::make_pair(part->M1,part->M2);
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

  // ---------------------------------------------------------------------------
  // Fill electrons
  // ---------------------------------------------------------------------------
  if (data_.Electron_!=0)
  {
    MAuint32 nelectrons = static_cast<MAuint32>(data_.Electron_->GetEntries());
    myEvent.rec()->electrons_.reserve(nelectrons);
    for (MAuint32 i=0;i<nelectrons;i++)
    {
      // getting the i-th particle
      Electron* part = dynamic_cast<Electron*>(data_.Electron_->At(i));
      if (part==0) continue;

      // creating new particle and filling particle info
      RecLeptonFormat * electron = myEvent.rec()->GetNewElectron();
      electron->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
      if (part->Charge>0) electron->charge_=true; else electron->charge_=false;
      electron->HEoverEE_ = part->EhadOverEem;
      electron->d0_       = part->D0;
      electron->d0error_  = part->ErrorD0;
      electron->dz_       = part->DZ;
      electron->dzerror_  = part->ErrorDZ;
      //      electron->vertex_prod_.SetPxPyPzE(part->Xp, part->Yp, part->Zp, part->Tp);
      //      electron->closest_point_.SetPxPyPzE(part->Xd, part->Yd, part->Zd, 0.);

      // setting corresponding gen particle
      const GenParticle* mc = dynamic_cast<const GenParticle*>(part->Particle.GetObject());
      if (mc!=0)
      {
        genit = gentable.find(mc);
        if (genit!=gentable.end()) electron->mc_=&(myEvent.mc()->particles()[genit->second]);
        else WARNING << "GenParticle corresponding to an electron is not found in the gen table" << endmsg;
      }
      electron->delphesTags_.push_back(reinterpret_cast<MAuint64>(mc));
    }
  }

  // ---------------------------------------------------------------------------
  // Fill photons
  // ---------------------------------------------------------------------------
  if (data_.Photon_!=0)
  {
    MAuint32 nphotons = static_cast<MAuint32>(data_.Photon_->GetEntries());
    myEvent.rec()->photons_.reserve(nphotons);
    for (MAuint32 i=0;i<nphotons;i++)
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
      for (MAuint32 j=0;j<static_cast<MAuint32>(part->Particles.GetEntries());j++)
      {
        GenParticle* ref = dynamic_cast<GenParticle*>(part->Particles.At(j));
        if (ref==0) continue;
        if (mc==0) mc=ref;
        else if (mc->PT<ref->PT) mc=ref;
      }
      if (mc!=0)
      {
        photon->delphesTags_.push_back(reinterpret_cast<MAuint64>(mc));
        genit = gentable.find(mc);
        if (genit!=gentable.end()) photon->mc_=&(myEvent.mc()->particles()[genit->second]);
        else WARNING << "GenParticle corresponding to a photon is not found in the gen table" << endmsg;
      }
    }
  }             

  // ---------------------------------------------------------------------------
  // Fill Event
  // ---------------------------------------------------------------------------
  if (data_.Event_!=0)
  {
    for (MAuint32 i=0;i<static_cast<MAuint32>(data_.Event_->GetEntries());i++)
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
  }


  // ---------------------------------------------------------------------------
  // Fill muons
  // ---------------------------------------------------------------------------
  if (data_.Muon_!=0)
  {
    MAuint32 nmuons = static_cast<MAuint32>(data_.Muon_->GetEntries());
    myEvent.rec()->muons_.reserve(nmuons);
    for (MAuint32 i=0;i<nmuons;i++)
    {
      // getting the i-th particle
      Muon* part = dynamic_cast<Muon*>(data_.Muon_->At(i));
      if (part==0) continue;

      // creating new particle and filling particle info
      RecLeptonFormat * muon = myEvent.rec()->GetNewMuon();
      muon->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,0.0);
      if (part->Charge>0) muon->charge_=true; else muon->charge_=false;

      muon->d0_      = part->D0;
      muon->d0error_ = part->ErrorD0;
      muon->dz_      = part->DZ;
      muon->dzerror_ = part->ErrorDZ;
      //      muon->vertex_prod_.SetPxPyPzE(part->Xp, part->Yp, part->Zp, part->Tp);
      //      muon->closest_point_.SetPxPyPzE(part->Xd, part->Yd, part->Zd, 0.);

      // setting corresponding gen particle
      const GenParticle* mc = dynamic_cast<const GenParticle*>(part->Particle.GetObject());
      if (mc!=0)
      {
        genit = gentable.find(mc);
        if (genit!=gentable.end()) muon->mc_=&(myEvent.mc()->particles()[genit->second]);
        else WARNING << "GenParticle corresponding to a muon is not found in the gen table" << endmsg;
      }
      muon->delphesTags_.push_back(reinterpret_cast<MAuint64>(mc));
    }
  }

  // ---------------------------------------------------------------------------
  // Fill Tower
  // ---------------------------------------------------------------------------
  if (data_.Tower_!=0)
  {
    MAuint32 ntowers = static_cast<MAuint32>(data_.Tower_->GetEntries());
    myEvent.rec()->towers_.reserve(ntowers);
    for (MAuint32 i=0;i<ntowers;i++)
    {
      // getting the i-th particle
      Tower* tower = dynamic_cast<Tower*>(data_.Tower_->At(i));
      if (tower==0) continue;

      // creating new tower and filling particle info
      RecTowerFormat * part = myEvent.rec()->GetNewTower();
      part->momentum_.SetPtEtaPhiM(tower->ET,tower->Eta,tower->Phi,0.0);

      // setting corresponding gen particle
      for (MAuint32 j=0;j<static_cast<MAuint32>(tower->Particles.GetEntries());j++)
      {
        const GenParticle* mc = dynamic_cast<const GenParticle*>(tower->Particles[j]);
        //       if (mc!=0)
        //       {
        //         genit = gentable.find(mc);
        //         if (genit!=gentable.end()) track->mc_=&(myEvent.mc()->particles()[genit->second]);
        //         else WARNING << "GenParticle corresponding to a track is not found in the gen table" << endmsg;
        //       }

        // setting 
        part->delphesTags_.push_back(reinterpret_cast<MAuint64>(mc));
      }
    }
  }

  // ---------------------------------------------------------------------------
  // Fill EFlowTrack
  // ---------------------------------------------------------------------------
  if (data_.EFlowTrack_!=0)
  {
    MAuint32 neflowtracks = static_cast<MAuint32>(data_.EFlowTrack_->GetEntries());
    myEvent.rec()->EFlowTracks_.reserve(neflowtracks);
    for (MAuint32 i=0;i<neflowtracks;i++)
    {
      // getting the i-th particle
      Track* track = dynamic_cast<Track*>(data_.EFlowTrack_->At(i));
      if (track==0) continue;

      // creating new track and filling particle info
      RecTrackFormat * part = myEvent.rec()->GetNewEFlowTrack();
      part->momentum_.SetPtEtaPhiM(track->PT,track->Eta,track->Phi,0.0);

      // setting corresponding gen particle
      const GenParticle* mc = dynamic_cast<const GenParticle*>(track->Particle.GetObject());
      part->delphesTags_.push_back(reinterpret_cast<MAuint64>(mc));

    }
  }

  // ---------------------------------------------------------------------------
  // Fill EFlowPhotons
  // ---------------------------------------------------------------------------
  if (data_.EFlowPhoton_!=0)
  {
    MAuint32 neflowphotons = static_cast<MAuint32>(data_.EFlowPhoton_->GetEntries());
    myEvent.rec()->EFlowPhotons_.reserve(neflowphotons);
    for (MAuint32 i=0;i<neflowphotons;i++)
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
  }

  // ---------------------------------------------------------------------------
  // Fill EFlowNeutralHadrons
  // ---------------------------------------------------------------------------
  if (data_.EFlowNeutral_!=0)
  {
    MAuint32 neflowneutrals = static_cast<MAuint32>(data_.EFlowNeutral_->GetEntries());
    myEvent.rec()->EFlowNeutralHadrons_.reserve(neflowneutrals);
    for (MAuint32 i=0;i<neflowneutrals;i++)
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
  }

  // ---------------------------------------------------------------------------
  // Fill jets and taus
  // ---------------------------------------------------------------------------
  if (data_.Jet_!=0)
  {
    MAuint32 njets = static_cast<MAuint32>(data_.Jet_->GetEntries());
    myEvent.rec()->jets_.reserve(njets);
    for (MAuint32 i=0;i<njets;i++)
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
        jet->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,part->Mass);
        jet->ntracks_  = 0; // To fix later
        jet->btag_     = part->BTag;
        jet->HEoverEE_ = part->EhadOverEem;

        // Setting corresponding gen particle
        /*      for (MAuint32 j=0;j<static_cast<MAuint32>(part->Particles.GetEntries());j++)
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
  }

  // ---------------------------------------------------------------------------
  // Fill fat jets
  // ---------------------------------------------------------------------------
  if (data_.FatJet_!=0)
  {
    MAuint32 njets = static_cast<MAuint32>(data_.FatJet_->GetEntries());
    myEvent.rec()->fatjets_.reserve(njets);
    for (MAuint32 i=0;i<njets;i++)
    {
      // getting the i-th particle
      Jet* part = dynamic_cast<Jet*>(data_.FatJet_->At(i));
      if (part==0) continue;

      // Creating new jet
      RecJetFormat * jet = myEvent.rec()->GetNewFatJet();

      // Setting jet info
      jet->momentum_.SetPtEtaPhiM(part->PT,part->Eta,part->Phi,part->Mass);
      jet->ntracks_  = 0; // To fix later
      jet->btag_     = part->BTag;
      jet->HEoverEE_ = part->EhadOverEem;
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
  {
    MAuint32 ntracks = static_cast<MAuint32>(data_.Track_->GetEntries());
    myEvent.rec()->tracks_.reserve(ntracks);
    for (MAuint32 i=0;i<ntracks;i++)
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
      track->delphesTags_.push_back(reinterpret_cast<MAuint64>(mc));
    }
  }

}

  
