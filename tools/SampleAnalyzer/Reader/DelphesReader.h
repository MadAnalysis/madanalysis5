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


#ifndef DELPHES_READER_h
#define DELPHES_READER_h

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

#ifdef DELPHES_USE

// Delphes header
#include "external/ExRootAnalysis/ExRootTreeReader.h"

namespace MA5
{

class DelphesReader : public ReaderBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /// Input file stream
  TFile * source_;

  /// Name of the input file (without prefix such as file: or rfio:)
  std::string filename_;

  /// Tree reader
  ExRootTreeReader *treeReader_;

  /// Tree
  TTree* tree_;

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

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  DelphesReader()
  { InitializeVariables(); } 

	/// Destructor
  virtual ~DelphesReader()
  { }

  /// Initialize
  virtual bool Initialize(const std::string& rawfilename,
                          const Configuration& cfg);

  /// Read the header
  virtual bool ReadHeader(SampleFormat& mySample);

  /// Finalize the header
  virtual bool FinalizeHeader(SampleFormat& mySample);

  /// Read the event
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample);

  /// Finalize the event
  virtual bool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent);

  /// Finalize
  virtual bool Finalize();

 private:

  void FillEvent(EventFormat& myEvent, SampleFormat& mySample);

  void InitializeVariables()
  {
    source_=0;
    filename_="";
    treeReader_=0;
    tree_=0;
    total_nevents_=0;
    read_nevents_=0;
    branchJet_=0;
    branchElectron_=0;
    branchPhoton_=0;
    branchMuon_=0;
    branchMissingET_=0;
    branchScalarHT_=0;
    branchGenParticle_=0;
  }

  /// Get the file size
  virtual Long64_t GetFinalPosition()
  { return total_nevents_; }

  /// Get the file size
  virtual Long64_t GetFileSize()
  {
    Long64_t length = 0;
    std::ifstream myinput(filename_.c_str());
    myinput.seekg(0,std::ios::beg);
    myinput.seekg(0,std::ios::end);
    length = myinput.tellg();
    myinput.close();
    return length;
  }

  /// Get the position in file (in octet)
  virtual Long64_t GetPosition()
  { return read_nevents_; }

};

};

#endif
#endif
