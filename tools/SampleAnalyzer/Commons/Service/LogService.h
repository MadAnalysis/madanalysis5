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
 * @file LogService.h
 * @brief Singleton giving access to the loggers (DEBUG, INFO, WARNING, ERROR, USER).
 */

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
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h" 


/** @brief Debug logger (usage: `DEBUG << ... << endmsg;`). */
#define DEBUG      MA5::LogService::GetInstance()->GetDebug()
/** @brief Information logger. */
#define INFO       MA5::LogService::GetInstance()->GetInfo()
/** @brief Warning logger. */
#define WARNING    MA5::LogService::GetInstance()->GetWarning()
/** @brief Error logger. */
#define ERROR      MA5::LogService::GetInstance()->GetError()
/** @brief User logger identified by a name. */
#define USER(id)   MA5::LogService::GetInstance()->GetUser(id)

namespace MA5
{

/** @brief Singleton giving access to the loggers, sorted by verbosity level. */
class LogService
{
 public:

  /** @brief Verbosity levels (a logger is muted if its level is below the chosen one). */
  enum VerbosityLevel{DEBUG_LEVEL=1,USER_LEVEL=2,
                      INFO_LEVEL=3,WARNING_LEVEL=4,
                      ERROR_LEVEL=5};

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /** @brief Unique instance. */
  static LogService* Service_;

  /** @brief Current verbosity level. */
  VerbosityLevel Level_;

  /** @brief Loggers with the DEBUG, INFO, WARNING and ERROR levels. */
  LogStream Debug_;

  /// Logger with INFO verbosity level
  LogStream Info_;

  /// Logger with WARNING verbosity level
  LogStream Warning_;

  /// Logger with ERROR verbosity level
  LogStream Error_;

  /** @brief Loggers with the USER level, by name. */
  std::map<std::string,LogStream> User_;

  /** @brief If set, only the USER logger with this name is active. */
  std::string ExclusiveUser_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 private:

  /** @brief Constructor (colours, prompts, INFO verbosity level). */
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

  /** @brief Destructor. */
  ~LogService()
  {}

  /**
   * @brief Mute or unmute all the USER loggers (respecting the exclusive user).
   *
   * @param mute true to mute.
   */
  void SetGlobalMuteUser(MAbool mute)
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

  /**
   * @brief Get the unique instance (created at the first call).
   *
   * @return the instance.
   */
  static LogService* GetInstance()
  {
    if (Service_==0) Service_ = new LogService;
    return Service_;
  }

  /** @brief Delete the unique instance. */
  static void Kill()
  {
    if (Service_!=0) delete Service_;
    Service_=0;
  }
  
  /**
   * @brief Accessor to the DEBUG logger.
   *
   * @return the logger.
   */
  LogStream& GetDebug()
  { return Debug_; }

  /**
   * @brief Accessor to the INFO logger.
   *
   * @return the logger.
   */
  LogStream& GetInfo()
  { return Info_; }

  /**
   * @brief Accessor to the WARNING logger.
   *
   * @return the logger.
   */
  LogStream& GetWarning()
  { return Warning_; }

  /**
   * @brief Accessor to the ERROR logger.
   *
   * @return the logger.
   */
  LogStream& GetError()
  { return Error_; }

  /**
   * @brief Get a USER logger (created if needed).
   *
   * @param name name of the logger.
   * @return the logger (the DEBUG logger if it cannot be created).
   */
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
        if (static_cast<MAuint32>(Level_) < static_cast<MAuint32>(USER_LEVEL)) 
             it->second.SetMute();
        if (ExclusiveUser_!="" && ExclusiveUser_!=name) it->second.SetMute();
      }
    }
    return it->second;
  }

  /**
   * @brief Set the verbosity level (and mute/unmute the loggers accordingly).
   *
   * @param level verbosity level.
   */
  void SetVerbosityLevel(VerbosityLevel level);

  /**
   * @brief Restrict the USER loggers to a single name.
   *
   * @param name name of the only active USER logger.
   */
  void SetExclusiveUser(const std::string& name)
  {
    ExclusiveUser_=name;
    for (std::map<std::string,LogStream>::iterator it=User_.begin(); it!=User_.end(); it++)
    {
      // FIXME: both branches mute the logger: the exclusive user is muted as well (SetUnMute was meant).
      if (it->first==name) it->second.SetMute();
      else it->second.SetMute();
    }
  }


};

}

#endif
