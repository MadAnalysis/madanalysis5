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


#ifndef LOG_STREAM_H
#define LOG_STREAM_H

// STL headers
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <typeinfo>
#include <sstream>

// ROOT headers
#include <Rtypes.h> 

namespace MA5
{

//////////////////////////////////////////////////////////////////////////////
/// The class LogStream is a logger which extends the class std::ostream
/// such as std::cout.
//////////////////////////////////////////////////////////////////////////////
class LogStream
{
 public:

  enum ColorType {NONE=0, BLACK=30, BLUE=34, GREEN=32, CYAN=36, 
                  RED=31, PURPLE=35, YELLOW=33, WHITE=37};

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /// Special manipulator which replaces std::endl for logger of LogStream type
  friend LogStream& endmsg(LogStream& os);

  /// Pointer to the output stream used 
  mutable std::ostream* Stream_; 

  /// Stream used as bugger 
  mutable std::stringstream Buffer_;

  /// Using color ?
  Bool_t ColorMode_;

  /// Color type
  ColorType Color_;

  /// Mute or UnMute ?
  Bool_t Mute_;

  /// Tag specifying if a new line is begun
  Bool_t NewLine_;

  /// String displayed at the beginning of a new line 
  std::string BeginLine_;

  /// String displayed at the end of a new line
  std::string EndLine_;

  /// Word used as prompt
  std::string Prompt_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------

 public:

  /// Constructor without argument
  LogStream() : Stream_(&std::cout), ColorMode_(true), 
                Color_(NONE), Mute_(false),
                NewLine_(true)
  {}
  
  /// Copy constructor
  LogStream(const LogStream& ref)
  {
    Stream_    = ref.Stream_; 
    ColorMode_ = ref.ColorMode_;
    Mute_      = ref.Mute_;
    NewLine_   = ref.NewLine_;
    Color_     = ref.Color_;
    BeginLine_ = ref.BeginLine_;
    EndLine_   = ref.EndLine_;
    Prompt_    = ref.Prompt_;
  }

  /// Clearing the content
  void Reset()
  {
    ColorMode_    = true;
    Color_        = NONE;
    Stream_       = &std::cout;
    Mute_         = false;
    NewLine_      = true;
    Prompt_       = "";
  }

  /// Mutator related to the output stream
  void SetStream(std::ostream* stream)
  { Stream_=stream; }

  /// Accessor to the output stream
  std::ostream* GetStream() const
  { return Stream_; }

