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


#ifndef EventFormat_h
#define EventFormat_h

// STL headers
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/MCEventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecEventFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class LHEReader;
class LHCOReader;
class HEPMCReader;
class ROOTReader;

class EventFormat
{
  friend class LHEReader;
  friend class LHCOReader;
  friend class HEPMCReader;
  friend class ROOTReader;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private : 

  /// pointer to reconstructed objects (by a detector simulation)
  RecEventFormat * rec_;

  /// pointer to generated particles
  MCEventFormat  * mc_;

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor withtout arguments
  EventFormat()
  { 
    rec_=0;
    mc_=0; 
  }

  /// Destructor
  ~EventFormat()
  { 
  }

  /// Accessor to generated particles (read-only mode)
  const MCEventFormat  * mc()   const {return mc_; }

  /// Accessor to reconstructed objects (read-only mode)
  const RecEventFormat * rec()  const {return rec_;}

  /// Accessor to generated particles
  MCEventFormat  * mc()   {return mc_; }

  /// Accessor to reconstructed objects
  RecEventFormat * rec()  {return rec_;}

  /// Initializing the pointer to generated particles
  void InitializeMC()
  {
    if (mc_!=0) 
    {
      mc_->Reset();
    }
    else mc_=new MCEventFormat();
  }

  /// Initializing the pointer to reconstructed objects
  void InitializeRec()
  {
    if (rec_!=0) 
    {
      rec_->Reset();
    }
    else rec_=new RecEventFormat();
  }

  /// Free allocated memory
  void Delete()
  {
    if (rec_!=0) delete rec_;
    if (mc_!=0)  delete mc_;
  }

};

}

#endif
