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


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"

using namespace MA5;

// Initializing the static member 
ExceptionService* ExceptionService::Service_ = 0;


/// Displaying and storing the exception
void ExceptionService::Display(const ExceptionType& e)
{ 
  Bool_t display = false;

  // Add the exception into the good report
  if (e.IsWarning()) display=WarningReport_.Add(e.GetFileName(),
                                                e.GetLine(),
                                                e.GetMsg(),
                                                e.GetFunction());
  else display=ErrorReport_.Add(e.GetFileName(),
                                e.GetLine(),
                                e.GetMsg(),
                                e.GetFunction());

  // Veto ?
  if (!display) return;

  // Choose the good logger : WARNING or ERROR
  LogStream* os = 0;
  if (e.IsWarning()) os = &WARNING;
  else os = &ERROR;

  // Display error
  os->repeat('-',80); *os << endmsg;
  *os << " Msg     | " << e.GetMsg() << endmsg;
  *os << " Details | " << e.GetDetails() << endmsg;
  *os << " Where   |" 
      << " Function = " << e.GetFunction()
      << " ; File="     << e.GetFileName()
      << " ; Line="     << e.GetLine() << endmsg;
  os->repeat('-',80); *os << endmsg;
}
