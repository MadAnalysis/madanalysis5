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


#ifndef DELPHES_MEMORY_INTERFACE_h
#define DELPHES_MEMORY_INTERFACE_h


// STL headers
#include <string>
#include <map>
#include <iostream>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Base/StatusCode.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Base/DetectorBase.h"


class TObjArray;
class TFolder;

namespace MA5
{

class DelphesMemoryInterface
{
 public : 

  /// Pointers to data
  TObjArray* Jet_;
  TObjArray* FatJet_;
  TObjArray* Electron_;
  TObjArray* Photon_;
  TObjArray* Muon_;
  TObjArray* MET_;
  TObjArray* HT_;
  TObjArray* GenParticle_;
  TObjArray* Track_;
  TObjArray* Tower_;
  TObjArray* Event_;
  TObjArray* EFlowTrack_;
  TObjArray* EFlowPhoton_;
  TObjArray* EFlowNeutral_;

   // Switch for MA5card
  MAbool delphesMA5card_;

  /// Constructor without arguments
  DelphesMemoryInterface();

  /// Destructor
  ~DelphesMemoryInterface();

  /// Initialize with delphesFolder
  void Initialize(TFolder* delphesFolder);

  /// Print -- DEBUG --
  static void Print(TFolder* delphesFolder);

  void Initialize(TFolder* delphesFolder, const std::map<std::string,std::string>& table, MAbool MA5card);

  TObjArray* GetCollection(TFolder* delphesFolder, 
                           const std::map<std::string,std::string>& table,
                           const std::string& name);

  MAbool TransfertDELPHEStoMA5(SampleFormat& mySample, EventFormat& myEvent);

};

}

#endif
