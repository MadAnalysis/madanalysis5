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

class xdr_istream
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------

 private:
  std::streambuf* sb_;

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:

  /// Constructor without arguments
  xdr_istream(std::streambuf* sb)
  { sb_=sb; }

  /// Constructor with argument
  xdr_istream(const std::istream &os)
  { 
    sb_=os.rdbuf();
  }

  /// Returns if end of file
  MAbool eof()
  { return (sb_->sgetc()==EOF); }

  /// Overloading operator >> for simple types
  xdr_istream& operator >> (MAint32       &v);
  xdr_istream& operator >> (MAuint32      &v);
  xdr_istream& operator >> (MAint64      &v);
  xdr_istream& operator >> (MAuint64     &v);
  xdr_istream& operator >> (MAfloat32     &v);
  xdr_istream& operator >> (MAfloat64    &v);
  xdr_istream& operator >> (std::string &v);

  /// Overloading operator >> for std::vector
  /// Template : definition inside the header
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
