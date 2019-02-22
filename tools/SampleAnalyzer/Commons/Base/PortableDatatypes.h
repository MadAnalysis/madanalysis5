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


#ifndef PORTABLE_DATATYPE_H
#define PORTABLE_DATATYPE_H


// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortabilityTags.h"


// bool 
typedef bool MAbool;

// 8-bit integer
typedef signed   char MAint8;
typedef unsigned char MAuint8;
typedef          char MAchar;

// 16-bit integer
typedef signed   short MAint16;
typedef unsigned short MAuint16;

// 32-bit integer
#ifndef INT_4BYTES
  typedef signed int   MAint32;
  typedef unsigned int MAuint32;
#else
  #if INT_4BYTES==1
    typedef signed int   MAint32;
    typedef unsigned int MAuint32;
  #else
    typedef signed long   MAint32;
    typedef unsigned long MAuint32;
  #endif
#endif

// 64-bit integer
#ifndef LONG_8BYTES
  typedef signed   long MAint64;
  typedef unsigned long MAuint64;
#else
  #if LONG_8BYTES==1
    typedef signed   long MAint64;
    typedef unsigned long MAuint64;
  #else
    typedef signed   long long MAint64;
    typedef unsigned long long MAuint64;
  #endif
#endif

// 32-bit floating values
typedef float  MAfloat32;

// 64-bit floating values
typedef double      MAfloat64;
typedef double      MAdouble64;
typedef long double MAfloat128;

#endif
