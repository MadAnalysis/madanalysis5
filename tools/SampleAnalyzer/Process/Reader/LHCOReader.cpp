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


#include "SampleAnalyzer/Process/Reader/LHCOReader.h"
#include <sstream>
#include <cmath>

using namespace MA5;

// -----------------------------------------------------------------------------
// ReadHeader
// -----------------------------------------------------------------------------
bool LHCOReader::ReadHeader(SampleFormat& mySample)
{
  EndOfFile_=false;

  mySample.InitializeRec();
  mySample.SetSampleFormat(MA5FORMAT::LHCO);

  firstevent_=true;
  saved_=false;

  // Normal end
  return true;
}

// -----------------------------------------------------------------------------
// FinalizeHeader
// -----------------------------------------------------------------------------
bool LHCOReader::FinalizeHeader(SampleFormat& mySample)
{
  // Normal end
  return true;
}


// -----------------------------------------------------------------------------
// ReadEvent
// -----------------------------------------------------------------------------
StatusCode::Type LHCOReader::ReadEvent(EventFormat& myEvent, SampleFormat& mySample)
{
  if(EndOfFile_) return StatusCode::FAILURE;

  myEvent.InitializeRec();

  bool EndOfLoop = false;
  
  // Declarging a new string for line
  std::string line;
  std::string tmp;
     
  do
  {
    std::stringstream str;
    str.str("");
      
    if (!ReadLine(line))
    {
      EndOfFile_=true;
      return StatusCode::KEEP;
    }
    str << line;
    str >> tmp;

    if (saved_)
    {
      FillEventInitLine(savedline_,myEvent);
      saved_=false;
    }

    if(tmp=="0")
    {
      if(firstevent_ )
	    {
	      FillEventInitLine(line,myEvent);
        firstevent_=false;
        continue;
	    }
      else 
	    {
	      EndOfLoop = true;
	      savedline_=line;
	      saved_=true;
	      continue;
	    }
    }

    FillEventParticleLine(line,myEvent);
      
  }
  while(!EndOfLoop);
  
  // Normal end
  return StatusCode::KEEP; 
}


