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


// SampleHeader headers
#include "SampleAnalyzer/Interfaces/delphes/DelphesMemoryInterface.h"

// Delphes header
#include "classes/DelphesClasses.h"


using namespace MA5;


// -----------------------------------------------------------------------------
// Constructor without arguments
// -----------------------------------------------------------------------------
DelphesMemoryInterface::DelphesMemoryInterface()
{
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
  delphesMA5card_ = false;
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
DelphesMemoryInterface::~DelphesMemoryInterface()
{
  // nothing to delete
}


// -----------------------------------------------------------------------------
// Print TFolder  -- ONLY FOR DEBUG
// -----------------------------------------------------------------------------
void DelphesMemoryInterface::Print(TFolder* delphesFolder)
{
  if (delphesFolder==0) std::cout << "Empty DelphesFolder" << std::endl;
  TCollection* folders = delphesFolder->GetListOfFolders();
  folders->Print();
  TFolder* myexport = dynamic_cast<TFolder*>(delphesFolder->FindObject("Export"));
  if (myexport==0) std::cout << "No export" << std::endl;
  myexport->Print();

}


// -----------------------------------------------------------------------------
// GetCollection
// -----------------------------------------------------------------------------
TObjArray* DelphesMemoryInterface::GetCollection(TFolder* delphesFolder, 
                                                 const std::map<std::string,std::string>& table,
                                                 const std::string& name)
{
  std::map<std::string,std::string>::const_iterator it =table.find(name);
  if (it==table.end()) return 0;

  std::string pathname="Export/"+it->second;
  return dynamic_cast<TObjArray*>(delphesFolder->FindObject(pathname.c_str()));
}


// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------
void DelphesMemoryInterface::Initialize(TFolder* delphesFolder, 
                                        const std::map<std::string,std::string>& table, 
                                        bool MA5card)
{
  // DelphesMA5 card ?
  delphesMA5card_=MA5card;

  // Official Delphes collections
  //  GenJet_       = GetCollection(delphesFolder,table,"GenJet");
  MET_          = GetCollection(delphesFolder,table,"MissingET");
  Tower_        = GetCollection(delphesFolder,table,"Tower");
  Track_        = GetCollection(delphesFolder,table,"Track");
  HT_           = GetCollection(delphesFolder,table,"ScalarHT");
  EFlowTrack_   = GetCollection(delphesFolder,table,"EFlowTrack");
  EFlowPhoton_  = GetCollection(delphesFolder,table,"EFlowPhoton");
  EFlowNeutral_ = GetCollection(delphesFolder,table,"EFlowNeutralHadron");

  // MA5 Delphes collections
  if (MA5card)
  {
    Jet_      = GetCollection(delphesFolder,table,"JetMA5");
    Electron_ = GetCollection(delphesFolder,table,"ElectronMA5");
    Muon_     = GetCollection(delphesFolder,table,"MuonMA5");
    Photon_   = GetCollection(delphesFolder,table,"PhotonMA5");
  }
  else
  {
    Jet_      = GetCollection(delphesFolder,table,"Jet");
    Electron_ = GetCollection(delphesFolder,table,"Electron");
    Muon_     = GetCollection(delphesFolder,table,"Muon");
    Photon_   = GetCollection(delphesFolder,table,"Photon");
  }

  // Display warning to main branches
  if (MET_==0)
  {
    WARNING << "Delphes output: MET is not found" << endmsg;
  }
  if (Electron_==0)
  {
    WARNING << "Delphes output: Electron collection is not found" << endmsg;
  }
  if (Muon_==0)
  {
    WARNING << "Delphes output: Muon collection is not found" << endmsg;
  }
  if (Photon_==0)
  {
    WARNING << "Delphes output: Photon collection is not found" << endmsg;
  }
  if (Jet_==0)
  {
    WARNING << "Jet output: Jet collection is not found" << endmsg;
  }

}


// -----------------------------------------------------------------------------
// TransfertDELPHEStoMA5
// -----------------------------------------------------------------------------
bool DelphesMemoryInterface::TransfertDELPHEStoMA5(SampleFormat& mySample, EventFormat& myEvent)
{
  // --------------Jet collection
  if (Jet_!=0)
  {
    for (unsigned int i=0;i<static_cast<UInt_t>(Jet_->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(Jet_->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th jet" << endmsg;
        continue;
      }
      if (cand->TauTag==1)
      {
        RecTauFormat* tau = myEvent.rec()->GetNewTau();
        tau->momentum_ = cand->Momentum;
        if (cand->Charge>0) tau->charge_=true; else tau->charge_=false;
        if (cand->Eem!=0) tau->HEoverEE_ = cand->Ehad/cand->Eem; else tau->HEoverEE_ = 999.;
        tau->ntracks_ = 0; // To fix later
      }
      else
      {
        RecJetFormat* jet = myEvent.rec()->GetNewJet();
        jet->momentum_ = cand->Momentum;
        jet->btag_ = cand->BTag;
        if (cand->Eem!=0) jet->HEoverEE_ = cand->Ehad/cand->Eem; else jet->HEoverEE_ = 999.;
        jet->ntracks_ = 0; // To fix later
      }
    }
  }

  // --------------GenJet collection
  /*  if (genjetsArray!=0)
  {
    for (unsigned int i=0;i<static_cast<UInt_t>(genjetsArray->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(genjetsArray->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th genjet" << endmsg;
        continue;
      }
      RecJetFormat* genjet = myEvent.rec()->GetNewGenJet();
      genjet->momentum_ = cand->Momentum;
      genjet->btag_ = cand->BTag;
    }
    }*/

  // --------------Muon collection
  if (Muon_!=0)
  {
    for (unsigned int i=0;i<static_cast<UInt_t>(Muon_->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(Muon_->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th muon" << endmsg;
        continue;
      }
      RecLeptonFormat* muon = myEvent.rec()->GetNewMuon();
      muon->momentum_ = cand->Momentum;
      muon->SetCharge(cand->Charge);
    }
  }

  // --------------Electron collection
  if (Electron_!=0)
  {
    for (unsigned int i=0;i<static_cast<UInt_t>(Electron_->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(Electron_->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th electron" << endmsg;
        continue;
      }
      RecLeptonFormat* elec = myEvent.rec()->GetNewElectron();
      elec->momentum_ = cand->Momentum;
      elec->SetCharge(cand->Charge);
    }
  }

  // --------------Photon collection
  if (Photon_!=0)
  {
    for (unsigned int i=0;i<static_cast<UInt_t>(Photon_->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(Photon_->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th photon" << endmsg;
        continue;
      }
      else
      {
        RecPhotonFormat* photon = myEvent.rec()->GetNewPhoton();
        photon->momentum_ = cand->Momentum;
        if (cand->Eem!=0) photon->HEoverEE_ = cand->Ehad/cand->Eem; else photon->HEoverEE_ = 999.;
      }
    }
  }

  // --------------Track collection
  if (Track_!=0)
  {
    for (unsigned int i=0;i<static_cast<UInt_t>(Track_->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(Track_->At(i));
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

  // --------------MET
  if (MET_!=0)
  {
    Candidate* metCand = dynamic_cast<Candidate*>(MET_->At(0));
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

  // --------------HT
  if (HT_!=0)
  {
    Candidate* Cand = dynamic_cast<Candidate*>(HT_->At(0));
    if (Cand==0) 
    {
      ERROR << "impossible to access the HT" << endmsg;
    }
    else
    {
      myEvent.rec()->THT_=Cand->Momentum.Pt();
    }
  }

  // --------------Tower collection
  if (Tower_!=0)
  {
    for (unsigned int i=0;i<static_cast<UInt_t>(Tower_->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(Tower_->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th track" << endmsg;
        continue;
      }
      RecTowerFormat* tower = myEvent.rec()->GetNewTower();
      tower->momentum_=cand->Momentum;
    }
  }

  // --------------EFlowTrack collection
  if (EFlowTrack_!=0)
  {
    for (unsigned int i=0;i<static_cast<UInt_t>(EFlowTrack_->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(EFlowTrack_->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th track" << endmsg;
        continue;
      }
      RecTrackFormat * track = myEvent.rec()->GetNewEFlowTrack();
      track->momentum_=cand->Momentum;
    }
  }

  // --------------EFlowPhoton collection
  if (EFlowPhoton_!=0)
  {
    for (unsigned int i=0;i<static_cast<UInt_t>(EFlowPhoton_->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(EFlowPhoton_->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th track" << endmsg;
        continue;
      }
      RecParticleFormat * tower = myEvent.rec()->GetNewEFlowPhoton();
      tower->momentum_=cand->Momentum;
    }
  }

  // --------------EFlowNeutral collection
  if (EFlowNeutral_!=0)
  {
    for (unsigned int i=0;i<static_cast<UInt_t>(EFlowNeutral_->GetEntries());i++)
    {
      Candidate* cand = dynamic_cast<Candidate*>(EFlowNeutral_->At(i));
      if (cand==0) 
      {
        ERROR << "impossible to access the " << i+1 << "th track" << endmsg;
        continue;
      }
      RecParticleFormat * tower = myEvent.rec()->GetNewEFlowNeutralHadron();
      tower->momentum_=cand->Momentum;
    }
  }

  return true;
}
