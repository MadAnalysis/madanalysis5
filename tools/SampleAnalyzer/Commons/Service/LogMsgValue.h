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


#ifndef LOG_MSG_VALUE_H
#define LOG_MSG_VALUE_H


// STL headers
#include <string>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h" 


namespace MA5
{

//////////////////////////////////////////////////////////////////////////////
/// The class LogMsgValue contains the occurence of an exception and extra
/// information.
//////////////////////////////////////////////////////////////////////////////
class LogMsgValue
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /// Occurence of the exception
  MAuint32 Counter_;

  /// Name of the function from where the exception is thrown
  std::string Function_;
  
  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument 
  LogMsgValue() : Counter_(0)
  { }

  /// Constructor with arguments 
  LogMsgValue(const MAuint32& Counter, 
              const std::string& Function) : Counter_(Counter), Function_(Function)
  { }
  
  /// Destructor
  ~LogMsgValue()
  {}

  /// Reset
  void Reset()
  { Counter_=0; Function_=""; } 
  
  /// Accessor to the occurence
  const MAuint32& GetCounter() const
  {return Counter_;}

  /// Accessor to the name of the function
  const std::string& GetFunction() const
  {return Function_;}
  
  /// Mutator related to the occurence
  void SetCounter(const MAuint32& Counter)
  {Counter_=Counter;}

  /// Mutator related to name of the function
  void SetFunction(const std::string& Function)
  {Function_=Function;}

};

}

#endif

