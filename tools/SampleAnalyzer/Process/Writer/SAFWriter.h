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


#ifndef WRITER_SAF_h
#define WRITER_SAF_h

// STL headers
#include <fstream>
#include <iostream>
#include <sstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Writer/WriterTextBase.h"

namespace MA5
{

class SAFWriter : public WriterTextBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  SAFWriter()
  { }

	/// Destructor
  virtual ~SAFWriter()
  { }

  /// Read the sample (virtual pure)
  virtual bool WriteHeader(const SampleFormat& mySample);
  virtual bool WriteHeader();

  /// Read the sample (virtual pure)
  bool WriteFiles(const std::vector<SampleFormat>& mySample);

  /// Read the event (virtual pure)
  virtual bool WriteEvent(const EventFormat& myEvent,
                          const SampleFormat& mySample);

  /// Finalize the event (virtual pure)
  virtual bool WriteFoot(const SampleFormat& mySample);
  virtual bool WriteFoot();

  /// Getting stream
  std::ostream* GetStream()
  { return output_; }
 
};

}

#endif
