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


#ifdef DELPHES_USE

// STL headers
#include <sstream>

// SampleHeader headers
#include "SampleAnalyzer/Reader/DelphesReader.h"
#include "SampleAnalyzer/Service/LogService.h"

// ROOT headers
#include <TROOT.h>


using namespace MA5;

// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------
bool DelphesReader::Initialize(const std::string& rawfilename,
                               const Configuration& cfg)
{
  // Set configuration
  cfg_=cfg;

  // Is the file stored in Rfio
  rfio_ = IsRfioMode(rawfilename);

  // Check consistency with compilation option
  if (rfio_)
  {
#ifndef RFIO_USE
    ERROR << "'-rfio' is not allowed. Please set the RFIO_USE"
          << " variable in the Makefile to 1 and recompile the program if"
          << " you would like to use this option." << endmsg;
    exit(1);
#endif
  }

  // Cleaning the file (remove rfio or local location)
  filename_ = rawfilename;
  CleanFilename(filename_);

  // Opening the file
  source_ = new TFile(filename_.c_str());
  
  // Check if the input is properly opened
  bool test=true;
  if (source_==0) test=false;
  else if (!source_->IsOpen() || source_->IsZombie()) test=false;
  if (!test)
  {
    ERROR << "Opening file " << filename_ << " failed" << endmsg;
    source_=0;
    return false;
  }
  
  // Initializing tree
  // Create chain of root trees
  tree_ = dynamic_cast<TTree*>(source_->Get("Delphes"));
  if (tree_==0)
  {
    ERROR << "Impossible to access the tree "
          << "called 'Delphes' in the input file" << endmsg;
    return false;
  }  
  // Create object of class ExRootTreeReader
  treeReader_    = new ExRootTreeReader(tree_);
  total_nevents_ = treeReader_->GetEntries();
  read_nevents_  = 0;

  // Get pointers to branches used in this analysis
  branchJet       = treeReader_->UseBranch("Jet");
  if (branchJet==0)
  {
    WARNING << "Jet collection is not found" << endmsg;
  }
  branchElectron  = treeReader_->UseBranch("Electron");
  if (branchElectron==0)
  {
    WARNING << "Electron collection is not found" << endmsg;
  }
  branchPhoton    = treeReader_->UseBranch("Photon");
  if (branchPhoton==0)
  {
    WARNING << "Photon collection is not found" << endmsg;
  }
  branchMuon      = treeReader_->UseBranch("Muon");
  if (branchMuon==0)
  {
    WARNING << "Muon collection is not found" << endmsg;
  }
  branchMissingET = treeReader_->UseBranch("MissingET");
  if (branchMissingET==0)
  {
    WARNING << "MissingEt is not found" << endmsg;
  }

  return test;
}


// -----------------------------------------------------------------------------
// ReadHeader
// -----------------------------------------------------------------------------
bool DelphesReader::ReadHeader(SampleFormat& mySample)
{
  // Initiliaze REC
  mySample.InitializeRec();

  // Checking ROOT version
  Int_t file_version = source_->GetVersion();
  Int_t lib_version = gROOT->GetVersionInt();
  if (file_version!=lib_version)
  {
    WARNING << "the input file has been produced with ROOT version " << file_version
            << " whereas the loaded ROOT libs are related to the version " << lib_version << endmsg;
  }

  return true;
}


// -----------------------------------------------------------------------------
// FinalizeHeader
// -----------------------------------------------------------------------------
bool DelphesReader::FinalizeHeader(SampleFormat& mySample)
{
  // Normal end 
  return true;
}


// -----------------------------------------------------------------------------
// ReadEvent
// -----------------------------------------------------------------------------
StatusCode::Type DelphesReader::ReadEvent(EventFormat& myEvent, SampleFormat& mySample)
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
bool DelphesReader::FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)
{
  /*  // Mother pointer assignment
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
    }*/

  // Normal end
  return true; 
}




// -----------------------------------------------------------------------------
// FillEventParticleLine
// -----------------------------------------------------------------------------
void DelphesReader::FillEvent(EventFormat& myEvent, SampleFormat& mySample)
{
  /*  // Fill MC particles
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
  */
}


// -----------------------------------------------------------------------------
// Finalize
// -----------------------------------------------------------------------------
bool DelphesReader::Finalize()
{
  // OK!
  if (source_!=0) source_->Close();
  return true;
}

#endif
