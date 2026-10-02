////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


/**
 * @file LoopService.h
 * @brief Recursive navigation in the Monte Carlo history (LOOP), protected against infinite loops.
 */

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


/** @brief Shortcut to the LoopService singleton. */
#define LOOP MA5::LoopService::GetInstance()   


namespace MA5
{

/**
 * @brief Singleton implementing recursive tests on the mother chain of Monte Carlo particles.
 *
 * The number of recursive calls is bounded to protect against cyclic histories.
 */
class LoopService
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private :

  /** @brief Unique instance. */
  static LoopService* Service_;

  /** @brief Maximum number of recursive calls. */
  MAuint32 NcallThreshold_;

  /** @brief Current number of recursive calls. */
  MAuint32 Ncalls_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 private:

  /** @brief Constructor. */
  LoopService() 
  {}

  /** @brief Destructor. */
  ~LoopService()
  {}

  /** @brief Reset the call counter. */
  void Initialize()
  { Ncalls_=0; }

 public:

  /**
   * @brief Get the unique instance (created at the first call).
   *
   * @return the instance.
   */
  static LoopService* GetInstance()
  {
    if (Service_==0) Service_ = new LoopService;
    return Service_;
  }

  /** @brief Delete the unique instance. */
  static void Kill()
  {
    if (Service_!=0) delete Service_;
    Service_=0;
  }

  /**
   * @brief Does a photon come from a tau decay (i.e. is it irrelevant for the photon selection)?
   *
   * @param part photon.
   * @param mySample sample (generator-specific patches).
   * @param Threshold maximum number of recursive calls.
   * @return true if a tau is found among the ancestors.
   */
  MAbool IrrelevantPhoton(const MCParticleFormat* part, 
                          const SampleFormat& mySample,
                          MAuint32 Threshold = 100000)
  {
    Ncalls_=0;
    NcallThreshold_ = Threshold;
    return IrrelevantPhoton_core(part,mySample);
  }

  /**
   * @brief Does a particle come from a hadron decay?
   *
   * @param part particle.
   * @param mySample sample (generator-specific patches).
   * @param Threshold maximum number of recursive calls.
   * @return true if a hadron is found among the ancestors.
   */
  MAbool ComingFromHadronDecay(const MCParticleFormat* part, 
                               const SampleFormat& mySample,
                               MAuint32 Threshold = 100000)
  {
    Ncalls_=0;
    NcallThreshold_ = Threshold;
    return ComingFromHadronDecay_core(part,mySample);
  }


 private:

  /**
   * @brief Recursive part of IrrelevantPhoton().
   *
   * @param part particle.
   * @param mySample sample.
   * @return true if a tau is found among the ancestors.
   */
  MAbool IrrelevantPhoton_core(const MCParticleFormat* part, 
                               const SampleFormat& mySample);

  /**
   * @brief Recursive part of ComingFromHadronDecay().
   *
   * @param part particle.
   * @param mySample sample.
   * @return true if a hadron is found among the ancestors.
   */
  MAbool ComingFromHadronDecay_core(const MCParticleFormat* part, 
                                    const SampleFormat& mySample);

  /**
   * @brief Count a recursive call.
   *
   * @return true (with a warning) if the maximum number of calls is exceeded.
   */
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
