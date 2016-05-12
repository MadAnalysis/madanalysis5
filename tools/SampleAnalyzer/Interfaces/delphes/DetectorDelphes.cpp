////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2016 Eric Conte, Benjamin Fuks
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


#include "SampleAnalyzer/Interfaces/delphes/DetectorDelphes.h"
#include "SampleAnalyzer/Interfaces/delphes/DelphesMemoryInterface.h"

#include <fstream>
#include <TROOT.h>


//Delphes header
#include "external/ExRootAnalysis/ExRootConfReader.h"
#include "external/ExRootAnalysis/ExRootTreeWriter.h"
#include "external/ExRootAnalysis/ExRootTreeBranch.h"
#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"
#include "modules/Delphes.h"

//SampleAnalyzer header
#include "SampleAnalyzer/Commons/Service/DisplayService.h"


using namespace MA5;


bool DetectorDelphes::Initialize(const std::string& configFile, const std::map<std::string,std::string>& options)
{ 
  nprocesses_=0;

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

    // output
    if (key=="output")
    {
      unsigned int tmp=0;
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
  }

  // Decode configuration file with Delphes class 'ExRootConfReader'
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

  bool isElectronMA5 = false;
  bool isMuonMA5     = false;
  bool isPhotonMA5   = false;
  bool isJetMA5      = false;
  for(unsigned int i=0; i<static_cast<unsigned int>(param.GetSize())/3; i++)
  {
    std::string branchInputArray = param[i*3+0].GetString();
    std::string branchName       = param[i*3+1].GetString();
    if      (branchName=="ElectronMA5") isElectronMA5 = true;
    else if (branchName=="MuonMA5")     isMuonMA5     = true;
    else if (branchName=="PhotonMA5")   isPhotonMA5   = true;
    else if (branchName=="JetMA5")      isJetMA5      = true;
    table_[branchName]=branchInputArray;
  }
  if (isElectronMA5 && isMuonMA5 && 
      isPhotonMA5   && isJetMA5      ) MA5card_=true; else MA5card_=false;

  // Creating output file
  if (output_) outputFile_ = TFile::Open("TheMouth.root", "RECREATE");
  else outputFile_ = TFile::Open("tmp.root", "RECREATE");

  // Creating output tree
  treeWriter_ = new ExRootTreeWriter(outputFile_, "Delphes");
  //  ExRootTreeBrancha* branchEvent_ = treeWriter_->NewBranch("Event", LHEFEvent::Class());
  branchEvent_  = treeWriter_->NewBranch("Event",  LHEFEvent::Class());
  branchWeight_ = treeWriter_->NewBranch("Weight", Weight::Class());

  // Creating all Delphes modules
  modularDelphes_ = new Delphes("Delphes");
  delphesFolder_ = dynamic_cast<TFolder*>(
       gROOT->GetListOfBrowsables()->FindObject("Delphes"));
  if (delphesFolder_==0)
  {
    ERROR << "Problem during initialization of Delphes" << endmsg;
    return false;
  }

  // Initializing Delphes modules
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

  // Initializing interface
  interface_.Initialize(delphesFolder_,table_,MA5card_);

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
  nprocesses_++;

  // Import particles to Delphes
  TranslateMA5toDELPHES(mySample, myEvent);

  // Applying fast-simulation
  modularDelphes_->ProcessTask();

  // Export particles from Delphes
  TranslateDELPHEStoMA5(mySample, myEvent);

  // Creater Event header
  StoreEventHeader(mySample, myEvent); 

  // Saving ROOT
  if (output_) treeWriter_->Fill();

  // Reset
  treeWriter_->Clear();
  modularDelphes_->Clear();
  
  return true;
}

void DetectorDelphes::Finalize()
{
  nprocesses_=0;
  modularDelphes_->FinishTask();
  if (output_) treeWriter_->Write();

  delete confReader_; confReader_=0;
  delete treeWriter_; treeWriter_=0;
  delete modularDelphes_; modularDelphes_=0;
}

void DetectorDelphes::StoreEventHeader(SampleFormat& mySample, EventFormat& myEvent)
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
    candidate->D1 = part.daughter1_ -1;
    candidate->D2 = part.daughter2_ -1;

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

  interface_.TransfertDELPHEStoMA5(mySample,myEvent);
}


