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
 * @file LHCOReader.h
 * @brief Reader of LHC Olympics (LHCO) files (reconstructed objects).
 */

#ifndef LHCO_READER_h
#define LHCO_READER_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Reader/ReaderTextBase.h"


namespace MA5
{

/** @brief Reader of LHC Olympics files (photons, electrons, muons, taus, jets and MET). */
class LHCOReader : public ReaderTextBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /** @brief Has the header line of the next event been saved? */
  MAbool saved_;
  /** @brief Is the first event being read? */
  MAbool firstevent_;
  /** @brief Has the end of the file been reached? */
  MAbool EndOfFile_;
  /** @brief Header line of the next event. */
  std::string savedline_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  LHCOReader()
  { }

  /** @brief Destructor. */
  virtual ~LHCOReader()
  { }

  /**
   * @brief Initialise the sample (no header in LHCO files).
   *
   * @param mySample sample.
   * @return true.
   */
  virtual MAbool ReadHeader(SampleFormat& mySample);

  /**
   * @brief Finalise the header (nothing to do).
   *
   * @param mySample sample.
   * @return true.
   */
  virtual MAbool FinalizeHeader(SampleFormat& mySample);

  /**
   * @brief Read the objects of the next event (until the next line starting with 0).
   *
   * @param myEvent event to fill.
   * @param mySample sample.
   * @return KEEP, or FAILURE once the end of the file has been reached.
   */
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample);

  /**
   * @brief Compute MHT, THT, TET and Meff.
   *
   * @param mySample sample.
   * @param myEvent event.
   * @return true.
   */
  virtual MAbool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent);


 private:

  /**
   * @brief Read an object line.
   *
   * @param line line.
   * @param myEvent event.
   */
  void FillEventParticleLine(const std::string& line, EventFormat& myEvent);
  /**
   * @brief Read the header line of an event (nothing is stored).
   *
   * @param line line.
   * @param myEvent event.
   */
  void FillEventInitLine(const std::string& line, EventFormat& myEvent);
};

}

#endif
