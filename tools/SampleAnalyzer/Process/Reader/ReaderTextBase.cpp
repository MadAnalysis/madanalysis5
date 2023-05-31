////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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


// STL headers
#include <fstream>

// Fifo headers
#include <fcntl.h>
#include <unistd.h>

// ZIP headers
#ifdef ZIP_USE
   #include "SampleAnalyzer/Interfaces/zlib/gz_istream.h"
#endif

// SampleHeader headers
#include "SampleAnalyzer/Process/Reader/ReaderTextBase.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


using namespace MA5;

// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------
MAbool ReaderTextBase::Initialize(const std::string& rawfilename,
                                const Configuration& cfg)
{
  // Set configuration
  cfg_=cfg;

  // Is the file stored in Rfio
  rfio_ = IsRfioMode(rawfilename);

  // Check consistency with compilation option
  if (rfio_)
  {
    ERROR << "'-rfio' is not allowed. Please set the RFIO_USE"
          << " variable in the Makefile to 1 and recompile the program if"
          << " you would like to use this option." << endmsg;
    exit(1);
  }

  // Cleaning the file (remove rfio or local location)
  filename_ = rawfilename;
  CleanFilename(filename_);

  // Is compressed file ?
  compress_ = IsCompressedMode(filename_);

  // Checking consistency with compilation option
  if (compress_)
  {
#ifndef ZIP_USE
    ERROR << "'zip file' is not allowed. Please set the RFIO_USE"
          << " variable in the Makefile to 1 and recompile the program if"
          << " you would like to use this option." << endmsg;
    exit(1);
#endif
  }

  // Is fifo file?
  fifo_ = IsFIFOMode(filename_);

  // Creating a tag indicating the file is opened correctlly
  MAbool test=false;

  // Input coming from RFIO and Compressed
  if (rfio_ && compress_ )
  {
    ERROR << "'zip file' is not allowed. Please set the RFIO_USE"
          << " variable in the Makefile to 1 and recompile the program if"
          << " you would like to use this option." << endmsg;
    exit(1);
  }

  // Input coming from RFIO 
  else if (rfio_)
  {
  }
 
  // Input coming from zip archive
  else if (compress_)
  {
#ifdef ZIP_USE
    input_=new gz_istream();
    gzinput_ = dynamic_cast<gz_istream*>(input_);
    gzinput_->open(const_cast<char*>(filename_.c_str()));
    test=gzinput_->good();
#endif
  }

  // Input coming from FIFO
  else if (fifo_)
  {
    MAint32 ififo = open(filename_.c_str(), O_RDONLY);
    if (ififo < 0) return false;

    input_fifo_ = new std::ifstream();
    input_fifo_->open(filename_.c_str());
    test=input_fifo_->good();
  }

  // Input coming from local disk
  else 
  {
    input_=new std::ifstream();
    std::ifstream * myinput = dynamic_cast<std::ifstream*>(input_);
    myinput->open(filename_.c_str());
    test=myinput->good();
  }

  // Check if the input is properly opened
  if (!test)
  {
    ERROR << "Opening file " << filename_ << " failed" << endmsg;
    return false;
  }

  return test;
}


// -----------------------------------------------------------------------------
// Finalize
// -----------------------------------------------------------------------------
MAbool ReaderTextBase::Finalize()
{
  if (rfio_ && compress_)
  {
    return true;
  } 
  else if (rfio_)
  {
  }
  else if (compress_)
  {
#ifdef ZIP_USE
    gzinput_->close();
    gzinput_->clear();
#endif
  }
  else if (fifo_)
  {
    input_fifo_->clear();
  }
  else 
  {
    std::ifstream * myinput = dynamic_cast<std::ifstream*>(input_);
    myinput->close();
    myinput->clear();   
  }

  // Free allocated memory for the file streamer
  if (input_     !=0) { delete input_; input_=0; }
  if (input_fifo_!=0) { delete input_fifo_; input_fifo_=0; }

  // OK!
  return true;
}


// -----------------------------------------------------------------------------
// ReadLine
// -----------------------------------------------------------------------------
MAbool ReaderTextBase::ReadLine(std::string& line, MAbool removeComment)
{
  MAbool getnewline=false;
  while (!getnewline)
  {
    // Getting a new line from the file
    if (!fifo_)
    {
      getline(*input_,line,'\n');
      if (input_->eof() || input_->fail()) return false;
    }
    else
    {
      std::getline(*input_fifo_,line);
      if (input_fifo_->eof()) return false;
    }

    // Removing possible comments
    if (removeComment)
    {
      std::string::size_type sharp_pos=line.find('#');
      if (sharp_pos==0) line="";
      if (sharp_pos!=std::string::npos) line.resize(sharp_pos);
    }

    std::stringstream str(line);
    std::string tmp; str >> tmp;
    if (tmp!="") getnewline=true; 
  }

  // Not the end of the file
  return true;
}


// -----------------------------------------------------------------------------
// GetFileSize
// -----------------------------------------------------------------------------
MAint64 ReaderTextBase::GetFileSize()
{
  if (input_==0) return 0;

  MAint64 length = 0;
  if (compress_)
  {
    std::ifstream myinput(filename_.c_str());
    myinput.seekg(0,std::ios::beg);
    myinput.seekg(0,std::ios::end);
    length = myinput.tellg();
    myinput.seekg(0,std::ios::beg);
    myinput.close();
  }
  else
  {
    input_->seekg(0,std::ios::beg);
    input_->seekg(0,std::ios::end);
    length = input_->tellg();
    input_->seekg(0,std::ios::beg);
  }
  return length;  
}


// -----------------------------------------------------------------------------
// GetFileSize
// -----------------------------------------------------------------------------
MAint64 ReaderTextBase::GetFinalPosition()
{
  return GetFileSize();
}


// -----------------------------------------------------------------------------
// GetPosition
// -----------------------------------------------------------------------------
MAint64 ReaderTextBase::GetPosition()
{
  if (input_==0) return 0;
#ifdef ZIP_USE
  if (compress_) return gzinput_->tellg();
#endif
  else  return input_->tellg();
}
