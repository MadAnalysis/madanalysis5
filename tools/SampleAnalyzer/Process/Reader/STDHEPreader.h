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
 * @file STDHEPreader.h
 * @brief Reader of STDHEP (XDR binary) files.
 */

#ifndef STDHEP_READER_h
#define STDHEP_READER_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Reader/ReaderTextBase.h"
#include "SampleAnalyzer/Process/Core/xdr_istream.h"


namespace MA5
{

/** @brief Reader of STDHEP files (XDR-encoded HEPEVT records, versions 1 to 2.01). */
class STDHEPreader : public ReaderTextBase
{
  /** @brief STDHEP versions. */
  enum STDHEPversion {UNKNOWN,V1,V2,V21};
  /** @brief Identifiers of the STDHEP/MCFIO blocks. */
  enum STDHEPblock { GENERIC=0, 
                     FILEHEADER=1, 
                     EVENTTABLE=2, 
                     SEQUENTIALHEADER=3,
                     EVENTHEADER=4,
                     NOTHING=5,
                     MCFIO_STDHEP=101,
                     MCFIO_OFFTRACKARRAYS=102,
                     MCFIO_OFFTRACKSSTRUCT=103,
                     MCFIO_TRACEARRAYS=104,
                     MCFIO_STDHEPM=105,
                     MCFIO_STDHEPBEG=106,
                     MCFIO_STDHEPEND=107,
                     MCFIO_STDHEPCXX=108,
                     MCFIO_STDHEP4=201,
                     MCFIO_STDHEP4M=202,
                     MCFIO_HEPEUP=203,
                     MCFIO_HEPRUP=204 };

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /** @brief HEPEVT content of the current event: event number (current and previous), number of particles, first-event flag. */
  MAint32 nevhept_;
  MAint32 nevhept_before_;
  MAint32 nhept_;
  MAbool firstevent;

  /** @brief HEPEVT arrays: status codes, PDG codes, mothers, daughters, momenta (5 per particle), vertices (4 per particle). */
  std::vector<MAint32>   isthept_;
  std::vector<MAint32>   idhept_;
  std::vector<MAint32>   jmohept_;
  std::vector<MAint32>   jdahept_;
  std::vector<MAfloat64> phept_;
  std::vector<MAfloat64> vhept_;
  /** @brief Mother indices of the particles. */
  std::vector< std::pair<MAint32,MAint32> > mothers_;

  /** @brief Version of the file. */
  STDHEPversion version_;
  /** @brief XDR decoder of the input stream. */
  xdr_istream * xdrinput_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  STDHEPreader()
  {
  }

  /** @brief Destructor. */
  virtual ~STDHEPreader()
  {
  }

  /** @brief Clear the HEPEVT arrays. */
  void Reset();

  /**
   * @brief Open the file and create the XDR decoder.
   *
   * @param rawfilename file name.
   * @param cfg run configuration.
   * @return false if the file cannot be opened.
   */
  virtual MAbool Initialize(const std::string& rawfilename,
                          const Configuration& cfg);

  /**
   * @brief Read the file header.
   *
   * @param mySample sample.
   * @return false if the header is invalid.
   */
  virtual MAbool ReadHeader(SampleFormat& mySample);

  /**
   * @brief Finalise the header.
   *
   * @param mySample sample.
   * @return true.
   */
  virtual MAbool FinalizeHeader(SampleFormat& mySample);

  /**
   * @brief Read blocks until the next event.
   *
   * @param myEvent event to fill.
   * @param mySample sample.
   * @return KEEP, SKIP (invalid event or unknown block) or FAILURE at the end of the file.
   */
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample);

  /**
   * @brief Decode the file header block.
   *
   * @param mySample sample.
   * @return false if the header is invalid.
   */
  MAbool DecodeFileHeader(SampleFormat& mySample);
  /**
   * @brief Decode an event header block.
   *
   * @param evt_version version of the block.
   * @return false in case of error.
   */
  MAbool DecodeEventHeader(const std::string& evt_version);
  /**
   * @brief Decode an event table block.
   *
   * @param evt_version version of the block.
   * @return false in case of error.
   */
  MAbool DecodeEventTable (const std::string& evt_version);
  /**
   * @brief Decode a STDCM1 block (beginning/end of run: cross section, ...).
   *
   * @param evt_version version of the block.
   * @param mySample sample.
   * @return false in case of error.
   */
  MAbool DecodeSTDCM1     (const std::string& evt_version, SampleFormat& mySample);
  /**
   * @brief Decode a STDHEP event block.
   *
   * @param evt_version version of the block.
   * @param myEvent event to fill.
   * @return false for an inconsistent event.
   */
  MAbool DecodeEventData  (const std::string& evt_version,EventFormat& myEvent);
  /**
   * @brief Decode a STDHEP4 event block (with weights).
   *
   * @param version version of the block.
   * @param myEvent event to fill.
   * @return false for an inconsistent event.
   */
  MAbool DecodeSTDHEP4    (const std::string& version,EventFormat& myEvent);

  /**
   * @brief Build the mother-daughter links and compute MET, MHT, TET, THT and Meff.
   *
   * @param mySample sample.
   * @param myEvent event.
   * @return false if the event number is repeated (duplicated event).
   */
  virtual MAbool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent);

  /**
   * @brief Close the file.
   *
   * @return false in case of error.
   */
  virtual MAbool Finalize();

 private :
  /**
   * @brief Decode the version string of the file.
   *
   * @param version version string.
   */
  void SetVersion(const std::string& version);
  /**
   * @brief Check the consistency of the sizes of the HEPEVT arrays.
   *
   * @param myEvent event (unused).
   * @param blk name of the block (for the messages).
   * @return false if inconsistent.
   */
  MAbool CheckEvent(const EventFormat&, const std::string&);

};

}

#endif
