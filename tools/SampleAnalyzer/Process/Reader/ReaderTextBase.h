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
 * @file ReaderTextBase.h
 * @brief Base class of the readers of text-based files (plain, gzip-compressed or FIFO).
 */

#ifndef READER_TEXT_BASE_h
#define READER_TEXT_BASE_h


// STL headers
#include <fstream>
#include <iostream>
#include <sstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Base/ReaderBase.h"


class gz_istream;

namespace MA5
{

/** @brief Base class of the readers of text-based files (plain, gzip-compressed or FIFO). */
class ReaderTextBase : public ReaderBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /** @brief Input stream for compressed files (same object as input_). */
  gz_istream * gzinput_;

  /** @brief Input stream and previous position. */
  std::istream*  input_;
  std::streampos oldpos_;

  /** @brief Input stream for FIFO files. */
  std::ifstream* input_fifo_;

  /** @brief Name of the file (without prefix such as file: or rfio:). */
  std::string filename_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  ReaderTextBase()
  {
    input_      = 0;
    input_fifo_ = 0;
  }

  /** @brief Destructor (closes the streams). */
  virtual ~ReaderTextBase()
  {
    if (input_     !=0) delete input_;
    if (input_fifo_!=0) delete input_fifo_;
  }

  /**
   * @brief Open the file (compressed, FIFO or plain).
   *
   * @param rawfilename file name.
   * @param cfg run configuration.
   * @return false if the file cannot be opened (the program exits for RFIO or compressed files without zlib).
   */
  virtual MAbool Initialize(const std::string& rawfilename,
                          const Configuration& cfg);

  /**
   * @brief Read the header of the file.
   *
   * @param mySample sample to fill.
   * @return false in case of error.
   */
  virtual MAbool ReadHeader(SampleFormat& mySample) = 0;

  /**
   * @brief Finalise the reading of the header.
   *
   * @param mySample sample.
   * @return false in case of error.
   */
  virtual MAbool FinalizeHeader(SampleFormat& mySample) = 0;

  /**
   * @brief Read the next event.
   *
   * @param myEvent event to fill.
   * @param mySample sample.
   * @return the status of the reading.
   */
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample) = 0;

  /**
   * @brief Finalise the event.
   *
   * @param mySample sample.
   * @param myEvent event.
   * @return false in case of error.
   */
  virtual MAbool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent) = 0;

  /**
   * @brief Close the file.
   *
   * @return true.
   */
  virtual MAbool Finalize();

  /**
   * @brief Read the next non-empty line.
   *
   * @param line output line.
   * @param removeComment remove what follows a '#'.
   * @return false at the end of the file.
   */
  MAbool ReadLine(std::string& line, MAbool removeComment=true);

  /**
   * @brief Get the size of the file (compressed size for gzip files).
   *
   * @return the size in bytes (0 if no file is open).
   */
  virtual MAint64 GetFileSize();

  /**
   * @brief Get the final position in the file (the file size).
   *
   * @return the size in bytes.
   */
  virtual MAint64 GetFinalPosition();

  /**
   * @brief Get the current position in the file.
   *
   * @return the position in bytes.
   */
  virtual MAint64 GetPosition();

};

}

#endif
