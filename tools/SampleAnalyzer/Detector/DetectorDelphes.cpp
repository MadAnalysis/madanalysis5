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


#include "SampleAnalyzer/Detector/DetectorDelphes.h"

#include <fstream>
#include <TROOT.h>

#ifdef DELPHES_USE

//Delphes header
#include "external/ExRootAnalysis/ExRootConfReader.h"
#include "external/ExRootAnalysis/ExRootTreeWriter.h"
#include "external/ExRootAnalysis/ExRootTreeBranch.h"
#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"
#include "modules/Delphes.h"

//SampleAnalyzer header
#include "SampleAnalyzer/Service/DisplayService.h"


using namespace MA5;

bool DetectorDelphes::Initialize(const std::string& configFile, const std::map<std::string,std::string>& options)
{ 
  // Save the name of the configuration file
  configFile_ = configFile;

  // Test the presence of the configuration file on the hard disk
  std::ifstream configTest(configFile.c_str());
  if (!configTest.is_open())
  {
    ERROR << "Configuration file '" << configFile_ << "' is not found" << endmsg;
    return false;
  }
  configTest.close();

  // Read parameters
  for (std::map<std::string,std::string>::const_iterator
       it=options.begin();it!=options.end();it++)
  {
    std::string key = DetectorBase::Lower(it->first);

    /*
    // radius
    if (key=="r")
    {
      float tmp=0;
      std::stringstream str;
      str << it->second;
      str >> tmp;
      if (tmp<0) WARNING << "R must be positive. Using default value R = " 
                         << R_ << endmsg;
      else R_=tmp;
      }*/
  }

  // Configure inputs
  confReader_ = new ExRootConfReader;
  confReader_->ReadFile(configFile_.c_str());

  // Configure outputs
  outputFile_ = TFile::Open("tmp.root", "RECREATE");
  treeWriter_ = new ExRootTreeWriter(outputFile_, "Delphes");
  //  branchEvent_ = treeWriter_->NewBranch("Event", LHEFEvent::Class());

  // Initializing delphes
  modularDelphes_ = new Delphes("Delphes");
  modularDelphes_->SetConfReader(confReader_);
  modularDelphes_->SetTreeWriter(treeWriter_);

  factory_ = modularDelphes_->GetFactory();
  allParticleOutputArray_    = modularDelphes_->ExportArray("allParticles");
  stableParticleOutputArray_ = modularDelphes_->ExportArray("stableParticles");
  partonOutputArray_         = modularDelphes_->ExportArray("partons");
  modularDelphes_->InitTask();

  // Delphes PDG service
  PDG_ = TDatabasePDG::Instance();

  // Reset
  treeWriter_->Clear();
  modularDelphes_->Clear();

  return true;
}


void DetectorDelphes::PrintParam()
{
  INFO << "" << endmsg; 
}


std::string DetectorDelphes::GetParameters()
{
  std::stringstream str;
  return str.str();
}


/// Jet clustering
bool DetectorDelphes::Execute(SampleFormat& mySample, EventFormat& myEvent)
{
  // Import particles to Delphes
  TranslateMA5toDELPHES(mySample, myEvent);

  // Applying fast-simulation
  modularDelphes_->ProcessTask();

  // Export particles from Delphes
  TranslateDELPHEStoMA5(mySample, myEvent);

  // Reset
  treeWriter_->Clear();
  modularDelphes_->Clear();
  
  return true;
}

void DetectorDelphes::Finalize()
{
  modularDelphes_->FinishTask();

  delete confReader_; confReader_=0;
  delete treeWriter_; treeWriter_=0;
  delete modularDelphes_; modularDelphes_=0;
}

