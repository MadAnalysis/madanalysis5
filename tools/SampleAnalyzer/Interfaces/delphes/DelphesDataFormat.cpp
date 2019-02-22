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


// SampleHeader headers
#include "SampleAnalyzer/Interfaces/delphes/DelphesDataFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"

// ROOT headers
#include <TBranchElement.h>
#include <TROOT.h>


using namespace MA5;

// -----------------------------------------------------------------------------
// Constructor without arguments
// -----------------------------------------------------------------------------
DelphesDataFormat::DelphesDataFormat()
{
  branchFatJet_       = 0;
  branchJet_          = 0;
  branchElectron_     = 0;
  branchPhoton_       = 0;
  branchMuon_         = 0;
  branchMET_          = 0;
  branchHT_           = 0;
  branchGenParticle_  = 0;
  branchTrack_        = 0;
  branchTower_        = 0;
  branchEFlowTrack_   = 0;
  branchEFlowPhoton_  = 0;
  branchEFlowNeutral_ = 0;
  branchEvent_        = 0;
  branchWeight_       = 0;
  FatJet_       = 0;
  Jet_          = 0;
  Electron_     = 0;
  Photon_       = 0;
  Muon_         = 0;
  MET_          = 0;
  HT_           = 0;
  GenParticle_  = 0;
  Track_        = 0;
  Tower_        = 0;
  EFlowTrack_   = 0;
  EFlowPhoton_  = 0;
  EFlowNeutral_ = 0;
  Event_        = 0;
  Weight_       = 0;
  delphesMA5card_ = false;
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
DelphesDataFormat::~DelphesDataFormat()
{
  if (FatJet_!=0)       delete FatJet_;
  if (Jet_!=0)          delete Jet_;
  if (Electron_!=0)     delete Electron_;
  if (Photon_!=0)       delete Photon_;
  if (Muon_!=0)         delete Muon_;
  if (MET_!=0)          delete MET_;
  if (HT_!=0)           delete HT_;
  if (GenParticle_!=0)  delete GenParticle_;
  if (Track_!=0)        delete Track_;
  if (Tower_!=0)        delete Tower_;
  if (EFlowTrack_!=0)   delete EFlowTrack_;
  if (EFlowPhoton_!=0)  delete EFlowPhoton_;
  if (EFlowNeutral_!=0) delete EFlowNeutral_;
  if (Event_!=0)        delete Event_;
  if (Weight_!=0)       delete Weight_;
}


// -----------------------------------------------------------------------------
// Getentry
// -----------------------------------------------------------------------------
MAbool DelphesDataFormat::GetEntry(MAint64 treeEntry)
{
  MAbool test = true;
  if (branchFatJet_!=0)       test &= (branchFatJet_       -> GetEntry(treeEntry) >=0);
  if (branchJet_!=0)          test &= (branchJet_          -> GetEntry(treeEntry) >=0);
  if (branchElectron_!=0)     test &= (branchElectron_     -> GetEntry(treeEntry) >=0);
  if (branchPhoton_!=0)       test &= (branchPhoton_       -> GetEntry(treeEntry) >=0);
  if (branchMuon_!=0)         test &= (branchMuon_         -> GetEntry(treeEntry) >=0);
  if (branchMET_!=0)          test &= (branchMET_          -> GetEntry(treeEntry) >=0);
  if (branchHT_!=0)           test &= (branchHT_           -> GetEntry(treeEntry) >=0);
  if (branchGenParticle_!=0)  test &= (branchGenParticle_  -> GetEntry(treeEntry) >=0);
  if (branchTrack_!=0)        test &= (branchTrack_        -> GetEntry(treeEntry) >=0);
  if (branchTower_!=0)        test &= (branchTower_        -> GetEntry(treeEntry) >=0);
  if (branchEFlowTrack_!=0)   test &= (branchEFlowTrack_   -> GetEntry(treeEntry) >=0);
  if (branchEFlowPhoton_!=0)  test &= (branchEFlowPhoton_  -> GetEntry(treeEntry) >=0);
  if (branchEFlowNeutral_!=0) test &= (branchEFlowNeutral_ -> GetEntry(treeEntry) >=0);
  if (branchEvent_!=0)        test &= (branchEvent_        -> GetEntry(treeEntry) >=0);
  if (branchWeight_!=0)       test &= (branchWeight_       -> GetEntry(treeEntry) >=0);
  return test;
}


// -----------------------------------------------------------------------------
// InitializeData
// -----------------------------------------------------------------------------
void DelphesDataFormat::InitializeData()
{
  // Official Delphes collections
  InitializeData(branchEvent_,        Event_);
  InitializeData(branchWeight_,       Weight_);
  InitializeData(branchGenParticle_,  GenParticle_);
  InitializeData(branchMET_,          MET_);
  InitializeData(branchTower_,        Tower_);
  InitializeData(branchTrack_,        Track_);
  InitializeData(branchFatJet_,       FatJet_);
  InitializeData(branchJet_,          Jet_);
  InitializeData(branchElectron_,     Electron_);
  InitializeData(branchPhoton_,       Photon_);
  InitializeData(branchMuon_,         Muon_);
  InitializeData(branchHT_,           HT_);
  InitializeData(branchEFlowTrack_,   EFlowTrack_);
  InitializeData(branchEFlowPhoton_,  EFlowPhoton_);
  InitializeData(branchEFlowNeutral_, EFlowNeutral_);
}


// -----------------------------------------------------------------------------
// InitializeBranch
// -----------------------------------------------------------------------------
void DelphesDataFormat::InitializeBranch(TTree* tree)
{
  // Getting all branches name
  TObjArray* branches = tree -> GetListOfBranches();
  if (branches!=0)
  {
    TIter next(branches);
    TBranch* mybranch=0;
    while((mybranch=dynamic_cast<TBranch*>(next())))
    {
//      std::cout << mybranch->GetClassName() << " - " << mybranch->GetName() << std::endl;
    }
  }

  // Official Delphes collections
  branchEvent_        = tree->GetBranch("Event");
  branchWeight_       = tree->GetBranch("Weight");
  branchGenParticle_  = tree->GetBranch("Particle");
  branchMET_          = tree->GetBranch("MissingET");
  branchTower_        = tree->GetBranch("Tower");
  branchTrack_        = tree->GetBranch("Track");
  branchJet_          = tree->GetBranch("Jet");
  branchFatJet_       = tree->GetBranch("FatJet");
  branchElectron_     = tree->GetBranch("Electron");
  branchPhoton_       = tree->GetBranch("Photon");
  branchMuon_         = tree->GetBranch("Muon");
  branchHT_           = tree->GetBranch("ScalarHT");
  branchEFlowTrack_   = tree->GetBranch("EFlowTrack");
  branchEFlowPhoton_  = tree->GetBranch("EFlowPhoton");
  branchEFlowNeutral_ = tree->GetBranch("EFlowNeutralHadron");

  // Special MA5 collection
  TBranch* branchJetMA5      = tree->GetBranch("JetMA5");
  TBranch* branchElectronMA5 = tree->GetBranch("ElectronMA5");
  TBranch* branchMuonMA5     = tree->GetBranch("MuonMA5");
  TBranch* branchPhotonMA5   = tree->GetBranch("PhotonMA5");

  // DelphesMA5 tune mode
  if (branchMuonMA5!=0   || branchElectronMA5!=0 || 
      branchPhotonMA5!=0 || branchJetMA5!=0)
  {
    delphesMA5card_=true;
    branchMuon_     = branchMuonMA5;
    branchElectron_ = branchElectronMA5;
    branchPhoton_   = branchPhotonMA5;
    branchJet_      = branchJetMA5;
  }
  else
  {
    delphesMA5card_=false;
  }

  // Display warning to main branches
  try
  {
    if (branchEvent_       ==0) throw EXCEPTION_WARNING("Event branch is not found","",0);
    if (branchMET_         ==0) throw EXCEPTION_WARNING("MET branch is not found","",0);
    if (branchGenParticle_ ==0) throw EXCEPTION_WARNING("GenParticle branch is not found","",0);
    if (branchElectron_    ==0) throw EXCEPTION_WARNING("Electron collection branch is not found","",0);
    if (branchMuon_        ==0) throw EXCEPTION_WARNING("Muon collection branch is not found","",0);
    if (branchPhoton_      ==0) throw EXCEPTION_WARNING("Photon collection branch is not found","",0);
    if (branchJet_         ==0) throw EXCEPTION_WARNING("Jet collection branch is not found","",0);
  }
  catch (const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }
}


// -----------------------------------------------------------------------------
// Initialize Datum
// -----------------------------------------------------------------------------
MAbool DelphesDataFormat::InitializeData(TBranch*& branch,TClonesArray*& array)
{
  // Convert to BranchElement
  TBranchElement* element = dynamic_cast<TBranchElement*>(branch);
  if (element==0) return false;

  // Get ClassInformation
  const MAchar *className = element->GetClonesName();
  MAint32 size = element->GetMaximum();
  TClass *cl = gROOT->GetClass(className);
  if (cl==0) return false;

  // Convert data to TClonesArray
  array = new TClonesArray(cl, size);
  //  array->SetName(branchName);
  branch->SetAddress(&array);

  // ok
  return true;
}
