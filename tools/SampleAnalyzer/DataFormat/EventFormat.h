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


#ifndef EventFormat_h
#define EventFormat_h

// STL headers
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/DataFormat/MCEventFormat.h"
#include "SampleAnalyzer/DataFormat/RecEventFormat.h"
#include "SampleAnalyzer/Service/LogService.h"
#ifdef FAC_USE
  #include "SampleAnalyzer/Reader/FACdataformat.h"
#endif


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

#ifdef FAC_USE
  /// pointer to FAC format
  FAC::EventFormat* fac_;
#endif

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor withtout arguments
  EventFormat()
  { 
    rec_=0;
    mc_=0; 
#ifdef FAC_USE
    fac_=0;
#endif

  }

  /// Destructor
  ~EventFormat()
  { 
  }

  /// Accessor to generated particles (read-only mode)
  const MCEventFormat  * mc()   const {return mc_; }

  /// Accessor to reconstructed objects (read-only mode)
  const RecEventFormat * rec()  const {return rec_;}

#ifdef FAC_USE
  /// Accessor FAC objects (read-only mode)
  const FAC::EventFormat* fac() const {return fac_;}
#endif

  /// Accessor to generated particles
  MCEventFormat  * mc()   {return mc_; }

  /// Accessor to reconstructed objects
  RecEventFormat * rec()  {return rec_;}

#ifdef FAC_USE
  /// Accessor to FAC objects
  FAC::EventFormat* fac() {return fac_;}
#endif

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


#ifdef FAC_USE
  void InitializeFac()
  {
    if (fac_!=0) 
    {
      WARNING << "FAC part of the SampleFormat is already initialized" 
              << endmsg;
    }
    else fac_=new FAC::EventFormat();
  }
#endif

  /// Free allocated memory
  void Delete()
  {
    if (rec_!=0) delete rec_;
    if (mc_!=0)  delete mc_;
  }

};

}

#endif
