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
 * @file ExceptionService.h
 * @brief Handling of the SampleAnalyzer exceptions (MANAGE_EXCEPTION).
 */

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


/**
 * @brief Display and count an exception: SampleAnalyzer exceptions are displayed with their
 * location, other std::exception objects are converted into errors.
 */
#define MANAGE_EXCEPTION(e) if (dynamic_cast<const MA5::ExceptionType*>(&e)==0) \
                          MA5::ExceptionService::GetInstance()->Display( \
                          EXCEPTION_ERROR(e.what(),\
                          "Standard exception",0)); \
                          else MA5::ExceptionService::GetInstance()->Display(\
                          *(dynamic_cast<const MA5::ExceptionType*>(&e)));


namespace MA5
{

/** @brief Singleton displaying the exceptions and counting them in the WARNING and ERROR reports. */
class ExceptionService
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private :

  /** @brief Unique instance. */
  static ExceptionService* Service_;

  /** @brief Report of the warnings. */
  LogReport WarningReport_;

  /** @brief Report of the errors. */
  LogReport ErrorReport_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 private:

  /** @brief Constructor. */
  ExceptionService() 
  {
    WarningReport_.SetName("Warning");
    ErrorReport_.SetName("Error");
  }

  /** @brief Destructor. */
  ~ExceptionService()
  {}

 public:

  /**
   * @brief Get the unique instance (created at the first call).
   *
   * @return the instance.
   */
  static ExceptionService* GetInstance()
  {
    if (Service_==0) Service_ = new ExceptionService;
    return Service_;
  }

  /** @brief Delete the unique instance. */
  static void Kill()
  {
    if (Service_!=0) delete Service_;
    Service_=0;
  }
  
  /**
   * @brief Accessor to the report of the warnings.
   *
   * @return the report.
   */
  LogReport& WarningReport()
  { return WarningReport_; }

  /**
   * @brief Accessor to the report of the errors.
   *
   * @return the report.
   */
  LogReport& ErrorReport()
  { return ErrorReport_; }

  /**
   * @brief Count an exception and display it (unless vetoed by the thresholds or its verbosity).
   *
   * @param e exception.
   */
  void Display(const ExceptionType& e);

};

}

#endif
