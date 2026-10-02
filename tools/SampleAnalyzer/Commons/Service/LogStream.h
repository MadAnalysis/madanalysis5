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
 * @file LogStream.h
 * @brief Logger stream with colour, prompt and mute support.
 */

#ifndef LOG_STREAM_H
#define LOG_STREAM_H


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"

// STL headers
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <typeinfo>
#include <sstream>


namespace MA5
{

/**
 * @brief Logger behaving like a std::ostream.
 *
 * The text is accumulated in a buffer and written to the output stream (std::cout by
 * default), with the prompt and the ANSI colour codes, when the manipulator endmsg
 * is streamed. A muted logger ignores everything.
 */
class LogStream
{
 public:

  /** @brief ANSI colour codes. */
  enum ColorType {NONE=0, BLACK=30, BLUE=34, GREEN=32, CYAN=36, 
                  RED=31, PURPLE=35, YELLOW=33, WHITE=37};

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /** @brief Manipulator flushing the buffer (replaces std::endl). */
  friend LogStream& endmsg(LogStream& os);

  /** @brief Output stream. */
  mutable std::ostream* Stream_; 

  /** @brief Buffer of the current line. */
  mutable std::stringstream Buffer_;

  /** @brief Are the colours enabled? */
  MAbool ColorMode_;

  /** @brief Colour of the logger. */
  ColorType Color_;

  /** @brief Is the logger muted? */
  MAbool Mute_;

  /** @brief Is a new line being started? */
  MAbool NewLine_;

  /** @brief String written at the beginning of each line (colour code and prompt). */
  std::string BeginLine_;

  /** @brief String written at the end of each line (colour reset). */
  std::string EndLine_;

  /** @brief Prompt. */
  std::string Prompt_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------

 public:

  /** @brief Constructor (output to std::cout, no colour, unmuted). */
  LogStream() : Stream_(&std::cout), ColorMode_(true), 
                Color_(NONE), Mute_(false),
                NewLine_(true)
  {}
  
  /**
   * @brief Copy constructor (the buffer is not copied).
   *
   * @param ref logger to copy.
   */
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

  /** @brief Reset the settings (the begin-of-line string is not updated). */
  void Reset()
  {
    ColorMode_    = true;
    Color_        = NONE;
    Stream_       = &std::cout;
    Mute_         = false;
    NewLine_      = true;
    Prompt_       = "";
  }

  /**
   * @brief Set the output stream.
   *
   * @param stream output stream.
   */
  void SetStream(std::ostream* stream)
  { Stream_=stream; }

