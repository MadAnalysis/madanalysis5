/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   RFLog.cc
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

#include <iomanip>
#include <stdlib.h>
#include "SampleAnalyzer/Commons/RestFrames/RFLog.h"
#include "SampleAnalyzer/Commons/RestFrames/RFBase.h"

namespace RestFrames {

  class RFBase;

  // default RFLog parameters
  std::map<RestFrames::LogType,bool> RFLog::m_PrintMap = InitPrintMap();
  std::ostream* RFLog::m_Ostr = &std::cerr;
  bool RFLog::m_Color = true;
  int RFLog::m_NMAX = 100;

  RFLog::RFLog(const std::string& source, LogType def_type)
    : m_DefType(def_type)
  {
    Init();
    SetSource(source);
  }

  RFLog::RFLog(){
    Init();
  }

  RFLog g_Log("RestFrames Global");

  RFLog::~RFLog() {}

  void RFLog::Init(){
    m_Source = "Unknown"; 
    m_CurType = m_DefType;
    m_Message.str("");
    m_TypeMap[LogVerbose]  = "VERBOSE";
    m_TypeMap[LogDebug]    = "DEBUG";
    m_TypeMap[LogInfo]     = "INFO";
    m_TypeMap[LogWarning]  = "WARNING";
    m_TypeMap[LogError]    = "ERROR";

    m_ColorMap[LogVerbose]  = "\x1b[36m";
    m_ColorMap[LogDebug]    = "\x1b[33m";
    m_ColorMap[LogInfo]     = "\x1b[32m";
    m_ColorMap[LogWarning]  = "\x1b[35m";
    m_ColorMap[LogError]    = "\x1b[31m";
  }

  std::map<LogType,bool> InitPrintMap(){
    std::map<LogType,bool> m;
    m[LogVerbose]  = false;
    m[LogDebug]    = false;
    m[LogInfo]     = true;
    m[LogWarning]  = true;
    m[LogError]    = true;
    return m;
  }

  std::string RFLog::GetFormattedSource() const {
    std::string source_name = m_Source;
    if (source_name.size() > 22){
      source_name = source_name.substr( 0, 22 - 3 );
      source_name += "...";
    }
    return source_name;
  }
  
  std::string RFLog::GetFormattedMessage(const std::string& message) {
    std::string output = "";
    int N = message.size();
    int OFF = 18;
    if(N-OFF > m_NMAX){
      int Ncut = (N-OFF)/m_NMAX;
      if((N-OFF)%m_NMAX == 0)
	Ncut--;
      std::string::size_type previous_pos = 0;
      for(int i = 0; i <= Ncut; i++){
	int off = m_NMAX;
	if(i == 0) off += OFF;
	std::string line = message.substr(previous_pos, off);
	if(i > 0){
	  if(m_Color)
	    output += m_ColorMap[m_CurType]+"<...>\x1b[0m ...";
	  else
	    output += "<...> ...";
	}
	output += line;
	previous_pos += off;
	if(int(previous_pos) != N && i != Ncut) output += "...\n";
      }
    } else {
      output = message;
    }
    return output;
  }

  void RFLog::Send(){
    std::string source_name = GetFormattedSource();
    std::string message = m_Message.str();
    std::string::size_type previous_pos = 0, current_pos = 0;
    if(m_PrintMap[m_CurType] && m_Ostr){
      std::string prefix;
      if(m_Color)
	prefix = m_ColorMap[m_CurType]+"<"+m_TypeMap[m_CurType]+">";
      else
	prefix = "<"+m_TypeMap[m_CurType]+">";
      for(int i = 0; i < 8-int(m_TypeMap[m_CurType].size()); i++){
	prefix += ' ';
      }
      prefix += source_name+": ";
      if(m_Color) 
	prefix +="\x1b[0m";
      while (true) {
	current_pos = message.find( '\n', previous_pos );
	std::string line = message.substr( previous_pos, current_pos - previous_pos );
	
	std::ostringstream message_to_send;
	message_to_send.setf(std::ios::adjustfield, std::ios::left); 
	line = GetFormattedMessage(prefix+line);
	message_to_send << line << std::endl;
	
	*m_Ostr << message_to_send.str();
	m_Ostr->flush();
	
	if (current_pos == message.npos) break;
	previous_pos = current_pos + 1;
      }
    }
   
    if (m_CurType == LogError)
      throw RestFramesException(m_Message.str());
    
    m_Message.str("");
    m_CurType = m_DefType;
    return;
  }

  RFLog& RFLog::EndMessage(RFLog& log){
    log.Send();
    return log;
  }

  void RFLog::PrintObject(const RFBase* objPtr){
    m_Message << objPtr->PrintString(LogVerbose);
  }

  template <class T>
  void RFLog::PrintList(const RFList<T>* listPtr){
    int N = listPtr->GetN();
    for(int i = 0; i < N; i++) 
      m_Message << listPtr->Get(i).GetName() << " ";
  }

  void RFLog::SetSource(const std::string& source){ 
    m_Source = source; 
  }

  void SetLogPrint(LogType type, bool print){
    RFLog::m_PrintMap[type] = print;
  }

  void SetLogPrint(bool print){
    for (std::map<LogType, bool>::iterator m = RFLog::m_PrintMap.begin(); 
	 m != RFLog::m_PrintMap.end(); ++m)
      m->second = (m->second && print);
  }

  void SetLogStream(std::ostream* ostr){
    if(ostr) RFLog::m_Ostr = ostr;
  }

  void SetLogColor(bool color){
    RFLog::m_Color = color;
  }

  void SetLogMaxWidth(int NMAX){
    if(NMAX > 0) RFLog::m_NMAX = NMAX;
  }

  template <> RFLog& RFLog::operator << (const RFBase* arg){
    PrintObject(arg);
    return *this;
  }

  template <> RFLog& RFLog::operator << (const RFBaseList* arg){
    PrintList(arg);
    return *this;
  }

  const RFBase* Log(const RFBase& obj){ return (const RFBase*)&obj; }
  const RFBase* Log(const RFBase* ptr){ return (const RFBase*)ptr; }

}
