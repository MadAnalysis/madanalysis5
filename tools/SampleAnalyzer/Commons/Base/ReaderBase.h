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

class ReaderBase
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /// Allowing to read data from RFIO
  MAbool rfio_;

  /// Allowing to read compressed file
  MAbool compress_;

  /// Allowing to read fifo file
  MAbool fifo_;

  /// User configuration
  Configuration cfg_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  ReaderBase()
  {
    rfio_=false;  compress_=false; 
  }

  /// Destructor
  virtual ~ReaderBase()
  {
  }

  /// Is FIFO file?
  static MAbool IsFIFOMode(const std::string& name)
  {
    if (name.size()<6) return false;
    if (name.find(".fifo")==(name.size()-5)) return true;
    return false;
  }

  /// Initialize (virtual pure)
  virtual MAbool Initialize(const std::string& rawfilename,
                          const Configuration& cfg) = 0;

  /// Read the sample (virtual pure)
  virtual MAbool ReadHeader(SampleFormat& mySample) = 0;

  /// Finalize the header (virtual pure)
  virtual MAbool FinalizeHeader(SampleFormat& mySample) = 0;

  /// Read the event (virtual pure)
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample) = 0;

  /// Finalize the event (virtual pure)
  virtual MAbool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent) = 0;

  /// Finalize
  virtual MAbool Finalize()=0;

  /// Is the file stored in Rfio ?
  static MAbool IsRfioMode(const std::string& name)
  {  
    if (name.find("rfio:")==0) return true;
    return false;
  }

  /// Is compressed file ?
  static MAbool IsCompressedMode(const std::string& name)
  {
    if (name.size()<4) return false;
    if (name.find(".gz")==name.size()-3) return true;
    return false;
  }

  /// Is the file stored in Rfio
  static std::string CleanFilename(const std::string& name)
  {
    if (name.find("rfio:")==0) return name.substr(5);
    else if (name.find("file:")==0) return name.substr(5);
    return name;
  }

  /// Get the file size
  virtual MAint64 GetFileSize()=0;

  /// Get the position in file
  virtual MAint64 GetPosition()=0;

  /// Get the final position in file
  virtual MAint64 GetFinalPosition()=0;


};

}

#endif