void DetectorDelphes::TranslateMA5toDELPHES(SampleFormat& mySample, EventFormat& myEvent)
{
  for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
  {
    const MCParticleFormat& part = myEvent.mc()->particles()[i];
    Candidate* candidate = factory_->NewCandidate();

    candidate->PID = part.pdgid();
    unsigned int pdgCode=std::abs(part.pdgid());

    candidate->Status = part.statuscode();
    candidate->Momentum.SetPxPyPzE(part.px(), part.py(), part.pz(), part.e());
    candidate->Position.SetXYZT(0., 0., 0., 0.);

    candidate->M1 = part.mothup1_ - 1;
    candidate->M2 = part.mothup2_ - 1;
    candidate->D1 = 0;//d1 - 1;
    candidate->D2 = 0;//d2 - 1;

    TParticlePDG* pdgParticle = PDG_->GetParticle(part.pdgid());
    if (pdgParticle==0) 
    { 
      //FIX ERIC: WARNING << "Particle not found in PDG" << endmsg;
      allParticleOutputArray_->Add(candidate);
      continue;
    }

    candidate->Charge = pdgParticle ? int(pdgParticle->Charge()/3.0) : -999;
    candidate->Mass = pdgParticle ? pdgParticle->Mass() : -999.9;
    allParticleOutputArray_->Add(candidate);

    if(part.statuscode() == 1 && pdgParticle->Stable())
    {
      stableParticleOutputArray_->Add(candidate);
    }
    else if(pdgCode <= 5 || pdgCode == 21 || pdgCode == 15)
    {
      partonOutputArray_->Add(candidate);
    }
  }
}

void DetectorDelphes::TranslateDELPHEStoMA5(SampleFormat& mySample, EventFormat& myEvent)
{
  if (myEvent.rec()==0)  myEvent.InitializeRec();
  if (mySample.rec()==0) mySample.InitializeRec();
  myEvent.rec()->Reset();

  // https://cp3.irmp.ucl.ac.be/projects/delphes/wiki/WorkBook/Arrays
  TFolder* folder = modularDelphes_->GetFolder();

  // Jet collection
  TObjArray* jetsArray = dynamic_cast<TObjArray*>(folder->FindObject("Export/FastJetFinder/jets"));
  if (jetsArray==0) WARNING << "no jets collection found" << endmsg;
  else
  {
    for (unsigned int i=0;i<static_cast<UInt_t>(jetsArray->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(jetsArray->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th jet" << endmsg;
        continue;
      }
      RecJetFormat* jet = myEvent.rec()->GetNewJet();
      jet->momentum_ = cand->Momentum;
      jet->btag_ = cand->BTag;
    }
  }

  // Muon collection
  TObjArray* muonArray = dynamic_cast<TObjArray*>(
         folder->FindObject("Export/MuonIsolation/muons"));
  if (muonArray==0) WARNING << "no muons collection found" << endmsg;
  else
  {
    for (unsigned int i=0;i<static_cast<UInt_t>(muonArray->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(muonArray->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th muon" << endmsg;
        continue;
      }
      RecLeptonFormat* muon = myEvent.rec()->GetNewMuon();
      muon->momentum_ = cand->Momentum;
    }
  }

  // Electron collection
  TObjArray* elecArray = dynamic_cast<TObjArray*>(
     folder->FindObject("Export/UniqueObjectFinder/electrons"));
  if (elecArray==0) WARNING << "no elecs collection found" << endmsg;
  else
  {
    for (unsigned int i=0;i<static_cast<UInt_t>(elecArray->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(elecArray->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th electron" << endmsg;
        continue;
      }
      RecLeptonFormat* elec = myEvent.rec()->GetNewElectron();
      elec->momentum_ = cand->Momentum;
    }
  }

  // Track collection
  TObjArray* trackArray = dynamic_cast<TObjArray*>(
    folder->FindObject("Export/TrackMerger/tracks"));
  if (trackArray==0) WARNING << "no tracks collection found" << endmsg;
  else
  {
    for (unsigned int i=0;i<static_cast<UInt_t>(trackArray->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(trackArray->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th track" << endmsg;
        continue;
      }
      RecTrackFormat* track = myEvent.rec()->GetNewTrack();
      track->pdgid_ = cand->PID;
      if (cand->Charge>0) track->charge_=true; else track->charge_=false;
      track->momentum_=cand->Momentum;
      track->etaOuter_=cand->Position.Eta();
      track->phiOuter_=cand->Position.Phi();
    }
  }

  // MET
  TObjArray* metArray  = dynamic_cast<TObjArray*>(
    folder->FindObject("Export/MissingET/momentum"));
  if (metArray==0) WARNING << "MET collection is not found" << endmsg;
  else
  {
    Candidate* metCand = dynamic_cast<Candidate*>(metArray->At(0));
    if (metCand==0) 
    {
      ERROR << "impossible to access the MET" << endmsg;
    }
    else
    {
      double pt = metCand->Momentum.Pt();
      double px = metCand->Momentum.Px();
      double py = metCand->Momentum.Py();
      myEvent.rec()->MET().momentum_.SetPxPyPzE(px,py,0,pt);
    }
  }

}


#endif
