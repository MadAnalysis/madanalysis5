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


#ifndef LHCO_WRITER_BASE_h
#define LHCO_WRITER_BASE_h

// STL headers
#include <fstream>
#include <iostream>
#include <sstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Writer/WriterTextBase.h"
#include "SampleAnalyzer/Process/Writer/LHCOParticleFormat.h"

namespace MA5
{

class LHCOWriter : public WriterTextBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  UInt_t counter_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  LHCOWriter() : WriterTextBase()
  { counter_=0; }

	/// Destructor
  virtual ~LHCOWriter()
  {}

  /// Read the sample (virtual)
  virtual bool WriteHeader(const SampleFormat& mySample);

  /// Read the event (virtual)
  virtual bool WriteEvent(const EventFormat& myEvent, 
                          const SampleFormat& mySample);

  /// Finalize the event (virtual)
  virtual bool WriteFoot(const SampleFormat& mySample);
 
 private:


  // Writing a reconstructed jet

  void WriteJet(const RecJetFormat& jet, LHCOParticleFormat* lhco);
  void WriteMuon(const RecLeptonFormat& muon, LHCOParticleFormat* lhco, const RecEventFormat* myEvent, unsigned int npart);
  void WriteElectron(const RecLeptonFormat& electron, LHCOParticleFormat* lhco);
  void WritePhoton(const RecPhotonFormat& photon, LHCOParticleFormat* lhco);
  void WriteTau(const RecTauFormat& tau, LHCOParticleFormat* lhco);
  void WriteMET(const ParticleBaseFormat& met, LHCOParticleFormat* lhco);


};

}

#endif
