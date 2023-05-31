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

class TimeMeasureType
{
 private:

  MAbool     StartFilled_;
  MAuint32   StartCurrent_;
  MAfloat32  Min_;
  MAfloat32  Max_;
  MAuint32   NIterations_;
  MAfloat32  Sum_;
  MAfloat32  Sum2_; 

 public:

  // Constructor without argument 
  TimeMeasureType()
  { Reset(); }
  
  // Destructor
  ~TimeMeasureType()
  {}


  // Reset
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
  
  // Accessors
  const MAfloat32& GetMin() const {return Min_;}
  const MAfloat32& GetMax() const {return Max_;}
  const MAuint32& GetNIterations() const {return NIterations_;}
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
  
  // Mutators
  void SetStart(const MAuint32& value)
  { StartCurrent_=value; StartFilled_=true; }

  void SetStop(const MAuint32& StopCurrent)
  {
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

  static void PrintHeader(LogStream& os = INFO)
  {
    os.width(10); os << std::left << "Min";
    os.width(10); os << std::left << "Max";
    os.width(12); os << std::left << "NIterations";
    os.width(10); os << std::left << "Sum";
    os.width(10); os << std::left << "Sum2";
    os << endmsg;
  }

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
