////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <http://madanalysis.irmp.ucl.ac.be>
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


#ifndef HEPMC_READER_h
#define HEPMC_READER_h

// SampleAnalyzer headers
#include "SampleAnalyzer/Reader/ReaderTextBase.h"

namespace MA5
{

class HEPMCReader : public ReaderTextBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
  
  bool firstevent_;
  bool endevent_;
  bool saved_;
  bool EndOfFile_;
  bool warnmother_;
  int partcode_;
  int vertcode_;
  int nevents_ ;              // number of events
  float energy_unit_;
  float length_unit_;
  double event_xsection_;     // event cross section
  double event_xsection_err_; // event cross section error
  std::string savedline_;     // last saved line
  std::vector<std::string> weightnames_;
  bool firstHeavyIons_;

  struct HEPVertex
  {
    Int_t barcode_;
    Double_t ctau_;
  };
  
  HEPVertex current_vertex_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:
  //! Constructor without argument
  HEPMCReader()
  { firstevent_=false; event_xsection_=0; event_xsection_err_=0; firstHeavyIons_=true;}

  //! Destructor
  virtual ~HEPMCReader()
    { }
  
  //! Read the header
  virtual bool ReadHeader(SampleFormat& mySample);
  
  //! Finalize the header
  virtual bool FinalizeHeader(SampleFormat& mySample);
  
  //! Read the event
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample);
  
  //! Finalize the event
  virtual bool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent);
  
 private:
  
  Bool_t FillEvent(const std::string& line, EventFormat& myEvent, SampleFormat& mySample);
  void FillEventInformations(const std::string& line, EventFormat& myEvent);
  void FillEventXS(const std::string& line);
  void FillEventUnits(const std::string& line);
  void FillEventPDFInfo(const std::string& line, SampleFormat& mySample, EventFormat& myEvent);
  void FillEventParticleLine(const std::string& line, EventFormat& myEvent);
  void FillEventVertexLine(const std::string& line, EventFormat& myEvent);
  void SetMother(MCParticleFormat* part, EventFormat& myEvent);
  Bool_t FillWeightNames(const std::string& line);
  Bool_t FillHeavyIons(const std::string& line);

};

}

#endif
