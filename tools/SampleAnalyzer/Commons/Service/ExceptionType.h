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
 * @file ExceptionType.h
 * @brief Exception type of SampleAnalyzer (EXCEPTION_WARNING, EXCEPTION_ERROR).
 */

#ifndef EXCEPTION_TYPE_H
#define EXCEPTION_TYPE_H


// STL headers
#include <iostream>
#include <string>
#include <exception>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h" 


/** @brief Create a warning exception located at the current file, line and function. */
#define EXCEPTION_WARNING(msg,details,num) MA5::ExceptionType(__FILE__,__LINE__,__FUNCTION__,true, msg,details,num)
/** @brief Create an error exception located at the current file, line and function. */
#define EXCEPTION_ERROR(msg,details,num)   MA5::ExceptionType(__FILE__,__LINE__,__FUNCTION__,false,msg,details,num)
/** @brief Create a warning exception that can be silenced (verbose = false). */
#define EXCEPTION_WARNING_VERBOSE(msg,details,num,verbose) MA5::ExceptionType(__FILE__,__LINE__,__FUNCTION__,true, msg,details,num,verbose)

namespace MA5
{

/** @brief Exception carrying a message, details, its location, its level (warning or error) and an identifier. */
class ExceptionType : public std::exception
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private :

  /** @brief File where the exception is thrown. */
  std::string FileName_;

  /** @brief Description of the exception. */
  std::string Msg_;

  /** @brief Function where the exception is thrown. */
  std::string Function_;

  /** @brief Details about the exception. */
  std::string Details_; 

  /** @brief Line where the exception is thrown. */
  MAuint32 Line_;

  /** @brief Level: warning (true) or error (false). */
  MAbool Warning_;

  /** @brief Identifier given by the user. */
  MAint32 Num_;

  /** @brief Should the exception be displayed? */
  MAbool Verbose_;
  

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /**
   * @brief Constructor.
   *
   * @param filename file name.
   * @param line line number.
   * @param function function name.
   * @param warning true for a warning, false for an error.
   * @param msg description.
   * @param details details.
   * @param Num identifier.
   * @param Verbose display the exception.
   */
  ExceptionType(const std::string& filename, 
                const MAuint32& line,
                const std::string function,
                const MAbool& warning,
                const std::string& msg,
                const std::string& details="",
                const MAint32& Num=0,
                const MAbool& Verbose=true) throw() :
                                            FileName_(filename),
                                            Msg_(msg),
                                            Function_(function),
                                            Details_(details),
                                            Line_(line),
                                            Warning_(warning),
                                            Num_(Num),
                                            Verbose_(Verbose)
  { }
 
  /** @brief Destructor. */
  virtual ~ExceptionType() throw()
  {}

  /**
   * @brief Accessor to the description.
   *
   * @return the description.
   */
  virtual const MAchar* what() const throw()
  { return Msg_.c_str(); }

  /**
   * @brief Accessor to the identifier.
   *
   * @return the identifier.
   */
  const MAint32& GetID() const throw()
  { return Num_; }

  /**
   * @brief Is it a warning?
   *
   * @return true for a warning.
   */
  MAbool IsWarning() const throw()
  { return Warning_; }

  /**
   * @brief Is it an error?
   *
   * @return true for an error.
   */
  MAbool IsError() const throw()
  { return !Warning_; }

  /**
   * @brief Accessor to the description.
   *
   * @return the description.
   */
  const std::string& GetMsg() const throw()
  { return Msg_; }

  /**
   * @brief Accessor to the file name.
   *
   * @return the file name.
   */
  const std::string& GetFileName() const throw()
  { return FileName_; }

  /**
   * @brief Accessor to the line number.
   *
   * @return the line number.
   */
  const MAuint32& GetLine() const throw()
  { return Line_; }

  /**
   * @brief Accessor to the function name.
   *
   * @return the function name.
   */
  const std::string& GetFunction() const throw()
  { return Function_; }

  /**
   * @brief Accessor to the details.
   *
   * @return the details.
   */
  const std::string& GetDetails() const throw()
  { return Details_; }

  /**
   * @brief Should the exception be displayed?
   *
   * @return the verbosity flag.
   */
  MAbool verbose() const throw()
  { return Verbose_; }

};

}

#endif