  /// Overloading operator << for bool value
  LogStream& operator<< (bool val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for short value
  LogStream& operator<< (short val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for ushort value
  LogStream& operator<< (unsigned short val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for char value
  LogStream& operator<< (char val)
  { 
    if (NewEntry()) Buffer_ << static_cast<signed int>(val);
    return *this;
  }

  /// Overloading operator << for uchar value
  LogStream& operator<< (unsigned char val)
  { 
    if (NewEntry()) Buffer_ << static_cast<unsigned int>(val);
    return *this;
  }

  /// Overloading operator << for int value
  LogStream& operator<< (int val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for uint value
  LogStream& operator<< (unsigned int val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for long value
  LogStream& operator<< (long val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for long value
  LogStream& operator<< (long long val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for ulong value
  LogStream& operator<< (unsigned long val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for ulong value
  LogStream& operator<< (unsigned long long val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for float value
  LogStream& operator<< (float val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for double value
  LogStream& operator<< (double val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for long double value
  LogStream& operator<< (long double val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for const char* value
  LogStream& operator<< (const char * val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for const signed char* value
  LogStream& operator<< (const signed char * val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for const unsigned char* value
  LogStream& operator<< (const unsigned char * val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for string value
  LogStream& operator<< (std::string val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for any function
  LogStream& operator<< (const void* val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /// Overloading operator << for other stream 
  LogStream& operator<< (std::ostream& ( *pf )(std::ostream&))
  {
    if (NewEntry()) pf(Buffer_);
    return *this;
  }

  /// Overloading operator << for LogStream stream 
  LogStream& operator<< (LogStream& ( *pf )(LogStream&))
  {
    if (NewEntry()) pf(*this);
    return *this;
  }

  /// Overloading operator << for manipulator
  LogStream& operator<< (std::ios& ( *pf )(std::ios&))
  {
    if (NewEntry()) pf(Buffer_);
    return *this;
  }

  /// Overloading operator << for manipulator
  LogStream& operator<< (std::ios_base& ( *pf )(std::ios_base&))
  {
    if (NewEntry()) pf(Buffer_);
    return *this;
  }

  /// Overloading operator << for manipulator std::setfill
//  LogStream& operator<<(std::_Setfill<char> v)
//  {
//    if (NewEntry()) Buffer_<<v;
//    return *this;
//  }
//
//  /// Overloading operator << for manipulator std::setiosflags
//  LogStream& operator<<(std::_Setiosflags v)
//  {
//    if (NewEntry()) Buffer_<<v;
//    return *this;
//  }
//
//  /// Overloading operator << for manipulator std::resetiosflags
//  LogStream& operator<<(std::_Resetiosflags v)
//  {
//    if (NewEntry()) Buffer_<<v;
//    return *this;
//  }
//
//  /// Overloading operator << for manipulator std::setbase
//  LogStream& operator<<(std::_Setbase v)
//  {
//    if (NewEntry()) Buffer_<<v;
//    return *this;
//  }
//
//  /// Overloading operator << for manipulator std::setprecision
//  LogStream& operator<<(std::_Setprecision v)
//  {
//    if (NewEntry()) Buffer_<<v;
//    return *this;
//  }
//
//  /// Overloading operator << for manipulator std::setw
//  LogStream& operator<<(std::_Setw v)
//  {
//    if (NewEntry()) Buffer_<<v;
//    return *this;
//  }

  /// Enabling the color mode
  void EnableColor()
  { ColorMode_=true; Update(); }

  /// Disabling the color mode
  void DisableColor()
  { ColorMode_=false; Update(); }

  /// Is the stream mute ?
  Bool_t IsMute()
  { return Mute_;}

  /// Is the stream unmute ?
  Bool_t IsUnMute()
  { return !Mute_;}

  /// Mute the stream
  void SetMute()
  { Mute_=true; }

  /// UnMute the stream
  void SetUnMute()
  { Mute_=false; }

  /// Mutator relative to the prompt
  void SetPrompt(const std::string& prompt)
  { Prompt_=prompt; Update(); }

  /// Accessor to the prompt
  const std::string& GetPrompt() const
  { return Prompt_; }

  /// Mutator relative to the prompt
  void SetColor(ColorType color)
  { Color_=color; Update(); }

  /// Accessor to the color
  ColorType GetColor() const
  { return Color_; }

  /// Getting the character used for filling
  char fill() const
  { return Buffer_.fill(); }

  /// Setting the character used for filling
  char fill(char fillch) 
  { return Buffer_.fill(fillch); }

  /// Setting specific format flags
  std::ios_base::fmtflags setf(std::ios_base::fmtflags fmtfl)
  { return Buffer_.setf(fmtfl); }

  /// Setting specific format flags and mask
  std::ios_base::fmtflags setf(std::ios_base::fmtflags fmtfl,
                               std::ios_base::fmtflags mask)
  { return Buffer_.setf(fmtfl, mask); }

  /// Clearing specific format flags
  void unsetf(std::ios_base::fmtflags mask)
  { Buffer_.unsetf(mask); }

  /// Getting the digit precision
  std::streamsize precision() const
  { return Buffer_.precision(); }

  /// Setting the digit precision
  std::streamsize precision(std::streamsize prec)
  { return Buffer_.precision(prec); }

  /// Getting the width
  std::streamsize width() const
  { return Buffer_.width(); }

  /// Setting the width
  std::streamsize width(std::streamsize wide)
  { return Buffer_.width(wide); }

  /// Displaying n times a character c 
  void repeat(char c, UInt_t n)
  {
    if (NewEntry())
    { 
      Buffer_ << ""; 
      std::streamsize fillc = Buffer_.fill();
      Buffer_.fill(c); 
      Buffer_.width(n); 
      Buffer_ << "";
      Buffer_.fill(fillc);
    }
  }
 
 private:

  /// Global veto applied on the stream
  Bool_t NewEntry()
  {
    if (Mute_) return false;
    if (NewLine_) {Buffer_ << BeginLine_; NewLine_=false;}
    return true;
  }

  /// Updating header and foot string
  void Update()
  {
    if (!ColorMode_  || Color_==NONE)
    {
      BeginLine_="";
      EndLine_="";
    }
    else
    {
      std::stringstream str;
      str << "\x1b[" << static_cast<UInt_t>(Color_) << "m";
      BeginLine_=str.str();
      EndLine_="\x1b[0m";
    }
    BeginLine_+=Prompt_;
  }

};


/// Special manipulator which replaces std::endl for logger of LogStream type
LogStream& endmsg(LogStream& os);


}



#endif
