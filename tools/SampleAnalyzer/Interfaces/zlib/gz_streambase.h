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
 * @file gz_streambase.h
 * @brief Stream buffer reading/writing gzip-compressed files (adapted from the gzstream library).
 */

#ifndef GZ_STREAM_BASE_H
#define GZ_STREAM_BASE_H


// STL headers
#include <iostream>
#include <fstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"


namespace MA5
{

/** @brief zlib file handle (defined in gz_file.h). */
class gz_file;

// -------------------------------------------------------------
//                   CLASS GZ_STREAMBUF
// -------------------------------------------------------------
/** @brief Stream buffer reading or writing a gzip-compressed file through zlib. */
class gz_streambuf : public std::streambuf
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /** @brief Size of the data buffer (4 bytes are reserved for put-back). */
  static const MAint32 bufferSize = 47+256;    

  /** @brief zlib file handle, data buffer, open/close state and I/O mode. */
  gz_file* file;               // file handle for compressed file
  MAchar   buffer[bufferSize]; // data buffer
  MAchar   opened;             // open/close state of stream
  MAint32  mode;               // I/O mode


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 private :

  /**
   * @brief Write the content of the buffer to the file.
   *
   * @return the number of bytes written, or EOF in case of error.
   */
  MAint32 flush_buffer();

 public:

  /** @brief Constructor. */
  gz_streambuf();

  /** @brief Destructor (closes the file). */
  ~gz_streambuf();

  /**
   * @brief Is the file open?
   *
   * @return non-zero if open.
   */
  MAint32 is_open() { return opened; }

  /**
   * @brief Open a gzip file (read or write mode only).
   *
   * @param name file name.
   * @param open_mode std::ios mode.
   * @return this buffer, or 0 in case of error.
   */
  gz_streambuf* open(const MAchar* name, MAint32 open_mode);

  /**
   * @brief Close the file.
   *
   * @return this buffer, or 0 in case of error.
   */
  gz_streambuf* close();

    
  /**
   * @brief Write a character and flush the buffer.
   *
   * @param c character.
   * @return the character, or EOF in case of error.
   */
  virtual MAint32 overflow(MAint32 c = EOF);

  /**
   * @brief Refill the buffer from the file.
   *
   * @return the next character, or EOF.
   */
  virtual MAint32 underflow();

  /**
   * @brief Flush the output buffer.
   *
   * @return 0, or -1 in case of error.
   */
  virtual MAint32 sync();

  /**
   * @brief Position in the compressed file.
   *
   * @return the number of compressed bytes read (approximation with zlib < 1.2.4).
   */
  virtual MAint64 tellg();

};


// -------------------------------------------------------------
//                   CLASS GZ_STREAMBASE
// -------------------------------------------------------------
/** @brief Base of the gzip streams (owns the stream buffer). */
class gz_streambase : virtual public std::ios
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
  /** @brief Stream buffer. */
  gz_streambuf buf;


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  gz_streambase() 
  { init(&buf); }

  /**
   * @brief Constructor opening a file.
   *
   * @param name file name.
   * @param open_mode std::ios mode.
   */
  gz_streambase( const MAchar* name, MAint32 open_mode)
  {
    init( &buf);
    open( name, open_mode);
  }

  /** @brief Destructor (closes the file). */
  ~gz_streambase()
  { buf.close(); }

  /**
   * @brief Open a gzip file (the bad bit is set in case of error).
   *
   * @param name file name.
   * @param open_mode std::ios mode.
   */
  void open( const MAchar* name, MAint32 open_mode)
  {
    if (!buf.open( name, open_mode))
        clear( rdstate() | std::ios::badbit);
  }

  /** @brief Close the file (the bad bit is set in case of error). */
  void close()
  {
    if (buf.is_open())
    {
      if (!buf.close())
        clear( rdstate() | std::ios::badbit);
    }
  }

  /**
   * @brief Accessor to the stream buffer.
   *
   * @return the buffer.
   */
  gz_streambuf* rdbuf()
  { return &buf; }

};


}


#endif