// -----------------------------------------------------------------------------
// FinalizeEvent
// -----------------------------------------------------------------------------
bool LHCOReader::FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)
{
  // MHT & THT
  for (unsigned int i=0; i<myEvent.rec()->jets_.size();i++)
  {
    myEvent.rec()->MHT_ -= myEvent.rec()->jets_[i].momentum();
    myEvent.rec()->THT_ += myEvent.rec()->jets_[i].pt();
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

  // Normal end
  return true; 
}


// -----------------------------------------------------------------------------                                     
// FillEventInitLine                                     
// -----------------------------------------------------------------------------     
void LHCOReader::FillEventInitLine(const std::string& line, EventFormat& myEvent)
{
  std::stringstream str;
  std::string muf;
  str << line;
  str >> muf;
}
// -----------------------------------------------------------------------------                                     
// FillEventParticleLine                                     
// -----------------------------------------------------------------------------     
void LHCOReader::FillEventParticleLine(const std::string& line, EventFormat& myEvent)
{
  std::stringstream str;

  // 1rst column : line number
  std::string firstchar;
  str << line;
  str >> firstchar;

  Double_t  tmp;  // temporary variable to fill in LorentzVector
  Double_t  eta;
  Double_t  phi;  // to define the MET
  Double_t  pt;
  Double_t  mass;

  // 2nd column : object type
  std::string muf;
  str >> muf;

  // -------------------------------------------
  //                  PHOTON
  // -------------------------------------------
  if (muf=="0")
  {
    RecPhotonFormat * photon = myEvent.rec()->GetNewPhoton();

    // 3rd column
    str >> eta; 

    // 4th column
    str >> phi; 

    // 5th column
    str >> pt; 

    // 6th column
    str >> mass;
    photon->momentum_.SetPtEtaPhiM(pt,eta,phi,mass);

    // 7th column 
    str >> tmp; 

    // 8th column
    str >> tmp;

    // 9th column
    str >> photon->HEoverEE_;
  }

  // -------------------------------------------
  //                  ELECTRON
  // -------------------------------------------
  else if (muf=="1")
  {
    RecLeptonFormat * elec = myEvent.rec()->GetNewElectron();

    // 3rd column
    str >> eta; 

    // 4th column
    str >> phi; 

    // 5th column
    str >> pt; 

    // 6th column
    str >> mass; 
    elec->momentum_.SetPtEtaPhiM(pt,eta,phi,mass);

    // 7th column 
    str >> tmp; 
    if(tmp<0) elec->charge_ = false;
    else elec->charge_= true ;

    // 8th column
    str >> tmp;

    // 9th column
    str >> elec->HEoverEE_;
  }

  // -------------------------------------------
  //                   MUON
  // -------------------------------------------
  else if (muf=="2")
  {
    RecLeptonFormat * muon = myEvent.rec()->GetNewMuon();

    // 3rd column
    str >> eta; 

    // 4th column
    str >> phi; 

    // 5th column
    str >> pt; 

    // 6th column
    str >> mass; 
    muon->momentum_.SetPtEtaPhiM(pt,eta,phi,mass);

    // 7th column : electric charge
    str >> tmp; 
    if(tmp<0) muon->charge_ = false;
    else muon->charge_= true ;

    // 8th column
    str >> tmp;

    // 9th column : isolation
    str >> tmp;
    muon->sumPT_isol_=std::floor(tmp);

    Float_t ET_PT=tmp-muon->sumPT_isol_;
    Bool_t test=false;
    for (unsigned int j=0;j<5;j++)
    {
      ET_PT*=10;
      if (std::floor(ET_PT)==ET_PT)
      {
        test=true;
        break;
      }
    }
    if (test) muon->sumET_isol_=std::floor(ET_PT)*muon->pt();
    else muon->sumET_isol_=0;
  }

  // -------------------------------------------
  //                   TAU
  // -------------------------------------------
  else if (muf=="3")
  {
    RecTauFormat * tau = myEvent.rec()->GetNewTau();

    // 3rd column
    str >> eta; 

    // 4th column
    str >> phi; 

    // 5th column
    str >> pt; 

    // 6th column
    str >> mass; 
    tau->momentum_.SetPtEtaPhiM(pt,eta,phi,mass);

    // 7th column 
    str >> tmp; 
    if(tmp<0) tau->charge_ = false;
    else tau->charge_= true ;

    // 8th column
    str >> tmp;

    // 9th column
    str >> tmp; 
    tau->HEoverEE_=tmp;
  }

  // -------------------------------------------
  //                    JET
  // -------------------------------------------
  else if (muf=="4")
  {
    RecJetFormat * jet = myEvent.rec()->GetNewJet(); 

    // 3rd column : eta
    str >> eta; 

    // 4th column
    str >> phi; 

    // 5th column
    str >> pt; 

    // 6th column
    str >> mass; 
    jet->momentum_.SetPtEtaPhiM(pt,eta,phi,mass);

    // 7th column : ntracks
    str >> tmp; 
    jet->ntracks_=static_cast<unsigned short>(tmp); 

    // 8th column : btag
    str >> tmp;
    if ( tmp == 1. || tmp ==2.) jet->btag_=true;
    else jet->btag_ =false;
    str >> jet->HEoverEE_;
  }

  // -------------------------------------------
  //                    MET
  // -------------------------------------------
  else if (muf=="6")
  {
    // 3rd column : eta
    str >> tmp; 

    // 4th column : phi
    str >> phi; 

    // 5th column : pt
    str >> tmp;
    myEvent.rec()->MET_.momentum_.SetPxPyPzE(tmp*cos(phi),
                                             tmp*sin(phi),
                                             0.,
                                             tmp);
  }

  // -------------------------------------------
  //                  other ...
  // -------------------------------------------
  else if (firstchar!="0")
  {
    WARNING << "Unknown type of object : " << muf << endmsg;
  }
}
