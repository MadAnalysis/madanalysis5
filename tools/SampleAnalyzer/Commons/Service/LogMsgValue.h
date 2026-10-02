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
 * @file LogMsgValue.h
 * @brief Occurrences of a logged message.
 */

#ifndef LOG_MSG_VALUE_H
#define LOG_MSG_VALUE_H


// STL headers
#include <string>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h" 


namespace MA5
{

/** @brief Number of occurrences of a message and name of the function issuing it. */
class LogMsgValue
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /** @brief Number of occurrences. */
  MAuint32 Counter_;

  /** @brief Name of the function issuing the message. */
  std::string Function_;
  
  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  LogMsgValue() : Counter_(0)
  { }

  /**
   * @brief Constructor.
   *
   * @param Counter number of occurrences.
   * @param Function function name.
   */
  LogMsgValue(const MAuint32& Counter, 
              const std::string& Function) : Counter_(Counter), Function_(Function)
  { }
  
  /** @brief Destructor. */
  ~LogMsgValue()
  {}

  /** @brief Reset the content. */
  void Reset()
  { Counter_=0; Function_=""; } 
  
  /**
   * @brief Accessor to the number of occurrences.
   *
   * @return the number.
   */
  const MAuint32& GetCounter() const
  {return Counter_;}

  /**
   * @brief Accessor to the function name.
   *
   * @return the name.
   */
  const std::string& GetFunction() const
  {return Function_;}
  
  /**
   * @brief Set the number of occurrences.
   *
   * @param Counter number.
   */
  void SetCounter(const MAuint32& Counter)
  {Counter_=Counter;}

  /**
   * @brief Set the function name.
   *
   * @param Function name.
   */
  void SetFunction(const std::string& Function)
  {Function_=Function;}

};

}

#endif

