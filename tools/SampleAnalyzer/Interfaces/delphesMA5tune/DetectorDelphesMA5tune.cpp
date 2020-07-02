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
#include <fstream>
#include <algorithm>

// SampleAnalyzer headers
#include "SampleAnalyzer/Interfaces/delphesMA5tune/DetectorDelphesMA5tune.h"
#include "SampleAnalyzer/Commons/Service/DisplayService.h"
#include "SampleAnalyzer/Commons/Service/ConvertService.h"

// ROOT headers
#include <TROOT.h>
#include <TObjArray.h>
#include <TFile.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>
#include <TFolder.h>

// Delphes headers
#include "external/ExRootAnalysis/ExRootConfReader.h"
#include "external/ExRootAnalysis/ExRootTreeWriter.h"
#include "external/ExRootAnalysis/ExRootTreeBranch.h"
#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"
#include "modules/Delphes.h"


using namespace MA5;


MAbool DetectorDelphesMA5tune::Initialize(const std::string& configFile, const std::map<std::string,std::string>& options)
{ 
  nprocesses_=0;

  // Save the name of the configuration file
  configFile_ = configFile;
  rootfile_="";

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

    // output
    if (key=="output")
    {
      MAuint32 tmp=0;
      std::stringstream str;
      str << it->second;
      str >> tmp;
      if (tmp!=0 && tmp!=1) WARNING << "allowed values for output are: 0, 1. Current value = " 
                                    << tmp << endmsg;
      else
      {
        if (tmp==0) output_=false;
        else output_=true;
      }
    }
    else if (key=="rootfile")
    {
      std::stringstream str;
      str << it->second;
      str >> rootfile_;
    }
    else if (key=="outputdir")
    {
      std::stringstream str;
      str << it->second;
      str >> outputdir_;
    }
  }

  // Configure outputs
  std::string ofname;
  if (output_)
  {
    if (rootfile_=="") ofname = outputdir_+"/DelphesMA5tuneEvents.root";
    else               ofname = outputdir_+"/"+rootfile_;
  }
  else ofname = outputdir_+"/tmp.root";
  outputFile_ = TFile::Open(ofname.c_str(), "RECREATE");

  // Configure inputs
  confReader_ = new ExRootConfReader;
  confReader_->ReadFile(configFile_.c_str());

  // Studying configuration part for TreeWriter module
  ExRootConfParam param = confReader_->GetParam("TreeWriter::Branch");
  if (param.GetSize()==0) 
  {
    ERROR << "Problem with Delphes card: no 'add Branch' line in TreeWriter module" << endmsg;
    return false;
  }
  if (param.GetSize()%3!=0) 
  {
    ERROR << "Problem with Delphes card: problem with one or several 'add Branch' lines" << endmsg;
    return false;
  }

  for(MAuint32 i=0; i<static_cast<MAuint32>(param.GetSize())/3; i++)
  {
    std::string branchInputArray = param[i*3+0].GetString();
    std::string branchName       = param[i*3+1].GetString();
    table_[branchName]=branchInputArray;
  }

  treeWriter_ = new ExRootTreeWriter(outputFile_, "DelphesMA5tune");
  branchEvent_ = treeWriter_->NewBranch("Event", LHEFEvent::Class());
  branchWeight_ = treeWriter_->NewBranch("Weight", Weight::Class());

  // Initializing delphes
  modularDelphes_ = new Delphes("Delphes");
  delphesFolder_ = dynamic_cast<TFolder*>(
       gROOT->GetListOfBrowsables()->FindObject("Delphes"));
  if (delphesFolder_==0)
  {
    ERROR << "Problem during initialization of Delphes" << endmsg;
    return false;
  }
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
  nprocesses_=0;


  return true;
}


void DetectorDelphesMA5tune::PrintParam()
{
  INFO << "" << endmsg; 
}


std::string DetectorDelphesMA5tune::GetParameters()
{
  std::stringstream str;
  return str.str();
}


