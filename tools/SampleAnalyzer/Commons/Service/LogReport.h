////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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


#ifndef LOG_REPORT_H
#define LOG_REPORT_H


// STL headers
#include <iostream>
#include <fstream>
#include <string>
#include <typeinfo>
#include <sstream>
#include <map>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogMsgKey.h"
#include "SampleAnalyzer/Commons/Service/LogMsgValue.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h" 


namespace MA5
{

//////////////////////////////////////////////////////////////////////////////
/// The class LogReport counts the occurence of WARNING or ERROR message.
/// It is possible to apply a veto on the message display by two possible
/// options : one threshold for each message or a global threshold
//////////////////////////////////////////////////////////////////////////////
class LogReport
{
 protected:

  // ShortCuts
  typedef std::map<LogMsgKey,LogMsgValue> MsgCollection;
  typedef std::map<LogMsgKey,LogMsgValue>::const_iterator MsgConstIterator;
  typedef std::map<LogMsgKey,LogMsgValue>::iterator MsgIterator;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------

  /// Name of the Report
  std::string Name_;

  /// Message table 
  MsgCollection MsgTable_;

  /// Total number of occurences
  MAuint32 GeneralCounter_;

  /// Threshold applied on the total number of occurences
  MAint32 GlobalThreshold_;

  /// Threshold applied on the number of a message occurence
  MAint32 MsgThreshold_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor withtout argument
  LogReport() : GeneralCounter_(0), GlobalThreshold_(1000000), MsgThreshold_(10)
  {}

  /// Destructor
  ~LogReport()
  {}

  /// Clearing the content
  void Reset()
  { 
    GlobalThreshold_ = 1000000;
    MsgThreshold_ = 10;
    GeneralCounter_ = 0;
    MsgTable_.clear();
    Name_="";
  }

  /// Accessor to the name of the report
  const std::string& GetName() const 
  { return Name_; }

  /// Mutator related to the name of the report
  void SetName(const std::string& Name)
  { Name_=Name; }

  /// Accessor to the global threshold
  MAint32 GetGlobalThreshold() const
  { return GlobalThreshold_; }

  /// Mutator related to the global threshold
  void SetGlobalThreshold(const MAint32& value)
  { GlobalThreshold_=value; }

  /// Accessor to the message threshold
  MAint32 GetMsgThreshold() const
  { return MsgThreshold_; }

  /// Mutator related to the message threshold 
  void SetMsgThreshold(const MAint32& value)
  { MsgThreshold_=value; }

  /// Getting an iterator to an entry in the report table
  MsgIterator GetIterator(const LogMsgKey& key)
  {
    MsgIterator it = MsgTable_.find(key);
    if (it==MsgTable_.end())
    {
      std::pair<MsgIterator,bool> test = 
        MsgTable_.insert(std::pair<LogMsgKey,LogMsgValue>(key,
                                                          LogMsgValue() ));
      if (!test.second)
      {
        std::cout << "ERREUR" << std::endl;
        return MsgTable_.end();
      }
      it = test.first;
    }
    return it;
  }

  /// Adding an occurence in the report table
  MAbool Add(const std::string& filename, 
             const MAuint32& line, 
             const std::string& msg,
             const std::string& function)
  {
    // Getting the iterator
    MsgIterator iter = GetIterator( LogMsgKey(filename,
                                              line,
                                              msg) );
    if (iter==MsgTable_.end()) return false;

    // Incrementing the counter
    iter->second.SetCounter(iter->second.GetCounter()+1);
    GeneralCounter_++;

    // Setting Function if not already stored
    if (iter->second.GetFunction()=="") { iter->second.SetFunction(function); }

    // Veto for display ?
    if (static_cast<MAint32>(GeneralCounter_)>GlobalThreshold_ || 
        static_cast<MAint32>(iter->second.GetCounter())>MsgThreshold_) return false;
    else return true;
  } 

  /// Displaying the table
  void Print(LogStream& os=INFO) const
  { WriteGenericReport(os); }

  /// Displaying the table
  void WriteGenericReport(LogStream& os=INFO) const;

  /// Order relation for sorting entries in the table
  static MAbool OccurencyOrder(const std::pair<const LogMsgKey, LogMsgValue>* a,
                               const std::pair<const LogMsgKey, LogMsgValue>* b)
  { return a->second.GetCounter() > b->second.GetCounter(); }

};

}

#endif
