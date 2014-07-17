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


#ifndef DELPHES_TREE_READER_h
#define DELPHES_TREE_READER_h

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/TreeReaderBase.h"

// ROOT header
#include <TChain.h>
#include <TLorentzVector.h>
#include <TObject.h>
#include <TFile.h>

// STL header
#include <iostream>

// Delphes header
class ExRootTreeReader;

namespace MA5
{

class DelphesTreeReader : public TreeReaderBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /// Tree reader
  ExRootTreeReader *treeReader_;

  /// Number of total entries in the file
  Long64_t total_nevents_;

  /// Number of entries read by MA5
  Long64_t read_nevents_;

  /// Pointers to the different branches
  TClonesArray *branchJet_;
  TClonesArray *branchElectron_;
  TClonesArray *branchPhoton_;
  TClonesArray *branchMuon_;
  TClonesArray *branchMissingET_;
  TClonesArray *branchScalarHT_;
  TClonesArray *branchGenParticle_;
  TClonesArray *branchTrack_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  DelphesTreeReader()
  { InitializeVariables(); } 

  /// Constructor with arguments
  DelphesTreeReader(TFile* source, TTree* tree): TreeReaderBase(source,tree)
  { }

	/// Destructor
  virtual ~DelphesTreeReader()
  { }

  /// Initialize
  virtual bool Initialize();

  /// Read the header
  virtual bool ReadHeader(SampleFormat& mySample);

  /// Read the event
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample);

  /// Finalize the event
  virtual bool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent);


 private:

  void FillEvent(EventFormat& myEvent, SampleFormat& mySample);

  void InitializeVariables()
  {
    treeReader_=0;
    total_nevents_=0;
    read_nevents_=0;
    branchJet_=0;
    branchElectron_=0;
    branchPhoton_=0;
    branchMuon_=0;
    branchMissingET_=0;
    branchScalarHT_=0;
    branchGenParticle_=0;
    branchTrack_=0;
  }

  /// Get the file size
  virtual Long64_t GetFinalPosition()
  { return total_nevents_; }

  /// Get the position in file (in octet)
  virtual Long64_t GetPosition()
  { return read_nevents_; }


};

}

#endif
