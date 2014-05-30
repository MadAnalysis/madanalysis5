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


// SampleHeader headers
#include "SampleAnalyzer/Process/Writer/SAFWriter.h"

using namespace MA5;

// -----------------------------------------------------------------------------
// WriteHeader
// -----------------------------------------------------------------------------
bool SAFWriter::WriteHeader()
{
  // Header
  *output_ << "<SAFheader>" << std::endl;
  *output_ << "</SAFheader>" << std::endl;
  *output_ << std::endl;
  return true;
}

bool SAFWriter::WriteHeader(const SampleFormat& mySample)
{
  // Header
  *output_ << "<SAFheader>" << std::endl;
  *output_ << "</SAFheader>" << std::endl;
  *output_ << std::endl;

  // SampleGlobalInfo
  *output_ << "<SampleGlobalInfo>" << std::endl;

  // title line
  output_->width(15);
  *output_ << std::left << "# xsection ";
  output_->width(15);
  *output_ << std::left << "xsection_error ";
  output_->width(15);
  *output_ << std::left << "nevents ";
  output_->width(15);
  *output_ << std::left << "sum_weight+ ";
  output_->width(15);
  *output_ << std::left << "sum_weight- ";
  *output_ << std::endl;

  // data
  if (mySample.mc()!=0)
  {
    output_->width(15);
    *output_ << std::left << std::scientific 
                        << mySample.mc()->xsection();
    output_->width(15);
    *output_ << std::left << std::scientific 
                        << mySample.mc()->xsection_error();
    output_->width(15);
    *output_ << std::left << std::scientific 
             << mySample.nevents();
    output_->width(15);
    *output_ << std::left << std::scientific 
                        << mySample.mc()->sumweight_positive();
    output_->width(15);
    *output_ << std::left << std::scientific 
                        << mySample.mc()->sumweight_negative();
    *output_ << std::endl;
  }
  else
  {
    output_->width(15);
    *output_ << std::left << std::scientific 
                        << 0;
    output_->width(15);
    *output_ << std::left << std::scientific 
                        << 0;
    output_->width(15);
    *output_ << std::left << std::scientific 
             << mySample.nevents();
    output_->width(15);
    *output_ << std::left << std::scientific 
                        << 0;
    output_->width(15);
    *output_ << std::left << std::scientific 
                        << 0;
    *output_ << std::endl;
  }
  *output_ << "</SampleGlobalInfo>" << std::endl;
  *output_ << std::endl;

  return true;
}


// -----------------------------------------------------------------------------
// WriteFiles
// -----------------------------------------------------------------------------
bool SAFWriter::WriteFiles(const std::vector<SampleFormat>& mySamples)
{
  // FileInfo
  *output_ << "<FileInfo>" << std::endl;
  for (unsigned int i=0;i<mySamples.size();i++)
  {
    output_->width(40); 
    *output_ << std::left << "\""+mySamples[i].name()+"\"";
    if (i<2 || i>=(mySamples.size()-2)) 
      *output_ << " # file " << i+1 << " / " << mySamples.size();
    *output_ << std::endl;
  }
  *output_ << "</FileInfo>" << std::endl;
  *output_ << std::endl;

  // SampleDetailedInfo
  *output_ << "<SampleDetailedInfo>" << std::endl;

  // title line
  output_->width(15);
  *output_ << std::left << "# xsection ";
  output_->width(15);
  *output_ << std::left << "xsection_error ";
  output_->width(15);
  *output_ << std::left << "nevents ";
  output_->width(15);
  *output_ << std::left << "sum_weight+ ";
  output_->width(15);
  *output_ << std::left << "sum_weight- ";
  *output_ << std::endl;

  // data
  for (unsigned int i=0;i<mySamples.size();i++)
  {
    if (mySamples[i].mc()!=0)
    {
      output_->width(15);
      *output_ << std::left << std::scientific 
                          << mySamples[i].mc()->xsection();
      output_->width(15);
      *output_ << std::left << std::scientific 
                          << mySamples[i].mc()->xsection_error();
      output_->width(15);
      *output_ << std::left << std::scientific 
               << mySamples[i].nevents();
      output_->width(15);
      *output_ << std::left << std::scientific 
                          << mySamples[i].mc()->sumweight_positive();
      output_->width(15);
      *output_ << std::left << std::scientific 
                          << mySamples[i].mc()->sumweight_negative(); 
    }
    else
    {
      output_->width(15);
      *output_ << std::left << std::scientific 
                          << 0;
      output_->width(15);
      *output_ << std::left << std::scientific 
                          << 0;
      output_->width(15);
      *output_ << std::left << std::scientific 
               << mySamples[i].nevents();
      output_->width(15);
      *output_ << std::left << std::scientific 
                          << 0;
      output_->width(15);
      *output_ << std::left << std::scientific 
                          << 0;
    }
    if (i<2 || i>=(mySamples.size()-2)) 
      *output_ << " # file " << i+1 << " / " << mySamples.size();
    *output_ << std::endl;
  }

  *output_ << "</SampleDetailedInfo>" << std::endl;
  *output_ << std::endl;

  return true;
}



// -----------------------------------------------------------------------------
// WriteEvent
// -----------------------------------------------------------------------------
bool SAFWriter::WriteEvent(const EventFormat& myEvent,
                           const SampleFormat& mySample)
{
  if (myEvent.mc()==0 && mySample.mc()==0) return true;
  return true;
}


// -----------------------------------------------------------------------------
// WriteFoot
// -----------------------------------------------------------------------------
bool SAFWriter::WriteFoot(const SampleFormat& mySample)
{
  *output_ << "<SAFfooter>" << std::endl;
  *output_ << "</SAFfooter>" << std::endl;
  return true;
}

bool SAFWriter::WriteFoot()
{
  *output_ << "<SAFfooter>" << std::endl;
  *output_ << "</SAFfooter>" << std::endl;
  return true;
}
