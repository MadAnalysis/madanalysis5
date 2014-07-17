////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
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

class WriterTextBase : public WriterBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
  /// Streaming for reading input
  std::ostream* output_;

  /// Configuration
  const Configuration* cfg_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  WriterTextBase()
  {
    output_=0;
  }

	/// Destructor
  virtual ~WriterTextBase()
  {
    if (output_ !=0) delete output_;
  }

  /// Initialize
  virtual bool Initialize(const Configuration* cfg,
                          const std::string& filename);

  /// Read the sample (virtual pure)
  virtual bool WriteHeader(const SampleFormat& mySample) = 0;

  /// Read the event (virtual pure)
  virtual bool WriteEvent(const EventFormat& myEvent,
                          const SampleFormat& mySample) = 0;

  /// Finalize the event (virtual pure)
  virtual bool WriteFoot(const SampleFormat& mySample) = 0;

  /// Finalize
  virtual bool Finalize();

  /// MA5 logo
  void WriteMA5header();
 
};

}

#endif
