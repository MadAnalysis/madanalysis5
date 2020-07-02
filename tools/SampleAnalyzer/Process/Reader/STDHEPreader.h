////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://launchpad.net/madanalysis5>
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


#ifndef STDHEP_READER_h
#define STDHEP_READER_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Reader/ReaderTextBase.h"
#include "SampleAnalyzer/Process/Core/xdr_istream.h"


namespace MA5
{

class STDHEPreader : public ReaderTextBase
{
  enum STDHEPversion {UNKNOWN,V1,V2,V21};
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

  // temporary data format
  MAint32 nevhept_;
  MAint32 nevhept_before_;
  MAint32 nhept_;
  MAbool firstevent;

  std::vector<MAint32>   isthept_;
  std::vector<MAint32>   idhept_;
  std::vector<MAint32>   jmohept_;
  std::vector<MAint32>   jdahept_;
  std::vector<MAfloat64> phept_;
  std::vector<MAfloat64> vhept_;
  std::vector< std::pair<MAint32,MAint32> > mothers_;

  // data related to the format
  STDHEPversion version_;
  xdr_istream * xdrinput_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  STDHEPreader()
  {
  }

  /// Destructor
  virtual ~STDHEPreader()
  {
  }

  /// Reset
  void Reset();

  /// Initialize
  virtual MAbool Initialize(const std::string& rawfilename,
                          const Configuration& cfg);

  /// Read the sample (virtual pure)
  virtual MAbool ReadHeader(SampleFormat& mySample);

  /// Finalize the header (virtual pure)
  virtual MAbool FinalizeHeader(SampleFormat& mySample);

  /// Read the event (virtual pure)
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample);

  MAbool DecodeFileHeader(SampleFormat& mySample);
  MAbool DecodeEventHeader(const std::string& evt_version);
  MAbool DecodeEventTable (const std::string& evt_version);
  MAbool DecodeSTDCM1     (const std::string& evt_version, SampleFormat& mySample);
  MAbool DecodeEventData  (const std::string& evt_version,EventFormat& myEvent);
  MAbool DecodeSTDHEP4    (const std::string& version,EventFormat& myEvent);

  /// Finalize the event (virtual pure)
  virtual MAbool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent);

  /// Finalize
  virtual MAbool Finalize();

 private :
  void SetVersion(const std::string& version);
  MAbool CheckEvent(const EventFormat&, const std::string&);

};

}

#endif
