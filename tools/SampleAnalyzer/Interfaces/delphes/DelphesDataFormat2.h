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


#ifndef DELPHES_DATA_FORMAT2_h
#define DELPHES_DATA_FORMAT2_h


// ROOT headers
#include <TTree.h>
#include <TClonesArray.h>

// STL headers
#include <iostream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"


namespace MA5
{

struct DelphesDataFormat2
{
  /// Pointers to data
  TClonesArray* FatJet_;
  TClonesArray* Jet_;
  TClonesArray* Electron_;
  TClonesArray* Photon_;
  TClonesArray* Muon_;
  TClonesArray* MET_;
  TClonesArray* HT_;
  TClonesArray* GenParticle_;
  TClonesArray* Track_;
  TClonesArray* Tower_;
  TClonesArray* Event_;
  TClonesArray* EFlowTrack_;
  TClonesArray* EFlowPhoton_;
  TClonesArray* EFlowNeutral_;

   // Switch for MA5card
  MAbool delphesMA5card_;

  /// Constructor without arguments
  DelphesDataFormat2();

  /// Destructor
  ~DelphesDataFormat2();


};

}

#endif
