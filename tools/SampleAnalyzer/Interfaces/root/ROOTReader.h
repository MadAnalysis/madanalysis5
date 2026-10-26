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
 * @file ROOTReader.h
 * @brief Reader of ROOT files produced by Delphes or Delphes-MA5tune.
 */

#ifndef ROOT_READER_h
#define ROOT_READER_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Base/ReaderBase.h"
#include "SampleAnalyzer/Interfaces/root/TreeReaderBase.h"

// STL headers
#include <iostream>


class TFile;

namespace MA5
{

/** @brief Reader of ROOT files, delegating the reading to the tree reader matching the file (Delphes or Delphes-MA5tune). */
class ROOTReader : public ReaderBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /** @brief Input ROOT file. */
  TFile *         source_;
  /** @brief Tree reader selected for the file. */
  TreeReaderBase* treeReader_;
  /** @brief Name of the file. */
  std::string     filename_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  ROOTReader()
  { 
    source_=0;
    treeReader_=0;
  } 

  /** @brief Destructor. */
  virtual ~ROOTReader()
  { }

  /**
   * @brief Open the file and select the tree reader.
   *
   * @param rawfilename file name.
   * @param cfg run configuration.
   * @return false if the file or the tree cannot be read.
   */
  virtual MAbool Initialize(const std::string& rawfilename,
                          const Configuration& cfg);

  /**
   * @brief Close the file.
   *
   * @return true.
   */
  virtual MAbool Finalize();

  /**
   * @brief Read the header (a warning is issued if the ROOT versions differ).
   *
   * @param mySample sample to fill.
   * @return the result of the tree reader.
   */
  virtual MAbool ReadHeader(SampleFormat& mySample);

  /**
   * @brief Finalise the header (nothing to do).
   *
   * @param mySample sample.
   * @return true.
   */
  virtual MAbool FinalizeHeader(SampleFormat& mySample)
  { return true; }

  /**
   * @brief Read the next event.
   *
   * @param myEvent event to fill.
   * @param mySample sample.
   * @return the status of the reading.
   */
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample)
  { return treeReader_->ReadEvent(myEvent,mySample); }

  /**
   * @brief Finalise the event.
   *
   * @param mySample sample.
   * @param myEvent event.
   * @return the result of the tree reader.
   */
  virtual MAbool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)
  { return treeReader_->FinalizeEvent(mySample,myEvent); }


  /**
   * @brief Final position (from the tree reader).
   *
   * @return the final position.
   */
  virtual MAint64 GetFinalPosition()
  { return treeReader_->GetFinalPosition(); }

  /**
   * @brief Size of the file.
   *
   * @return the size in bytes.
   */
  virtual MAint64 GetFileSize()
  {
    MAint64 length = 0;
    std::ifstream myinput(filename_.c_str());
    myinput.seekg(0,std::ios::beg);
    myinput.seekg(0,std::ios::end);
    length = myinput.tellg();
    myinput.close();
    return length;
  }

  /**
   * @brief Current position (from the tree reader).
   *
   * @return the position.
   */
  virtual MAint64 GetPosition()
  { return treeReader_->GetPosition(); }


 private:
  /**
   * @brief Select the tree reader matching the content of the file.
   *
   * @return false if no suitable tree is found.
   */
  MAbool SelectTreeReader();


};

}

#endif
