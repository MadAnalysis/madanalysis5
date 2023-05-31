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


#ifndef LOOP_SERVICE_H
#define LOOP_SERVICE_H


// STL headers 
#include <iostream>
#include <string>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h" 


// ShortCut to access to LoopService
#define LOOP MA5::LoopService::GetInstance()   


namespace MA5
{

//////////////////////////////////////////////////////////////////////////////
/// The class LoopService contains static methods used for converting
/// all types into string type.
///
/// LoopService is a singleton-pattern-based class : only one instance.
/// Getting the only one instance : LoopService::GetInstance()
//////////////////////////////////////////////////////////////////////////////
class LoopService
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private :

  /// Pointer to the unique instance of LoopService
  static LoopService* Service_;

  /// Threshold to the number of calls
  MAuint32 NcallThreshold_;

  /// Current number of calls
  MAuint32 Ncalls_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 private:

  /// Constructor without argument
  LoopService() 
  {}

  /// Destructor
  ~LoopService()
  {}

  /// (Re)initialzing the streamer
  void Initialize()
  { Ncalls_=0; }

 public:

  /// Getting the unique instance of LoopService
  static LoopService* GetInstance()
  {
    if (Service_==0) Service_ = new LoopService;
    return Service_;
  }

  /// Deleting the unique instance of Convert Service
  static void Kill()
  {
    if (Service_!=0) delete Service_;
    Service_=0;
  }

  /// Determing if a photon coming from signal
  MAbool IrrelevantPhoton(const MCParticleFormat* part, 
                          const SampleFormat& mySample,
                          MAuint32 Threshold = 100000)
  {
    Ncalls_=0;
    NcallThreshold_ = Threshold;
    return IrrelevantPhoton_core(part,mySample);
  }

  /// Determing if a particle coming from hadron decay
  MAbool ComingFromHadronDecay(const MCParticleFormat* part, 
                               const SampleFormat& mySample,
                               MAuint32 Threshold = 100000)
  {
    Ncalls_=0;
    NcallThreshold_ = Threshold;
    return ComingFromHadronDecay_core(part,mySample);
  }


 private:

  /// Determing if a photon coming from signal
  MAbool IrrelevantPhoton_core(const MCParticleFormat* part, 
                               const SampleFormat& mySample);

  /// Determing if a particle coming from hadron decay
  MAbool ComingFromHadronDecay_core(const MCParticleFormat* part, 
                                    const SampleFormat& mySample);

  /// Threshold
  MAbool ReachThreshold()
  {
    try
    {
      if (Ncalls_ > NcallThreshold_) throw EXCEPTION_WARNING("Number of calls exceed: infinite loops detected","",0);
      Ncalls_++;
    }
    catch(const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
      return true;
    }    

    return false;
  }

};

}

#endif
