////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <http://madanalysis.irmp.ucl.ac.be>
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
#ifdef FAC_USE
#include "SampleAnalyzer/Reader/ROOTReader.h"
#endif
#include "SampleAnalyzer/Service/LogService.h"

// ROOT headers
#include <TROOT.h>


#ifdef FAC_USE

using namespace MA5;

// -----------------------------------------------------------------------------
// ReadHeader
// -----------------------------------------------------------------------------
bool ROOTReader::ReadHeader(SampleFormat& mySample)
{
  // Initiliaze REC
  mySample.InitializeRec();

  // Opening file
  source_ = new TFile(mySample.name().c_str());

  // Checking file
  if (!source_->IsOpen()) 
  {
    ERROR << "ROOT file called '" << mySample.name() << "' is not found" << endmsg;
    return false;
  }

  // Checking ROOT version
  Int_t file_version = source_->GetVersion();
  Int_t lib_version = gROOT->GetVersionInt();
  if (file_version!=lib_version)
  {
    WARNING << "the input file has been produced with ROOT version " << file_version
            << " whereas the loaded ROOT libs are related to the version " << lib_version << endmsg;
  }

  // Getting TTree
  tree_ = dynamic_cast<TTree*>(source_->Get("tree"));

  // Checking TTree
  if (tree_==0)
  {
    ERROR << "the ROOT TTree 'tree' is not found in the file" << endmsg;
    return false;
  }


  // Getting number of entries
  nevents_ = tree_->GetEntries();

  // Getting event object
  tree_->SetMakeClass(1);
  evt_=0;
  tree_->SetBranchAddress("event", &evt_);

  INFO << "ROOTdebug nevents in ttree : " << nevents_ << endmsg;
  ncurr_=0;
  return true;
}


// -----------------------------------------------------------------------------
// FinalizeHeader
// -----------------------------------------------------------------------------
bool ROOTReader::FinalizeHeader(SampleFormat& mySample)
{
  // Normal end 
  return true;
}


// -----------------------------------------------------------------------------
// ReadEvent
// -----------------------------------------------------------------------------
StatusCode::Type ROOTReader::ReadEvent(EventFormat& myEvent, SampleFormat& mySample)
{
  // Initiliaze MC
  myEvent.InitializeRec();
  myEvent.InitializeMC();
  myEvent.fac_=evt_;

  if (ncurr_ >= nevents_) return StatusCode::FAILURE;

  Long64_t centry = tree_->LoadTree(ncurr_);
  tree_->GetEntry(ncurr_);

  ncurr_++;

  FillEvent(myEvent,mySample);
  return StatusCode::KEEP;

}


// -----------------------------------------------------------------------------
// FinalizeEvent
// -----------------------------------------------------------------------------
bool ROOTReader::FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)
{
  // Mother pointer assignment
  for (unsigned int i=0; i<myEvent.mc()->particles_.size();i++)
  {
    unsigned int index1=myEvent.mc()->particles_[i].mothup1_;
    unsigned int index2=myEvent.mc()->particles_[i].mothup2_;
    if (index1!=0 && index2!=0)
    {
      if (index1>=myEvent.mc()->particles_.size() ||
          index2>=myEvent.mc()->particles_.size())
      {
        ERROR << "mother index is greater to nb of particles";
        ERROR << endmsg;
        ERROR << " - index1 = " << index1 << endmsg;
        ERROR << " - index2 = " << index2 << endmsg;
        ERROR << " - particles.size() " << myEvent.mc()->particles_.size();
        ERROR << endmsg;
        exit(1);
      }

      myEvent.mc()->particles_[i].mother1_ = &myEvent.mc()->particles_[index1-1];
      myEvent.mc()->particles_[index1-1].Daughters_.push_back(&myEvent.mc()->particles_[i]);
      myEvent.mc()->particles_[i].mother2_ = &myEvent.mc()->particles_[index2-1];
      myEvent.mc()->particles_[index2-1].Daughters_.push_back(&myEvent.mc()->particles_[i]);
    }
  }

  // Normal end
  return true; 
}




// -----------------------------------------------------------------------------
// FillEventParticleLine
// -----------------------------------------------------------------------------
void ROOTReader::FillEvent(EventFormat& myEvent, SampleFormat& mySample)
{
  // Fill MC particles
  for (unsigned int i=0;i<evt_->mcparticles.size();i++)
  {
    MCParticleFormat * part = myEvent.mc()->GetNewParticle();
    part->momentum_ = evt_->mcparticles[i].momentum();
    part->pdgid_    = evt_->mcparticles[i].pid();
  }

  // Fill electrons
  for (unsigned int i=0;i<evt_->electrons.size();i++)
  {
    RecLeptonFormat * electron = myEvent.rec()->GetNewElectron();
    electron->momentum_ = evt_->electrons[i].momentum();
    if (evt_->electrons[i].charge()>0) electron->charge_=true; else electron->charge_=false;
    //electron->isoflag_  = evt_->electrons[i].isolated();
    electron->HEoverEE_ = evt_->electrons[i].ehoveree();
  }

  // Fill muons
  for (unsigned int i=0;i<evt_->muons.size();i++)
  {
    RecLeptonFormat * muon = myEvent.rec()->GetNewMuon();
    muon->momentum_ = evt_->muons[i].momentum();
    if (evt_->muons[i].charge()>0) muon->charge_=true; else muon->charge_=false;
    //muon->isoflag_  = evt_->muons[i].isolated();
    //    muon->HEoverEE_ = evt_->muons[i].ehoveree();
  }

  // Fill jets
  for (unsigned int i=0;i<evt_->jets.size();i++)
  {
    RecJetFormat * jet = myEvent.rec()->GetNewJet();
    jet->momentum_ = evt_->jets[i].momentum();
    jet->ntracks_  = evt_->jets[i].ntracks();
    jet->btag_     = evt_->jets[i].btag();
    jet->HEoverEE_ = evt_->jets[i].ehoveree();
  }

  // MET
  myEvent.rec()->MET_.momentum_.SetPx(evt_->met.px());
  myEvent.rec()->MET_.momentum_.SetPy(evt_->met.py());
  myEvent.rec()->MET_.momentum_.SetE(sqrt(evt_->met.px()*evt_->met.px()+evt_->met.py()*evt_->met.py()));
}

#endif
