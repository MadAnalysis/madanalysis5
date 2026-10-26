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
 * @file ReaderBase.h
 * @brief Interface of the event-file readers (LHE, LHCO, HepMC, STDHEP, ROOT).
 */

#ifndef READER_BASE_h
#define READER_BASE_h


// STL headers
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Base/StatusCode.h"
#include "SampleAnalyzer/Commons/Base/Configuration.h"
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"


namespace MA5
{

/**
 * @brief Abstract base class of the event-file readers.
 *
 * A reader is used as follows: Initialize(), ReadHeader(), FinalizeHeader(), then
 * ReadEvent()/FinalizeEvent() for each event, and Finalize().
 */
class ReaderBase
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /** @brief Is the file read through RFIO? */
  MAbool rfio_;

  /** @brief Is the file gzip-compressed? */
  MAbool compress_;

  /** @brief Is the file a named pipe (FIFO)? */
  MAbool fifo_;

  /** @brief User configuration of the run. */
  Configuration cfg_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  ReaderBase()
  {
    // FIXME: fifo_ is not initialised.
    rfio_=false;  compress_=false; 
  }

  /** @brief Destructor. */
  virtual ~ReaderBase()
  {
  }

  /**
   * @brief Is the file a named pipe (name ending with `.fifo`)?
   *
   * @param name file name.
   * @return true for a FIFO.
   */
  static MAbool IsFIFOMode(const std::string& name)
  {
    if (name.size()<6) return false;
    if (name.find(".fifo")==(name.size()-5)) return true;
    return false;
  }

  /**
   * @brief Open the file.
   *
   * @param rawfilename file name (possibly prefixed by `rfio:` or `file:`).
   * @param cfg user configuration.
   * @return false in case of error.
   */
  virtual MAbool Initialize(const std::string& rawfilename,
                          const Configuration& cfg) = 0;

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
   * @param mySample sample to finalise.
   * @return false in case of error.
   */
  virtual MAbool FinalizeHeader(SampleFormat& mySample) = 0;

  /**
   * @brief Read the next event.
   *
   * @param myEvent event to fill.
   * @param mySample current sample.
   * @return the status of the reading (FAILURE at the end of the file).
   */
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample) = 0;

  /**
   * @brief Finalise the reading of an event (computation of derived quantities).
   *
   * @param mySample current sample.
   * @param myEvent event to finalise.
   * @return false in case of error.
   */
  virtual MAbool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent) = 0;

  /**
   * @brief Close the file.
   *
   * @return false in case of error.
   */
  virtual MAbool Finalize()=0;

  /**
   * @brief Is the file stored on RFIO (name starting with `rfio:`)?
   *
   * @param name file name.
   * @return true for an RFIO file.
   */
  static MAbool IsRfioMode(const std::string& name)
  {  
    if (name.find("rfio:")==0) return true;
    return false;
  }

  /**
   * @brief Is the file gzip-compressed (name ending with `.gz`)?
   *
   * @param name file name.
   * @return true for a compressed file.
   */
  static MAbool IsCompressedMode(const std::string& name)
  {
    if (name.size()<4) return false;
    if (name.find(".gz")==name.size()-3) return true;
    return false;
  }

  /**
   * @brief Remove the `rfio:` or `file:` prefix of a file name.
   *
   * @param name file name.
   * @return the cleaned name.
   */
  static std::string CleanFilename(const std::string& name)
  {
    if (name.find("rfio:")==0) return name.substr(5);
    else if (name.find("file:")==0) return name.substr(5);
    return name;
  }

  /**
   * @brief Get the size of the file.
   *
   * @return the size in bytes.
   */
  virtual MAint64 GetFileSize()=0;

  /**
   * @brief Get the current position in the file.
   *
   * @return the position in bytes.
   */
  virtual MAint64 GetPosition()=0;

  /**
   * @brief Get the final position in the file.
   *
   * @return the position in bytes.
   */
  virtual MAint64 GetFinalPosition()=0;


};

}

#endif
