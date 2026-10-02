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
 * @file DelphesTreeReader.h
 * @brief Reader of Delphes ROOT files.
 */

#ifndef DELPHES_TREE_READER_h
#define DELPHES_TREE_READER_h


// STL headers
#include <iostream>
#include <vector>
#include <map>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Vector/MALorentzVector.h"
#include "SampleAnalyzer/Interfaces/root/TreeReaderBase.h"
#include "SampleAnalyzer/Interfaces/delphes/DelphesDataFormat.h"

// ROOT headers
#include <TChain.h>
#include <TLorentzVector.h>
#include <TObject.h>
#include <TFile.h>


namespace MA5
{

/** @brief Reader of Delphes ROOT files (standard or MA5-tuned cards). */
class DelphesTreeReader : public TreeReaderBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /** @brief Number of entries in the tree. */
  MAint64 total_nevents_;

  /** @brief Number of entries read so far. */
  MAint64 read_nevents_;

  /** @brief Branches and arrays of the Delphes tree. */
  DelphesDataFormat data_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor without argument. */
  DelphesTreeReader()
  { InitializeVariables(); } 

  /**
   * @brief Constructor.
   *
   * @param source ROOT file.
   * @param tree Delphes tree.
   */
  // FIXME: unlike the default constructor, this one does not call InitializeVariables().
  DelphesTreeReader(TFile* source, TTree* tree): TreeReaderBase(source,tree)
  { }

  /** @brief Destructor. */
  virtual ~DelphesTreeReader()
  { }

  /**
   * @brief Find the branches and create the arrays.
   *
   * @return true.
   */
  virtual MAbool Initialize();

  /**
   * @brief Set the sample format (Delphes or Delphes with an MA5 card).
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


 private:

  /**
   * @brief Fill the MC and reconstructed event from the Delphes arrays.
   *
   * @param myEvent current event.
   * @param mySample current sample.
   */
  void FillEvent(EventFormat& myEvent, SampleFormat& mySample);

  /** @brief Reset the entry counters. */
  void InitializeVariables()
  {
    total_nevents_=0;
    read_nevents_=0;
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
