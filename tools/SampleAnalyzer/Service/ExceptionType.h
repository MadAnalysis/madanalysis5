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


#ifndef EXCEPTION_TYPE_H
#define EXCEPTION_TYPE_H

// ShortCut to the creation of ExceptionType instance 
#define EXCEPTION_WARNING(msg,details,num) ExceptionType(__FILE__,__LINE__,__FUNCTION__,true,msg,details,num)
#define EXCEPTION_ERROR(msg,details,num)   ExceptionType(__FILE__,__LINE__,__FUNCTION__,false,msg,details,num)

// STL headers
#include <iostream>
#include <string>
#include <exception>

// ROOT headers
#include <Rtypes.h> 

namespace MA5
{

//////////////////////////////////////////////////////////////////////////////
/// The class ExceptionType extends the content of standard exception 
/// (std::exception).
///
/// ExceptionService contains : <BR>
///  <DT> description of the exception </DT>
///  <DT> details about the exception </DT>
///  <DT> location of the exception : file name, 
///                                   function name, 
///                                   line number </DT>
///  <DT> logger to be used : WARNING or ERROR </DT>
///  <DT> number ID </DT>
//////////////////////////////////////////////////////////////////////////////
class ExceptionType : public std::exception
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private :

  /// Name of the file where the exception has been thrown
  std::string FileName_;

  /// Description of the exception
  std::string Msg_;

  /// Name of the function where the exception has been thrown
  std::string Function_;

  /// Details about the exception
  std::string Details_; 

  /// Line number of the file where the exception has been thrown
  UInt_t Line_;

  /// Logger to be used : WARNING (=true), ERROR(=false)
  Bool_t Warning_;

  /// Number ID specified by the user
  Int_t Num_;
  

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor with arguments
  ExceptionType(const std::string& filename, 
                const UInt_t& line,
                const std::string function,
                const Bool_t& warning,
                const std::string& msg,
                const std::string& details="",
                const Int_t& Num=0  ) : FileName_(filename),
                                        Msg_(msg), 
                                        Function_(function),
                                        Details_(details),
                                        Line_(line),
                                        Warning_(warning),
                                        Num_(Num)
  { }
 
  /// Destructor
  virtual ~ExceptionType() throw()
  {}

  /// Accessor to the description of the exception
  virtual const char* what() const throw()
  { return Msg_.c_str(); }

  /// Accessor to the number ID
  const Int_t& GetID() const throw()
  { return Num_; }

  /// Is WARNING logger used ? 
  Bool_t IsWarning() const
  { return Warning_; }

  /// Is ERROR logger used ? 
  Bool_t IsError() const
  { return !Warning_; }

  /// Accessor to the description of the exception
  const std::string& GetMsg() const
  { return Msg_; }

  /// Accessor to the file name
  const std::string& GetFileName() const
  { return FileName_; }

  /// Accessor to the line number
  const UInt_t& GetLine() const
  { return Line_; }

  /// Accessor to the function name
  const std::string& GetFunction() const
  { return Function_; }

  /// Accessor to the details about the exception 
  const std::string& GetDetails() const
  { return Details_; }

};

}

#endif
