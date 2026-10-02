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
 * @file WriterTextBase.h
 * @brief Base class of the writers of text files (plain or gzip-compressed).
 */

#ifndef WRITER_TEXT_BASE_h
#define WRITER_TEXT_BASE_h


// STL headers
#include <fstream>
#include <iostream>
#include <sstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Writer/WriterBase.h"


namespace MA5
{

/** @brief Base class of the writers of text files (plain or gzip-compressed). */
class WriterTextBase : public WriterBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
  /** @brief Output stream. */
  std::ostream* output_;

  /** @brief Run configuration. */
  const Configuration* cfg_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  WriterTextBase()
  {
    output_=0;
  }

  /** @brief Destructor (deletes the output stream). */
  virtual ~WriterTextBase()
  {
    if (output_ !=0) delete output_;
  }

  /**
   * @brief Open the output file (gzip-compressed if the name ends with .gz).
   *
   * @param cfg run configuration.
   * @param filename output file name.
   * @return true (the program exits if the file cannot be opened).
   */
  virtual MAbool Initialize(const Configuration* cfg,
                          const std::string& filename);

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
   * @return true.
   */
  virtual MAbool Finalize();

  /** @brief Write the MadAnalysis 5 banner. */
  void WriteMA5header();
 
};

}

#endif
