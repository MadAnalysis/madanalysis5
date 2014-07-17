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


#ifndef TIMER_SERVICE_H
#define TIMER_SERVICE_H


// STL headers
#include<map>
#include<string>
#include<iostream>
#include<ctime>

// ROOT headerse
#include <Rtypes.h> 

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/TimeMeasureType.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


#ifdef TIMER_MODE

  // ShortCut for starting the chronometer 
  #define START_TIMER(name) TimeService::GetInstance()->StartTime(name);

  // ShortCut for stopping the chronometer 
  #define STOP_TIMER(name)  TimeService::GetInstance()->StopTime(name);

#else

  #define START_TIMER(name) 
  #define STOP_TIMER(name)  

#endif


namespace MA5
{

//////////////////////////////////////////////////////////////////////////////
/// The class TimeService allows to determine the time budget of the program
/// and stores information in a table.
///
/// TimeService is a singleton-pattern-based class : only one instance.
/// Getting the only one instance : TimeService::GetInstance()
//////////////////////////////////////////////////////////////////////////////
class TimeService
{
 private:

  // ShortCuts
  typedef std::map<std::string,TimeMeasureType> TimeCollection;
  typedef std::map<std::string,TimeMeasureType>::const_iterator TimeConstIterator;
  typedef std::map<std::string,TimeMeasureType>::iterator TimeIterator;


  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /// Pointer to the unique instance of TimeService
  static TimeService* service_;

  // Table containing stats about each measure
  TimeCollection MeasureTable_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 private:

  /// Constructor without arguments
  TimeService()
  {}

  /// Destructor
  ~TimeService()
  {}

  /// Order relation for sorting the table according to timing
  static bool timingOrder(const std::pair<const std::string,TimeMeasureType>* a,
                          const std::pair<const std::string,TimeMeasureType>* b)
  { return (a->second.GetAverage()>b->second.GetAverage()); }

  /// Getting an iterator to a given entry in the table
  TimeIterator GetIterator(const std::string& name)
  {
    TimeIterator it = MeasureTable_.find(name);
    if (it==MeasureTable_.end())
    {
      std::pair<TimeIterator,bool> test = 
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

  /// Getting the unique instance of TimeService
  static TimeService* GetInstance()
  {
    if (service_==0) service_ = new TimeService;
    return service_;
  }

  /// Deleting the unique instance of TimeService
  static void Kill()
  {
    if (service_!=0) delete service_;
    service_=0;
  }

  /// Start a timing measure
  void StartTime(const std::string& name)
  {
    TimeIterator it = GetIterator(name);
    if (it==MeasureTable_.end()) return;

    // Take time measure + storage
    it->second.SetStart(std::clock());
  }

  /// Stop a timing measure
  void StopTime(const std::string& name)
  {
    // Take time measure
    UInt_t timing = std::clock();

    // 
    TimeIterator it = GetIterator(name);
    if (it==MeasureTable_.end()) return;

    //Store the value
    it->second.SetStop(timing);
  }

  /// Display the measure table
  void Print(LogStream& os=INFO) const
  {
    WriteGenericReport(os);
  }

  /// Display the measure table
  void WriteGenericReport(LogStream& os=INFO) const;

};

}

#endif
