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
 * @file LogReport.h
 * @brief Counter of the WARNING/ERROR messages, with display thresholds.
 */

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

/**
 * @brief Counts the occurrences of the WARNING or ERROR messages.
 *
 * A message is displayed only while its number of occurrences is below the message
 * threshold (5 by default) and the total number of messages is below the global
 * threshold (1000000 by default). A summary table is printed at the end of the run.
 */
class LogReport
{
 protected:

  /** @brief Table of the messages: key (file, line, text) and occurrences. */
  typedef std::map<LogMsgKey,LogMsgValue> MsgCollection;
  typedef std::map<LogMsgKey,LogMsgValue>::const_iterator MsgConstIterator;
  typedef std::map<LogMsgKey,LogMsgValue>::iterator MsgIterator;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------

  /** @brief Name of the report. */
  std::string Name_;

  /** @brief Table of the messages. */
  MsgCollection MsgTable_;

  /** @brief Total number of messages. */
  MAuint32 GeneralCounter_;

  /** @brief Threshold on the total number of messages. */
  MAint32 GlobalThreshold_;

  /** @brief Threshold on the number of occurrences of each message. */
  MAint32 MsgThreshold_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor (default thresholds). */
  LogReport() : GeneralCounter_(0), GlobalThreshold_(1000000), MsgThreshold_(5)
  {}

  /** @brief Destructor. */
  ~LogReport()
  {}

  /** @brief Reset the thresholds and the table. */
  void Reset()
  { 
    GlobalThreshold_ = 1000000;
    MsgThreshold_ = 5;
    GeneralCounter_ = 0;
    MsgTable_.clear();
    Name_="";
  }

  /**
   * @brief Accessor to the name of the report.
   *
   * @return the name.
   */
  const std::string& GetName() const 
  { return Name_; }

  /**
   * @brief Set the name of the report.
   *
   * @param Name name.
   */
  void SetName(const std::string& Name)
  { Name_=Name; }

  /**
   * @brief Accessor to the global threshold.
   *
   * @return the threshold.
   */
  MAint32 GetGlobalThreshold() const
  { return GlobalThreshold_; }

  /**
   * @brief Set the global threshold.
   *
   * @param value threshold.
   */
  void SetGlobalThreshold(const MAint32& value)
  { GlobalThreshold_=value; }

  /**
   * @brief Accessor to the message threshold.
   *
   * @return the threshold.
   */
  MAint32 GetMsgThreshold() const
  { return MsgThreshold_; }

  /**
   * @brief Set the message threshold.
   *
   * @param value threshold.
   */
  void SetMsgThreshold(const MAint32& value)
  { MsgThreshold_=value; }

  /**
   * @brief Get the entry of a message (created if needed).
   *
   * @param key key of the message.
   * @return an iterator to the entry.
   */
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
        std::cout << "ERROR" << std::endl;
        return MsgTable_.end();
      }
      it = test.first;
    }
    return it;
  }

  /**
   * @brief Count an occurrence of a message.
   *
   * @param filename file name.
   * @param line line number.
   * @param msg text.
   * @param function function name.
   * @return true if the message must be displayed (thresholds not reached).
   */
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

  /**
   * @brief Print the summary table.
   *
   * @param os logger.
   */
  void Print(LogStream& os=INFO) const
  { WriteGenericReport(os); }

  /**
   * @brief Print the summary table, sorted by number of occurrences.
   *
   * @param os logger.
   */
  void WriteGenericReport(LogStream& os=INFO) const;

  /**
   * @brief Order the entries by decreasing number of occurrences.
   *
   * @param a first entry.
   * @param b second entry.
   * @return true if a comes first.
   */
  static MAbool OccurencyOrder(const std::pair<const LogMsgKey, LogMsgValue>* a,
                               const std::pair<const LogMsgKey, LogMsgValue>* b)
  { return a->second.GetCounter() > b->second.GetCounter(); }

};

}

#endif
