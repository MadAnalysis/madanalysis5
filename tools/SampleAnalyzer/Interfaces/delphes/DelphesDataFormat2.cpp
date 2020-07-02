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
#include "SampleAnalyzer/Interfaces/delphes/DelphesDataFormat2.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

// ROOT headers
#include <TBranchElement.h>
#include <TROOT.h>


using namespace MA5;

// -----------------------------------------------------------------------------
// Constructor without arguments
// -----------------------------------------------------------------------------
DelphesDataFormat2::DelphesDataFormat2()
{
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
  delphesMA5card_ = false;
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
DelphesDataFormat2::~DelphesDataFormat2()
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
}
