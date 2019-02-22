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


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Core/xdr_istream.h"

// STL headers
#include <iostream>


using namespace MA5;

// -----------------------------------------------------------------------------
// overloading >> operator for std::string
// -----------------------------------------------------------------------------
xdr_istream& xdr_istream::operator>>(std::string &s)
{
  if (eof()) return (*this);
  MAuint32 len=0;
  (*this)>>len;

  if (eof()) return (*this);

  MAchar line[len];
  sb_->sgetn(line, len);
  s=std::string(line,line+len);

  size_t pad = (4-len)&3; //change 4-len&3
  MAchar dummy[pad];
  sb_->sgetn(dummy,pad);
  return *this;
}

// -----------------------------------------------------------------------------
// overloading >> operator for MAuint32
// -----------------------------------------------------------------------------
xdr_istream& xdr_istream::operator>>(MAuint32 &v)
{
  if (eof()) return (*this);

  v=0;
  for(MAuint32 i=0;i<32; i+=8)
  {
    v<<=8;
    v += static_cast<MAuint32>(sb_->sbumpc());
  }
  return *this;
}

// -----------------------------------------------------------------------------
// overloading >> operator for MAint32
// -----------------------------------------------------------------------------
xdr_istream& xdr_istream::operator>>(MAint32 &v)
{
  if (eof()) return (*this);

  MAuint32 _v=0;
  (*this)>>_v;
  v=static_cast<MAint32>(_v);
  return (*this);
}


// -----------------------------------------------------------------------------
// overloading >> operator for MAuint64
// -----------------------------------------------------------------------------
xdr_istream& xdr_istream::operator>>(MAuint64 &v)
{
  if (eof()) return (*this);

  v=0;
  for(MAuint32 i=0;i<64; i+=8)
  {
    v<<=8;
    v += static_cast<MAuint64>(sb_->sbumpc());
  }
  return *this;
}


// -----------------------------------------------------------------------------
// overloading >> operator for MAint64
// -----------------------------------------------------------------------------
xdr_istream& xdr_istream::operator>>(MAint64 &v)
{
  if (eof()) return (*this);

  MAuint64 _v=0;
  (*this)>>_v;
  v=static_cast<MAint64>(_v);
  return (*this);
}


// -----------------------------------------------------------------------------
// overloading >> operator for MAfloat32
// -----------------------------------------------------------------------------
xdr_istream& xdr_istream::operator>>(MAfloat32 &v)
{
  if (eof()) return (*this);

  MAuint32 n=0;
  (*this)>>n;
  MAfloat32* vp = reinterpret_cast<MAfloat32*>(&n);
  v=*vp;
  return *this;
}


// -----------------------------------------------------------------------------
// overloading >> operator for MAfloat64
// -----------------------------------------------------------------------------
xdr_istream& xdr_istream::operator>>(MAfloat64 &v)
{
  if (eof()) return (*this);

  MAuint64 n=0;
  (*this)>>n;
  MAfloat64* vp = reinterpret_cast<MAfloat64*>(&n);
  v=*vp;
  return *this;
}
