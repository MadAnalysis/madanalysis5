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
 * @file TimeService.h
 * @brief Measurement of the time spent in portions of code (START_TIMER/STOP_TIMER, active with TIMER_MODE).
 */

// FIXME: same include guard as CompilationService.h (TIMER_SERVICE_H).
#ifndef TIMER_SERVICE_H
#define TIMER_SERVICE_H


// STL headers
#include <map>
#include <string>
#include <iostream>
#include <ctime>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h" 
#include "SampleAnalyzer/Commons/Service/TimeMeasureType.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


#ifdef TIMER_MODE

  /** @brief Start the chronometer called name (TIMER_MODE only). */
  #define START_TIMER(name) TimeService::GetInstance()->StartTime(name);

  /** @brief Stop the chronometer called name (TIMER_MODE only). */
  #define STOP_TIMER(name)  TimeService::GetInstance()->StopTime(name);

#else

  #define START_TIMER(name) 
  #define STOP_TIMER(name)  

#endif


namespace MA5
{

/** @brief Singleton measuring the CPU time spent in named portions of code. */
class TimeService
{
 private:

  /** @brief Table of the measurements, by name. */
  typedef std::map<std::string,TimeMeasureType> TimeCollection;
  typedef std::map<std::string,TimeMeasureType>::const_iterator TimeConstIterator;
  typedef std::map<std::string,TimeMeasureType>::iterator TimeIterator;


  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /** @brief Unique instance. */
  static TimeService* service_;

  /** @brief Measurements, by name. */
  TimeCollection MeasureTable_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 private:

  /** @brief Constructor. */
  TimeService()
  {}

  /** @brief Destructor. */
  ~TimeService()
  {}

  /**
   * @brief Order the measurements by decreasing average time.
   *
   * @param a first measurement.
   * @param b second measurement.
   * @return true if a comes first.
   */
  static MAbool timingOrder(const std::pair<const std::string,TimeMeasureType>* a,
                          const std::pair<const std::string,TimeMeasureType>* b)
  { return (a->second.GetAverage()>b->second.GetAverage()); }

  /**
   * @brief Get the measurement of a name (created if needed).
   *
   * @param name name.
   * @return an iterator to the measurement.
   */
  TimeIterator GetIterator(const std::string& name)
  {
    TimeIterator it = MeasureTable_.find(name);
    if (it==MeasureTable_.end())
    {
      std::pair<TimeIterator,MAbool> test = 
       MeasureTable_.insert(
              std::pair<std::string,TimeMeasureType>(name,TimeMeasureType()));
      if (!test.second)
      {
        std::cout << "ERREUR" << std::endl;
        return MeasureTable_.end();
      }
      it = test.first;
    }
    return it;
  }


 public:

  /**
   * @brief Get the unique instance (created at the first call).
   *
   * @return the instance.
   */
  static TimeService* GetInstance()
  {
    if (service_==0) service_ = new TimeService;
    return service_;
  }

  /** @brief Delete the unique instance. */
  static void Kill()
  {
    if (service_!=0) delete service_;
    service_=0;
  }

  /**
   * @brief Start a measurement.
   *
   * @param name name of the measurement.
   */
  void StartTime(const std::string& name)
  {
    TimeIterator it = GetIterator(name);
    if (it==MeasureTable_.end()) return;

    // Take time measure + storage
    it->second.SetStart(std::clock());
  }

  /**
   * @brief Stop a measurement.
   *
   * @param name name of the measurement.
   */
  void StopTime(const std::string& name)
  {
    // Take time measure
    MAuint32 timing = std::clock();

    // 
    TimeIterator it = GetIterator(name);
    if (it==MeasureTable_.end()) return;

    //Store the value
    it->second.SetStop(timing);
  }

  /**
   * @brief Print the table of the measurements.
   *
   * @param os logger.
   */
  void Print(LogStream& os=INFO) const
  {
    WriteGenericReport(os);
  }

  /**
   * @brief Print the table of the measurements (TIMER_MODE only).
   *
   * @param os logger.
   */
  void WriteGenericReport(LogStream& os=INFO) const;

};

}

#endif
