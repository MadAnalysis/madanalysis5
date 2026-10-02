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
 * @file TimeMeasureType.h
 * @brief Statistics of the time measurements of one portion of code.
 */

#ifndef TIMER_MEASURE_TYPE_H
#define TIMER_MEASURE_TYPE_H


// STL headers
#include <map>
#include <string>
#include <iostream>
#include <cmath>
#include <ctime>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h" 
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"
#include "SampleAnalyzer/Commons/Service/LogStream.h"
#include "SampleAnalyzer/Commons/Service/ConvertService.h"


namespace MA5
{

/** @brief Statistics (min, max, average, deviation) of the CPU time spent in a portion of code. */
class TimeMeasureType
{
 private:

  /** @brief Has the chronometer been started? */
  MAbool     StartFilled_;
  /** @brief Clock value at the start. */
  MAuint32   StartCurrent_;
  /** @brief Minimum, maximum, number of measurements, sum and sum of squares of the times [s]. */
  MAfloat32  Min_;
  MAfloat32  Max_;
  MAuint32   NIterations_;
  MAfloat32  Sum_;
  MAfloat32  Sum2_; 

 public:

  /** @brief Constructor. */
  TimeMeasureType()
  { Reset(); }
  
  /** @brief Destructor. */
  ~TimeMeasureType()
  {}


  /** @brief Reset the statistics. */
  void Reset()
  {
    StartFilled_=false; 
    StartCurrent_=0; 
    Min_=0.; 
    Max_=0.;
    NIterations_=0; 
    Sum_=0.;
    Sum2_=0.;
  } 
  
  /**
   * @brief Accessor to the minimum time.
   *
   * @return the time [s].
   */
  const MAfloat32& GetMin() const {return Min_;}
  /**
   * @brief Accessor to the maximum time.
   *
   * @return the time [s].
   */
  const MAfloat32& GetMax() const {return Max_;}
  /**
   * @brief Accessor to the number of measurements.
   *
   * @return the number.
   */
  const MAuint32& GetNIterations() const {return NIterations_;}
  /**
   * @brief Average time.
   *
   * @return the time [s] (0 with a warning if there is no measurement).
   */
  const MAfloat32 GetAverage() const
  { 
    try
    {
      if (NIterations_==0) throw EXCEPTION_WARNING("Number of iterations is null","",0);
      return Sum_/static_cast<MAfloat32>(NIterations_); 
    }
    catch (const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
      return 0.;
    }    
  }
  /**
   * @brief Standard deviation of the time.
   *
   * @return the deviation [s] (0 with a warning or error if undefined).
   */
  const MAfloat32 GetDeviation() const
  { 
    try
    {
      if (NIterations_==0) throw EXCEPTION_WARNING("Number of iterations is null",
                                                   "",0);
      MAfloat32 value = Sum2_/static_cast<MAfloat32>(NIterations_) - GetAverage()*GetAverage();
      if (value<0) throw EXCEPTION_ERROR("Impossible to calcultate the square root of a negative value",
                                         "negative value = "+CONVERT->ToString(value),
                                         1);
      return std::sqrt(value);
    }
    catch(const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
      return 0.;
    }
  }
  
  /**
   * @brief Start a measurement.
   *
   * @param value clock value.
   */
  void SetStart(const MAuint32& value)
  { StartCurrent_=value; StartFilled_=true; }

  /**
   * @brief Stop a measurement and update the statistics.
   *
   * @param StopCurrent clock value.
   */
  void SetStop(const MAuint32& StopCurrent)
  {
    // NOTE: unsigned subtraction: 'timing' can never be negative (a clock wrap-around gives a huge value).
    MAfloat32 timing = static_cast<MAfloat32>(StopCurrent-StartCurrent_)/CLOCKS_PER_SEC;
    if (timing<0) return;  
    if (!StartFilled_) return;

    NIterations_++;
    if (Sum_==0. || timing<Min_) Min_=timing;
    if (Sum_==0. || timing>Max_) Max_=timing;
    Sum_  += timing;
    Sum2_ += timing*timing;    

    StartCurrent_=0;
  }

  /**
   * @brief Print the column titles of Print().
   *
   * @param os logger.
   */
  static void PrintHeader(LogStream& os = INFO)
  {
    os.width(10); os << std::left << "Min";
    os.width(10); os << std::left << "Max";
    os.width(12); os << std::left << "NIterations";
    os.width(10); os << std::left << "Sum";
    os.width(10); os << std::left << "Sum2";
    os << endmsg;
  }

  /**
   * @brief Print the statistics.
   *
   * @param os logger.
   */
  void Print(LogStream& os = INFO) const
  {
    TimeMeasureType::PrintHeader(os);    
    os.width(10); os << std::left << Min_;
    os.width(10); os << std::left << Max_;
    os.width(12); os << std::left << NIterations_;
    os.width(10); os << std::left << Sum_;
    os.width(10); os << std::left << Sum2_;
    os << endmsg;
  } 

};

}

#endif
