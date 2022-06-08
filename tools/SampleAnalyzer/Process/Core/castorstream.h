////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
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


#ifndef CASTORSTREAM_H
#define CASTORSTREAM_H

// standard C++ with new header file names and std:: namespace
#include <iostream>
#include <fstream>


namespace MA5
{

class castorstreambuf : public std::streambuf
{
private:
    static const int bufferSize = 1024;    // size of data buff

    int              file_;               // file handle for compressed file
    char             buffer[bufferSize]; // data buffer
    char             opened;             // open/close state of stream
    int              mode;               // I/O mode

    int flush_buffer();


public:
    castorstreambuf() : opened(0)
    {
        setp( buffer, buffer + (bufferSize-1));
        setg( buffer + 4,     // beginning of putback area
              buffer + 4,     // read position
              buffer + 4);    // end position      
        // ASSERT: both input & output capabilities will not be used together
    }

    int is_open() { return opened; }

    castorstreambuf* open( char* name, int open_mode);

    castorstreambuf* close();

    ~castorstreambuf() { close(); }
    
    virtual int     overflow( int c = EOF);
    virtual int     underflow();
    virtual int     sync();
};

class castorstreambase : virtual public std::ios
{
protected:
    castorstreambuf buf;

public:

    castorstreambase() { init(&buf); }

    castorstreambase( char* name, int open_mode);

    ~castorstreambase();

    void open( char* name, int open_mode);

    void close();

    castorstreambuf* rdbuf() { return &buf; }
};


class icastorstream : public castorstreambase, public std::istream
{
public:
    icastorstream() : std::istream( &buf) {} 

    icastorstream( char* name, int open_mode = std::ios::in)
        : castorstreambase( name, open_mode), std::istream( &buf) {}  

    castorstreambuf* rdbuf() { return castorstreambase::rdbuf(); }

    void open( char* name, int open_mode = std::ios::in)
    { castorstreambase::open( name, open_mode); }
};

}

#endif

