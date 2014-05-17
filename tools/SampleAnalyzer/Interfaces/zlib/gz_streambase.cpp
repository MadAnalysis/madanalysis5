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


// SampleAnalyzer headers
#include "SampleAnalyzer/Interfaces/zlib/gz_streambase.h"
#include "SampleAnalyzer/Interfaces/zlib/gz_file.h"

// STL headers
#include <iostream>
#include <string.h>
#include <memory.h>


using namespace MA5;



// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
gz_streambuf::gz_streambuf()
{
  opened=0;
  setp( buffer, buffer + (bufferSize-1));
  setg( buffer + 4, buffer + 4, buffer + 4); 
  file = new gz_file;
}


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
gz_streambuf::~gz_streambuf()
{
  close();
  if (file!=0) delete file;
  file=0;
}


// -----------------------------------------------------------------------------
// Getting cursor position in file
// -----------------------------------------------------------------------------
Long64_t gz_streambuf::tellg()
{ 
#if ZLIB_VERNUM >= 0x1240
  // gzoffset is only available in zlib 1.2.4 or later
  return pos_type(gzoffset(file->get()));
#else
  // return an approximation of file size (only used in progress dialog)
  return pos_type(gztell(file->get()) / 4);
#endif
}


// -----------------------------------------------------------------------------
// Opening a gzip file
// -----------------------------------------------------------------------------
gz_streambuf* gz_streambuf::open( const char* name, int open_mode) 
{
  if (is_open()) return (gz_streambuf*)0;

  mode = open_mode;
  // no append nor read/write mode
  if ((mode & std::ios::ate) || 
      (mode & std::ios::app) ||
      (  (mode & std::ios::in) && 
         (mode & std::ios::out)    )) return (gz_streambuf*)0;

  char  fmode[10];
  char* fmodeptr = fmode;
  if ( mode & std::ios::in) *fmodeptr++ = 'r';
  else if ( mode & std::ios::out) *fmodeptr++ = 'w';

  *fmodeptr++ = 'b';
  *fmodeptr = '\0';
  
  file->get() = gzopen( name, fmode);
  if (file->get() == 0) return (gz_streambuf*)0;
  opened = 1;

  return this;
}


// -----------------------------------------------------------------------------
// Closing a gzip file
// -----------------------------------------------------------------------------
gz_streambuf * gz_streambuf::close()
{
  if ( is_open()) 
  {
    sync();
    opened = 0;
    if ( gzclose( file->get() ) == Z_OK)
      return this;
  }
  return (gz_streambuf*)0;
}


// -----------------------------------------------------------------------------
// Underflow
// -----------------------------------------------------------------------------
int gz_streambuf::underflow()
{ 
  if ( gptr() && ( gptr() < egptr()))
    return * reinterpret_cast<unsigned char *>( gptr());

  if ( ! (mode & std::ios::in) || ! opened)
    return EOF;
  
  int n_putback = gptr() - eback();
  if ( n_putback > 4)
    n_putback = 4;
  memcpy( buffer + (4 - n_putback), gptr() - n_putback, n_putback);

  signed int num = gzread( file->get(), buffer+4, bufferSize-4);
  if (num <= 0) return EOF;

  // reset buffer pointers
  setg( buffer + (4 - n_putback),   // beginning of putback area
        buffer + 4,                 // read position
        buffer + 4 + num);          // end of buffer

  // return next character
  return * reinterpret_cast<unsigned char *>( gptr());    
}


// -----------------------------------------------------------------------------
// Flush the buffer
// -----------------------------------------------------------------------------
int gz_streambuf::flush_buffer()
{
    int w = pptr() - pbase();
    if ( gzwrite( file->get(), pbase(), w) != w) return EOF;
    pbump( -w);
    return w;
}


// -----------------------------------------------------------------------------
// Overflow
// -----------------------------------------------------------------------------
int gz_streambuf::overflow( int c)
{
  if ( ! ( mode & std::ios::out) || ! opened) return EOF;
  if (c != EOF) 
  {
    *pptr() = c;
    pbump(1);
  }
  if ( flush_buffer() == EOF)
    return EOF;
  return c;
}


// -----------------------------------------------------------------------------
// Synchronize the input buffer
// -----------------------------------------------------------------------------
int gz_streambuf::sync() 
{
  if ( pptr() && pptr() > pbase())
  {
    if ( flush_buffer() == EOF) return -1;
  }
  return 0;
}

