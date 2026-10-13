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
 * @file WriterBase.h
 * @brief Interface of the event-file writers.
 */

#ifndef WRITER_BASE_h
#define WRITER_BASE_h


// STL headers
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/Configuration.h"
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"


namespace MA5
{

/** @brief Abstract base class of the event-file writers. */
class WriterBase
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /** @brief Is the file written through RFIO? */
  MAbool rfio_;

  /** @brief Is the file gzip-compressed? */
  MAbool compress_;

  /** @brief Has the first event not been written yet? */
  MAbool FirstEvent_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  WriterBase()
  {
    rfio_=false; compress_=false; FirstEvent_=true;
  }

  /** @brief Destructor. */
  virtual ~WriterBase()
  {
  }

  /**
   * @brief Open the output file.
   *
   * @param cfg run configuration.
   * @param filename output file name.
   * @return false in case of error.
   */
  virtual MAbool Initialize(const Configuration* cfg,
                          const std::string& filename) = 0;

  /**
   * @brief Write the header of the file.
   *
   * @param mySample sample.
   * @return false in case of error.
   */
  virtual MAbool WriteHeader(const SampleFormat& mySample) = 0;

  /**
   * @brief Write an event.
   *
   * @param myEvent event.
   * @param mySample sample.
   * @return false in case of error.
   */
  virtual MAbool WriteEvent(const EventFormat& myEvent, 
                          const SampleFormat& mySample) = 0;

  /**
   * @brief Write the footer of the file.
   *
   * @param mySample sample.
   * @return false in case of error.
   */
  virtual MAbool WriteFoot(const SampleFormat& mySample) = 0;

  /**
   * @brief Close the file.
   *
   * @return false in case of error.
   */
  virtual MAbool Finalize() = 0;


};

}

#endif
