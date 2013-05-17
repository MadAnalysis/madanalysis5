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


#ifndef ROOT_READER_h
#define ROOT_READER_h

// SampleAnalyzer headers
#include "SampleAnalyzer/Reader/ReaderTextBase.h"
#include "SampleAnalyzer/Reader/FACdataformat.h"

// ROOT header
#include <TChain.h>
#include <TLorentzVector.h>
#include <TObject.h>
#include <TFile.h>

// STL header
#include <iostream>

#ifdef FAC_USE

namespace MA5
{

class ROOTReader : public ReaderTextBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  TFile * source_;
  TTree * tree_;
  Int_t nevents_;
  Int_t ncurr_;
  FAC::EventFormat* evt_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  ROOTReader()
  { } 

	/// Destructor
  virtual ~ROOTReader()
  { }

  /// Read the header
  virtual bool ReadHeader(SampleFormat& mySample);

  /// Finalize the header
  virtual bool FinalizeHeader(SampleFormat& mySample);

  /// Read the event
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample);

  /// Finalize the event
  virtual bool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent);

 private:

  void FillEvent(EventFormat& myEvent, SampleFormat& mySample);

};

};

#endif
#endif
