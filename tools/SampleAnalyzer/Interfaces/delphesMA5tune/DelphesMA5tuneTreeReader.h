////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://github.com/MadAnalysis/madanalysis5>
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


/**
 * @file DelphesMA5tuneTreeReader.h
 * @brief Reader of DelphesMA5tune ROOT files.
 */

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

/** @brief Reader of ROOT files produced by DelphesMA5tune (Delphes patched with isolation cones). */
class DelphesMA5tuneTreeReader : public TreeReaderBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /** @brief Delphes tree reader (owned). */
  ExRootTreeReader *treeReader_;

  /** @brief Number of entries in the tree. */
  MAint64 total_nevents_;

  /** @brief Number of entries read so far. */
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

  /** @brief Constructor without argument. */
  DelphesMA5tuneTreeReader()
  { InitializeVariables(); } 

  /**
   * @brief Constructor.
   *
   * @param source ROOT file.
   * @param tree DelphesMA5tune tree.
   */
  // FIXME: unlike the default constructor, this one does not call InitializeVariables().
  DelphesMA5tuneTreeReader(TFile* source, TTree* tree): TreeReaderBase(source,tree)
  { }

  /** @brief Destructor. */
  virtual ~DelphesMA5tuneTreeReader()
  { }

  /**
   * @brief Create the tree reader and find the branches.
   *
   * @return true.
   */
  virtual MAbool Initialize();

  /**
   * @brief Set the sample format (DelphesMA5tune).
   *
   * @param mySample sample.
   * @return true.
   */
  virtual MAbool ReadHeader(SampleFormat& mySample);

  /**
   * @brief Read the next entry and fill the event.
   *
   * @param myEvent current event.
   * @param mySample current sample.
   * @return StatusCode::KEEP, or StatusCode::FAILURE at the end of the tree.
   */
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample);

  /**
   * @brief Compute the global observables (MHT, THT, TET, Meff and their MC-level counterparts).
   *
   * @param mySample current sample.
   * @param myEvent current event.
   * @return true.
   */
  virtual MAbool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent);

  /** @brief Index (+1) of the generator particle of each muon (0 if none). */
  std::vector<MAint32> MuonIndex_;
  /** @brief Index (+1) of the generator particle of each electron (0 if none). */
  std::vector<MAint32> ElectronIndex_;

 private:

  /**
   * @brief Fill the MC and reconstructed event from the branches.
   *
   * @param myEvent current event.
   * @param mySample current sample.
   */
  void FillEvent(EventFormat& myEvent, SampleFormat& mySample);

  /** @brief Reset the pointers and the entry counters. */
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

  /**
   * @brief Accessor to the number of entries (used by the progress bar).
   *
   * @return the number of entries.
   */
  virtual MAint64 GetFinalPosition()
  { return total_nevents_; }

  /**
   * @brief Accessor to the number of entries read (used by the progress bar).
   *
   * @return the number of entries read.
   */
  virtual MAint64 GetPosition()
  { return read_nevents_; }


};

}

#endif