/// Jet clustering
MAbool DetectorDelphesMA5tune::Execute(SampleFormat& mySample, EventFormat& myEvent)
{
  nprocesses_++;

  // Import particles to Delphes
  TranslateMA5toDELPHES(mySample, myEvent);

  // Applying fast-simulation
  gErrorIgnoreLevel=kError;
  modularDelphes_->ProcessTask();
  gErrorIgnoreLevel=kWarning;

  // Export particles from Delphes
  TranslateDELPHEStoMA5(mySample, myEvent);

  // Creater Event header
  StoreEventHeader(mySample, myEvent); 

  // Saving ROOT
  if (output_) treeWriter_->Fill();

  // Reset
  treeWriter_->Clear();
  modularDelphes_->Clear();
  // Second Reset (seems necessary)
  modularDelphes_->Clear();
  
  return true;
}

void DetectorDelphesMA5tune::StoreEventHeader(SampleFormat& mySample, EventFormat& myEvent)
{
  LHEFEvent *element = dynamic_cast<LHEFEvent *>(branchEvent_->NewEntry());
  
  element->Number    = nprocesses_;
  if (myEvent.mc()==0) return;

  element->ProcessID = myEvent.mc()->processId();
  element->Weight    = myEvent.mc()->weight();
  element->ScalePDF  = myEvent.mc()->scale();
  element->AlphaQED  = myEvent.mc()->alphaQED();
  element->AlphaQCD  = myEvent.mc()->alphaQCD();
  element->ReadTime  = 0; //? readStopWatch->RealTime();
  element->ProcTime  = 0; //? procStopWatch->RealTime();
}

void DetectorDelphesMA5tune::Finalize()
{
  nprocesses_=0;
  modularDelphes_->FinishTask();
  if (output_) treeWriter_->Write();

  delete confReader_; confReader_=0;
  delete treeWriter_; treeWriter_=0;
  delete modularDelphes_; modularDelphes_=0;
}

void DetectorDelphesMA5tune::TranslateMA5toDELPHES(SampleFormat& mySample, EventFormat& myEvent)
{
  // Create a table for generated particle
  std::map<const MCParticleFormat*,MAuint32> gentable; 
  std::map<const MCParticleFormat*,MAuint32>::iterator ret;
  for (MAuint32 i=0;i<myEvent.mc()->particles().size();i++)
  {
    const MCParticleFormat* part = &(myEvent.mc()->particles()[i]);
    gentable[part]=i;
  }

  for (MAuint32 i=0;i<myEvent.mc()->particles().size();i++)
  {
    const MCParticleFormat* part = &(myEvent.mc()->particles()[i]);
    Candidate* candidate = factory_->NewCandidate();

    candidate->PID = part->pdgid();
    MAuint32 pdgCode=std::abs(part->pdgid());

    candidate->Status = part->statuscode();
    candidate->Momentum.SetPxPyPzE(part->px(), part->py(), part->pz(), part->e());
    candidate->Position.SetXYZT(0., 0., 0., 0.);

    // Filling mother-daughter information
    candidate->M1=0;
    candidate->M2=0;
    candidate->D1=0;
    candidate->D2=0;
    std::vector<MAint32*> mothers(2);
    mothers[0]=&(candidate->M1);
    mothers[1]=&(candidate->M2);
    *(mothers[0])=-1;
    *(mothers[1])=-1;
    std::vector<MAint32*> daughters(2);
    daughters[0]=&(candidate->D1);
    daughters[1]=&(candidate->D2);
    *(daughters[0])=-1;
    *(daughters[1])=-1;

    for(MAuint32 mum=0;mum<std::min(static_cast<MAuint32>(part->mothers().size()),
                                    static_cast<MAuint32>(2));mum++)
    {
      ret = gentable.find(part->mothers()[mum]);
      if (ret!= gentable.end())
      {
        *(mothers[mum])=ret->second;
      }
      else
      {
        ERROR << "internal problem with daughter-mother relation" << endmsg;
      }
    }

    for(MAuint32 mum=0;mum<std::min(static_cast<MAuint32>(part->daughters().size()),
                                    static_cast<MAuint32>(2));mum++)
    {
      ret = gentable.find(part->daughters()[mum]);
      if (ret!= gentable.end())
      {
        *(daughters[mum])=ret->second;
      }
      else
      {
        ERROR << "internal problem with daughter-mother relation" << endmsg;
      }
    }

    /*    candidate->M1 = part.mothup1_ - 1;
    candidate->M2 = part.mothup2_ - 1;
    candidate->D1 = part.daughter1_ -1;
    candidate->D2 = part.daughter2_ -1;
    */

    TParticlePDG* pdgParticle = PDG_->GetParticle(part->pdgid());
    if (pdgParticle==0) 
    { 
      //FIX ERIC: WARNING << "Particle not found in PDG" << endmsg;
      allParticleOutputArray_->Add(candidate);
      continue;
    }

    candidate->Charge = pdgParticle ? MAint32(pdgParticle->Charge()/3.0) : -999;
    candidate->Mass = pdgParticle ? pdgParticle->Mass() : -999.9;
    allParticleOutputArray_->Add(candidate);

    if(part->statuscode() == 1 && pdgParticle->Stable())
    {
      stableParticleOutputArray_->Add(candidate);
    }
    else if(pdgCode <= 5 || pdgCode == 21 || pdgCode == 15)
    {
      partonOutputArray_->Add(candidate);
    }
  }
}

