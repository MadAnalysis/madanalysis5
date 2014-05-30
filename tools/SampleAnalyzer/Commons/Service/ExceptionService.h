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


#ifndef EXCEPTION_SERVICE_H
#define EXCEPTION_SERVICE_H

// STL headers
#include <iostream>
#include <string>
#include <exception>
#include <cstdlib>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogReport.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionType.h"

// ShortCut to display and store an exception
#define MANAGE_EXCEPTION(e) if (dynamic_cast<const MA5::ExceptionType*>(&e)==0) \
                          MA5::ExceptionService::GetInstance()->Display( \
                          EXCEPTION_ERROR(e.what(),\
                          "Standard exception",0)); \
                          else MA5::ExceptionService::GetInstance()->Display(\
                          *(dynamic_cast<const MA5::ExceptionType*>(&e)));


namespace MA5
{

//////////////////////////////////////////////////////////////////////////////
/// The class ExceptionService manages the reports related to the logger
/// ERROR and WARNING. 
///
/// ExceptionService is a singleton-pattern-based class : only one instance.
/// Getting the only one instance : ExceptionService::GetInstance()
//////////////////////////////////////////////////////////////////////////////
class ExceptionService
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private :

  /// Pointer to the unique instance of ExceptionService
  static ExceptionService* Service_;

  /// Report for WARNING logger
  LogReport WarningReport_;

  /// Report for ERROR logger
  LogReport ErrorReport_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 private:

  /// Constructor without argument
  ExceptionService() 
  {
    WarningReport_.SetName("Warning");
    ErrorReport_.SetName("Error");
  }

  /// Destructor
  ~ExceptionService()
  {}

 public:

  /// Getting the unique instance of ExceptionService
  static ExceptionService* GetInstance()
  {
    if (Service_==0) Service_ = new ExceptionService;
    return Service_;
  }

  /// Deleting the unique instance of Exception Service
  static void Kill()
  {
    if (Service_!=0) delete Service_;
    Service_=0;
  }
  
  /// Accessor to the WARNING logger report
  LogReport& WarningReport()
  { return WarningReport_; }

  /// Accessor to the ERROR logger report
  LogReport& ErrorReport()
  { return ErrorReport_; }

  /// Displaying and storing the exception
  void Display(const ExceptionType& e);

};

}

#endif
