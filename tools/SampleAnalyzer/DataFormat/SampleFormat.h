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


#ifndef SAMPLE_DATAFORMAT_H
#define SAMPLE_DATAFORMAT_H

// STL headers
#include <map>
#include <iostream>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/DataFormat/MCSampleFormat.h"
#include "SampleAnalyzer/DataFormat/RecSampleFormat.h"
#include "SampleAnalyzer/Service/LogService.h"

namespace MA5
{

class LHEReader;
class LHCOReader;
class HEPMCReader;
class SampleAnalyzer;

class SampleFormat
{
  friend class LHEReader;
  friend class LHCOReader;
  friend class SampleAnalyzer;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  std::string       name_;      /// file name
  ULong64_t           nevents_;   /// Number of events in the file
  MCSampleFormat  * mc_;
  RecSampleFormat * rec_;


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor withtout arguments
  SampleFormat()
  { 
    rec_=0;
    mc_=0; 
    nevents_=0;
  }

  /// Destructor
  ~SampleFormat()
  { 
  }
 
  /// Accessor to Monte Carlo information (read-only)
  const MCSampleFormat * mc() const
  { return mc_; }

  /// Accessor to reconstruction information (read-only)
  const RecSampleFormat * rec() const
  { return rec_; } 

  /// Accessor to Monte Carlo information
  MCSampleFormat * mc()
  { return mc_; }

  /// Accessor to reconstruction information
  RecSampleFormat * rec()
  { return rec_; }

  /// Accessor to the sample name
  const std::string& name() const
  { return name_; }

  /// Accessor to the number of events
  const ULong64_t& nevents() const
  { return nevents_; }

  /// Set the sample name
  void setName(const std::string& name)
  {name_=name;}

  /// Set the number of events in the sample
  void setNEvents(ULong64_t v)
  {nevents_=v;}

  /// Initialize MonteCarlo part
  void InitializeMC()
  {
    if (mc_!=0) 
    {
      WARNING << "MC part of the SampleFormat is already initialized" << endmsg;
    }
    else mc_=new MCSampleFormat();
  }

  /// Initialize Rec part
  void InitializeRec()
  {
    if (rec_!=0) 
    {
      WARNING << "REC part of the SampleFormat is already initialized" << endmsg;
    }
    else rec_=new RecSampleFormat();
  }

  /// Free allocates memory
  void Delete()
  {
    if (rec_!=0) delete rec_;
    if (mc_!=0)  delete mc_;
  }

};

}

#endif
