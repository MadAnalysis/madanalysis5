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


#ifndef LOG_MSG_KEY_H
#define LOG_MSG_KEY_H

// STL headers
#include<string>

// ROOT headers
#include <Rtypes.h> 


namespace MA5
{

//////////////////////////////////////////////////////////////////////////////
/// The class LogMsgKey contains data which characterizes (unique ID) of an
/// exception.
//////////////////////////////////////////////////////////////////////////////
class LogMsgKey
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /// Name of the file where the exception is thrown
  std::string FileName_;

  /// Number of the line where the exception is thrown
  UInt_t Line_;

  /// Description of the exception
  std::string Msg_;
  
  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument 
  LogMsgKey() : Line_(0)
  { }

  /// Constructor without argument 
  LogMsgKey(const std::string& FileName, 
            const UInt_t& Line,
            const std::string& Msg) : FileName_(FileName), 
                                      Line_(Line),
                                      Msg_(Msg)
  { }
  
  /// Destructor
  ~LogMsgKey()
  {}

  /// Reseting the content
  void Reset()
  {
    FileName_=""; Line_=0; Msg_="";
  } 
  
  /// Accessor to the file name
  const std::string& GetFileName() const
  {return FileName_;}

  /// Accessor to the line number
  const UInt_t& GetLine() const 
  {return Line_;}

  /// Accessor to the description of the exception
  const std::string& GetMsg() const 
  {return Msg_;}
  
  /// Mutator related to the file name
  void SetFileName(const std::string& name) 
  {FileName_=name;}

  /// Mutator related to the line number
  void SetLine(const UInt_t& line)          
  {Line_=line;}

  /// Mutator related to the description of the exception
  void SetMsg(const std::string& msg)       
  {Msg_=msg;}

  /// Order relation
  Bool_t operator < (const LogMsgKey& b) const
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

