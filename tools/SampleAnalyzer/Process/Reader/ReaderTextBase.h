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

class ReaderTextBase : public ReaderBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /// Streaming in GZ format
  gz_istream * gzinput_;

  /// Streaming for reading input
  std::istream*  input_;
  std::streampos oldpos_;

  /// Streaming for fifo
  std::ifstream* input_fifo_;

  /// Name of the file (without prefix such as file: or rfio:)
  std::string filename_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  ReaderTextBase()
  {
    input_      = 0;
    input_fifo_ = 0;
  }

  /// Destructor
  virtual ~ReaderTextBase()
  {
    if (input_     !=0) delete input_;
    if (input_fifo_!=0) delete input_fifo_;
  }

  /// Initialize
  virtual MAbool Initialize(const std::string& rawfilename,
                          const Configuration& cfg);

  /// Read the sample (virtual pure)
  virtual MAbool ReadHeader(SampleFormat& mySample) = 0;

  /// Finalize the header (virtual pure)
  virtual MAbool FinalizeHeader(SampleFormat& mySample) = 0;

  /// Read the event (virtual pure)
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample) = 0;

  /// Finalize the event (virtual pure)
  virtual MAbool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent) = 0;

  /// Finalize
  virtual MAbool Finalize();

  /// Read line text
  MAbool ReadLine(std::string& line, MAbool removeComment=true);

  /// Get the file size (in octet)
  virtual MAint64 GetFileSize();

  /// Get the final position in file
  virtual MAint64 GetFinalPosition();

  /// Get the position in file
  virtual MAint64 GetPosition();

};

}

#endif