void DetectorDelphesMA5tune::TranslateDELPHEStoMA5(SampleFormat& mySample, EventFormat& myEvent)
{
  if (myEvent.rec()==0)  myEvent.InitializeRec();
  if (mySample.rec()==0) mySample.InitializeRec();
  myEvent.rec()->Reset();

  // https://cp3.irmp.ucl.ac.be/projects/delphes/wiki/WorkBook/Arrays

  // Jet collection
  TObjArray* jetsArray = dynamic_cast<TObjArray*>(delphesFolder_->FindObject("Export/JetEnergyScale/jets"/* FastJetFinder/jets"*/));
  if (jetsArray==0) {if (!first_) WARNING << "no jets collection found" << endmsg;}
  else
  {
    for (MAuint32 i=0;i<static_cast<MAuint32>(jetsArray->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(jetsArray->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th jet" << endmsg;
        continue;
      }
      if (cand->TauTag==1)
      {
        RecTauFormat* tau = myEvent.rec()->GetNewTau();
        MAfloat64 px = cand->Momentum.Px();
        MAfloat64 py = cand->Momentum.Py();
        MAfloat64 pz = cand->Momentum.Pz();
        MAfloat64 e  = cand->Momentum.E();
        tau->momentum_.SetPxPyPzE(px,py,pz,e);
        if (cand->Charge>0) tau->charge_=true; else tau->charge_=false;

        if (cand->Eem!=0) tau->HEoverEE_ = cand->Ehad/cand->Eem; else tau->HEoverEE_ = 999.;
        tau->ntracks_ = 0; // To fix later
      }
      else
      {
        RecJetFormat* jet = myEvent.rec()->GetNewJet();
        MAfloat64 px = cand->Momentum.Px();
        MAfloat64 py = cand->Momentum.Py();
        MAfloat64 pz = cand->Momentum.Pz();
        MAfloat64 e  = cand->Momentum.E();
        jet->momentum_.SetPxPyPzE(px,py,pz,e);
        jet->btag_ = cand->BTag;
        if (cand->Eem!=0) jet->HEoverEE_ = cand->Ehad/cand->Eem; else jet->HEoverEE_ = 999.;
        jet->ntracks_ = 0; // To fix later
      }
    }
  }

  // GenJet collection
  TObjArray* genjetsArray = dynamic_cast<TObjArray*>(delphesFolder_->FindObject("Export/GenJetFinder/jets"));
  if (genjetsArray==0) {if (!first_) WARNING << "no genjets collection found" << endmsg;}
  else
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
      MAfloat64 px = cand->Momentum.Px();
      MAfloat64 py = cand->Momentum.Py();
      MAfloat64 pz = cand->Momentum.Pz();
      MAfloat64 e  = cand->Momentum.E();
      genjet->momentum_.SetPxPyPzE(px,py,pz,e);
      genjet->btag_ = cand->BTag;
    }
  }

  // Muon collection
  TObjArray* muonArray = dynamic_cast<TObjArray*>(
         delphesFolder_->FindObject("Export/MuonIsolationCalculation/DelphesMA5tuneMuons"));
  if (muonArray==0) {if (!first_) WARNING << "no muons collection found" << endmsg;}
  else
  {
    for (MAuint32 i=0;i<static_cast<MAuint32>(muonArray->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(muonArray->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th muon" << endmsg;
        continue;
      }
      RecLeptonFormat* muon = myEvent.rec()->GetNewMuon();
      MAfloat64 px = cand->Momentum.Px();
      MAfloat64 py = cand->Momentum.Py();
      MAfloat64 pz = cand->Momentum.Pz();
      MAfloat64 e  = cand->Momentum.E();
      muon->momentum_.SetPxPyPzE(px,py,pz,e);
      muon->SetCharge(cand->Charge);
    }
  }

  // Electron collection
  TObjArray* elecArray = dynamic_cast<TObjArray*>(
     delphesFolder_->FindObject("Export/ElectronIsolationCalculation/DelphesMA5tuneElectrons"));
  if (elecArray==0) {if (!first_) WARNING << "no elecs collection found" << endmsg;}
  else
  {
    for (MAuint32 i=0;i<static_cast<MAuint32>(elecArray->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(elecArray->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th electron" << endmsg;
        continue;
      }
      RecLeptonFormat* elec = myEvent.rec()->GetNewElectron();
      MAfloat64 px = cand->Momentum.Px();
      MAfloat64 py = cand->Momentum.Py();
      MAfloat64 pz = cand->Momentum.Pz();
      MAfloat64 e  = cand->Momentum.E();
      elec->momentum_.SetPxPyPzE(px,py,pz,e);
      elec->SetCharge(cand->Charge);
    }
  }

  // Track collection
  TObjArray* trackArray = dynamic_cast<TObjArray*>(
    delphesFolder_->FindObject("Export/TrackIsolationCalculation/DelphesMA5tuneTracks"));
  if (trackArray==0) {if (!first_) WARNING << "no tracks collection found" << endmsg;}
  else
  {
    for (MAuint32 i=0;i<static_cast<MAuint32>(trackArray->GetEntries());i++)
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
      MAfloat64 px = cand->Momentum.Px();
      MAfloat64 py = cand->Momentum.Py();
      MAfloat64 pz = cand->Momentum.Pz();
      MAfloat64 e  = cand->Momentum.E();
      track->momentum_.SetPxPyPzE(px,py,pz,e);
      track->etaOuter_=cand->Position.Eta();
      track->phiOuter_=cand->Position.Phi();
    }
  }

  // MET
  TObjArray* metArray  = dynamic_cast<TObjArray*>(
    delphesFolder_->FindObject("Export/MissingET/momentum"));
  if (metArray==0) {if (!first_) WARNING << "MET collection is not found" << endmsg;}
  else
  {
    Candidate* metCand = dynamic_cast<Candidate*>(metArray->At(0));
    if (metCand==0) 
    {
      ERROR << "impossible to access the MET" << endmsg;
    }
    else
    {
      MAfloat64 pt = metCand->Momentum.Pt();
      MAfloat64 px = metCand->Momentum.Px();
      MAfloat64 py = metCand->Momentum.Py();
      myEvent.rec()->MET().momentum_.SetPxPyPzE(px,py,0,pt);
    }
  }
  first_=true;

}

