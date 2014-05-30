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

// ROOT headers
#include <TVector.h>
#include <TClonesArray.h>

namespace MA5
{

class WriterBase
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /// Allowing to read data from RFIO
  bool rfio_;

  /// Allowing to read compressed file
  bool compress_;

  /// Tag for the first event
  bool FirstEvent_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  WriterBase()
  {
    rfio_=false; compress_=false; FirstEvent_=true;
  }

	/// Destructor
  virtual ~WriterBase()
  {
  }

  /// Initialize (virtual pure)
  virtual bool Initialize(const Configuration* cfg,
                          const std::string& filename) = 0;

  /// Read the sample (virtual pure)
  virtual bool WriteHeader(const SampleFormat& mySample) = 0;

  /// Read the event (virtual pure)
  virtual bool WriteEvent(const EventFormat& myEvent, 
                          const SampleFormat& mySample) = 0;

  /// Finalize the event (virtual pure)
  virtual bool WriteFoot(const SampleFormat& mySample) = 0;

  /// Finalize
  virtual bool Finalize() = 0;


};

}

#endif
