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
 * @file TreeReaderBase.h
 * @brief Interface of the readers of ROOT trees (Delphes, Delphes-MA5tune).
 */

#ifndef TREE_READER_BASE_h
#define TREE_READER_BASE_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Base/Configuration.h"
#include "SampleAnalyzer/Commons/Base/StatusCode.h"
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"

// STL headers
#include <iostream>


class TFile;
class TTree;

namespace MA5
{

/** @brief Abstract base class of the readers of ROOT trees. */
class TreeReaderBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /** @brief Input ROOT file. */
  TFile* source_;

  /** @brief Tree to read. */
  TTree* tree_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  TreeReaderBase()
  { source_=0; tree_=0; } 

  /**
   * @brief Constructor.
   *
   * @param source input ROOT file.
   * @param tree tree to read.
   */
  TreeReaderBase(TFile* source, TTree* tree)
  { source_=source; tree_=tree; }

  /** @brief Destructor. */
  virtual ~TreeReaderBase()
  { }

  /**
   * @brief Initialise the branches of the tree.
   *
   * @return false in case of error.
   */
  virtual MAbool Initialize()=0;

  /**
   * @brief Read the header.
   *
   * @param mySample sample to fill.
   * @return false in case of error.
   */
  virtual MAbool ReadHeader(SampleFormat& mySample)=0;

  /**
   * @brief Read the next entry of the tree.
   *
   * @param myEvent event to fill.
   * @param mySample sample.
   * @return the status of the reading.
   */
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample)=0;

  /**
   * @brief Finalise the event.
   *
   * @param mySample sample.
   * @param myEvent event.
   * @return false in case of error.
   */
  virtual MAbool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)=0;

  /**
   * @brief Final position (number of entries of the tree).
   *
   * @return the final position.
   */
  virtual MAint64 GetFinalPosition()=0;

  /**
   * @brief Current position (index of the current entry).
   *
   * @return the position.
   */
  virtual MAint64 GetPosition()=0;

};

}

#endif
