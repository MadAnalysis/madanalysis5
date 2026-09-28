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
 * @file xdr_istream.h
 * @brief Reader of XDR-encoded (big-endian) binary data, used by the STDHEP reader.
 */

#ifndef XDR_ISTREAM_H
#define XDR_ISTREAM_H


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"

// STL headers
#include <vector>
#include <string>
#include <streambuf>
#include <istream>
#include <cstdio>


namespace MA5
{

/** @brief Input stream decoding XDR (RFC 4506) data. */
class xdr_istream
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------

 private:
  /** @brief Underlying stream buffer. */
  std::streambuf* sb_;

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:

  /**
   * @brief Constructor from a stream buffer.
   *
   * @param sb stream buffer.
   */
  xdr_istream(std::streambuf* sb)
  { sb_=sb; }

  /**
   * @brief Constructor from an input stream.
   *
   * @param os input stream.
   */
  xdr_istream(const std::istream &os)
  { 
    sb_=os.rdbuf();
  }

  /**
   * @brief Has the end of the stream been reached?
   *
   * @return true at the end of the stream.
   */
  MAbool eof()
  { return (sb_->sgetc()==EOF); }

  /**
   * @brief Read a 32-bit integer.
   *
   * @param v value read.
   * @return this stream.
   */
  xdr_istream& operator >> (MAint32       &v);
  /**
   * @brief Read a unsigned 32-bit integer.
   *
   * @param v value read.
   * @return this stream.
   */
  xdr_istream& operator >> (MAuint32      &v);
  /**
   * @brief Read a 64-bit integer.
   *
   * @param v value read.
   * @return this stream.
   */
  xdr_istream& operator >> (MAint64      &v);
  /**
   * @brief Read a unsigned 64-bit integer.
   *
   * @param v value read.
   * @return this stream.
   */
  xdr_istream& operator >> (MAuint64     &v);
  /**
   * @brief Read a single-precision float.
   *
   * @param v value read.
   * @return this stream.
   */
  xdr_istream& operator >> (MAfloat32     &v);
  /**
   * @brief Read a double-precision float.
   *
   * @param v value read.
   * @return this stream.
   */
  xdr_istream& operator >> (MAfloat64    &v);
  /**
   * @brief Read a string (length followed by the characters, padded to 4 bytes).
   *
   * @param v value read.
   * @return this stream.
   */
  xdr_istream& operator >> (std::string &v);

  /**
   * @brief Read a vector (size followed by the elements).
   *
   * @tparam T element type.
   * @param t vector to extend.
   * @return this stream.
   */
  template <typename T>
  xdr_istream& operator >> (std::vector<T> &t)
  {
    if (eof()) return (*this);
 
    MAuint32 sz;
    T val;
    (*this)>>sz;

    if (eof()) return (*this);

    while(sz--!=0)
    {
      (*this)>>val;
      t.push_back(val);
    }
    return (*this);
  }

};

}

#endif
