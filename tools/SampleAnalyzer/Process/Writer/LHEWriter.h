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


#ifndef LHE_WRITER_BASE_h
#define LHE_WRITER_BASE_h

// STL headers
#include <fstream>
#include <iostream>
#include <sstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Writer/WriterTextBase.h"
#include "SampleAnalyzer/Process/Writer/LHEParticleFormat.h"

namespace MA5
{

class LHEWriter : public WriterTextBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:



  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  LHEWriter() : WriterTextBase()
  {}

	/// Destructor
  virtual ~LHEWriter()
  {}

  /// Read the sample (virtual)
  virtual bool WriteHeader(const SampleFormat& mySample);

  /// Read the event (virtual)
  virtual bool WriteEvent(const EventFormat& myEvent, 
                          const SampleFormat& mySample);

  /// Finalize the event (virtual)
  virtual bool WriteFoot(const SampleFormat& mySample);
 
 private:

  /// Writing event global information
  bool WriteEventHeader(const EventFormat& myEvent,
                        unsigned int nevents);

  /// Writing a particle
  void WriteParticle(const MCParticleFormat& myPart, Int_t mother1, Int_t mother2, 
                     Int_t statuscode, LHEParticleFormat& lhe);

  static std::string FortranFormat_SimplePrecision(Float_t value,UInt_t precision=7); 
  static std::string FortranFormat_DoublePrecision(Double_t value,UInt_t precision=11); 

  // Writing a reconstructed jet
  void WriteJet(const RecJetFormat& jet, LHEParticleFormat& lhe, Int_t& mother);
  void WriteMuon(const RecLeptonFormat& muon, LHEParticleFormat& lhe, Int_t& mother);
  void WriteElectron(const RecLeptonFormat& electron, LHEParticleFormat& lhe, Int_t& mother);
  void WritePhoton(const RecPhotonFormat& photon, LHEParticleFormat& lhe, Int_t& mother);
  void WriteTau(const RecTauFormat& tau, LHEParticleFormat& lhe, Int_t& mother);
  void WriteMET(const ParticleBaseFormat& met, LHEParticleFormat& lhe);


};

}

#endif
