/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   RFLog.h
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2015 Jan
///
//   This file is part of RestFrames.
//
//   RestFrames is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
// 
//   RestFrames is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
// 
//   You should have received a copy of the GNU General Public License
//   along with RestFrames. If not, see <http://www.gnu.org/licenses/>.
/////////////////////////////////////////////////////////////////////////

#ifndef RFLog_H
#define RFLog_H
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <exception>

#include "SampleAnalyzer/Commons/RestFrames/RFList.h"

namespace RestFrames {

  class RFBase;

  /// Type of Log Message
  enum LogType { LogError, LogWarning, LogInfo, 
		 LogDebug, LogVerbose };

  ///////////////////////////////////////////////
  // RFLog class
  ///////////////////////////////////////////////
  class RFLog {
  public:
    RFLog(const std::string& source, LogType def_type = LogInfo);
    RFLog();
    ~RFLog();

    void SetSource(const std::string& source);

    friend void SetLogPrint(bool print);
    friend void SetLogPrint(LogType type, bool print);
    friend void SetLogStream(std::ostream* ostr);
    friend void SetLogColor(bool color);
    friend void SetLogMaxWidth(int NMAX);

    static RFLog& EndMessage(RFLog& log);
      
    RFLog& operator<< (LogType type);
    RFLog& operator<< (RFLog& (*_f)( RFLog&));
    RFLog& operator<< (std::ostream& (*_f)(std::ostream&));
    RFLog& operator<< (std::ios& (*_f)(std::ios&));

    template <class T> 
    RFLog& operator<< (T arg){
      m_Message << arg;
      return *this;
    }

  private:
    static std::ostream* m_Ostr;
    static bool          m_Color;
    static int           m_NMAX;
    static std::map<LogType, bool> m_PrintMap;
    std::map<LogType, std::string> m_TypeMap;      
    std::map<LogType, std::string> m_ColorMap;  

    void Send();
    void Init();
    std::string GetFormattedSource() const;
    std::string GetFormattedMessage(const std::string& message);

    void PrintObject(const RFBase* objPtr);
    template <class T>
    void PrintList(const RFList<T>* listPtr);

    LogType m_DefType;
    LogType m_CurType;
    std::string m_Source;
    std::ostringstream m_Message;

  };

  template <> RFLog& RFLog::operator<< (const RFBase* arg);
  template <> RFLog& RFLog::operator<< (const RFBaseList* arg);

  inline RFLog& RFLog::operator<< (RFLog& (*_f)(RFLog&)){
    return (_f)(*this);
  }

  inline RFLog& RFLog::operator<< (std::ostream& (*_f)(std::ostream&)){
   (_f)(m_Message);
   return *this;
  }

  inline RFLog& RFLog::operator<< (std::ios& (*_f)(std::ios&)){
    (_f)(m_Message);
    return *this;
  }

  inline RFLog& RFLog::operator<< (LogType type){
    m_CurType = type;
    return *this;
  }

  extern RFLog g_Log;

  const RFBase* Log(const RFBase& obj);
  const RFBase* Log(const RFBase* ptr);
  template <class T> 
  const RFList<RFBase>* Log(const RFList<T>& list){ return (const RFBaseList*)&list; }
  template <class T> 
  const RFList<RFBase>* Log(const RFList<T>* ptr){ return (const RFBaseList*)ptr; }

#ifndef __MAKECINT__
  #define LogEnd RFLog::EndMessage
#endif

  class RestFramesException : public std::exception {

  public:
    RestFramesException(const std::string& message) : m_Message(message) {} 

    virtual ~RestFramesException() throw() {}

    virtual const char* what() const throw(){
      return m_Message.c_str();
    }
	
  private:
    std::string m_Message;
    
  };

  void SetLogPrint(bool print = true);
  void SetLogPrint(LogType type, bool print = true);
  void SetLogStream(std::ostream* ostr);
  void SetLogColor(bool color = true);
  void SetLogMaxWidth(int NMAX);
  std::map<RestFrames::LogType,bool> InitPrintMap();

}

#endif 
