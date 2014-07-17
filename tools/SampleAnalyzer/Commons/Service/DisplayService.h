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


#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H

// STL headers
#include <iostream>
#include <string>
#include <sstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogReport.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionType.h"

// ShortCut to display and store an exception
#define DISPLAY MA5::DisplayService::GetInstance()

namespace MA5
{

//////////////////////////////////////////////////////////////////////////////
/// The class ExceptionService manages the reports related to the logger
/// ERROR and WARNING. 
///
/// ExceptionService is a singleton-pattern-based class : only one instance.
/// Getting the only one instance : ExceptionService::GetInstance()
//////////////////////////////////////////////////////////////////////////////
class DisplayService
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private :

  /// Pointer to the unique instance of DisplayService
  static DisplayService* Service_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 private:

  /// Constructor without argument
  DisplayService() 
  {
    oldCoutStreamBuf_=0;
    oldCerrStreamBuf_=0;
  }

  /// Destructor
  ~DisplayService()
  {}

  /// Private data
  std::streambuf* oldCoutStreamBuf_;
  std::streambuf* oldCerrStreamBuf_;

 public:

  /// Getting the unique instance of DisplayService
  static DisplayService* GetInstance()
  {
    if (Service_==0) Service_ = new DisplayService;
    return Service_;
  }

  /// Deleting the unique instance of Display Service
  static void Kill()
  {
    if (Service_!=0) delete Service_;
    Service_=0;
  }
  
  /// Redirecting std::cout to a stringstream
  void beginCoutRedirection(std::stringstream& str);
  void endCoutRedirection();

  /// Redirecting std::cerr to a stringstream
  void beginCerrRedirection(std::stringstream& str);
  void endCerrRedirection();

  /// Redirecting a stringstream to a file
  bool redirectToFile(std::stringstream& str,
                      const std::string& filename,
                      bool recreate=true);

};

}

#endif