  /**
   * @brief Accessor to the output stream.
   *
   * @return the stream.
   */
  std::ostream* GetStream() const
  { return Stream_; }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (bool val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (short val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (unsigned short val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a character, printed as its integer code (ignored if muted).
   *
   * @param val character.
   * @return this logger.
   */
  LogStream& operator<< (char val)
  { 
    if (NewEntry()) Buffer_ << static_cast<signed int>(val);
    return *this;
  }

  /**
   * @brief Append an unsigned character, printed as its integer code (ignored if muted).
   *
   * @param val character.
   * @return this logger.
   */
  LogStream& operator<< (unsigned char val)
  { 
    if (NewEntry()) Buffer_ << static_cast<unsigned int>(val);
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (int val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (unsigned int val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (long val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (long long val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (unsigned long val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (unsigned long long val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (float val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (double val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (long double val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (const char * val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (const signed char * val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (const unsigned char * val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (std::string val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Append a value to the current line (ignored if muted).
   *
   * @param val value.
   * @return this logger.
   */
  LogStream& operator<< (const void* val)
  { 
    if (NewEntry()) Buffer_ << val;
    return *this;
  }

  /**
   * @brief Apply a std::ostream manipulator (e.g. std::setw) to the buffer.
   *
   * @param pf manipulator.
   * @return this logger.
   */
  LogStream& operator<< (std::ostream& ( *pf )(std::ostream&))
  {
    if (NewEntry()) pf(Buffer_);
    return *this;
  }

  /**
   * @brief Apply a LogStream manipulator (e.g. endmsg).
   *
   * @param pf manipulator.
   * @return this logger.
   */
  LogStream& operator<< (LogStream& ( *pf )(LogStream&))
  {
    if (NewEntry()) pf(*this);
    return *this;
  }

  /**
   * @brief Apply a std::ios manipulator to the buffer.
   *
   * @param pf manipulator.
   * @return this logger.
   */
  LogStream& operator<< (std::ios& ( *pf )(std::ios&))
  {
    if (NewEntry()) pf(Buffer_);
    return *this;
  }

  /**
   * @brief Apply a std::ios_base manipulator (e.g. std::fixed) to the buffer.
   *
   * @param pf manipulator.
   * @return this logger.
   */
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

  /** @brief Enable the colours. */
  void EnableColor()
  { ColorMode_=true; Update(); }

  /** @brief Disable the colours. */
  void DisableColor()
  { ColorMode_=false; Update(); }

  /**
   * @brief Is the logger muted?
   *
   * @return true if muted.
   */
  MAbool IsMute()
  { return Mute_;}

  /**
   * @brief Is the logger active?
   *
   * @return true if not muted.
   */
  MAbool IsUnMute()
  { return !Mute_;}

  /** @brief Mute the logger. */
  void SetMute()
  { Mute_=true; }

  /** @brief Unmute the logger. */
  void SetUnMute()
  { Mute_=false; }

  /**
   * @brief Set the prompt.
   *
   * @param prompt prompt.
   */
  void SetPrompt(const std::string& prompt)
  { Prompt_=prompt; Update(); }

  /**
   * @brief Accessor to the prompt.
   *
   * @return the prompt.
   */
  const std::string& GetPrompt() const
  { return Prompt_; }

  /**
   * @brief Set the colour.
   *
   * @param color colour.
   */
  void SetColor(ColorType color)
  { Color_=color; Update(); }

  /**
   * @brief Accessor to the colour.
   *
   * @return the colour.
   */
  ColorType GetColor() const
  { return Color_; }

  /**
   * @brief Accessor to the fill character.
   *
   * @return the character.
   */
  char fill() const
  { return Buffer_.fill(); }

  /**
   * @brief Set the fill character.
   *
   * @param fillch character.
   * @return the previous character.
   */
  char fill(char fillch) 
  { return Buffer_.fill(fillch); }

  /**
   * @brief Set format flags.
   *
   * @param fmtfl flags.
   * @return the previous flags.
   */
  std::ios_base::fmtflags setf(std::ios_base::fmtflags fmtfl)
  { return Buffer_.setf(fmtfl); }

  /**
   * @brief Set format flags under a mask.
   *
   * @param fmtfl flags.
   * @param mask mask.
   * @return the previous flags.
   */
  std::ios_base::fmtflags setf(std::ios_base::fmtflags fmtfl,
                               std::ios_base::fmtflags mask)
  { return Buffer_.setf(fmtfl, mask); }

  /**
   * @brief Clear format flags.
   *
   * @param mask flags to clear.
   */
  void unsetf(std::ios_base::fmtflags mask)
  { Buffer_.unsetf(mask); }

  /**
   * @brief Accessor to the precision.
   *
   * @return the precision.
   */
  std::streamsize precision() const
  { return Buffer_.precision(); }

  /**
   * @brief Set the precision.
   *
   * @param prec precision.
   * @return the previous precision.
   */
  std::streamsize precision(std::streamsize prec)
  { return Buffer_.precision(prec); }

  /**
   * @brief Accessor to the field width.
   *
   * @return the width.
   */
  std::streamsize width() const
  { return Buffer_.width(); }

  /**
   * @brief Set the field width.
   *
   * @param wide width.
   * @return the previous width.
   */
  std::streamsize width(std::streamsize wide)
  { return Buffer_.width(wide); }

  /**
   * @brief Write a character several times.
   *
   * @param c character.
   * @param n number of repetitions.
   */
  void repeat(char c, MAuint32 n)
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

  /**
   * @brief Start a new entry: write the begin-of-line string if needed.
   *
   * @return false if the logger is muted.
   */
  MAbool NewEntry()
  {
    if (Mute_) return false;
    if (NewLine_) {Buffer_ << BeginLine_; NewLine_=false;}
    return true;
  }

  /** @brief Recompute the begin/end-of-line strings from the colour and the prompt. */
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
      str << "\x1b[" << static_cast<MAuint32>(Color_) << "m";
      BeginLine_=str.str();
      EndLine_="\x1b[0m";
    }
    BeginLine_+=Prompt_;
  }

};


/**
 * @brief Flush the current line of a logger (replaces std::endl).
 *
 * @param os logger.
 * @return the logger.
 */
LogStream& endmsg(LogStream& os);


}



#endif
