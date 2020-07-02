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


#ifndef DELPHESMA5TUNE_TREE_READER_h
#define DELPHESMA5TUNE_TREE_READER_h


// STL headers
#include <iostream>
#include <vector>
#include <map>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Vector/MALorentzVector.h"
#include "SampleAnalyzer/Interfaces/root/TreeReaderBase.h"

// ROOT headers
#include <TChain.h>
#include <TLorentzVector.h>
#include <TObject.h>
#include <TFile.h>


// Delphes header
class ExRootTreeReader;

namespace MA5
{

class DelphesMA5tuneTreeReader : public TreeReaderBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /// Tree reader
  ExRootTreeReader *treeReader_;

  /// Number of total entries in the file
  MAint64 total_nevents_;

  /// Number of entries read by MA5
  MAint64 read_nevents_;

  /// Pointers to the different branches
  TClonesArray *branchJet_;
  TClonesArray *branchElectron_;
  TClonesArray *branchPhoton_;
  TClonesArray *branchMuon_;
  TClonesArray *branchMissingET_;
  TClonesArray *branchScalarHT_;
  TClonesArray *branchGenParticle_;
  TClonesArray *branchTrack_;
  TClonesArray *branchEvent_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  DelphesMA5tuneTreeReader()
  { InitializeVariables(); } 

  /// Constructor with arguments
  DelphesMA5tuneTreeReader(TFile* source, TTree* tree): TreeReaderBase(source,tree)
  { }

  /// Destructor
  virtual ~DelphesMA5tuneTreeReader()
  { }

  /// Initialize
  virtual MAbool Initialize();

  /// Read the header
  virtual MAbool ReadHeader(SampleFormat& mySample);

  /// Read the event
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample);

  /// Finalize the event
  virtual MAbool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent);

  std::vector<MAint32> MuonIndex_;
  std::vector<MAint32> ElectronIndex_;

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
    branchEvent_=0;
  }

  /// Get the file size
  virtual MAint64 GetFinalPosition()
  { return total_nevents_; }

  /// Get the position in file (in octet)
  virtual MAint64 GetPosition()
  { return read_nevents_; }


};

}

#endif
