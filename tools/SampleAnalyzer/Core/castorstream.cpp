////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <http://madanalysis.irmp.ucl.ac.be>
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


#include "SampleAnalyzer/Core/castorstream.h"
#include <iostream>
#include <string> 
#include <memory.h>

#ifdef CASTOR_USE
   #include "rfio_api.h"
#endif


using namespace MA5;

// --------------------------------------
// class castorstreambuf:
// --------------------------------------

castorstreambuf* castorstreambuf::open(char* name, int open_mode)
{
    if ( is_open()) return (castorstreambuf*)0;

    mode = open_mode;

    // no append nor read/write mode
    if (   (mode & std::ios::ate) || (mode & std::ios::app)
        || ((mode & std::ios::in) && (mode & std::ios::out)))
        return (castorstreambuf*)0;

#ifdef CASTOR_USE
    file_ = rfio_open(name, 0);
#else
    file_ = -1;
#endif

    if (file_ < 0) return (castorstreambuf*)0;
    opened = 1;
    return this;
}

castorstreambuf * castorstreambuf::close()
{
  if ( is_open() )
  {
    sync();
    opened = 0;
#ifdef CASTOR_USE
    rfio_close(file_);
#endif
    return this;
    }
  return (castorstreambuf*)0;
}

int castorstreambuf::underflow()
{
    if ( gptr() && ( gptr()<egptr() ) )
        return * reinterpret_cast<unsigned char *>(gptr());
 
    if   ( !(mode & std::ios::in) || !opened) return EOF;

    // Josuttis' implementation of inbuf
    int n_putback = gptr() - eback();
    if ( n_putback > 4) n_putback = 4;
    memcpy( buffer + (4 - n_putback), gptr() - n_putback, n_putback);

#ifdef CASTOR_USE 
   signed int num = rfio_read( file_, buffer+4, bufferSize-4);
#else
   signed int num = -1;
#endif
      if (num <= 0) return EOF;

    // reset buffer pointers
    setg( buffer + (4 - n_putback),   // beginning of putback area
          buffer + 4,                 // read position
          buffer + 4 + num);          // end of buffer

    // return next character
    return * reinterpret_cast<unsigned char *>( gptr());    
}

int castorstreambuf::flush_buffer() {
    // Separate the writing of the buffer from overflow() and
    // sync() operation.
    int w = pptr() - pbase();
    //if ( gzwrite( file_, pbase(), w) != w)
    //    return EOF;
    //pbump( -w);
    return w;
}

int castorstreambuf::overflow( int c) { // used for output buffer only
  if ( ! ( mode & std::ios::out) || ! opened)
        return EOF;
    if (c != EOF) {
        *pptr() = c;
        pbump(1);
    }
    if ( flush_buffer() == EOF)
        return EOF;
    return c;
}

int castorstreambuf::sync() {
    // Changed to use flush_buffer() instead of overflow( EOF)
    // which caused improper behavior with std::endl and flush(),
    // bug reported by Vincent Ricard.
    if ( pptr() && pptr() > pbase()) {
        if ( flush_buffer() == EOF)
            return -1;
    }
    return 0;
}

// --------------------------------------
// class castorstreambase:
// --------------------------------------

castorstreambase::castorstreambase( char* name, int mode) {
    init( &buf);
    open( name, mode);
}

castorstreambase::~castorstreambase() {
    buf.close();
}

void castorstreambase::open( char* name, int open_mode) {
    if ( ! buf.open( name, open_mode))
        clear( rdstate() | std::ios::badbit);
}

void castorstreambase::close() {
    if ( buf.is_open())
        if ( ! buf.close())
            clear( rdstate() | std::ios::badbit);
}
