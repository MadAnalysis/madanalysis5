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


#ifndef LOG_SERVICE_H
#define LOG_SERVICE_H

// STL headers
#include <iostream>
#include <fstream>
#include <string>
#include <typeinfo>
#include <sstream>
#include <map>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogStream.h"

// ROOT headers
#include <Rtypes.h> 


// ShortCuts to the different loggers
#define DEBUG      MA5::LogService::GetInstance()->GetDebug()
#define INFO       MA5::LogService::GetInstance()->GetInfo()
#define WARNING    MA5::LogService::GetInstance()->GetWarning()
#define ERROR      MA5::LogService::GetInstance()->GetError()
#define USER(id)   MA5::LogService::GetInstance()->GetUser(id)

namespace MA5
{

//////////////////////////////////////////////////////////////////////////////
/// The class LogService gives the access to loggers ; the loggers are sorted
/// according to their level of verbosity.
///
/// LogService is a singleton-pattern-based class : only one instance.
/// Getting the only one instance : LogService::GetInstance()
//////////////////////////////////////////////////////////////////////////////
class LogService
{
 public:

  // Definition of the different levels of verbosity
  enum VerbosityLevel{DEBUG_LEVEL=1,USER_LEVEL=2,
                      INFO_LEVEL=3,WARNING_LEVEL=4,
                      ERROR_LEVEL=5};

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /// Pointer to the unique instance of LogService
  static LogService* Service_;

  /// Veto on verbosity
  VerbosityLevel Level_;

  /// Logger with DEBUG verbosity level
  LogStream Debug_;

  /// Logger with INFO verbosity level
  LogStream Info_;

  /// Logger with WARNING verbosity level
  LogStream Warning_;

  /// Logger with ERROR verbosity level
  LogStream Error_;

  /// Collection of logger with USER verbosity level
  std::map<std::string,LogStream> User_;

  /// Veto on the user name for USER logger
  std::string ExclusiveUser_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 private:

  /// Constructor without argument
  LogService() 
  {
    // Initializing Debug streamer
    Debug_.SetColor(LogStream::YELLOW);
    Debug_.SetPrompt("DEBUG:   ");

    // Initializing Info streamer
    Info_.SetColor(LogStream::NONE);
    Info_.SetPrompt("");

    // Initializing Warning streamer
    Warning_.SetColor(LogStream::PURPLE);
    Warning_.SetPrompt("WARNING: ");

    // Initializing Error streamer
    Error_.SetColor(LogStream::RED);
    Error_.SetPrompt("ERROR:   ");

    // Setting default verbosity level
    SetVerbosityLevel(INFO_LEVEL);
  }

  /// Destructor
  ~LogService()
  {}

  /// Mute a given USER logger 
  void SetGlobalMuteUser(Bool_t mute)
  {
    for (std::map<std::string,LogStream>::iterator 
    it=User_.begin(); it!=User_.end(); it++)
    {
      if (mute) it->second.SetMute();
      else if (ExclusiveUser_!="" && ExclusiveUser_!=it->first) 
           it->second.SetMute();
      else it->second.SetUnMute();
    }
  }

 public:

  /// Getting the unique instance of LogService
  static LogService* GetInstance()
  {
    if (Service_==0) Service_ = new LogService;
    return Service_;
  }

  /// Deleting the unique instance of LogService
  static void Kill()
  {
    if (Service_!=0) delete Service_;
    Service_=0;
  }
  
  /// Accessor to the DEBUG logger
  LogStream& GetDebug()
  { return Debug_; }

  /// Accessor to the INFO logger
  LogStream& GetInfo()
  { return Info_; }

  /// Accessor to the WARNING logger
  LogStream& GetWarning()
  { return Warning_; }

  /// Accessor to the ERROR logger
  LogStream& GetError()
  { return Error_; }

  /// Accessor to the appropriate logger
  LogStream& GetUser(const std::string& name)
  { 
    std::map<std::string, LogStream>::iterator it = User_.find(name);
    if (it==User_.end())
    {
      std::pair<std::map<std::string, LogStream>::iterator, bool> test =
        User_.insert(std::pair<std::string,LogStream>(name,LogStream()));
      if (!test.second)
      {
        ERROR << "[LogService] Cannot create new DebugUser logger @"
              << " function = " << __FUNCTION__
              << " file = " << __FILE__
              << " line = " << __LINE__
              << endmsg;
        ERROR << "DEBUG will be choosen" << endmsg;
        return Debug_;
      }
      else
      {
        it = test.first;
        it->second.SetColor(LogStream::CYAN);
        it->second.SetPrompt("USER["+name+"]: ");
        if (static_cast<UInt_t>(Level_) < static_cast<UInt_t>(USER_LEVEL)) 
             it->second.SetMute();
        if (ExclusiveUser_!="" && ExclusiveUser_!=name) it->second.SetMute();
      }
    }
    return it->second;
  }

  /// Modifiying level of verbosity
  void SetVerbosityLevel(VerbosityLevel level);

  /// Applying a veto on the user name for the USER logger
  void SetExclusiveUser(const std::string& name)
  {
    ExclusiveUser_=name;
    for (std::map<std::string,LogStream>::iterator it=User_.begin(); it!=User_.end(); it++)
    {
      if (it->first==name) it->second.SetMute();
      else it->second.SetMute();
    }
  }


};

}

#endif
