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


#ifndef TREE_READER_BASE_h
#define TREE_READER_BASE_h

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/Configuration.h"
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"
#include "SampleAnalyzer/Commons/Base/StatusCode.h"

// ROOT header
#include <TChain.h>
#include <TLorentzVector.h>
#include <TObject.h>
#include <TFile.h>

// STL header
#include <iostream>

namespace MA5
{

class TreeReaderBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /// Input file stream
  TFile* source_;

  /// Tree
  TTree* tree_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  TreeReaderBase()
  { source_=0; tree_=0; } 

  /// Constructor with arguments
  TreeReaderBase(TFile* source, TTree* tree)
  { source_=source; tree_=tree; }

	/// Destructor
  virtual ~TreeReaderBase()
  { }

  /// Read the header
  virtual bool Initialize()=0;

  /// Read the header
  virtual bool ReadHeader(SampleFormat& mySample)=0;

  /// Read the event
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample)=0;

  /// Finalize the event
  virtual bool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)=0;

  /// Get the file size
  virtual Long64_t GetFinalPosition()=0;

  /// Get the position in file (in octet)
  virtual Long64_t GetPosition()=0;

};

};

#endif
