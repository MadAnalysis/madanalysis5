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


#ifndef GZ_STREAM_BASE_H
#define GZ_STREAM_BASE_H


// STL headers
#include <iostream>
#include <fstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"


namespace MA5
{

// Implicit class
class gz_file;

// -------------------------------------------------------------
//                   CLASS GZ_STREAMBUF
// -------------------------------------------------------------
class gz_streambuf : public std::streambuf
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /// size of the data buffer
  static const MAint32 bufferSize = 47+256;    

  gz_file* file;               // file handle for compressed file
  MAchar   buffer[bufferSize]; // data buffer
  MAchar   opened;             // open/close state of stream
  MAint32  mode;               // I/O mode


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 private :

  /// Flush the buffer
  MAint32 flush_buffer();

 public:

  /// Constructor withtout arguments
  gz_streambuf();

  /// Destructor
  ~gz_streambuf();

  /// Is opened
  MAint32 is_open() { return opened; }

  /// Opening the gzip file
  gz_streambuf* open(const MAchar* name, MAint32 open_mode);

  /// Closing the file
  gz_streambuf* close();

    
  /// Overflow
  virtual MAint32 overflow(MAint32 c = EOF);

  /// Underflow
  virtual MAint32 underflow();

  /// Synchronize input buffer
  virtual MAint32 sync();

  /// get position of the cursor in the file
  virtual MAint64 tellg();

};


// -------------------------------------------------------------
//                   CLASS GZ_STREAMBASE
// -------------------------------------------------------------
class gz_streambase : virtual public std::ios
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
  gz_streambuf buf;


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:

  /// Constructor without arguments
  gz_streambase() 
  { init(&buf); }

  /// Constructor with arguments
  gz_streambase( const MAchar* name, MAint32 open_mode)
  {
    init( &buf);
    open( name, open_mode);
  }

  /// Destructor
  ~gz_streambase()
  { buf.close(); }

  /// Open a gzip file
  void open( const MAchar* name, MAint32 open_mode)
  {
    if (!buf.open( name, open_mode))
        clear( rdstate() | std::ios::badbit);
  }

  /// Close a gzip file
  void close()
  {
    if (buf.is_open())
    {
      if (!buf.close())
        clear( rdstate() | std::ios::badbit);
    }
  }

  /// Read the buffer
  gz_streambuf* rdbuf()
  { return &buf; }

};

};


#endif

