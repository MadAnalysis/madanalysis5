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


#ifndef DJR_EXTRACTOR_H
#define DJR_EXTRACTOR_H


// STL headers
#include <vector>
#include <map>
#include <string>

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Commons/Base/Configuration.h"


namespace fastjet
{
  class JetDefinition;
  class PseudoJet;
}

namespace MA5
{
class DJRextractor
{

//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  private :

  /// clustering algorithm [FastJet]
  fastjet::JetDefinition* JetDefinition_;

  /// User configuration
  MAuint32  merging_njets_;
  MAuint8   merging_nqmatch_;
  MAbool    merging_nosingrad_;


//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
 public : 

  /// Constructor
  DJRextractor() 
  {
    // Jet algo
    JetDefinition_=0;
    // Options
    merging_nqmatch_=4;
    merging_nosingrad_=false;
  }

  /// Destructor
  ~DJRextractor() {}

  /// Initialization
  MAbool Initialize();

  /// Finalization
  void Finalize();

  /// Execution
  MAbool Execute(SampleFormat& sample, const EventFormat& event, std::vector<MAdouble64>& DJR);

  /// Extracting the number of additionnal jets contained in the event 
  MAuint32 ExtractJetNumber(const MCEventFormat* myEvent, MCSampleFormat* mySample);

  /// Selecting particles
  void SelectParticles(std::vector<fastjet::PseudoJet>& inputs, const MCEventFormat* myEvent);

  /// Extracting the DJR information
  void ExtractDJR(const std::vector<fastjet::PseudoJet>& inputs,std::vector<MAdouble64>& DJRvalues);

  MAdouble64 rapidity(MAdouble64 px, MAdouble64 py, MAdouble64 pz);


};

}

#endif

