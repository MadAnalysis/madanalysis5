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


#ifndef GZ_ISTREAM_H
#define GZ_ISTREAM_H

// SampleAnalyzer headers
#include "SampleAnalyzer/Interfaces/zlib/gz_streambase.h"

namespace MA5
{

// -------------------------------------------------------------
//                      CLASS GZ_ISTREAM
// -------------------------------------------------------------

class gz_istream : public gz_streambase, public std::istream
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:


  /// Constructor without arguments
  gz_istream() : std::istream(&buf)
  {}

  /// Constructor with arguments
  gz_istream( const char* name, int open_mode = std::ios::in)
      : gz_streambase( name, open_mode), std::istream( &buf) 
  {}

  /// Destructor
  virtual ~gz_istream()
  {}

  /// Read buffer
  gz_streambuf* rdbuf()
  { return gz_streambase::rdbuf(); }

  /// open a gzip file
  void open( const char* name, int open_mode = std::ios::in)
  { gz_streambase::open( name, open_mode); }

  /// get position of the cursor in the file
  virtual std::streampos tellg()
  { return buf.tellg(); }

};

}

#endif

