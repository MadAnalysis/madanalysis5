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
 * @file gz_ostream.h
 * @brief Output stream for gzip-compressed files.
 */

#ifndef GZ_OSTREAM_H
#define GZ_OSTREAM_H


// SampleAnalyzer headers
#include "SampleAnalyzer/Interfaces/zlib/gz_streambase.h"


namespace MA5
{

// -------------------------------------------------------------
//                      CLASS GZ_OSTREAM
// -------------------------------------------------------------

/** @brief Output stream for gzip-compressed files. */
class gz_ostream : public gz_streambase, public std::ostream
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:


  /** @brief Constructor. */
  gz_ostream() : std::ostream(&buf)
  {}

  /**
   * @brief Constructor opening a file.
   *
   * @param name file name.
   * @param open_mode std::ios mode (std::ios::out by default).
   */
  gz_ostream( const MAchar* name, MAint32 open_mode = std::ios::out)
      : gz_streambase( name, open_mode), std::ostream( &buf) 
  {}
  
  /**
   * @brief Accessor to the stream buffer.
   *
   * @return the buffer.
   */
  gz_streambuf* rdbuf()
  { return gz_streambase::rdbuf(); }

  /**
   * @brief Open a gzip file.
   *
   * @param name file name.
   * @param open_mode std::ios mode.
   */
  void open( const MAchar* name, MAint32 open_mode = std::ios::out)
  { gz_streambase::open( name, open_mode); }

};

}

#endif

