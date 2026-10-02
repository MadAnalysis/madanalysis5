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
 * @file Terminate.h
 * @brief Handler of abnormal terminations (reports printed before aborting).
 */

#ifndef TERMINATE_H
#define TERMINATE_H


// STL headers
#include <exception>
#include <cstdlib>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/TimeService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

/** @brief Handler of abnormal terminations. */
class Terminate
{
  // -------------------------------------------------------------
  //                       static methods
  // -------------------------------------------------------------
 public:

  /** @brief Install TerminateAndDisplayReport() as std::terminate handler. */
  static void Initialize()
  { std::set_terminate(Terminate::TerminateAndDisplayReport); }

  /** @brief Print the timing and exception reports, kill the services and abort. */
  static void TerminateAndDisplayReport()
  {
    // Warning
    ERROR << endmsg;
    ERROR.repeat('=',80); ERROR << endmsg;
    ERROR << " Abnormal termination " << endmsg;
    ERROR.repeat('=',80); ERROR << endmsg;
    ERROR << endmsg;

    // Display reports
    TimeService::GetInstance()->WriteGenericReport();
    ExceptionService::GetInstance()->WarningReport().WriteGenericReport();
    ExceptionService::GetInstance()->ErrorReport().WriteGenericReport();

    // Kill all singleton services
    ExceptionService::Kill();
    TimeService::Kill();
    LogService::Kill();

    // Exit
    abort();
  }

};

}

#endif
