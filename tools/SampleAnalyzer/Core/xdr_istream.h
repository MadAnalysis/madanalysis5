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


#ifndef XDR_ISTREAM_H
#define XDR_ISTREAM_H

// STL headers
#include <vector>
#include <string>
#include <streambuf>
#include <istream>

// ROOT headers
#include <TRint.h>

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
  bool eof()
  { return (sb_->sgetc()==EOF); }

  /// Overloading operator >> for simple types
  xdr_istream& operator >> (Int_t       &v);
  xdr_istream& operator >> (UInt_t      &v);
  xdr_istream& operator >> (Long64_t      &v);
  xdr_istream& operator >> (ULong64_t     &v);
  xdr_istream& operator >> (Float_t     &v);
  xdr_istream& operator >> (Double_t    &v);
  xdr_istream& operator >> (std::string &v);

  /// Overloading operator >> for std::vector
  /// Template : definition inside the header
  template <typename T>
  xdr_istream& operator >> (std::vector<T> &t)
  {
    if (eof()) return (*this);
 
    UInt_t sz;
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
