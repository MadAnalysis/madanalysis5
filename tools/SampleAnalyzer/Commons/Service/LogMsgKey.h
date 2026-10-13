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
 * @file LogMsgKey.h
 * @brief Key identifying a logged message (file, line, text).
 */

#ifndef LOG_MSG_KEY_H
#define LOG_MSG_KEY_H


// STL headers
#include <string>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h" 


namespace MA5
{

/** @brief Key identifying a logged message or exception (file, line and text). */
class LogMsgKey
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /** @brief Name of the file where the message is issued. */
  std::string FileName_;

  /** @brief Line where the message is issued. */
  MAuint32 Line_;

  /** @brief Text of the message. */
  std::string Msg_;
  
  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  LogMsgKey() : Line_(0)
  { }

  /**
   * @brief Constructor.
   *
   * @param FileName file name.
   * @param Line line number.
   * @param Msg text.
   */
  LogMsgKey(const std::string& FileName, 
            const MAuint32& Line,
            const std::string& Msg) : FileName_(FileName), 
                                      Line_(Line),
                                      Msg_(Msg)
  { }
  
  /** @brief Destructor. */
  ~LogMsgKey()
  {}

  /** @brief Reset the content. */
  void Reset()
  {
    FileName_=""; Line_=0; Msg_="";
  } 
  
  /**
   * @brief Accessor to the file name.
   *
   * @return the file name.
   */
  const std::string& GetFileName() const
  {return FileName_;}

  /**
   * @brief Accessor to the line number.
   *
   * @return the line number.
   */
  const MAuint32& GetLine() const 
  {return Line_;}

  /**
   * @brief Accessor to the text.
   *
   * @return the text.
   */
  const std::string& GetMsg() const 
  {return Msg_;}
  
  /**
   * @brief Set the file name.
   *
   * @param name file name.
   */
  void SetFileName(const std::string& name) 
  {FileName_=name;}

  /**
   * @brief Set the line number.
   *
   * @param line line number.
   */
  void SetLine(const MAuint32& line)          
  {Line_=line;}

  /**
   * @brief Set the text.
   *
   * @param msg text.
   */
  void SetMsg(const std::string& msg)       
  {Msg_=msg;}

  /**
   * @brief Order relation (line, then file name, then text).
   *
   * @param b other key.
   * @return true if this key comes first.
   */
  MAbool operator < (const LogMsgKey& b) const
  {
    if (Line_ < b.Line_) return true;
    else if (Line_ > b.Line_) return false;
    else 
    {
      if (FileName_ < b.FileName_) return true;
      else if (FileName_ > b.FileName_) return false;
      else 
      {
        return (Msg_ < b.Msg_);
      }
    }
  }

};

}

#endif

